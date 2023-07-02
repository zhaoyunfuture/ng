#include "src/lib/loopthread.h"


namespace ZLIB
{

void loopThread::run() {
    printf("loop thread run\n");
    (void)process_loop(base_loop);
}

void loopThread::onInit() {
    printf("loop thread init\n");
    this->base_loop = loop_init(this->base_max_prio);

    //ywatchdog_start();

    this->init_cb(this->user_ctx);
}

void loopThread::onExit() {
    printf("loop thread exit\n");
    //ywatchdog_stop();
}

int loopThread::create(const char thread_name[16], void*(*init_cb)(void*), void* arg, int max_prio ) {
    this->init_cb = init_cb;
    this->base_max_prio = max_prio;
    this->user_ctx = arg;

    return UThread::create(thread_name, NULL, NULL);
}

}
