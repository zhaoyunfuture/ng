#pragma once

#ifndef DIM
#define DIM(x) (sizeof(x)/sizeof((x)[0]))
#endif

#define Z_SUCCESS (0)
#define Z_FAIL (-1)