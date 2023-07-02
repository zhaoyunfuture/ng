#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "src/lib/zmutex.h"

namespace ZLIB
{

int zmutex_create(thread_mutex_t* mutex) {
    int rc = 0;

    pthread_mutexattr_t attrib;
    rc = pthread_mutexattr_init(&attrib);
    if (rc != 0) {
        printf("pthread error mutexattr_init (%s)", strerror(errno));
        return rc;
    }

    rc = pthread_mutexattr_settype(&attrib, PTHREAD_MUTEX_RECURSIVE);
    if (rc != 0) {
        printf("pthread error mutexattr_settype (%s)", strerror(errno));
        goto _mutex_create_cleanup;
    }

    rc = pthread_mutexattr_setprotocol(&attrib, PTHREAD_PRIO_INHERIT);
    if (rc != 0) {
        printf("pthread error mutexattr_setprotocol (%s)", strerror(errno));
        goto _mutex_create_cleanup;
    }

    rc = pthread_mutex_init(&(mutex->mutex_id), &attrib);
    if (rc != 0) {
        printf("pthread error mutex_init (%s)", strerror(errno));
    }


_mutex_create_cleanup:
    (void)pthread_mutexattr_destroy(&attrib);
    return rc;
}

int zmutex_lock(thread_mutex_t* mutex) {
    return pthread_mutex_lock(&(mutex->mutex_id));
}

int zmutex_trylock(thread_mutex_t* mutex) {
    return pthread_mutex_trylock(&mutex->mutex_id);
}

int zmutex_timedlock(thread_mutex_t* mutex, unsigned long timeout_ms) {
    struct timespec abs_time;

    if (timeout_ms == 0) {
        return zmutex_lock(mutex);
    }

    if (clock_gettime(CLOCK_REALTIME, &abs_time) != 0) {
        return EINVAL;
    }

    abs_time.tv_sec  += (timeout_ms / 1000);
    abs_time.tv_nsec += (timeout_ms % 1000) * 1000000UL;

    if (abs_time.tv_nsec >= 1000000000) {
        abs_time.tv_nsec -= 1000000000;
        abs_time.tv_sec  += 1;
    }

    return pthread_mutex_timedlock(&(mutex->mutex_id), &abs_time);
}

int zmutex_unlock(thread_mutex_t* mutex) {
    int res = pthread_mutex_unlock(&(mutex->mutex_id));
    return res;
}

int zmutex_delete(thread_mutex_t* mutex) {
    return pthread_mutex_destroy(&(mutex->mutex_id));
}

}