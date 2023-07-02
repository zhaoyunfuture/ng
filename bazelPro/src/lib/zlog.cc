#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>


#include "src/lib/zlog.h"


namespace ZLIB
{

void print_error_asyncsignalsafe(const char* str) {
    int len = strlen(str);

    write(STDERR_FILENO, "\n", 1);
    write(STDERR_FILENO, str, len);

    return;
}


}