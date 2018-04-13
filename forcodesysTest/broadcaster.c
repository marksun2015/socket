/*
** broadcaster.c -- 一個類似 talker.c 的 datagram "client"，
** 差異在於這個可以廣播
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT 1740 // 所要連線的 port

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in their_addr; // 連線者的位址資訊
  struct hostent *he;
  int numbytes;
  int broadcast = 1;
  //char broadcast = '1'; // 如果上面這行不能用的話，改用這行

  if (argc != 3) {
    fprintf(stderr,"usage: broadcaster hostname message\n");
    exit(1);
  }

  if ((he=gethostbyname(argv[1])) == NULL) { // 取得 host 資訊
    perror("gethostbyname");
    exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  // 這個 call 就是要讓 sockfd 可以送廣播封包
  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
    sizeof broadcast) == -1) {
    perror("setsockopt (SO_BROADCAST)");
    exit(1);
  }

  their_addr.sin_family = AF_INET; // host byte order
  their_addr.sin_port = htons(SERVERPORT); // short, network byte order
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

  if ((numbytes=sendto(sockfd, argv[2], strlen(argv[2]), 0,
          (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
    perror("sendto");
    exit(1);
    }

  printf("sent %d bytes to %s\n", numbytes,
      inet_ntoa(their_addr.sin_addr));

  close(sockfd);

  return 0;
}
