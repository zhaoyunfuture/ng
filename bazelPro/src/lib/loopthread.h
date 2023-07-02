#pragma once

#include "src/lib/uthread.h"
#include "src/lib/loop.h"

namespace ZLIB
{
class loopThread : public UThread
{
    public:
        loopThread():base_loop(NULL),init_cb(NULL),user_ctx(NULL),base_max_prio(1) {}
        virtual ~loopThread() {}

        virtual int create(const char name[16], void*(*init_cb)(void*), void* arg, int max_prio);

    protected:
        virtual void run();
        virtual void onInit();
        virtual void onExit();
    private:
        struct event_base *base_loop;
        void*(*init_cb)(void*);
        void* user_ctx;
        int base_max_prio;
};
}

