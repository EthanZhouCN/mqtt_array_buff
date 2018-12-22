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
#include "cJSON.h"
#include "cjson_utils.h"


#define SOCK_BUFF_SIZE 1024

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
#if 1
	uint8_t sock_buff[SOCK_BUFF_SIZE] = {0};
	uint8_t payload_buff[SOCK_BUFF_SIZE] = {0};
	
	uint16_t ret = 0;
	int sockfd = CreateTcpConnect("183.230.40.39", 6002);
	if(sockfd > 0)
	{
		printf("connect success.\n");
	}
	
	uint16_t len = GetDataConnet(sock_buff);
	
	int i;
	printf("send len = 0x%02X.\n", len);
	for(i=0;i<len+2;i++)
	{
		printf("0x%02X ", sock_buff[i]);
	}
	printf("\n");
	
	ret = send(sockfd, sock_buff, len+2, 0);
	
	if(ret == len+2)
	{
		printf("send success.\n");
	}

	ret = recv(sockfd, sock_buff, SOCK_BUFF_SIZE, 0);

	printf("recv len = 0x%02X.\n", ret);
	for(i=0;i<ret;i++)
	{
		printf("0x%02X ", sock_buff[i]);
	}
	printf("\n");


	DoubleTypeDataPoint2String(payload_buff, "tmp", 55.4);	
	
    printf("json:\n%s\n", payload_buff);

	len = GetDataPointPUBLISH((unsigned char *)sock_buff, 0, 2, 0, (char *)"$dp", (char *)payload_buff);

	printf("json send len = 0x%02X.\n", len);
	for(i=0;i<len;i++)
	{
		printf("0x%02X ", sock_buff[i]);
	}
	printf("\n");
	
	uint8_t tmp_buff[100] = {0};
	
	do
	{
		ret = send(sockfd, sock_buff, len, 0);   
		
		if(ret == len+2)
		{
			printf("data send success.\n");
		}

		
		
		ret = recv(sockfd, tmp_buff, 10, 0);
		
		printf("recv len = 0x%02X.\n", ret);
		for(i=0;i<ret;i++)
		{
			printf("0x%02X ", tmp_buff[i]);
		}
		printf("\n");


	}
	while(tmp_buff[0] != 0x50);
	
	tmp_buff[0] = 0x62;
	tmp_buff[1] = 0x02;
	tmp_buff[2] = 0x00;
	tmp_buff[3] = 0x02;
	
	ret = send(sockfd, tmp_buff, 4, 0);	 
			
	if(ret == 4)
	{
		printf("data send success.\n");
	}

	printf("send len = 0x%02X.\n", ret);
	for(i=0;i<ret;i++)
	{
		printf("0x%02X ", tmp_buff[i]);
	}
	printf("\n");


	ret = recv(sockfd, tmp_buff, 10, 0);
			
	printf("recv len = 0x%02X.\n", ret);
	for(i=0;i<ret;i++)
	{
		printf("0x%02X ", tmp_buff[i]);
	}
	printf("\n");

	ret = recv(sockfd, tmp_buff, 100, 0);
				
	printf("cmd recv len = 0x%02X.\n", ret);
	for(i=0;i<ret;i++)
	{
		printf("0x%02X ", tmp_buff[i]);
	}
	printf("\n");


	
	
	unsigned char topicname[200] = {0};
	unsigned short topiclen = 0;
	unsigned char payload[200] = {0};
	unsigned short payloadlen = 0;
	
	FixedHeader_t FixedHeader;

	PlatfromCmdAnalysis(tmp_buff, &FixedHeader, &topiclen, topicname, &payloadlen, payload);

	printf("FixedHeader.PacketType = %d.\n", FixedHeader.PacketType);
	printf("FixedHeader.PacketType = %d.\n", FixedHeader.RemainingLength);

	printf("topicname = %s.\n", topicname);

	
	printf("cmd payload recv len = 0x%02X.\n", payloadlen);
	for(i=0;i<payloadlen;i++)
	{
		printf("0x%02X ", payload[i]);
	}
	printf("\n");

#endif	

	while(1)
	{
		sleep(100);
	}
	return 0;

	
}
