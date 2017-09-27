#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <unistd.h>

#define PORT 8902

void print_usage(char * cmd)
{
	fprintf(stderr," %s usage:\n",cmd);
	fprintf(stderr,"%s IP_Addr [port]\n",cmd);

}
 
int main(int argc,char** argv)  
{  
    	int sockfd;                               //socket()����ֵ
	int len;                                  //sendto()��recvfrom()����ֵ

	char send_buf[2048];                      //�洢��Ҫ���͵���Ϣ
	char recv_buf[2048];                      //�洢�Ѿ��յ�����Ϣ

	int adr_len = sizeof(struct sockaddr_in); //sockaddr_in�ṹ����
	
	int port = PORT;                                 //�˿ڱ���

	struct sockaddr_in server;                //�ṹ��server     
 	bzero(&server, sizeof(server));

	//��ʼ���ṹ��server
	server.sin_family = AF_INET;              
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);
  
 	//����socket
	 if (-1==(sockfd=socket(AF_INET,SOCK_DGRAM,0)))
	{
		perror("can not create socket\n");
		exit(1);
	}
   

   	printf("To server:\n");
	fgets(send_buf,2048,stdin);
	
	//sendto
	len = sendto (sockfd, send_buf, sizeof(send_buf), 0, 
			(struct sockaddr *)&server, sizeof(server));  
    if (len < 0)  
    {  
        perror("send data error\n"); 
		  close(sockfd);
        exit(1);  
    }  
	//recvfrom
	len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, 
			(struct sockaddr *)&server,  (socklen_t*)&adr_len);  
    if (len < 0)  
    {  
        perror("recv data error\n");
		close(sockfd);
        exit(1);  
    }  
	recv_buf[len]='\0';

	printf("the message from the server is:%s\n",recv_buf);
	close(sockfd);


}  
