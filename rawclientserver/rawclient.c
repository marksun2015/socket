/**
 * @file arp_request.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netpacket/packet.h>

/* 乙太網幀首部長度 */
#define ETHER_HEADER_LEN sizeof(struct ether_header)
/* 整個arp結構長度 */
#define ETHER_ARP_LEN sizeof(struct ether_arp)
/* 乙太網 + 整個arp結構長度 */
#define ETHER_ARP_PACKET_LEN ETHER_HEADER_LEN + ETHER_ARP_LEN
/* IP地址長度 */
#define IP_ADDR_LEN 4
/* 廣播地址 */
#define BROADCAST_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

void err_exit(const char *err_msg)
{
    perror(err_msg);
    exit(1);
}

void send_request(const char *if_name, const char *dst_ip)
{
    struct sockaddr_ll saddr_ll;
    struct ether_header *eth_header;
    struct ether_arp *arp_packet;
    struct ifreq ifr;
    char buf[ETHER_ARP_PACKET_LEN];
    unsigned char src_mac_addr[ETH_ALEN];
    unsigned char dst_mac_addr[ETH_ALEN] = BROADCAST_ADDR;
    char *src_ip;
    int sock_raw_fd, ret_len, i;

    unsigned char buffer[74]={
	0x00, 0x0c, 0x26, 0x00, 0x0b, 0x88, 0xcc, 0x0c, 
	0x26, 0x01, 0x02, 0x03, 0x08, 0x00, 0x45, 0x00, 
	0x00, 0x3c, 0xa5, 0x91, 0x40, 0x00, 0x40, 0x06,  
	0x10, 0x18, 0xc0, 0xa8, 0x01, 0xe1, 0xc0, 0xa8,  
	0x01, 0xe2, 0xa1, 0xa8, 0x2d, 0xdc, 0x48, 0x0c,  
	0x65, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02,  
	0xaa, 0xaa, 0x85, 0x41, 0x00, 0x00, 0x02, 0x04,  
	0xff, 0xd7, 0x04, 0x02, 0x08, 0x0a, 0xff, 0xff,  
	0xc0, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,  
	0x03, 0x06 };


    //if ((sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1)
    if ((sock_raw_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP))) == -1)
        err_exit("socket()");

    bzero(&saddr_ll, sizeof(struct sockaddr_ll));
    bzero(&ifr, sizeof(struct ifreq));
    /* 網卡介面名 */
    memcpy(ifr.ifr_name, if_name, strlen(if_name));

    /* 獲取網卡介面索引 */
    if (ioctl(sock_raw_fd, SIOCGIFINDEX, &ifr) == -1)
        err_exit("ioctl() get ifindex");
    saddr_ll.sll_ifindex = ifr.ifr_ifindex;
    saddr_ll.sll_family = PF_PACKET;

    /* 發送請求 */
    ret_len = sendto(sock_raw_fd, buffer, 74, 0, (struct sockaddr *)&saddr_ll, sizeof(struct sockaddr_ll));
    if ( ret_len > 0)
        printf("sendto() ok!!!\n");

    close(sock_raw_fd);
}

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        printf("usage:%s device_name dst_ip\n", argv[0]);
        exit(1);
    }

    send_request(argv[1], argv[2]);
    
    return 0;
}
