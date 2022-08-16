#pragma once

#include <fstream>
#include <string.h>
#include <stdlib.h>

// useage: 
#if 0
    int len = sizeof(HEADER_S);
    unsigned char buffer[len];
    HEADER_S* phead = (HEADER_S*)&buffer;
    memset(buffer, 0, len);
    std::string id = "agv_id";
    phead->agv_id_len = id.length();
    std::cout << "len:" << id.length() << std::endl;
    memcpy(phead->agv_id, id.c_str() , id.length());
    hexdump(buffer, len);
#endif

int hexdump(void *addr, size_t len);

