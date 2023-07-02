#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>
#include <utility> //pair
#include <vector>

#include "src/tmp/tmp.h"

//for test_cross_map
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"

//for test zlib
#include "src/lib/uthread.h"
#include "src/lib/loopthread.h"
#include "src/lib/loop.h"

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

static bool active_saving_ = false;
static bool has_obstacle_ = true;
static bool not_auto_mode = false;
static bool emergency_stop = false;
static uint64_t last_active_save_falling_time_ = 0;

void test_rapidjson() {
  rapidjson::Document d;

  char text[128] = "{\"a\":\"b\",\"c\":{\"d\":\"e\"}}";
  if(!d.Parse(text).HasParseError() && d.IsObject()) {
    if(d.HasMember("a")) {
      printf("text, has member a, content: %s\n", d["a"].GetString());
    }
    if(d.HasMember("c")) {
      printf("text, has member c\n");
      if(d["c"].HasMember("d")) {
        printf("text, has member d, content: %s\n", d["c"]["d"].GetString());
      }
    }

  }
}
int test_edge(bool active){
/* *****************
  catch falling edge
    ___     ___
   |   |   |   |
___|   |___|   |___
       ^       ^
   *****************
*/

  //rising edge
  if(!active_saving_ && active) {
    printf("rising edge\n");
    active_saving_ = true;
    return 0;
  }

  //falling edge
  if(active_saving_ && !active) {
    printf("falling edge\n");
    active_saving_ = false;

    if(last_active_save_falling_time_ == 0) {
      last_active_save_falling_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      printf("last_active_save_falling_time_ : %u\n", last_active_save_falling_time_);
    } else {
      uint64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      printf("current_time : %u\n", current_time);
      if(current_time - last_active_save_falling_time_ > 3000) {
        //timeout, re-record falling-time
        printf("timeout\n");
        last_active_save_falling_time_ = current_time;
        return 1;
      } else {
        last_active_save_falling_time_ = 0;
        //catch two falling edge with-in timeout

        //step1 check obstacle and auto_mode
        printf("ActiveSavingCB, has_obstacle_: %d, not_auto_mode: %d, emergency_stop: %d \n",
                 has_obstacle_, not_auto_mode, emergency_stop);
        if(has_obstacle_ && !not_auto_mode) {
          //step2 check emergency_stop
          if(emergency_stop) {
            //tts
            printf("tts: emergency_stop\n");
            return 2;
          } else {
            //step3 send event to APP
            printf("send event to APP\n");
            not_auto_mode = true;
            return 3;
          }
        } else { 
          //reset
          printf("has_obstacle_ or not_auto_mode not meet, return\n");
          return 4;
        }
      }
    }
  }

  return 0;
}
std::pair<bool,std::string> load_point(const std::string& id, const std::string& neighbor) {
  std::string file = "./map_cross/" + id + "/cross.json";
  printf("file: %s\n", file.c_str());
  FILE* fp = fopen(file.c_str(), "r");
  if(fp == nullptr) {
    printf("cross.json open fail\n");
    return std::make_pair<bool,std::string>(false,"");
  }
 
  char readBuffer[1024];
  rapidjson::Document d;

  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  if(d.ParseStream(is).HasParseError()) {
    printf("cross parse fail,err:[%d]%s\n", d.GetParseError(), rapidjson::GetParseError_En(d.GetParseError()));
    return std::make_pair<bool,std::string>(false,"");
  }

  if(d.IsArray()) {
    printf("rule size: %d\n", d.Size());
    
    for(rapidjson::SizeType i = 0; i < d.Size(); i++) {
      std::string n = d[i]["neighbor"].GetString();
      
      if(n == neighbor) {
        return std::make_pair<bool,std::string>(true, d[i]["point"]["info"].GetString());
      }
    }
  }

  return std::make_pair<bool,std::string>(false,"");
}
bool test_cross_map(const std::string& from, const std::string& to) {
  printf("test_cross_map\n");

  FILE* fp = fopen("./map_cross/cross_rule.json", "r");
  if(fp == nullptr) {
    printf("cross_rule.json open fail\n");
    return false;
  }
 
  char readBuffer[1024];
  rapidjson::Document d;

  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  if(d.ParseStream(is).HasParseError()) {
    printf("cross_rule parse fail,err:[%d]%s\n", d.GetParseError(), rapidjson::GetParseError_En(d.GetParseError()));
    return false;
  }

  std::vector<std::string> result;

  if(d.IsArray()) {
    printf("cross_rule size: %d\n", d.Size());
    bool find_match = false;
    for(rapidjson::SizeType i = 0; i < d.Size(); i++) {
      int from_index = -1;
      int to_index = -1;
      const rapidjson::Value& r = d[i]["rule"];

      if(r.IsArray()) {
        printf("rule size: %d\n", r.Size());
        for(rapidjson::SizeType j = 0; j < r.Size(); j++) {
          std::string id = r[j].GetString();
          if(id == from) {
            from_index = j;
          }
          if(id == to) {
            to_index = j;
          }
        }

        if(from_index == -1 || to_index == -1) {
          printf("[%d] not match, from:%d, to:%d\n", i, from_index, to_index);
          continue;
        }
        find_match = true;
        printf("match, from:%d, to:%d\n", from_index, to_index);

        if(from_index < to_index) {
          for(rapidjson::SizeType k = from_index; k < to_index; k++) {
            auto p = load_point(r[k].GetString(), r[k+1].GetString());
            if(p.first == false)
              return false;
            result.push_back(p.second);
          }
        } else {
          for(rapidjson::SizeType k = from_index; k > to_index; k--) {
            auto p = load_point(r[k].GetString(), r[k-1].GetString());
            if(p.first == false)
              return false;
            result.push_back(p.second);
          }
        }
        
        break;
      }
    }
    if(!find_match) {
      printf("not match, from:%s, to:%s\n", from.c_str(), to.c_str());
      return false;
    }
  }
  printf("res size: %d\n", result.size());
  for(auto& itr:result) {
    printf("res: %s\n", itr.c_str());
  }

  return true;
}

#include <boost/optional.hpp>
void test_boost_optinal() {
  boost::optional<int> test;
  if(test) {
    printf("test non-null, %d\n", test.get());
  } else {
    printf("test non-null\n");
  }
  
  test.reset();

  if(test) {
    printf("test non-null, %d\n", test.get());
  } else {
    printf("test non-null\n");
  }

  test = 32;
  if(test) {
    printf("test non-null, %d\n", test.get());
  }

}

uint32_t Hk_Shm::crc32(uint8_t *data, uint16_t length) {
    uint8_t i;
    uint32_t crc = 0xffffffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;                // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320= reverse 0x04C11DB7
            else
                crc = (crc >> 1);
        }
    }
    return ~crc;
}

int Hk_Shm::Get(agv_info_s& payload, uint32_t time_range/*sec*/) {
    auto& context = GetContext();
    //index check
    int index = context.helper.index;
    if(index >= cache_buf_max) {
        printf("index error: %d\n", index);
        return -1;
    }
    printf("index reader: %d\n", index);
    //timeline check
    uint32_t read_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    printf("diff time: %ld, range accept(sec): %d\n", read_time - context.agvs.wrapper[index].timestamp, time_range);
    if(read_time - context.agvs.wrapper[index].timestamp > time_range) {
        printf("time out of data: %d(sec)\n", read_time - context.agvs.wrapper[index].timestamp);
        return -1;
    }

    //copy payload
    memcpy(&payload, (char*)&context.agvs.wrapper[index].agv_info, sizeof(agv_info_s));

    //crc check
    uint32_t crc = crc32((uint8_t*)&payload, sizeof(agv_info_s));
    printf("read crc: %d\n", crc);
    if(crc != context.agvs.wrapper[index].crc32) {
        int retry_index;
        if(index == 0){
            retry_index = cache_buf_max-1;
        } else {
            retry_index = index - 1;
        }
        printf("crc error, will try former one with retry_index: %d\n", retry_index);
        if(read_time - context.agvs.wrapper[retry_index].timestamp > time_range) {
            printf("time out of data: %d(mm)\n", read_time - context.agvs.wrapper[retry_index].timestamp);
            return -1;
        }
        memcpy(&payload, (char*)&context.agvs.wrapper[retry_index].agv_info, sizeof(agv_info_s));
        uint32_t crc_again = crc32((uint8_t*)&payload, sizeof(agv_info_s));
        if(crc_again != context.agvs.wrapper[retry_index].crc32) {
            printf("crc still error with index: %d\n", retry_index);
            return -1;
        }
    }
    
    return 0;
}

int Hk_Shm::Put(agv_info_s* agv_msg) {
    auto& context = GetContext();

    // crc32 calc with payload
    uint32_t crc = crc32((uint8_t*)agv_msg, sizeof(agv_info_s));
    printf("crc: %d\n", crc);
    // time now
    uint32_t time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    printf("time: %ld\n", time);

    int index = context.helper.index + 1;
    index %= cache_buf_max;
    printf("index: %d\n", index);

    context.agvs.wrapper[index].timestamp = time;
    context.agvs.wrapper[index].crc32 = crc;

    memcpy((char*)&context.agvs.wrapper[index].agv_info, agv_msg, sizeof(agv_info_s));
    context.helper.index = index;
    
    return 0;
}
