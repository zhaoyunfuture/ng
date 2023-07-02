#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>
#include <utility> //pair
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


//for test zlib
#include "src/lib/uthread.h"
#include "src/lib/loopthread.h"
#include "src/lib/loop.h"
#include "src/lib/comm.h"
#include "src/lib/zsignal.h"

static char ctx[16] = "my_ctx";


static void fifo_read(evutil_socket_t fd, short event, void *arg) {
	char buf[255];
	int len;
	struct event *ev = (struct event *)arg;


	//fprintf(stderr, "fifo_read called with fd: %d, event: %d, arg: %p\n",
	//    (int)fd, event, arg);

	len = read(fd, buf, sizeof(buf) - 1);

	if (len <= 0) {
		if (len == -1)
			perror("read");
		else if (len == 0)
			//fprintf(stderr, "Connection closed\n");
		//event_del(ev);
		//event_base_loopbreak(event_get_base(ev));
		return;
	}

	buf[len] = '\0';

	fprintf(stdout, "Read: %s\n", buf);
}

void* thread_cb(void* cookie) {
    while(1) {
        printf("thread_cb: %s\n", (char*)cookie);
        sleep(2);

        // printf("zsignal_raise\n");
        // ZLIB::zsignal_raise(2);
        // ZLIB::zsignal_abort();

        //trigger coredump
        //char * p = nullptr;
        //memcpy(p, "str", 3);
    }
    

    return nullptr;
}

void* thread_init_cb(void* cookie) {
    printf("thread_init_cb: %s\n", (char*)cookie);

    //add fd into loop in init_cb
    struct stat st;
    const char *fifo = "event.fifo";
    int socket;

    if (lstat(fifo, &st) == 0) {
        if ((st.st_mode & S_IFMT) == S_IFREG) {
            errno = EEXIST;
            perror("lstat");
            exit(1);
        }
    }

    unlink(fifo);
    if (mkfifo(fifo, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }

    socket = open(fifo, O_RDONLY | O_NONBLOCK, 0);

    if (socket == -1) {
        perror("open");
        exit(1);
    }

    struct event *example_ev = event_new(get_loop(), socket, EV_READ | EV_PERSIST, fifo_read, NULL);
    event_add(example_ev, NULL);

    return nullptr;
}

void test_loop_thread() {
    printf("test loop thread\n");
    ZLIB::loopThread *lt = new ZLIB::loopThread;
    lt->create("loop_me", thread_init_cb, (void*)ctx, 4);

    char t_name[16];
    lt->getName(t_name);

    int count = 30;
    while(count) {
        count--;
        printf("loop thread info>> name:%s, id:%08x, run:%d,\n", t_name, lt->getId(), lt->isRunning());
        sleep(1);
    }
    printf("exit loop thread\n");
    lt->exit();

    printf("after exit info>> name:%s, id:%08x, run:%d,\n", t_name, lt->getId(), lt->isRunning());
    delete lt;

}
void test_thread() {
    printf("test thread\n");

    ZLIB::UThread *t = new ZLIB::UThread;
    t->create("me", thread_cb, (void*)ctx);

    char t_name[16];
    t->getName(t_name);

    int count = 30;
    while(count) {
        count--;
        printf("thread info>> name:%s, id:%08x, run:%d,\n", t_name, t->getId(), t->isRunning());
        sleep(1);
    }
    printf("exit thread\n");
    t->exit();

    printf("after exit>> name:%s, id:%08x, run:%d,\n", t_name, t->getId(), t->isRunning());

    delete t;
}


int app_init(int argc, char* argv[]) {
    printf("test loop thread\n");

    ZLIB::UThread *t = new ZLIB::UThread;
    t->create("me", thread_cb, (void*)ctx);

    ZLIB::loopThread *lt = new ZLIB::loopThread;
    lt->create("loop_me", thread_init_cb, (void*)ctx, 4);

    
    return Z_SUCCESS;
}
