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

#include "utils/dump.h"
#include "jw/msg_header.h"
#include "tmp/tmp.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h" //for FileReadStream

int main(int argc, char* argv[])
{
    std::cout << "hello Jacob.." << std::endl;
    using namespace rapidjson;
 

    FILE* fp = fopen("./file/jw.json", "rb"); 
 
    char readBuffer[1024];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    
    Document d;
    d.ParseStream(is);
    printf("isArray: %d\n", d.IsArray());
    if(d.IsArray()) {
        printf("isArray size: %d\n", d.Size());
        for (SizeType i = 0; i < d.Size(); i++) {
            const Value& v = d[i];
            printf("pose[%d] = %s\n", i, v["pose"].GetString());
        }
        
    }
    
    fclose(fp);
    return 0;
}
