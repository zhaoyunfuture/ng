#ifndef _MSG_GEN_
#define _MSG_GEN_

#include <string.h>
#include "src/hk/msg_gen.h"

static inline int bswap_32(int le) {
	return (le & 0xff) << 24 
			| (le & 0xff00) << 8 
			| (le & 0xff0000) >> 8 
			| (le >> 24) & 0xff;
}

static inline uint16_t bswap_16(uint16_t x)
{
	return (x >> 8) | (x << 8); 
}

//generate msg to robot 
int gen_register_resp(char** buffer);
int gen_door_info(char** buffer);
int gen_robot_status_resp(char** buffer);
int gen_apply_clean_area_resp(char** buffer,int32_t area_id);
int gen_trp_ctrl_req(char** buffer);
int gen_apply_for_door_resp(char** buffer, int n_door_id,int n_x, int n_y, int n_order  );
int gen_apply_traffic_res_resp(char** buffer, int n_order, int n_area_id);
int gen_apply_offine_resp(char** buffer, int n_order);
int gen_apply_plan_resp(char** buffer, int n_order);
int gen_interrupt_ctrl_req(char** buffer);
int gen_path_plan_req(char** buffer);
#endif
