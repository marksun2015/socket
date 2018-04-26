#include <stdio.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 

int toCidr(char* ipAddress)
{
	int netmask_cidr;
	int ipbytes[4];
	netmask_cidr=0;
	
	sscanf(ipAddress, "%d.%d.%d.%d", &ipbytes[0], &ipbytes[1], &ipbytes[2], &ipbytes[3]);

	for (int i=0; i<4; i++)
	{
		while (ipbytes[i])
		{
			netmask_cidr += (ipbytes[i] & 0x01 );
			ipbytes[i] >>= 1;
		}
	}

	return netmask_cidr;
}

int main(int argc, char* argv[]) 
{ 
	int fd_arp; /* socket fd for receive packets */ 
	char device[10]; /* ethernet device name */ 
	unsigned char *ptr; 
	struct in_addr myip, mymask; 
	struct ifreq ifr; /* ifr structure */ 
	struct sockaddr_in *sin_ptr; 

	if(argc < 2) 
		strcpy(device, "eth0"); 
	else 
		strcpy(device, argv[1]); 

	strcpy(ifr.ifr_name, device); 
	if ((fd_arp = socket(AF_INET, SOCK_PACKET, htons(0x0806))) < 0) 
	{ 
		perror( "arp socket error");
		return -1; 
	} 
	/* ifr.ifr_addr.sa_family = AF_INET; */ 

	/* get ip address of my interface */ 
	if(ioctl(fd_arp, SIOCGIFADDR, &ifr) < 0) 
	{ 
		perror("ioctl SIOCGIFADDR error"); myip.s_addr = 0; 
	} 
	else 
	{ 
		sin_ptr = (struct sockaddr_in *)&ifr.ifr_addr; myip = sin_ptr->sin_addr; 
	} 

	/* get network mask of my interface */ 
	if (ioctl(fd_arp, SIOCGIFNETMASK, &ifr) < 0) 
	{ 
		perror("ioctl SIOCGIFNETMASK error"); mymask.s_addr = 0; 
	} 
	else 
	{ 
		sin_ptr = (struct sockaddr_in *)&ifr.ifr_addr; mymask = sin_ptr->sin_addr; 
	} 
	
	/* get mac address of the interface */ 
	if (ioctl(fd_arp, SIOCGIFHWADDR, &ifr) < 0) 
	{ 
		perror("ioctl SIOCGIFHWADDR error"); ptr = NULL; 
	} 
	else 
	{ 
		ptr = (u_char *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0]; 
	} 
	printf( "device: %s\n", device); 
	printf( "request netmask %s\n", inet_ntoa(mymask)); 
	printf( "CIDR: %d\n", toCidr(inet_ntoa(mymask))); 
	printf( "request IP %s\n", inet_ntoa(myip)); 
	if (ptr) 
		printf( "request mac %02x:%02x:%02x:%02x:%02x:%02x\n", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5) ); 
	else 
		printf( "request mac ?:?:?:?:?:?\n"); 
	return 0; 
}
