#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "src/hk/msg_gen.h"
#include "src/hk/msg_type.h"
#include "src/utils/dump.h"
#define PORT 8987
using namespace HK_AGENT;
using namespace UTILS;

static int tick = 0;
int main(int argc, char **argv)
{
	int server_sockfd = -1;
	int server_len = 0;
	int client_len = 0;
	char buffer[1024];
	char *sendbuffer = (char *)calloc(1, 1024);
	char *dump;
	int recv_len = 0;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	server_len = sizeof(server_addr);
	bind(server_sockfd, (struct sockaddr *)&server_addr, server_len);
	signal(SIGCHLD, SIG_IGN);

	bool registered = false;
	bool if_send_door_info = false;

	printf("begin receive package with port:%d\n", PORT);
	while (1)
	{
		tick++;

		recv_len = recvfrom(server_sockfd, buffer, sizeof(buffer), 0,
							(struct sockaddr *)&client_addr, (socklen_t *)&client_len);
		uint16_t type = 0;
		memcpy(&type, &buffer[8], 2);
		type = bswap_16(type);
		printf("rece len: %d, rece type:%d\n", recv_len, type);
		hexdump(buffer, recv_len);

		if (type == GS_MSG_TYPE_REGISTER_REQ)
		{
			int send_len = gen_register_resp(&sendbuffer);
			// registered = true;

			// debug dump buffer
			printf("\nregister resp send len:%d\n", send_len);
			hexdump(sendbuffer, send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);
		}
		else if (type == GS_MSG_TYPE_ROBOT_STATUS_REQ)
		{
			int send_len = gen_robot_status_resp(&sendbuffer);

			// debug dump buffer
			printf("\nrobot responce send len:%d\n", send_len);
			hexdump(sendbuffer, send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);
			// send trp control
			uint8_t need_plan = buffer[16 + 6];
			printf("\nrobot need plan: %d\n", need_plan);
			if (need_plan)
			{
				send_len = gen_trp_ctrl_req(&sendbuffer);
				printf("\ntrp control req send len:%d\n", send_len);
				hexdump(sendbuffer, send_len);

				sendto(server_sockfd, sendbuffer, send_len, 0,
					   (struct sockaddr *)&client_addr, client_len);
			}

			registered = true;
			// send door info
			if (!if_send_door_info && registered)
			{
				if_send_door_info = true;
				// ready send doors info
				send_len = gen_door_info(&sendbuffer);
				printf("\ndoor info req send len:%d\n", send_len);
				hexdump(sendbuffer, send_len);

				sendto(server_sockfd, sendbuffer, send_len, 0,
					   (struct sockaddr *)&client_addr, client_len);
			}
		}
		else if (type == GS_MSG_TYPE_APPLY_CLEAN_AREA_REQ)
		{
			int area_id;
			memcpy(&area_id, &buffer[20], 4);
			area_id = bswap_32(area_id);
			printf("\nclean area id: %d\n", area_id);
			int send_len = gen_apply_clean_area_resp(&sendbuffer, area_id);

			// debug dump buffer
			printf("\nclean area responce send len:%d\n", send_len);
			hexdump(sendbuffer, send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);
		}
		else if (type == GS_MSG_TYPE_APPLY_FOR_DOOR_REQ)
		{
			char *buf = (char *)&buffer[0];
			int n_door_id;
			int n_x;
			int n_y;
			int n_order;
			memcpy(&n_order, &buffer[4], 4);
			memcpy(&n_door_id, &buffer[20], 4);
			memcpy(&n_x, &buffer[24], 4);
			memcpy(&n_y, &buffer[28], 4);

			int send_len = gen_apply_for_door_resp(&sendbuffer, n_door_id, n_x, n_y, n_order);
			// debug dump buffer
			printf("\napply door responce send len:%d\n", send_len);
			hexdump(sendbuffer, send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);
		}
		else if (type == GS_MSG_TYPE_APPLY_TRAFFIC_RES_REQ)
		{
			int n_area_id;
			int n_order;
			memcpy(&n_order, &buffer[4], 4);
			memcpy(&n_area_id, &buffer[20], 4);

			int send_len = gen_apply_traffic_res_resp(&sendbuffer, n_order, n_area_id);
			// debug dump buffer
			printf("\napply traffic responce send len:%d\n", send_len);
			hexdump(sendbuffer, send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);
		}
		else if (type == GS_MSG_TYPE_APPLY_OFFLINE_REQ)
		{
			int n_order;
			memcpy(&n_order, &buffer[4], 4);

			int send_len = gen_apply_offine_resp(&sendbuffer, n_order);
			// debug dump buffer
			printf("\napply offline responce send len:%d\n", send_len);
			hexdump(sendbuffer, send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);
		}
		else if (type == GS_MSG_TYPE_APPLY_PLAN_REQ)
		{
			int n_order;
			memcpy(&n_order, &buffer[4], 4);

			int type;
			int index = sizeof(HK_HEADER_S) + sizeof(PLAN_REQ_S) - 2;
			memcpy(&type, &buffer[index], 2);
			type = bswap_16(type);

			int send_len = gen_apply_plan_resp(&sendbuffer, n_order);
			printf("\ntype:[%d] apply plan responce send len:%d\n", type, send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);

			if (type == 1)
			{
				int send_len = gen_path_plan_req(&sendbuffer);
				printf("\ntype:[%d] plan path req send len:%d\n", type, send_len);

				sendto(server_sockfd, sendbuffer, send_len, 0,
					   (struct sockaddr *)&client_addr, client_len);
			}
		}

		if (tick % 3 == 0)
		{
			int send_len = gen_interrupt_ctrl_req(&sendbuffer);
			printf("\n interrupt control req send len:%d\n", send_len);
			// hexdump(sendbuffer,send_len);

			sendto(server_sockfd, sendbuffer, send_len, 0,
				   (struct sockaddr *)&client_addr, client_len);
		}
	}

	close(server_sockfd);
	if (1)
	{
		printf("hello");
		printf("eee");
	}
}
