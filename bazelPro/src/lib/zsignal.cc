#ifndef _GNU_SOURCE
#define _GNU_SOURCE // pipe2
#endif

#include <errno.h> // errno
#include <limits.h> // PIPE_BUF
#include <fcntl.h> // O_CLOEXEC, O_NONBLOCK
#include <unistd.h> // read, write
#include <string.h> // strerror
#include <fenv.h> // feenableexcept
#include <stdlib.h> // exit
#include <execinfo.h> // backtrace


#include <event2/event.h>

#include "src/lib/comm.h"
#include "src/lib/zsignal.h"
#include "src/lib/zmutex.h"
#include "src/lib/loop.h"
#include "src/lib/zerror.h"
#include "src/lib/prettyprint.h"
#include "src/lib/zlog.h"

namespace ZLIB
{

static const int fatal_signals[] = { SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT, SIGSYS };
static volatile sig_atomic_t abort_expected = 0;
static int selfpipe[2] = {-1, -1};

typedef struct {
  struct event_base *evbase; // event loop of the thread that registered the callback
  signal_cb_t cb;           // user callback to be called when signo is caught
  void *ctxt;                // user context to be given to cb
} user_info_t;

user_info_t users[NSIG];
thread_mutex_t mtx; // mutex to protect the list

typedef struct {
  siginfo_t siginfo;    // signal information
  signal_cb_t user_cb; // user callback to be called
  void *user_ctxt;      // user context to be given to cb
  struct event *event;  // libevent event used
} signal_ctxt_t;

static int is_fatal(int signo) {
  unsigned int i;

  for (i = 0 ; i < DIM(fatal_signals) ; i++)
    if (fatal_signals[i] == signo)
      return 1;

  return 0;
}


static void prettyprint_siginfo_and_ucontext(buffer_t *buf, const char *data, size_t datalen) {
 
  void **siginfo_and_uctxt = (void**) data;
  siginfo_t *si = (siginfo_t *)siginfo_and_uctxt[0];
  ucontext_t *ucontext = (ucontext_t *)siginfo_and_uctxt[1];

  bufprintf_asyncsignalsafe(buf, "\nThe following signal was received:\n");
  prettyprint_siginfo(buf, si);

  bufprintf_asyncsignalsafe(buf, "- Architecture specific information:\n");
  prettyprint_ucontext(buf, ucontext);
 
}

static void prettyprint_siginfo_wrap(buffer_t *buf, const char *data, size_t datalen) {

  siginfo_t *si = (siginfo_t*) data;

  bufprintf_asyncsignalsafe(buf, "\nThe following signal was received:\n");
  prettyprint_siginfo(buf, si);

}

static void asyncsignalsafe_fatal_error_record(const char *file, int line, siginfo_t *si, void *ucontext) { 
  char error_record[ERROR_MAX_RECORD_SIZE];
  const void *siginfo_and_ucontext[2] = {si, ucontext};

  error_custom(error_record, 1, "ysignal", file, line, 1,
      (const char*)siginfo_and_ucontext, 0, prettyprint_siginfo_and_ucontext);

  print_error_asyncsignalsafe(error_record);
}

static void asyncsignalsafe_nonfatal_error_record(const char *file, int line, const char *fmt, ...) {
#if 0
  va_list args;
  char error_record[ERROR_MAX_RECORD_SIZE];

  va_start(args, fmt);
  yerror_vstr(error_record, YLOG_ERROR, 0, "ysignal", file, line, 1, fmt, args);
  va_end(args);

  print_error_asyncsignalsafe(error_record);
#endif  
}

static void fatal_error_record(const char *file, int line, siginfo_t *si) { 
  char error_record[ERROR_MAX_RECORD_SIZE];

  error_custom(error_record, 1, "ysignal", file, line, 0,
      (const char*)si, 0, prettyprint_siginfo_wrap);

  print_error_asyncsignalsafe(error_record);
}

static void fatal_signal_handler(int signum, siginfo_t *si, void *ucontext) {
  printf("fatal_signal_handler \n");
  /* Generate error record. If SIGABRT was raised via ysignal_abort,
   * the error record was already printed there */
  if (!(signum == SIGABRT && abort_expected))
    asyncsignalsafe_fatal_error_record(__FILE__, __LINE__, si, ucontext);

  //ysystem_stop_childs();

  /* Call system default handler (restored by SA_RESETHAND) which
   * will terminate the process. This call will never return. */
  raise(signum);
}

static void nonfatal_signal_handler(int signum, siginfo_t *si, void *ucontext) {
  int rc;
  int saved_errno = errno;

  if (selfpipe[1] == -1) {
    asyncsignalsafe_nonfatal_error_record(__FILE__, __LINE__, "Self-pipe is not initialized: signum=%d", signum);
    errno = saved_errno;
    return;
  }

  // only a write of less than PIPE_BUF is guaranteed to be atomic
  //STATIC_ASSERT(sizeof(*si) <= PIPE_BUF);

  rc = write(selfpipe[1], si, sizeof(*si));

  if (rc != sizeof(*si)) {
    asyncsignalsafe_nonfatal_error_record(__FILE__, __LINE__,
        "Failed to write to self-pipe: signum=%d errno=%d (%s)", signum, errno, strerror(errno));
  }

  errno = saved_errno;
}

static void install_fatal_signal_handler(int signo) {
  struct sigaction action;

  action.sa_sigaction = fatal_signal_handler;

  // should we use sigfillset to block all signals while we handle one?
  sigemptyset(&action.sa_mask);

  action.sa_flags  = SA_SIGINFO;
  action.sa_flags |= SA_RESETHAND; // reset default handler before executing ours

  sigaction(signo, &action, NULL);
}

static void install_nonfatal_signal_handler(int signo) {
  struct sigaction action;

  action.sa_sigaction = nonfatal_signal_handler;

  // should we use sigfillset to block all signals while we handle one?
  sigemptyset(&action.sa_mask);

  action.sa_flags  = SA_SIGINFO;

  sigaction(signo, &action, NULL);
}

static void install_all_fatal_signal_handlers(void) {
  unsigned int i;

  for (i = 0 ; i < DIM(fatal_signals) ; i++)
    install_fatal_signal_handler(fatal_signals[i]);
}

// this function is executed within the user thread (thread that registered the user callback)
// it is responsible for calling the user callback
static void signal_cb(evutil_socket_t fd, short events, void *ctxt) {
  signal_ctxt_t *sig_ctxt = (signal_ctxt_t*)ctxt;
  sig_ctxt->user_cb(&sig_ctxt->siginfo, sig_ctxt->user_ctxt);
  event_free(sig_ctxt->event);
  free(sig_ctxt);
}

// this function is executed within the init thread (thread that called ysignal_init)
// it sends one event to a user thread
static void send_signal_event(struct event_base *base, signal_cb_t user_cb, siginfo_t *siginfo, void *user_ctxt) {
  int rc;
  static struct timeval zero = {0, 0};
  signal_ctxt_t *ctxt = (signal_ctxt_t*) malloc(sizeof(signal_ctxt_t));
  ctxt->user_cb = user_cb;
  ctxt->event = evtimer_new(base, signal_cb, ctxt);
  ctxt->user_ctxt = user_ctxt;
  memcpy(&ctxt->siginfo, siginfo, sizeof(siginfo_t));
  rc = evtimer_add(ctxt->event, &zero);
  if (rc == -1) {
    printf("Failed to add signal event");
  }
}

// this function is executed within the init thread
// it reads one siginfo from the self-pipe
// then it loops over registered user callbacks and sends one event per callback
static void selfpipe_cb(evutil_socket_t fd, short events, void *ctx) {
  int rc;
  siginfo_t siginfo;
  user_info_t *user;

  rc = read(fd, &siginfo, sizeof(siginfo));
  if (rc != sizeof(siginfo)) {
    printf("Failed to read siginfo from self-pipe: errno=%d (%s)",
               errno, strerror(errno));
    return;
  }

  if (siginfo.si_signo < 0 || siginfo.si_signo >= NSIG) {
    printf("Invalid signal number %d received on self-pipe", siginfo.si_signo);
    return;
  }

  rc = zmutex_lock(&mtx);
  if (rc != 0)
      return;

  user = &users[siginfo.si_signo];

  if (user->cb != NULL)
    send_signal_event(user->evbase, user->cb, &siginfo, user->ctxt);

  zmutex_unlock(&mtx);
}

static int create_selfpipe(void) {
  int rc;
  struct event *selfpipe_event;

  // XXX O_DIRECT guarantees that we read exactly sizeof(siginfo_t) at a time, but requires Linux >= 3.4
  // O_NONBLOCK to avoid any possible deadlock if the pipe is full
  rc = pipe2(selfpipe, O_CLOEXEC | O_NONBLOCK);

  if (rc == -1) {
    printf("Failed to create self-pipe (errno=%d: %s)", errno, strerror(errno));
    return -1;
  }

  selfpipe_event = event_new(get_loop(), selfpipe[0], EV_READ | EV_PERSIST, selfpipe_cb, NULL);
  rc = event_add(selfpipe_event, NULL);

  if (rc == -1) {
    printf("Failed to add self-pipe event");
    return -1;
  }

  return 0;
}

static void delayed_exit() {
  const char *delay_ms_str;
  delay_ms_str = getenv("APP_DELAY_EXIT");
  if (delay_ms_str == NULL)
    return;

  int delay_ms = atoi(delay_ms_str);
  if (delay_ms > 0)
    usleep(delay_ms*1000);
}

static void do_nothing(siginfo_t *si, void *ctxt) { }

static void log_error_and_call_system_default_handler(siginfo_t *si, void *ctxt) {
  printf("log_error_and_call_system_default_handler \n");
  fatal_error_record(__FILE__, __LINE__, si);

  signal(si->si_signo, SIG_DFL);
  raise(si->si_signo);
}

static void install_default_nonfatal_signals_handlers(void)
{
  /* Avoid termination upon SIGPIPE (see changeset 1a939ebb335f) */
  zsignal_register(SIGPIPE, do_nothing, NULL);

  /* For non-fatal signals for which the system default action is to
   * terminate (see 'man 7 signal'), we install our own handlers to
   * make sure we print an error message before terminating. */
  zsignal_register(SIGHUP, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGTERM, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGINT, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGQUIT, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGTRAP, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGIO, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGALRM, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGUSR1, log_error_and_call_system_default_handler, NULL);
  zsignal_register(SIGUSR2, log_error_and_call_system_default_handler, NULL);
}

static void enable_floating_point_exceptions(void)
{
  /* See changesets 00e6d4b3be9f and 6b26b48fabf1 for background.
   * Enable floating point exceptions
   * On some (all?) systems (seen on CATAN), a divide-by-zero error was not
   * detected, unless floating point exceptions where explicitly enabled.
   */
  feenableexcept(FE_INVALID   |
                 FE_DIVBYZERO |
                 FE_OVERFLOW  |
                 FE_UNDERFLOW);
}

int zsignal_init(void) {
  /* Make sure we load the backtrace library already here to make it safe to call it from signal handler context. */
  /* see https://stackoverflow.com/questions/29982643/how-to-get-proper-backtrace-in-process-signal-handler-armv7-uclibc for a detailed description */
  void* dummy = NULL;
  backtrace(&dummy, 1);

  zmutex_create(&mtx);
  memset(users, 0, sizeof(users));

  if (create_selfpipe() == -1)
    return -1;

  install_all_fatal_signal_handlers();
  install_default_nonfatal_signals_handlers();

  enable_floating_point_exceptions();

  return 0;
}

int zsignal_register(int signo, signal_cb_t cb, void *ctxt) {
  user_info_t *user;

  if (is_fatal(signo)) {
    printf("User callbacks for fatal signals are not supported (%s)", strsignal(signo));
    return -1;
  }

  int ret = zmutex_lock(&mtx);
  if (ret != 0)
      return -1;

  user = &users[signo];
  if (user->cb != NULL) {
    zmutex_unlock(&mtx);
    printf("Cannot register callback for signal %d: already registered", signo);
    return -1;
  }

  user->cb = cb;
  user->ctxt = ctxt;
  user->evbase = get_loop();

  install_nonfatal_signal_handler(signo);

  zmutex_unlock(&mtx);

  return signo;
}

int zsignal_deregister(int signo) {
  int ret = zmutex_lock(&mtx);
  if (ret != 0)
      return -1;

  users[signo].evbase = NULL;
  users[signo].cb = NULL;
  users[signo].ctxt = NULL;

  zmutex_unlock(&mtx);

  return 0;
}

int zsignal_reset_default(int signo) {
  zsignal_deregister(signo);

  // restore default handler
  zsignal_register(signo, log_error_and_call_system_default_handler, NULL);

  return 0;
}

int zsignal_raise(int signo) {
  if (is_fatal(signo)) {
    printf("User is trying to raise a fatal signal (%s)", strsignal(signo));
    return -1;
  }

  return raise(signo);
}

void zsignal_abort(void) {
  abort_expected = 1;
  printf("Abort requested, process will terminate\n"); // print error record
  raise(SIGABRT);
  exit(-1); // never executed, this is only to comply with the __noreturn__ attribute of ysignal_abort()
}

/* Overwrite abort() to allow error record generation
 * when e.g. a third-party library calls abort(). */
void abort(void) {
  zsignal_abort();
}

}