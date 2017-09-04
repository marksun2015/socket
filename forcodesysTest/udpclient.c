/*
*  ip:20.0.0.2 --> 20.0.0.1
*  $ udpclinet 20.0.0.1 1741 
* 
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>

#define SERV_PORT 1741
#define MAXDATA   1024

#define MAXNAME 1024
int main(int argc, char **argv){
        int fd;     /* fd into transport provider */
        int i;     /* loops through user name */
        int length;    /* length of message */
        int size;    /* the length of servaddr */
        int fdesc;    /* file description */
        int ndata;    /* the number of file data */
        int byte_count;    
        char ch;
        char data[MAXDATA]; /* read data form file */
        char data1[MAXDATA];  /*server response a string */
        char buf[BUFSIZ];     /* holds message from server */
        struct hostent *hp;   /* holds IP address of server */
        struct sockaddr_in myaddr;   /* address that client uses */
        struct sockaddr_in servaddr; /* the server's full addr */

	//tagv[7],tagv[9],ip: 02 --> 01  
	unsigned char tagv[32]={0xc5,0xf3,0x40,0x40,0x00,0x11,0x00,0x02,0x00,0x01,
				0x00,0x00,0xc3,0x00,0x01,0x01,0x42,0xba,0x26,0xe3,
				0x01,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x02,0x00,
				0x00,0x00};
        /*
         * Check for proper usage.
         */
        if (argc < 3) {
        //if (argc < 2) {
                fprintf (stderr, "Usage: %s host_name(IP address) port\n", argv[0]);
                //fprintf (stderr, "Usage: %s host_name(IP address) \n", argv[0]);
                exit(2);
        }
        /*
         *  Get a socket into UDP
         */
        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror ("socket failed!");
                exit(1);
        }
        /*
         * Bind to an arbitrary return address.
         */
        bzero((char *)&myaddr, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        //myaddr.sin_addr.s_addr = inet_addr("192.168.1.225");
        //myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        myaddr.sin_port = htons(1740);

        if (bind(fd, (struct sockaddr *)&myaddr,
                                sizeof(myaddr)) <0) {
                perror("bind failed!");
                exit(1);
        }
        /*
         * Fill in the server's UDP/IP address
         */

        bzero((char *)&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        //servaddr.sin_port = htons(SERV_PORT);
        servaddr.sin_port = htons(atoi(argv[2]));
        hp = gethostbyname(argv[1]);

        if (hp == 0) {
                fprintf(stderr, "could not obtain address of %s\n", argv[2]);
                return (-1);
        }
        bcopy(hp->h_addr_list[0], (caddr_t)&servaddr.sin_addr,
                        hp->h_length);

        /* 發送資料給 Server */
        size = sizeof(servaddr);
        if (sendto(fd, tagv, sizeof(tagv), 0, (struct sockaddr*)&servaddr, size) == -1) {
                perror("write to server error !");
                exit(1);
        }
        /** 由伺服器接收回應 **/
	byte_count = recvfrom(fd, data1, MAXDATA, 0, (struct sockaddr*)&servaddr, &size);
        if (byte_count < 0) {
                perror ("read from server error !");
                exit (1);
        }
        /* 印出 server 回應 **/
        printf("---response from server--- ");
	for(i=0;i<byte_count;i++){
		if((i%8)==0){
        		printf("\n");
		}
        	printf("%02x ", data1[i]);
	}
        printf(" \n");

}
