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
#include "tmp/tmp.h"

int main(int argc, char* argv[])
{
    std::cout << "hello Jacob.." << std::endl;
    
    std::cout << "3 => 1" << std::endl;
    test_cross_map("3","1");
    std::cout << "1 => 3" << std::endl;
    test_cross_map("1","3");
    std::cout << "2 => 3" << std::endl;
    test_cross_map("2","3");

    std::cout << "1 => 4" << std::endl;
    test_cross_map("1","4");

    return 0;
}
