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
#endif
	//------------------------
	
#if 1

	//先创建空对象
    cJSON *json = cJSON_CreateObject();
    //添加数组
    cJSON *datastreams_array = NULL;
    cJSON_AddItemToObject(json,"datastreams", datastreams_array=cJSON_CreateArray());
    //在数组上添加对象
    cJSON *obj = NULL;
    cJSON_AddItemToArray(datastreams_array, obj=cJSON_CreateObject());
    cJSON_AddStringToObject(obj,"id","tmp");
	cJSON *datapoints_array = NULL;
	cJSON_AddItemToObject(obj, "datapoints", datapoints_array=cJSON_CreateArray());
    //在对象上添加键值对
    cJSON_AddItemToArray(datapoints_array, obj=cJSON_CreateObject());
    cJSON_AddItemToObject(obj,"at",cJSON_CreateString("2018-12-10 17:09:38"));
    cJSON_AddItemToObject(obj,"value",cJSON_CreateNumber(45.6));

    
  
    
    //清理工作
    char *buf = cJSON_Print(json);
    printf("json:\n%s\n", buf);

	len = GetDataPointPUBLISH(sock_buff, 0, 2, 0, "$dp", buf);

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




	
    cJSON_Delete(json);

#endif	

	while(1)
	{
		sleep(100);
	}
	return 0;

	
}
