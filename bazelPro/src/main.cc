#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>
#include <map>
#include <vector>

#include <streambuf>
#include <cstdio>
#include <thread>

#include "utils/sys.h"


int main(int argc, char* argv[])
{
    std::cout << "hello Jacob.." << std::endl;
    
    std::cout << UTILS::get_pid_with_name("udp_srv") << std::endl;

    UTILS::terminate_with_pid(UTILS::get_pid_with_name("udp_srv"));
    

    return 0;
}
