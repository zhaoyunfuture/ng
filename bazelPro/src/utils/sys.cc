#include "sys.h"

#include <sys/types.h>
#include <error.h> //errno
#include <stdio.h> //popen
#include <stdlib.h> //str
#include <signal.h> //kill
#include <string.h> //strerror

#include <sys/vfs.h> //statfs
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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



using namespace std;

// 获取系统 CPU 使用率
double get_cpu_usage() {
    string ignore;
    double user, nice, sys, idle, iowait, irq, softirq, steal, guest, guest_nice;

    ifstream stat_file("/proc/stat");
    stat_file >> ignore >> user >> nice >> sys >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    double prev_active = user + nice + sys + irq + softirq + steal;
    double prev_total = user + nice + sys + idle + iowait + irq + softirq + steal + guest + guest_nice;

    // Wait for a short period to calculate the difference
    usleep(500000);

    stat_file.seekg(0);
    stat_file >> ignore >> user >> nice >> sys >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    double active = user + nice + sys + irq + softirq + steal;
    double total = user + nice + sys + idle + iowait + irq + softirq + steal + guest + guest_nice;

    return (active - prev_active) / (total - prev_total) * 100.0;
}

// 获取系统内存使用率
double get_memory_usage() {
    string ignore, unit;
    int total_memory, free_memory, available_memory, buffer, cache, swap, free_swap;

    ifstream mem_file("/proc/meminfo");

    mem_file >> ignore >> total_memory >> unit;
    mem_file >> ignore >> free_memory >> unit;
    mem_file >> ignore >> available_memory >> unit;
    mem_file >> ignore >> ignore >> buffer >> unit;
    mem_file >> ignore >> ignore >> cache >> unit;
    mem_file >> ignore >> ignore >> ignore >> ignore >> swap >> unit;
    mem_file >> ignore >> ignore >> ignore >> ignore >> free_swap >> unit;

    double used_memory = (total_memory - free_memory) - (buffer + cache);
    return used_memory / total_memory * 100.0;
}

// 获取系统磁盘空间使用率
double get_disk_usage() {
    string mount_point, ignore;
    long long total_blocks, free_blocks, available_blocks, block_size;

    ifstream stat_file("/proc/self/mounts");

    while (stat_file >> ignore >> mount_point >> ignore >> ignore >> ignore >> ignore) {
        if (mount_point == "/") {
            break;
        }
    }

    struct statfs fs;
    if (statfs(mount_point.c_str(), &fs) == -1) {
        return -1.0;
    }

    total_blocks = fs.f_blocks * (fs.f_bsize / 1024);
    free_blocks = fs.f_bfree * (fs.f_bsize / 1024);
    available_blocks = fs.f_bavail * (fs.f_bsize / 1024);

    std::cout << "total_blocks: " << total_blocks << std::endl;
    std::cout << "free_blocks: " << free_blocks << std::endl;
    std::cout << "available_blocks: " << available_blocks << std::endl;

    double used_blocks = total_blocks - free_blocks;
    return used_blocks / total_blocks * 100.0;
}


}//NAMESPACE