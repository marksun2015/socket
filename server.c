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

	int sockfd, newfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int port, sin_size;
	int domain=AF_INET;
	int type=SOCK_STREAM;
	char hello[]="hello world!\n";
	int nbytes;
	char buffer[1024];
	
	if(argc!=2){
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}

	if((port=atoi(argv[1]))<0){
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}

	sockfd=socket(domain, type, 0);
	if(sockfd==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}
	
	bzero(&server, sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	server.sin_port=htons(port);
	
	if(bind(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	// listen port
	if(listen(sockfd, 5)==-1){
		fprintf(stderr, "%s\n", strerror(errno));
		exit(1);
	}

	while(1){
		sin_size=sizeof(struct sockaddr_in);
		newfd=accept(sockfd, (struct sockaddr *)&client, &sin_size);
		if(newfd==-1){
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
		fprintf(stderr,"Server get connection from %s\n", inet_ntoa(client.sin_addr));
		
		if(write(newfd, hello, sizeof(hello))==-1){
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
		
		if((nbytes=read(newfd, buffer, 1024))==-1){
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}

		buffer[nbytes]='\0';
		printf("%s\n", buffer);
		
		close(newfd);	
	}
	close(sockfd);

	return 0;
}
