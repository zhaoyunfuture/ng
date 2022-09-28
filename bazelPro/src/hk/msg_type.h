/*
 *Copyright (c) 2022 Copyright Holder All Rights Reserved.
 */

/**
 * @file msg_type.h
 * @author zhaoyun<zhaoyun@gs-robot.com>
 * @date 2022-02-11
 */

#ifndef _MSG_TYPE_
#define _MSG_TYPE_
#include <stdint.h>
namespace HK_AGENT {

#define HK_SUCC 200
#define HK_FAIL 201

#define DEVICE_TYPE 129
#define DEVICE_LEN 860
#define DEVICE_WIDTH 700
#define DEVICE_HEIGHT 1030
#define DEVICE_DIMETER 1100

enum E_GS_MSG_TYPE {
  GS_MSG_TYPE_REGISTER_REQ = 1,
  GS_MSG_TYPE_REGISTER_RSP,
  GS_MSG_TYPE_APPLY_CLEAN_AREA_REQ,
  GS_MSG_TYPE_APPLY_CLEAN_AREA_RSP,
  GS_MSG_TYPE_ROBOT_STATUS_REQ,
  GS_MSG_TYPE_ROBOT_STATUS_RSP,
  GS_MSG_TYPE_TRP_CTRL_REQ,
  GS_MSG_TYPE_TRP_CTRL_RSP,
  GS_MSG_TYPE_APPLY_FOR_DOOR_REQ,
  GS_MSG_TYPE_APPLY_FOR_DOOR_RSP,
  GS_MSG_TYPE_APPLY_TRAFFIC_RES_REQ,
  GS_MSG_TYPE_APPLY_TRAFFIC_RES_RSP,
  GS_MSG_TYPE_DOOR_CONFIG_REQ,
  GS_MSG_TYPE_DOOR_CONFIG_RSP,
  GS_MSG_TYPE_APPLY_OFFLINE_REQ,
  GS_MSG_TYPE_APPLY_OFFLINE_RSP,

  //below introduced by HK phase 3
  GS_MSG_TYPE_PATH_UPLOAD_REQ, //NOT USED
  GS_MSG_TYPE_PATH_UPLOAD_RSP, //NOT USED
  GS_MSG_TYPE_INTERRUPT_CTRL_REQ,
  GS_MSG_TYPE_INTERRUPT_CTRL_RSP,
  GS_MSG_TYPE_APPLY_PLAN_REQ,
  GS_MSG_TYPE_APPLY_PLAN_RSP,
  GS_MSG_TYPE_PATH_PLAN_REQ,
  GS_MSG_TYPE_PATH_PLAN_RSP,

  GS_MSG_TYPE_UNKNOWN
};

enum E_TASK_STATUS {
  TASK_STATUS_IDLE = 0,
  TASK_STATUS_CLEANNING,
  TASK_STATUS_MOVING,
  TASK_STATUS_CHARGING,
  TASK_STATUS_OBSTACLE,
  TASK_STATUS_CRASHING,
  TASK_STATUS_CTRLING
};

// header total_len:16
struct HK_HEADER_S {
  char DBHK[4];         //4
  int32_t msg_order;    //4
  uint16_t msg_type;    //2
  uint16_t msg_length;  //2
  char msg_version;     //1
  char msg_crypt;       //1
  char remain[2];       //2
};
//AMS header
struct HK_AMS_HEADER_S {
  char HKPS[4];         //4
  uint16_t msg_length;  //2
  uint16_t msg_type;    //2
  int32_t msg_order;    //4
  char msg_version;     //1
  char msg_crypt;       //1
  char remain[2];       //2
};
//AMS ALARM
struct HK_AMS_ALARM_REQ_S {
  uint16_t robot_id;         //2
  char res[2];               //2
  int32_t alarm_type;        //4
  int32_t alarm_process;     //4
  char alarm_status;         //1
  char alarm_grade;          //1
  char res1[2];              //2
  int32_t alarm_code;        //4
  int32_t alarm_x;           //4
  int32_t alarm_y;           //4
  int32_t alarm_res[4];      //16
  int32_t alarm_code_x;      //4
  int32_t alarm_code_y;      //4
};
struct HK_AMS_ALARM_RSP_S {
  int32_t result;            //4
  char res[4];               //4
};
// DOOR_CONFIG
struct DOOR_INFO_S {
  int32_t door_id;  //4
  int32_t pos_x;    //4
  int32_t pos_y;    //4
};
struct DOOR_INFO_REQ_S {
  uint16_t agv_num;
  int16_t door_num;
  DOOR_INFO_S door_info[0];
};
struct INTERRUPT_CMD_S {
  uint16_t agv_num;
  uint16_t cmd_type;
};
struct PATH_PLAN_REQ_S {
  uint16_t agv_num;
  uint16_t move_type;
  int32_t px;
  int32_t py;
  uint16_t dir;
  uint16_t task_type;
  int32_t dest_px;
  int32_t dest_py;
  int32_t avoid_robot;
};
struct COMMON_RESP_S {
  int32_t success;
  uint16_t agv_num;
  char remain[2];
};
struct PLAN_REQ_S {
  uint16_t agv_num;
  uint16_t reserved;
  int32_t id;
  int32_t px;
  int32_t py;
  uint16_t task_type;
  uint16_t ctrl_type;
};
//robot status info
struct MOVE_POS_PARAM_S {
  int32_t pos_x;      //4,
  int32_t pos_y;      //4,
  int32_t direction;  //4，
  char chqr_code[2];  //2，
  uint16_t ver;       //2，AGV speed(mm/s)
};
struct POINT_S {
  int32_t pos_x;  // x
  int32_t pos_y;  // y
};
struct PATH_INFO_S {
  int16_t valid;
  int16_t path_num;
  POINT_S path[0];
};
struct ROBOT_STATUS_INFO_S {
  uint16_t robot_id;                //2, AGV ID
  uint16_t task_status;             //2，0:IDLE 1:CLEANNING 2:MOVING 3:CHARGING 4:OBSTACLE 5:CRASHING 6:CTRLING
  uint8_t battery_level;            //1，
  uint8_t signal_level;             //1，
  uint8_t need_plan;                //1, 0:no-need RCS 1:need RCS control
  uint8_t task_schdule;             //1, task process
  MOVE_POS_PARAM_S move_pos_param;  //16, AGV runtime info
  PATH_INFO_S path_info;            //introduced from phase3
};
struct ROBOT_STATUS_INFO_RESP_S {
  int32_t status_info_res;  //4, 200 true, 201 false
  uint16_t agv_num;         //2, agv id
  char remain[2];
};

//register
struct REGISTER_REQ_S {
  uint16_t agv_num;          //2, agv ID:6001
  uint16_t device_type;      //2, 0x81 (129)
  uint16_t device_length;    //2, 860mm
  uint16_t device_width;     //2, 700mm
  uint16_t device_height;    //2, 1030mm
  uint16_t device_diameter;  //2, 1100mm
  //uint32_t port;                   //4, 7788，not shown in guide  //todo remove
};

//clean area
typedef enum { APPLY_AREA_IN_USE = 1, APPLY_AREA_FREE } E_APPLY_AREA;


struct RECTANGLE_S {
  POINT_S lu;  //left up x,y
  POINT_S rd;  //right down x,y
};
struct CLEAN_AREA_S {
  uint16_t agv_num;
  int16_t apply_type;
  int32_t area_id;  //generate by robot
  RECTANGLE_S rect;
};
struct APPLY_CLEAN_RESP_S {
  int16_t apply_res_ret;  //2, 200 true, 201 false
  uint16_t agv_num;       //2, agv id
  int32_t area_id;        //4, area id
};

//TRP_CTRL
struct TRP_CTRL_RESP_S {
  int32_t reg_res;   //4, 200 true, 201 false
  uint16_t agv_num;  //2, agv id
  char remain[2];
};
struct TRP_CTRL_REQ_S {
  uint16_t robot_id;
  int16_t action_type;
  POINT_S point;
  int16_t dir;
  int16_t max_vel;
  int32_t avoid_robot;
};

//APPLY_FOR_DOOR
enum E_DOOR_TYPE { DOOR_TYPE_OPEN = 1, DOOR_TYPE_CLOSE };
enum E_DOOR_ACTION_TYPE { DOOR_ACTION_TYPE_OPEN = 1, DOOR_ACTION_TYPE_CLOSE };
struct APPLY_DOOR_REQ_S {
  uint16_t robot_id;
  int16_t action_type;
  int32_t door_id;
  POINT_S point;
};
struct APPLY_DOOR_RESP_S {
  uint16_t robot_id;
  int16_t apply_ret;
  int32_t door_id;
  POINT_S point;
  int32_t door_type;
};

//APPLY_TRAFFIC_RES
enum E_APPLY_TRAFFIC_RES_TYPE { APPLY_TRAFFIC_RES_USE = 1, APPLY_TRAFFIC_RES_FREE };
struct APPLY_TRAFFIC_RES_REQ_S {
  uint16_t robot_id;
  int16_t apply_res_type;
  int32_t area_id;
  RECTANGLE_S rect;
};

struct AGV_INFO_S {
  int32_t agv_id;
  POINT_S agv_pos;
  int32_t agv_block_point;
  POINT_S points[0];
};
struct APPLY_TRAFFIC_RES_RESP_S {
  int16_t apply_res_ret;  //2, 200 true, 201 false
  uint16_t agv_num;       //2, agv id
  int32_t area_id;        //4, area id
  int32_t error_code;     //4, sucess:0, fail: agv num
  AGV_INFO_S agv_info[0];
};

struct APPLY_OFFLINE_REQ_S {
  uint16_t robot_id;
  int16_t remain;
};

struct APPLY_OFFLINE_RESP_S {
  int32_t offline_res;  //4, 200 true, 201 false
  uint16_t agv_num;     //2, agv id
  char remain[2];
};

}  //namespace HK_AGENT
#endif
