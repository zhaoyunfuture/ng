#include <string.h>
#include <string>
#include <unistd.h>
#include <chrono>

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