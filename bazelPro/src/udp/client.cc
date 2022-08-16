#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h> //for inet_addr

#define PORT 7777
int main(int argc, char** argv)
{
	struct sockaddr_in addr;
    int sockfd, len = 0;    
    int addr_len = sizeof(struct sockaddr_in);      
    char buffer[1024];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT); 
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    strcpy(buffer, "{\"hello\":\"jacob\"}");
    len = sizeof(buffer);

    sendto(sockfd, buffer, len, 0, (struct sockaddr *)&addr, addr_len);

    //len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len);
    //printf("Receive from server: %s\n", buffer);

    return 0;
}


