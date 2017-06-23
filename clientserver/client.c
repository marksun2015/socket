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
	char hi[]="hi server!\n";
	struct sockaddr_in server;
	struct hostent * host;
	int port, nbytes;
	
	if(argc!=3){
		fprintf(stderr, "%s\n", argv[0]);
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
	
	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(port);
	server.sin_addr=*((struct in_addr *)host->h_addr);

	// get connection
	if(connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	
	if((nbytes=read(sockfd, buffer, 1024))==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	
	if(write(sockfd, hi, sizeof(hi))==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	buffer[nbytes]='\0';
	printf("%s\n", buffer);
	close(sockfd);
		
	return 0;
}
