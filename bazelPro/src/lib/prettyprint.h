#ifndef __PRETTYPRINT_H__
#define __PRETTYPRINT_H__

#include <signal.h>

#include "bufprintf.h"

void prettyprint_siginfo(buffer_t *buf, const siginfo_t *si);
void prettyprint_ucontext(buffer_t *buf, ucontext_t *ucontext);

#endif
