#pragma once

#include <pthread.h>

namespace ZLIB
{

#ifdef __cplusplus
extern "C" {
#endif

typedef struct thread_mutex {
    pthread_mutex_t mutex_id; 
} thread_mutex_t;

int zmutex_create(thread_mutex_t* mutex);
int zmutex_lock(thread_mutex_t* mutex);
int zmutex_trylock(thread_mutex_t* mutex);
int zmutex_timedlock(thread_mutex_t* mutex, unsigned long timeout_ms);
int zmutex_unlock(thread_mutex_t* mutex);
int zmutex_delete(thread_mutex_t* mutex);

#ifdef __cplusplus
}
#endif

}
