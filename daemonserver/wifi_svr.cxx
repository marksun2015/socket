#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h> 
#include <sys/param.h> 
#include <semaphore.h>
#include <dirent.h>
#include <sys/wait.h> 

#ifdef DEBUG
#define log_debug( x... )  fprintf(stdout, ##x )
#else
#define log_debug( x... )
#endif

#define CMD_WPA_SUPPLICANT  ("wpa_supplicant")
#define WPA_PROCESS "wpa_supplicant"
#define CMD_SIZE 256
#define WIFI_SVR_PORT 15858

sem_t mutex;

int cmd_wpa_supplicant(char *pbuffer, int data_len)
{
	int ret = 0;
	char cmd[CMD_SIZE];

	sem_wait(&mutex);

	if (data_len > CMD_SIZE){
		return 1;
	}

	//snprintf in gcc need consider '\0', so data_len plus 1
	snprintf(cmd, data_len+1, "%s", pbuffer); 
	ret = system(cmd);

	sem_post(&mutex);

	return 0;
}

int main(int argc, char *argv[]) 
{
	sem_init(&mutex, 0, 1);
	int SERVER_PORT = WIFI_SVR_PORT;
	int listen_sock;
	int wait_size = 16;  // maximum number of waiting clients, after which
						// dropping begins
	struct sockaddr_in client_address;
	socklen_t client_address_len = 0;

	int sock;
	int n = 0,ret = 0;
	char buffer[CMD_SIZE];
	char *pbuffer = buffer;

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		log_debug("could not create listen socket\n");
		return 1;
	}

	if ((bind(listen_sock, (struct sockaddr *)&server_address,
					sizeof(server_address))) < 0) {
		log_debug("could not bind socket\n");
		return 1;
	}

	if (listen(listen_sock, wait_size) < 0) {
		log_debug("could not open socket for listening\n");
		return 1;
	}

	while (true) {
		if ((sock = accept(listen_sock, (struct sockaddr *)&client_address, 
						&client_address_len)) < 0) {
			log_debug("could not open a socket to accept data\n");
			return 1;
		}

		n = 0;
		ret = 0;
		memset(buffer,0,CMD_SIZE);

		log_debug("client connected with ip address: %s\n",
				inet_ntoa(client_address.sin_addr));

		while ((n = recv(sock, pbuffer, CMD_SIZE, 0)) > 0) {
			if (!memcmp(pbuffer,CMD_WPA_SUPPLICANT,strlen(CMD_WPA_SUPPLICANT))) {
				ret = cmd_wpa_supplicant(pbuffer,n);
			}
		}

		close(sock);
	}

	sem_destroy(&mutex);
	close(listen_sock);
	return 0;
}
