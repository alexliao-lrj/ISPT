#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>  
#include <errno.h>
#include <unistd.h>  
#include <sys/types.h>  

#define PORT 8902;  

int main()  
{   
	int listend;                    //socket()返回值
	int sendnum;                    //sendto()返回值
	int recvnum;                    //recvfrom()返回值

	char send_buf[2048];            //存储将要发送的消息
	char recv_buf[2048];            //存储已经收到的消息
	memset(send_buf,0,2048);        //初始化send_buf
	memset(recv_buf,0,2048);        //初始化recv_buf

	int port = PORT;                //端口变量

	int adr_len = sizeof(struct sockaddr_in);          //sockaddr_in结构长度            
	struct sockaddr_in client;                         //实例化结构体client

 	bzero(&client, sizeof(client));

	//初始化sockaddr_in结构
	client.sin_family = AF_INET;                       //选择IPv4网络协议套接字类型
    	client.sin_addr.s_addr = htonl(INADDR_ANY);
    	client.sin_port = htons(port);
 
	//建立socket
	 if (-1==(listend=socket(AF_INET,SOCK_DGRAM,0)))   
      {
	 perror("create listen socket error\n");
	 exit(1);
      }

	//绑定server和client
	if (-1==bind(listend,(struct sockaddr *)&client,sizeof(client)))
     {
	perror("bind error\n");
	exit(1);

     }

	
    while(1)  
    {  
	bzero(recv_buf, sizeof(recv_buf));

	recvnum = recvfrom(listend, recv_buf, sizeof(recv_buf), 0, 
			(struct sockaddr *)&client, (socklen_t*)&adr_len);  
    if (recvnum < 0)  
    {  
     	perror("recv error\n");
        exit(1);  
    }  
	recv_buf[recvnum]='\0';

	printf ("the message from the client is: %s\n",recv_buf);

	if (0==strcmp(recv_buf,"quit"))
    {
		perror("the client break the server process\n");
		break;
	}  

	sendnum = sendto (listend, send_buf, sizeof(send_buf), 0, 
			(struct sockaddr *)&client, sizeof(client));   
    if (sendnum < 0)  
    {  
        perror("sendto error\n");  
        exit(1);  
    }  
	
	continue;

   }

    close(listend);
    return 0;

}  
