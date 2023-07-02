#include "src/lib/comm.h"
#include "src/lib/loop.h"
#include "src/lib/zsignal.h"

static __thread struct event_base* g_loop = nullptr;

int loop_main(int argc, char* argv[]) {
    int rc = Z_FAIL;
    struct event_base* base_loop;

    base_loop = loop_init(4);
    if(!base_loop) {
        printf("base loop create fail\n");
        return -1;
    }

    //systerm init
    ZLIB::zsignal_init();

    //app overwrite init
    rc = app_init(argc, argv);

    if(rc != Z_SUCCESS) {
        printf("app init fail: %d\n", rc);
        return rc;
    }

    //lanuch loop
    rc = process_loop(base_loop);

    if (rc == 0 || rc == 1) {
        printf("event loop exit: %d\n", rc);
    } else {
        printf("event loop fail: %d\n", rc);
    }

    return rc;
}

int process_loop(struct event_base* base_loop) {
    int rc = 0;

    do {
        int flags = EVLOOP_ONCE; //EVLOOP_NONBLOCK
        rc = event_base_loop(base_loop, flags);        
    } while (rc == 0);

    printf("event_base_loop rc: %d\n", rc);
    return rc;
}


struct event_base* loop_init(int max_prio) {
    //evthread_use_pthreads();

    struct event_base* base_loop = event_base_new();
    g_loop = base_loop;

    return base_loop;
}

struct event_base* get_loop() {
    return g_loop;
}

