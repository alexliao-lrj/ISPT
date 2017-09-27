#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define START_PORT 1
#define END_PORT 65535

void print_usage(char * cmd)
{
	fprintf(stderr," %s usage:\n",cmd);
	fprintf(stderr,"%s scanip [start_port] [end_port]\n",cmd);

}

int main(int argc,char** argv)
{
	struct sockaddr_in server;
	int port, start_port, end_port;
	int sockfd;
	struct servent *sptr;

	//invalid input
	if ((2>argc)|| (argc >4))
	{
		print_usage(argv[0]);
		exit(1);
	}
	//valid input
	if (4==argc) 
    {
		start_port = atoi(argv[2]);
		end_port = atoi(argv[3]);
	}
	if (3==argc)  
	{
		start_port = atoi(argv[2]);
		end_port = END_PORT;
	}
	if (2==argc)  
	{
		start_port = START_PORT;
		end_port = END_PORT;
	}
	//loop to scan each port
	for ( port=start_port; port <= end_port; port++)
	{
		
		//init socket
	    if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
		{
			perror("can not create socket\n");
			exit(1);
		}
		
		memset(&server,0,sizeof(struct sockaddr_in));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(argv[1]);
		server.sin_port = htons(port);

		//start connection
		if (0==connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))
		{
			if ((sptr = getservbyport(htons(port), "tcp")) == NULL)
			{
				printf("%d\tnone\n", port);
			}else if(sptr->s_name == NULL){ 
				printf("%d\tnone\n", port);
				}else{ 
					printf("%d\t%s\n", port, sptr->s_name);
			}
		}
		// break connection
		close(sockfd);
		
	}
	
	printf("Complete.\n");
	return 0;
}
