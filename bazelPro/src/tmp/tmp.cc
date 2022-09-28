#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>
#include <utility> //pair
#include <vector>

//for test_cross_map
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"

static bool active_saving_ = false;
static bool has_obstacle_ = true;
static bool not_auto_mode = false;
static bool emergency_stop = false;
static uint64_t last_active_save_falling_time_ = 0;

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