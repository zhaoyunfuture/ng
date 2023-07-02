#include "src/lib/prettyprint.h"
#include "src/lib/bufprintf.h"

#include <string.h>

static char *strcode(const int signo, const int code)
{
    switch (code) {
    case SI_USER: return "kill or raise"; break;
    case SI_KERNEL: return "kernel"; break;
    case SI_QUEUE: return "sigqueue"; break;
    case SI_TIMER: return "POSIX timer expired"; break;
    case SI_MESGQ: return "POSIX message queue state changed"; break;
    case SI_ASYNCIO: return "AIO completed"; break;
    case SI_TKILL: return "tkill or tgkill"; break;
    default: break;
    }

    switch (signo) {
    case SIGILL:
        switch (code) {
        case ILL_ILLOPC: return "illegal opcode"; break;
        case ILL_ILLOPN: return "illegal operand"; break;
        case ILL_ILLADR: return "illegal addressing mode"; break;
        case ILL_ILLTRP: return "illegal trap"; break;
        case ILL_PRVOPC: return "privileged opcode"; break;
        case ILL_PRVREG: return "privileged register"; break;
        case ILL_COPROC: return "coprocessor error"; break;
        case ILL_BADSTK: return "internal stack error"; break;
        default: break;
        }

        break;

    case SIGFPE:
        switch (code) {
        case FPE_INTDIV: return "integer divide by zero"; break;
        case FPE_INTOVF: return "integer overflow"; break;
        case FPE_FLTDIV: return "floating-point divide by zero"; break;
        case FPE_FLTOVF: return "floating-point overflow"; break;
        case FPE_FLTUND: return "floating-point underflow"; break;
        case FPE_FLTRES: return "floating-point inexact result"; break;
        case FPE_FLTINV: return "floating-point invalid operation"; break;
        case FPE_FLTSUB: return "subscript out of range"; break;
        default: break;
        }

        break;

    case SIGSEGV:
        switch (code) {
        case SEGV_MAPERR: return "address not mapped to object"; break;
        case SEGV_ACCERR: return "invalid permissions for mapped object"; break;
        default: break;
        }

        break;

    case SIGBUS:
        switch (code) {
        case BUS_ADRALN: return "invalid address alignment"; break;
        case BUS_ADRERR: return "nonexistent physical address"; break;
        case BUS_OBJERR: return "object-specific hardware error"; break;
        default: break;
        }

        break;

    case SIGTRAP:
        switch (code) {
        case TRAP_BRKPT: return "process breakpoint"; break;
        case TRAP_TRACE: return "process trace trap"; break;
        default: break;
        }

        break;

    case SIGCHLD:
        switch (code) {
        case CLD_EXITED: return "child has exited"; break;
        case CLD_KILLED: return "child was killed"; break;
        case CLD_DUMPED: return "child terminated abnormally"; break;
        case CLD_TRAPPED: return "traced child has trapped"; break;
        case CLD_STOPPED: return "child has stopped"; break;
        case CLD_CONTINUED: return "stopped child has continued"; break;
        default: break;
        }

        break;

    case SIGIO: /* == SIGPOLL */
        switch (code) {
        case POLL_IN: return "data input available"; break;
        case POLL_OUT: return "output buffers available"; break;
        case POLL_MSG: return "input message available"; break;
        case POLL_ERR: return "I/O error"; break;
        case POLL_PRI: return "high priority input available"; break;
        case POLL_HUP: return "device disconnected"; break;
        default: break;
        }

        break;

    default:
        break;
    }

    /* any unhandled case */
    return "";
}

void prettyprint_siginfo(buffer_t *buf, const siginfo_t *si)
{
  /* See 'man sigaction' for details */
  bufprintf_asyncsignalsafe(buf, "- signo = %d (%s)\n", si->si_signo, strsignal(si->si_signo));
  bufprintf_asyncsignalsafe(buf, "- errno = %d (%s)\n", si->si_errno, strerror(si->si_errno));
  bufprintf_asyncsignalsafe(buf, "- code = %d (%s)\n", si->si_code, strcode(si->si_signo, si->si_code));

  switch (si->si_signo) {
    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
    case SIGTRAP:
      if (si->si_code != SI_USER) {
        bufprintf_asyncsignalsafe(buf, "- addr = 0x%lx\n", (unsigned long)si->si_addr);
      }
      break;

    case SIGCHLD:
      bufprintf_asyncsignalsafe(buf, "- status = %d\n", si->si_status);
      bufprintf_asyncsignalsafe(buf, "- utime = %lu\n", (unsigned long)si->si_utime);
      bufprintf_asyncsignalsafe(buf, "- stime = %lu\n", (unsigned long)si->si_stime);
      break;

    case SIGIO: /* == SIGPOLL */
      bufprintf_asyncsignalsafe(buf, "- fd = %d\n", si->si_fd);
      break;

    default:
      break;
  }

  if (si->si_signo == SIGCHLD || si->si_code == SI_QUEUE || si->si_code == SI_USER) {
    bufprintf_asyncsignalsafe(buf, "- pid = %lu\n", (unsigned long)si->si_pid);
    bufprintf_asyncsignalsafe(buf, "- uid = %lu\n", (unsigned long)si->si_uid);
  }

  if (si->si_code == SI_QUEUE) {
    bufprintf_asyncsignalsafe(buf, "- int = %d\n", si->si_int);
    bufprintf_asyncsignalsafe(buf, "- ptr = 0x%lx\n", (unsigned long)si->si_ptr);
  }
}
