#pragma once

#include <stdint.h>
#include <stdlib.h>

struct HEADER_S {
  char agv_id[64];
  int32_t agv_id_len;         
  int32_t type;  
  char area_id[64];
  int32_t area_id_len;   
  int32_t px;     
  int32_t py;
};
