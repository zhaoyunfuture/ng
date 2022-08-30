#include "sys.h"

#include <sys/types.h>
#include <error.h> //errno
#include <stdio.h> //popen
#include <stdlib.h> //str
#include <signal.h> //kill
#include <string.h> //strerror

namespace UTILS {

pid_t get_pid_with_name(const char* name) {
    char cmd[64] = {0};
    snprintf(cmd, sizeof(cmd), "pidof %s", name);
    FILE* fp = popen(cmd, "re");
    
    if(!fp) return -1;

    char line[15] = {0};
    fgets(line, 15, fp);

    if(pclose(fp) !=0 ) return -1;

    pid_t pid = strtoul(line, NULL, 10);
    return pid;
}

void terminate_with_pid(pid_t pid) {
    if(kill(pid, SIGKILL) != 0) printf("can't kill pid: %d : ", pid);
}

}//NAMESPACE