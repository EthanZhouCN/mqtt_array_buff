#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>



#include "mqtt.h"


static int CreateTcpConnect(const char *host, unsigned short port)
{
    struct sockaddr_in add;
    int fd;
    struct hostent *server;

    bzero(&add, sizeof(add));
    add.sin_family = AF_INET;
    add.sin_port = htons(port);
    server = gethostbyname(host);
    if(NULL == server) {
        printf("Failed to get the ip of the host(%s).\n", host);
        return -1;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        printf("Failed to create socket file descriptor.\n");
        return fd;
    }

    bcopy((char*)server->h_addr, (char*)&add.sin_addr.s_addr, server->h_length);
    if(-1 == connect(fd, (struct sockaddr*)&add, sizeof(add))) {
        printf("Failed to connect to the server.\n");
        close(fd);
        return -1;
    }

    return fd;
}

int main(int args, char **argv)
{
	uint8_t sock_buff[128] = {0};
	uint16_t ret = 0;
	int sockfd = CreateTcpConnect("183.230.40.39", 6002);
	if(sockfd > 0)
	{
		printf("connect success.\n");
	}
	
	uint16_t len = GetDataConnet(sock_buff);

	int i;
	printf("len = 0x%02X.\n", len);
	for(i=0;i<128;i++)
	{
		printf("0x%02X ", sock_buff[i]);
	}
	printf("\n");
	
	ret = send(sockfd, sock_buff, len+2, 0);
	
	if(ret == len+2)
	{
		printf("send success.\n");
	}
	
	return 0;

	
}
