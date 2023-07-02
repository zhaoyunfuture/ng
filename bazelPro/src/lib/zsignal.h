#pragma once

#include <signal.h>

namespace ZLIB
{

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*signal_cb_t)(siginfo_t *siginfo, void *ctxt);

int zsignal_init(void);
int zsignal_register(int signo, signal_cb_t cb, void *ctxt);
int zsignal_deregister(int handle);
int zsignal_reset_default(int signo);
int zsignal_raise(int signo);
void zsignal_abort(void) __attribute__ ((__noreturn__));

#ifdef __cplusplus
}
#endif

}
