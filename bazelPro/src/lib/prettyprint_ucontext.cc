#include "src/lib/prettyprint.h"
#include "src/lib/bufprintf.h"

#include <execinfo.h> // backtrace

#define MAX_CALLSTACK_DEPTH 30

void prettyprint_ucontext(buffer_t *buf, ucontext_t *ucontext)
{
    void *buffer[MAX_CALLSTACK_DEPTH];
    int j, nptrs;

    nptrs = backtrace(buffer, MAX_CALLSTACK_DEPTH);
    for ( j = 0; j < nptrs; j++ ) {
        bufprintf_asyncsignalsafe(buf, "  ip[%02d]      : 0x%08lx\n", j, (unsigned long)buffer[j]);
    }
}
