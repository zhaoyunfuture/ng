#include "src/lib/bufprintf.h"
#include "src/lib/snprintf_asyncsignalsafe.h"

typedef int (*string_formatter_t)(char*, size_t, const char*, va_list);

static int __bufprintf(buffer_t *buf, const char *format, va_list args, string_formatter_t formatter)
{
  unsigned int nc = formatter(buf->data, buf->available, format, args);

  // if output was truncated
  if (nc >= buf->available)
    nc = buf->available - 1; // number of characters actually printed to buf (excluding terminating null byte)

  buf->data += nc;
  buf->available -= nc;
  return nc;
}

int bufprintf(buffer_t *buf, const char *format, ...)
{
  int rc;
  va_list args;
  va_start(args, format);
  rc = __bufprintf(buf, format, args, vsnprintf);
  va_end(args);
  return rc;
}

int bufprintf_asyncsignalsafe(buffer_t *buf, const char *format, ...)
{
  int rc;
  va_list args;
  va_start(args, format);
  rc = __bufprintf(buf, format, args, vsnprintf_asyncsignalsafe);
  va_end(args);
  return rc;
}
