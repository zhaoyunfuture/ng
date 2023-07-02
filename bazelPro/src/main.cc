#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>
#include <map>
#include <vector>
#include <map>
#include <streambuf>
#include <cstdio>
#include <thread>
#include <pthread.h>

#include <sys/syscall.h>
#include "utils/sys.h"
#include <iomanip>
// #include "tmp/tmp.h"
// #include "libtest/libtest.h"


#define gettid() syscall(SYS_gettid)


std::string threadID() {
    pthread_t tid = pthread_self();
    std::cout << "id:" << tid << std::endl;
    std::cout << "gettid:" << gettid() << std::endl;
    return std::to_string(tid);
}

int main(int argc, char *argv[])
{
    std::cout << "jacob main \n";


    std::cout << "CPU Usage: " << UTILS::get_cpu_usage() << "%" << std::endl;
    std::cout << "Memory Usage: " << UTILS::get_memory_usage() << "%" << std::endl;
    std::cout << "Disk Usage: " << UTILS::get_disk_usage() << "%" << std::endl;

    std::cout << threadID() << std::endl;

    int a = 1, b = 10, c = 100;
    std::cout << "a: " << std::setw(2) << std::setfill('0') << a << std::endl;
    std::cout << "b: " << std::setw(2) << std::setfill('0') << b << std::endl;
    std::cout << "c: " << std::setw(2) << std::setfill('0') << c << " " << "tttt" << std::endl;


    std::string str("this is for test resize");
    str.resize(str.size() - 3);
    std::cout << str << std::endl;

    return 0;
}
