#ifndef __VSNPRINTF_ASYNCSIGNALSAFE__
#define __VSNPRINTF_ASYNCSIGNALSAFE__

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

int vsnprintf_asyncsignalsafe(char *buf, size_t size, const char *format, va_list args);
int snprintf_asyncsignalsafe(char *buf, size_t size, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
