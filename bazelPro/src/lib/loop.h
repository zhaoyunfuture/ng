#pragma once

#include <event2/event.h>


#ifdef __cplusplus
extern "C" {
#endif

int loop_main(int argc, char* argv[]);
int app_init(int argc, char* argv[]) __attribute__((weak));
struct event_base* loop_init(int max_prio);

int process_loop(struct event_base* base_loop);
struct event_base* get_loop();

#ifdef __cplusplus
}
#endif
