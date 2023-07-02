#pragma once

int test_edge(bool active);
bool test_cross_map(const std::string& from, const std::string& to);
void test_rapidjson();
void test_boost_optinal();

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;
class Hk_Shm {
 public:
 // these struct should be sync with hk_agent and PNC,
 // so make this common will be better, now just copy here
 static const int agv_buf_max = 15; //one shot max agv num
 static const int cache_buf_max = 15;//3s
 struct agv_helper_s{
    uint32_t tag1;
    uint32_t version;
    uint32_t tag2;
    uint32_t index;
 };
 struct agv_path_s {
    int32_t pos_x;
    int32_t pos_y;
 };
 struct trp_lock_s {
    int32_t form;
    int32_t width;
    int32_t length;
    int32_t dir;
    int32_t pos_x;
    int32_t pos_y;
 };
 //max path=15
 struct agv_msg_s {
    int32_t id;
    int32_t pos_x;
    int32_t pos_y;
    int32_t dir;
    int32_t path_num;
    trp_lock_s trp;
    agv_path_s path[15];
 };
 struct agv_info_s {
    int32_t agv_num;
    agv_msg_s agv_msg[agv_buf_max];
 };
 struct agv_wrapper_s {
    uint32_t timestamp;
    uint32_t crc32;
    agv_info_s agv_info;
 };
 //max agv_num=15
 struct agvs_s {
    agv_wrapper_s wrapper[cache_buf_max];
 };
 struct agv_buf_s{
    agv_helper_s helper;
    agvs_s agvs;
 };

 
 #define AGV_SYMBOL ("agv_symbol")
 public:
  Hk_Shm()
    : shm_obj_(open_or_create, AGV_SYMBOL, read_write), region_(shm_obj_, read_write, 0, sizeof(agv_buf_s)) {
    static const uint32_t s_tag1 = 0x5A5A5A5AU;
    static const uint32_t s_tag2 = 0xA5A5A5A5U;
    static const uint32_t s_version = 1;
    printf("agvsize: %d\n", sizeof(agv_buf_s));
    shm_obj_.truncate(sizeof(agv_buf_s));
    printf("b GetContext\n");
    auto& context = GetContext();
    printf("a GetContext\n");
    if ((context.helper.tag1 != s_tag1) || (context.helper.tag2 != s_tag2) || (context.helper.version != s_version)) {
      printf("B memset\n");
      memset(&context, 0, sizeof(agv_buf_s));
      printf("a memset\n");
      context.helper.tag1 = s_tag1;
      context.helper.tag2 = s_tag2;
      context.helper.version = s_version;
    }
  }

  // 放入负载
  //void Put(const agv_info_s& payload);

  // 获取负载
  // payload: 出参
  // time_range: 时间范围。表示可接受几秒前的数据
  // return: tag。tag小于0，表示没取到符合要求的数据；tag与上一次取值相同，代表当前数据与上一次相同
  int Get(agv_info_s& payload, uint32_t time_range = 1 /*sec*/);
  int Put(agv_info_s* agv_msg);
 private:
  uint32_t crc32(uint8_t *data, uint16_t length);
  agv_buf_s& GetContext() const { return *(agv_buf_s*)region_.get_address(); }

 private:
  shared_memory_object shm_obj_;
  mapped_region region_;
};
