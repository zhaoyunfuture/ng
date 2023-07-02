#include <string.h>
#include <stdio.h>
#include <execinfo.h>
#include <stdarg.h>
#include <ctype.h> // isgraph

// #include <yAPI/ylog.h>
// #include <yAPI/ytime.h>
// #include <yAPI/yloop.h>
// #include <yAPI/ythread.h>
// #include <yAPI/ytime_asyncsignalsafe.h>

// #include "yerror.h"
// #include "bufprintf.h"
// #include "ythread_asyncsignalsafe.h"
#include "src/lib/zerror.h"
#include "src/lib/uthread.h"


#define MAX_CALLSTACK_DEPTH 30

/* The sequence number of error records is a process global (shared by all
 * threads). We should therefore protect it with a mutex. However this would
 * prevent us from accessing it from a signal handler (risk of deadlock).
 * Using sig_atomic_t would not guarantee that it is thread-safe. Anyway
 * this is just a harmless counter. */
static int seq_num = 0;

static void print_callstack(buffer_t *out_buf, int skip_count)
{
  int j, nptrs;
  void *buffer[MAX_CALLSTACK_DEPTH];

  nptrs = backtrace(buffer, MAX_CALLSTACK_DEPTH);

  bufprintf(out_buf, "\nCall stack :\n");

  for ( j = skip_count; j < nptrs; j++ ) {
    bufprintf(out_buf, "  ip[%02d]      : 0x%08lx\n", j, (unsigned long)buffer[j]);
  }

  bufprintf(out_buf, "\n");
}

/* Translate to ascii len bytes from byte_ptr and put result in result_buf */
static void translate_to_ascii(unsigned char *byte_ptr,
                               unsigned len,
                               char *result_buf)
{
  while (len) {
    if (isgraph(*byte_ptr))
      *result_buf++ = *byte_ptr;
    else
      *result_buf++ = '.';
    byte_ptr++; len--;
  }
}

static void hexdump_formatter(buffer_t *out_buf, const char *data, size_t datalen)
{
  unsigned int i;
  unsigned long end_addr, dump_addr;
  char ascii_buf[17]; // ascii translation of 16 bytes
  char padding[50];   // padding for the last line
  const char *byte_ptr;

  bufprintf_asyncsignalsafe(out_buf, "error info       :\n\t");
  ascii_buf[16] = 0;
  for (i = 0; i < sizeof(padding); i++)
    padding[i] = ' ';
  dump_addr = (unsigned long)data;
  end_addr = dump_addr + datalen;
  byte_ptr = data;
  i = 0;

  /* Loop over bytes */
  while (dump_addr < end_addr) {
    /* At the beginning of a line, translate 16 bytes to ascii */
    if (i == 0)
      translate_to_ascii ((unsigned char *) byte_ptr, 16, ascii_buf);

    /* Print current byte */
    bufprintf_asyncsignalsafe(out_buf, "%02x ", *byte_ptr++);
    i++;
    dump_addr++;

    /* At the end of a line, print ascii translation */
    if (i >= 16) {
      i = 0;
      bufprintf_asyncsignalsafe(out_buf, "  %s\n\t", ascii_buf);
    }
  }

  /* If we are in the middle of a line, print padding + remaining ascii translation */
  if (i > 0) {
    padding[49 - i * 3] = 0;
    ascii_buf[i] = 0;
    bufprintf_asyncsignalsafe(out_buf, "%s %s\n", padding, ascii_buf);
  }
}

static void string_formatter(buffer_t *out_buf, const char *data, size_t datalen)
{
  bufprintf_asyncsignalsafe(out_buf, "error info       : %s\n", data);
}

static int error_format_record(char output[ERROR_MAX_RECORD_SIZE], int level, unsigned long type,
                                const char *app, const char *file, int line,
                                int async_signal_safe, const char *data, size_t datalen,
                                error_data_formatter_t formatter)
{
  buffer_t out_buf;
  struct timespec time;
  char date_time_str[36];
  pthread_t thread_id;
  const char *thread_name;

  bufinit(&out_buf, output, ERROR_MAX_RECORD_SIZE);
#if 0
  /* Get date and time */
  ytime_get_wallclock_asyncsignalsafe(&time);
  if (async_signal_safe)
    ytime_time_date_str_asyncsignalsafe(date_time_str, sizeof(date_time_str), &time); // UTC
  else
    ytime_time_date_str(date_time_str, sizeof(date_time_str), &time); // local time
#endif
  /* Get thread id and name */
  thread_id = ZLIB::getSelf();
  thread_name = ZLIB::getSelfName_AsyncSignalSafe();

  /* Format generic fields */
  bufprintf_asyncsignalsafe(&out_buf, "************* ERROR RECORD *************\n");
  //bufprintf_asyncsignalsafe(&out_buf, "time             : %s\n", date_time_str);
  bufprintf_asyncsignalsafe(&out_buf, "seq-number       : %08x\n", seq_num++);
  //bufprintf_asyncsignalsafe(&out_buf, "application      : %s\n", yloop_get_application_name());
  bufprintf_asyncsignalsafe(&out_buf, "error thread     : %s-#%08lx\n", thread_name, thread_id);
  //bufprintf_asyncsignalsafe(&out_buf, "error level      : %s\n", ylog_level_to_str(level));
  if (type){
  bufprintf_asyncsignalsafe(&out_buf, "error type       : %lx\n", type);
  }
  bufprintf_asyncsignalsafe(&out_buf, "user name        : %s\n", app);
  //bufprintf_asyncsignalsafe(&out_buf, "build name       : %s\n", yloop_get_build_name());
  //bufprintf_asyncsignalsafe(&out_buf, "executable name  : %s\n", yloop_get_executable_name());
  bufprintf_asyncsignalsafe(&out_buf, "file name        : %s\n", file);
  bufprintf_asyncsignalsafe(&out_buf, "line number      : %d\n", line);

  /* Format error-specific data */
  formatter(&out_buf, data, datalen);

  /* Format call stack */
  // Note: getting the backtrace is not async-signal-safe (see man 7 signal)
  if (!async_signal_safe)
    print_callstack(&out_buf, 0);

  bufprintf_asyncsignalsafe(&out_buf, "****************** END *****************\n");

  return 0;
}
#if 0
int error_str(char output[ERROR_MAX_RECORD_SIZE], ylog_level_t level, unsigned long type,
               const char *app, const char *file, int line,
               int async_signal_safe, const char *format, ...)
{
  int error = 0;
  va_list arg;
  va_start(arg, format);
  error = error_vstr(output, level, type, app, file, line, async_signal_safe, format, arg);
  va_end(arg);
  return error;
}

int error_vstr(char output[ERROR_MAX_RECORD_SIZE], ylog_level_t level, unsigned long type,
                const char *app, const char *file, int line,
                int async_signal_safe, const char *format, va_list arg)
{
  int nc;
  char format_buffer[400];
  nc = vsnprintf(format_buffer, sizeof(format_buffer), format, arg);
  return error_format_record(output, level, type, app, file, line, async_signal_safe, format_buffer, nc, string_formatter);
}

int error_hexdump(char output[ERROR_MAX_RECORD_SIZE], ylog_level_t level,
                   const char *app, const char *file, int line,
                   int async_signal_safe, const char *data, size_t datalen)
{
  return error_format_record(output, level, 0, app, file, line, async_signal_safe, data, datalen, hexdump_formatter);
}
#endif
int error_custom(char output[ERROR_MAX_RECORD_SIZE], int level,
                  const char *app, const char *file, int line,
                  int async_signal_safe, const char *data, size_t datalen,
                  error_data_formatter_t formatter)
{
  return error_format_record(output, level, 0, app, file, line, async_signal_safe, data, datalen, formatter);
}
