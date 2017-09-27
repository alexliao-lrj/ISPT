#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<signal.h>

#define PORT 8900
int execute(char* command,char* buf);
static void sig_child(int signo);			

int main(int argc,char** argv)
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	int len;
	int port;
	int listend;
	int connectd;
	int sendnum;
	int opt;
	int recvnum;
	char send_buf[2048];
	char recv_buf[2048];
	char cmd[2048];


	port= PORT;
	memset(send_buf,0,2048);
	memset(recv_buf,0,2048);
	
      opt = SO_REUSEADDR;
      
	/*建立socket*/
      if (-1==(listend=socket(AF_INET,SOCK_STREAM,0)))
      {
	 perror("create listen socket error\n");
	 exit(1);
      }
      setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

      #ifdef DEBUG
		printf("the listen id is %d\n",listend);
      #endif

     memset(&server,0,sizeof(struct sockaddr_in));
     server.sin_family = AF_INET;
     server.sin_addr.s_addr = htonl(INADDR_ANY);
     server.sin_port = htons(port);
	/*建立连接*/
     if (-1==bind(listend,(struct sockaddr *)&server,sizeof(struct sockaddr)))
     {
		perror("bind error\n");
		exit(1);
	 }

     if (-1==listen(listend,5))
	 {
		perror("listen error\n");
		exit(1);
	 }
	 signal(SIGCHLD, sig_child);			//注册函数

	/*进入服务循环*/
while(1){

	 int a = 0;             

	 /*accept*/
	 if (-1==(connectd=accept(listend,(struct sockaddr*)&client,&len)))
	 {
		perror("create connect socket error\n");
		exit(1);
     }
   	#ifdef DEBUG
		printf("the connect id is %d",connect);
		printf("the client ip addr is %s",inet_ntoa(client.sin_addr));
   	#endif

	pid_t pid;
	pid = fork();

	/*子进程处理*/
	if (pid == 0){
		close(listend);				//关闭连接
		while(1){
			memset(recv_buf,0,2048);
			memset(send_buf,0,2048);

		
        if (0>(recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0)))
			{
				perror("recv error\n");
				close(connectd);
				continue;
			}

       if (0==recvnum)
			{
				perror("client quit error\n");
				close(connectd);
				a = 1;              
				break;
			}

			recv_buf[recvnum]='\0';
			if (0==strcmp(recv_buf,"quit"))
    			{
			perror("the client break the server process\n");
			close(connectd);
			break;
			}
			printf ("the message from the client is: %s\n",recv_buf);
			strcpy(cmd,"/bin/");
			strcat(cmd,recv_buf);
			execute(cmd,send_buf);

			if ('\0'==*send_buf)
			{
				memset(cmd,0,sizeof(cmd));
				strcpy(cmd,"/sbin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);
			
				if ('\0'==*send_buf)
				{	
					memset(cmd,0,sizeof(cmd));
					strcpy(cmd,"/usr/bin/");
					strcat(cmd,recv_buf);
					execute(cmd,send_buf);
				}
			
				if ('\0'==*send_buf)
				{	
					memset(cmd,0,sizeof(cmd));
					strcpy(cmd,"/usr/sbin/");
					strcat(cmd,recv_buf);
					execute(cmd,send_buf);
				}
			}


			if ('\0'==*send_buf)
			sprintf(send_buf,"command is not vaild,check it please\n");
			printf("the server message is:%s\n",send_buf);

       	if ( 0 > send(connectd,send_buf,sizeof(send_buf),0))
			{
				perror("send error\n");
				close(connectd);
				continue;

			}
		}

		close(connectd);			//关闭accept
		exit(0);
	}else if(pid > 0){				/*父进程运行*/
		close(connectd);
		continue;
	}
}
    close(listend);
    return 0;
}

static void sig_child(int signo){
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated.\n", pid);
}

int execute(char* command,char* buf)
{
	FILE * 	fp;
	int count;

	if (NULL==(fp = popen(command,"r")))
	{
		perror("creating pipe error\n");
		exit(1);

	}
	
	count = 0 ;

	while(((buf[count] = fgetc(fp))!=EOF)&&count<2047)
	count++;
	buf[count]='\0';

	pclose(fp);
	return count;

}
