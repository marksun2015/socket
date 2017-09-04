/*
*   $ tcpclient 20.0.0.1 11740
*
*
*/

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

int main(int argc, char * argv[]){
	
	int sockfd;
	char buffer[1024];
	struct sockaddr_in server;
	struct hostent * host;
	int port, nbytes;
	//char hi[]="hi server!\n";
	
	//0x2ddc:11740  
	unsigned char hi[36]={  0x00,0x01,0x17,0xe8,0x24,0x00,0x00,0x00,0xc5,0x6b,  
				0x40,0x03,0x00,0x43,0x2d,0xdc,0x14,0x00,0x00,0x02,  
				0x2d,0xec,0x14,0x00,0x00,0x01,0x85,0xea,0x02,0xc2,
				0x03,0x01,0x00,0x00,0x28,0x38};
	
	int i=0;
	if(argc!=3){
                fprintf (stderr, "Usage: %s host_name(IP address) port\n", argv[0]);
		exit(1);
	}
	
	if((host=gethostbyname(argv[1]))==NULL){
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}

	if((port=atoi(argv[2]))<0){
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}
	
	printf("socket \n");
	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(port);
	server.sin_addr=*((struct in_addr *)host->h_addr);

        //if (bind(fd, (struct sockaddr *)&myaddr,sizeof(myaddr)) <0) {
        //        perror("bind failed!");
        //        exit(1);
        //}

	printf("connection \n");
	// get connection
	if(connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	
	printf("write \n");
	if(write(sockfd, hi, sizeof(hi))==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	#if 1	
	printf("read \n");
	if((nbytes=read(sockfd, buffer, 1024))==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	printf("nbytes:%d \n",nbytes);

	buffer[nbytes]='\0';
	for(i=0;i<nbytes;i++){
		if((i%8)==0){
			printf("\n");
		}
		printf("%02x ", buffer[i]);
	}
	printf("\n");
	#endif	
	
	close(sockfd);
		
	return 0;
}
