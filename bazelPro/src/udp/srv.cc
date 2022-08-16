#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "rapidjson/document.h"

#define PORT 7777
int main(int argc, char** argv)
{
	int server_sockfd = -1;
	int server_len = 0;
	int client_len = 0;
	char buffer[1024];
	char* sendbuffer = (char*)calloc(1,1024);
	char* dump;
	int recv_len = 0;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	
	server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	server_len = sizeof(server_addr);
	bind(server_sockfd, (struct sockaddr*)&server_addr, server_len);
	signal(SIGCHLD, SIG_IGN);

	bool registered = false;
	bool if_send_door_info =  false;
	printf("begin receive package with port:%d\n",PORT);
	while(1)
	{	
		recv_len = recvfrom(server_sockfd, buffer, sizeof(buffer), 0, 
				(struct sockaddr*)&client_addr,(socklen_t*) &client_len);
		
        using namespace rapidjson;
        StringStream s(buffer);
        
        Document d;
        d.ParseStream(s);
        printf("hello = %s\n", d["hello"].GetString());
		
	}

	close(server_sockfd);
}


