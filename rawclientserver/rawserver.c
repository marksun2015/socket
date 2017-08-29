#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/filter.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <net/route.h>

#include <sys/ioctl.h>
#include <net/if.h>

#include <pthread.h> 

int create_recv_socket(void)
{
	int sock;
	struct ifreq ethreq;

	struct sock_fprog Filter;
	/*
	socket filter
	if there were no filter, test modbus TCP would easily disconnect
	FIXME: need dynamically change port
	tcp:11740
	udp:1740
	modbus:502
	 */
	struct sock_filter BPF_code[]= {
		// tcpdump -dd -i eth0 "dst port 1740 or port 502 or 11740 or arp"
		{ 0x28, 0, 0, 0x0000000c },
		{ 0x15, 0, 10, 0x000086dd },
		{ 0x30, 0, 0, 0x00000014 },
		{ 0x15, 2, 0, 0x00000084 },
		{ 0x15, 1, 0, 0x00000006 },
		{ 0x15, 0, 23, 0x00000011 },
		{ 0x28, 0, 0, 0x00000038 },
		{ 0x15, 20, 0, 0x000006cc },
		{ 0x15, 19, 0, 0x000001f6 },
		{ 0x15, 18, 0, 0x00002ddc },
		{ 0x28, 0, 0, 0x00000036 },
		{ 0x15, 16, 14, 0x000001f6 },
		{ 0x15, 0, 14, 0x00000800 },
		{ 0x30, 0, 0, 0x00000017 },
		{ 0x15, 2, 0, 0x00000084 },
		{ 0x15, 1, 0, 0x00000006 },
		{ 0x15, 0, 12, 0x00000011 },
		{ 0x28, 0, 0, 0x00000014 },
		{ 0x45, 10, 0, 0x00001fff },
		{ 0xb1, 0, 0, 0x0000000e },
		{ 0x48, 0, 0, 0x00000010 },
		{ 0x15, 6, 0, 0x000006cc },
		{ 0x15, 5, 0, 0x000001f6 },
		{ 0x15, 4, 0, 0x00002ddc },
		{ 0x48, 0, 0, 0x0000000e },
		{ 0x15, 2, 0, 0x000001f6 },
		{ 0x15, 1, 2, 0x00002ddc },
		{ 0x15, 0, 1, 0x00000806 },
		{ 0x6, 0, 0, 0x00040000 },
		{ 0x6, 0, 0, 0x00000000 },
	};
 
	Filter.len = sizeof(BPF_code)/sizeof(BPF_code[0]);;
	Filter.filter = BPF_code;

	if ((sock=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0) {
	//if ((sock=socket(AF_INET, SOCK_RAW, IPPROTO_TCP))) {
		printf("create socket fail\n");
		return 0;
	}

	/* Attach the filter to the socket */
	if (setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &Filter, sizeof(Filter))<0){
		int temp=sizeof(Filter);
		printf( "%d" , temp);
		printf("setsockopt \n");
		close(sock);
		return 0;
	}

	return sock;
}


//void *recived_socket(void *ptr)
void recived_socket(void)
{
	int rec_socket;
	int i=0;
	int rec_num;
	unsigned char buffer[1024];
	rec_socket = create_recv_socket();

	while(1){
		rec_num = recv(rec_socket, &buffer[0], sizeof(buffer), 0);
		if( rec_num < 0){
			perror("recv():");
			close(rec_socket);
		}
		#if 0
		if(buffer[29]==0xb5 || buffer[29]==0xe1){
			printf("\n nbytes:%d \n",rec_num);
			for(i=0;i<rec_num;i++){
				if((i%8)==0){
					printf("\n");
				}
				printf("%02x ", buffer[i]);
			}
		}
		#endif
	}

	close(rec_socket);
}


int main(int argc, char * argv[]){

	int sockfd, newfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int port, sin_size;
	int domain=AF_INET;
	int type=SOCK_STREAM;
	char hello[]="hello world!\n";
	int nbytes;

	recived_socket();

	return 0;
}
