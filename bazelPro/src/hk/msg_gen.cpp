
#include "src/hk/msg_type.h"
#include "src/hk/msg_gen.h"

using namespace HK_AGENT;
static int flip = 0;


int gen_path_plan_req(char** buffer) {
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	PATH_PLAN_REQ_S ctrl;	
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_PATH_PLAN_REQ;
	header.msg_length = sizeof(HK_HEADER_S) + sizeof(PATH_PLAN_REQ_S);
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	temp32 = bswap_32(header.msg_order);
	memcpy(&send_buffer[msg_len],&temp32,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;
	

	ctrl.agv_num = bswap_16(6001);
	ctrl.move_type = bswap_16(2);
	ctrl.px = bswap_32(11111);
	ctrl.py = bswap_32(22222);
	ctrl.dir = bswap_16(33);
	ctrl.task_type = bswap_16(1);
	ctrl.dest_px = bswap_32(33333);
	ctrl.dest_py = bswap_32(44444);
	ctrl.avoid_robot = bswap_32(1201);

	memcpy(&send_buffer[msg_len],&ctrl.agv_num,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.move_type,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.px,4);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&ctrl.py,4);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&ctrl.dir,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.task_type,2);
	msg_len += 2;

	memcpy(&send_buffer[msg_len],&ctrl.dest_px,4);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&ctrl.dest_py,4);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&ctrl.avoid_robot,4);
	msg_len += 4;

	return msg_len;
}

int gen_apply_plan_resp(char** buffer, int n_order){
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_APPLY_PLAN_RSP;
	header.msg_length = 16+sizeof(COMMON_RESP_S);
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	memcpy(&send_buffer[msg_len],&n_order,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;

	int32_t apply_ret = 200;
	apply_ret = bswap_32(apply_ret);
	memcpy(&send_buffer[msg_len],&apply_ret,4);
	msg_len += 4;

	uint16_t robot_id = 6001;
	uint16_t remain = 0;
	robot_id = bswap_16(robot_id);
	memcpy(&send_buffer[msg_len],&robot_id,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&remain,2);
	msg_len += 2;

	return msg_len;
}

int gen_apply_offine_resp(char** buffer, int n_order){
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_APPLY_OFFLINE_RSP;
	header.msg_length = 16+sizeof(APPLY_OFFLINE_RESP_S);
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	memcpy(&send_buffer[msg_len],&n_order,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;

	int32_t apply_ret = 200;
	apply_ret = bswap_32(apply_ret);
	memcpy(&send_buffer[msg_len],&apply_ret,4);
	msg_len += 4;

	uint16_t robot_id = 6001;
	uint16_t remain = 0;
	robot_id = bswap_16(robot_id);
	memcpy(&send_buffer[msg_len],&robot_id,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&remain,2);
	msg_len += 2;

	return msg_len;
}

int gen_apply_traffic_res_resp(char** buffer, int n_order, int n_area_id){
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_APPLY_TRAFFIC_RES_RSP;
	header.msg_length = 16+sizeof(APPLY_TRAFFIC_RES_RESP_S);
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	memcpy(&send_buffer[msg_len],&n_order,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;

	int16_t apply_ret = 200;
	apply_ret = bswap_16(apply_ret);
	memcpy(&send_buffer[msg_len],&apply_ret,2);
	msg_len += 2;

	uint16_t robot_id = 6001;
	robot_id = bswap_16(robot_id);
	memcpy(&send_buffer[msg_len],&robot_id,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&n_area_id,4);
	msg_len += 4;
	int32_t error_code = 0;
	memcpy(&send_buffer[msg_len],&error_code,4);
	msg_len += 4;

	return msg_len;


}

int gen_apply_for_door_resp(char** buffer,int n_door_id,int n_x,int n_y,int n_order){
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_APPLY_FOR_DOOR_RSP;
	header.msg_length = 16+sizeof(APPLY_DOOR_RESP_S);
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	memcpy(&send_buffer[msg_len],&n_order,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;
	uint16_t robot_id = 6001;
	robot_id = bswap_16(robot_id);
	memcpy(&send_buffer[msg_len],&robot_id,2);
	msg_len += 2;
	int16_t apply_ret = 200;
	apply_ret = bswap_16(apply_ret);
	memcpy(&send_buffer[msg_len],&apply_ret,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&n_door_id,4);//door id
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&n_x,4);//pos x
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&n_y,4);//pos y
	msg_len += 4;
	int door_type = 1;//1:open 2:close
	door_type = bswap_32(door_type);
	memcpy(&send_buffer[msg_len],&door_type,4);
	msg_len += 4;

	return msg_len;
}


int gen_apply_clean_area_resp(char** buffer,int32_t area_id){
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_APPLY_CLEAN_AREA_RSP;
	header.msg_length = 16+8;
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	int16_t reply = 200;
	int16_t machine = 6001;
	int32_t areaID = area_id;
	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	temp32 = bswap_32(header.msg_order);
	memcpy(&send_buffer[msg_len],&temp32,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;
	reply = bswap_16(reply);
	memcpy(&send_buffer[msg_len],&reply,2);
	msg_len += 2;
	machine = bswap_16(machine);
	memcpy(&send_buffer[msg_len],&machine,2);
	msg_len += 2;
	areaID = bswap_32(areaID);
	memcpy(&send_buffer[msg_len],&areaID,4);
	msg_len += 4;

	return msg_len;

}

int gen_trp_ctrl_req(char** buffer) {
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	TRP_CTRL_REQ_S ctrl;	
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_TRP_CTRL_REQ;
	header.msg_length = sizeof(HK_HEADER_S) + sizeof(TRP_CTRL_REQ_S);
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	temp32 = bswap_32(header.msg_order);
	memcpy(&send_buffer[msg_len],&temp32,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;
	

	ctrl.robot_id = bswap_16(6001);
	ctrl.action_type = bswap_16(1);
	ctrl.point.pos_x = bswap_32(11);
	ctrl.point.pos_y = bswap_32(22);
	ctrl.dir = bswap_16(33);
	ctrl.max_vel = bswap_16(100);
	ctrl.avoid_robot = bswap_32(1201);

	memcpy(&send_buffer[msg_len],&ctrl.robot_id,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.action_type,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.point.pos_x,4);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&ctrl.point.pos_y,4);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&ctrl.dir,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.max_vel,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.avoid_robot,4);
	msg_len += 4;

	return msg_len;
}
int gen_robot_status_resp(char** buffer) {
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_ROBOT_STATUS_RSP;
	header.msg_length = 16+8;
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	int reply = 200;
	int16_t machine = 6001;
	int16_t remain = 0;
	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	temp32 = bswap_32(header.msg_order);
	memcpy(&send_buffer[msg_len],&temp32,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;
	reply = bswap_32(reply);
	memcpy(&send_buffer[msg_len],&reply,4);
	msg_len += 4;
	machine = bswap_16(machine);
	memcpy(&send_buffer[msg_len],&machine,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&remain,2);
	msg_len += 2;

	return msg_len;
}

int gen_door_info(char** buffer) {
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	DOOR_INFO_S door1, door2, door3;
	door1.door_id = 1;
	door1.pos_x = 1;
	door1.pos_y = 1;
    door2.door_id = 2;
    door2.pos_x = 2;
    door2.pos_y = 2;
    door3.door_id = 3;
    door3.pos_x = 3;
    door3.pos_y = 3;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_DOOR_CONFIG_REQ;
	header.msg_length = 16+4+sizeof(DOOR_INFO_S)*3;
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	temp32 = bswap_32(header.msg_order);
	memcpy(&send_buffer[msg_len],&temp32,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;

	uint16_t agv_id = 6001;
	int16_t doors_num = 3;
	//agv id
	temp16 = bswap_16(agv_id);
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	//doors number
	temp16 = bswap_16(doors_num);
    memcpy(&send_buffer[msg_len],&temp16,2);
    msg_len += 2;
	//door1
	temp32 = bswap_32(door1.door_id);
	memcpy(&send_buffer[msg_len],&temp32,4);
	msg_len += 4;
	temp32 = bswap_32(door1.pos_x);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
	temp32 = bswap_32(door1.pos_y);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
	//door2
	temp32 = bswap_32(door2.door_id);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
    temp32 = bswap_32(door2.pos_x);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
    temp32 = bswap_32(door2.pos_y);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
	//door3
	temp32 = bswap_32(door3.door_id);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
    temp32 = bswap_32(door3.pos_x);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
    temp32 = bswap_32(door3.pos_y);
    memcpy(&send_buffer[msg_len],&temp32,4);
    msg_len += 4;
	
	return msg_len;
}

int gen_register_resp(char** buffer){
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_REGISTER_RSP;
	header.msg_length = 16+8;
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	int reply = 200;
	int16_t machine = 6001;
	int16_t remain = 0;
	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	temp32 = bswap_32(header.msg_order);
	memcpy(&send_buffer[msg_len],&temp32,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;
	reply = bswap_32(reply);
	memcpy(&send_buffer[msg_len],&reply,4);
	msg_len += 4;
	machine = bswap_16(machine);
	memcpy(&send_buffer[msg_len],&machine,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&remain,2);

	return 16+8;
}

int gen_interrupt_ctrl_req(char** buffer) {
	int msg_len = 0;
	int32_t temp32;
	int16_t temp16;
	char* send_buffer = *buffer;
	HK_HEADER_S header;
	INTERRUPT_CMD_S ctrl;	
	header.msg_order = 1;
	header.msg_type = GS_MSG_TYPE_INTERRUPT_CTRL_REQ;
	header.msg_length = sizeof(HK_HEADER_S) + sizeof(INTERRUPT_CMD_S);
	header.msg_version = '0';
	header.msg_crypt = '0';
	strncpy(header.remain, "00",2);

	send_buffer[msg_len++] = 0x44;//D
	send_buffer[msg_len++] = 0x42;//B
	send_buffer[msg_len++] = 0x48;//H
	send_buffer[msg_len++] = 0x4b;//k
	temp32 = bswap_32(header.msg_order);
	memcpy(&send_buffer[msg_len],&temp32,4);
	temp16 = bswap_16(header.msg_type);
	msg_len += 4;
	memcpy(&send_buffer[msg_len],&temp16,2);
	temp16 = bswap_16(header.msg_length);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&temp16,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len++],&header.msg_version,1);
	memcpy(&send_buffer[msg_len++],&header.msg_crypt,1);
	memcpy(&send_buffer[msg_len],&header.remain,2);
	msg_len += 2;
	
	flip++;
	ctrl.agv_num = bswap_16(6001);

	ctrl.cmd_type = bswap_16(flip%3);

	memcpy(&send_buffer[msg_len],&ctrl.agv_num,2);
	msg_len += 2;
	memcpy(&send_buffer[msg_len],&ctrl.cmd_type,2);
	msg_len += 2;

	return msg_len;


}