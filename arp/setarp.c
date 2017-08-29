#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <arpa/inet.h> 
#include <fcntl.h> // for open
#include <unistd.h> // for close

int set_arp(char * ip, char * mac, char * dev) {

	struct arpreq req;
	struct sockaddr_in addr;
	int sockfd, rc;
	unsigned int s_addr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		return 1;
	}

	/* Inet sockaddr_in struct */
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);

	/* ARP arpreq struct */
	memset(&req, 0, sizeof(req));
	memcpy(&req.arp_pa, &addr, sizeof(struct sockaddr));
	memcpy(req.arp_ha.sa_data, mac, 6);
	req.arp_flags = ATF_PERM | ATF_COM;
	strcpy(req.arp_dev, dev);

	rc = ioctl(sockfd, SIOCSARP, &req);
	close(sockfd);

	return rc;
}
#if 0
int StaticArpGet(char *ip, char *devname)
{
	struct arpreq arpreq;
	int rtv, sock_fd;
	char macaddr[18]={0};
	unsigned char *hw_addr;
	printf("check arp table Ip : %s\n", ip);

	memset(&arpreq, 0, sizeof(struct arpreq));
	arpreq.arp_pa.sa_family = AF_INET;
	arpreq.arp_ha.sa_family = AF_UNIX;

	//SIN_ADDR(arpreq.arp_pa) = inet_addr(ip);
	SIN_ADDR(arpreq.arp_pa) = inet_addr("192.168.2.89");
	//strcpy(arpreq.arp_dev, devname);
	strcpy(arpreq.arp_dev, "enx00e04c534458");
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	rtv = ioctl(sock_fd, SIOCGARP, (caddr_t)&arpreq);
	close(sock_fd);
	if (rtv < 0)
	{
		printf("%s\n", "Not found in cache");
		return -1;
	}
	else
	{
		printf("%s\n", "success");
		hw_addr = (unsigned char *) arpreq.arp_ha.sa_data;
		snprintf(macaddr,sizeof(macaddr),"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
				hw_addr[0], hw_addr[1], hw_addr[2], hw_addr[3], hw_addr[4],
				hw_addr[5]);
		printf("macaddr [ %s ]\n",macaddr);
	}
	return 0;
}
#endif

int main(int argc, char* argv[]){
	char *devname="eth0";
	char *devip="192.168.1.226";
	char devmac[6];
	devmac[0]=0x00;
	devmac[1]=0x0c;
	devmac[2]=0x26;
	devmac[3]=0x01;
	devmac[4]=0x02;
	devmac[5]=0x03;
	set_arp(devip, devmac, devname); 

}
