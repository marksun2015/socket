#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CMD_WPA_SUPPLICANT      ("wpa_supplicant")


int main(int argc, char *argv[]) {
	int SERVER_PORT = 8877;

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	int listen_sock;
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("could not create listen socket\n");
		return 1;
	}

	if ((bind(listen_sock, (struct sockaddr *)&server_address,
	          sizeof(server_address))) < 0) {
		printf("could not bind socket\n");
		return 1;
	}

	int wait_size = 16;  // maximum number of waiting clients, after which
	                     // dropping begins
	if (listen(listen_sock, wait_size) < 0) {
		printf("could not open socket for listening\n");
		return 1;
	}

	struct sockaddr_in client_address;
	int client_address_len = 0;

	// run indefinitely
	while (true) {
		int sock;
		if ((sock = accept(listen_sock, (struct sockaddr *)&client_address,
		                &client_address_len)) < 0) {
			printf("could not open a socket to accept data\n");
			return 1;
		}

		int n = 0;
		int len = 0, maxlen = 100;
		char buffer[maxlen];
		char *pbuffer = buffer;

		printf("client connected with ip address: %s\n",
		       inet_ntoa(client_address.sin_addr));

		while ((n = recv(sock, pbuffer, maxlen, 0)) > 0) {
			//pbuffer += n;
			//maxlen -= n;
			//len += n;
			printf("received: '%s'\n", buffer);
			if (!memcmp(pbuffer,CMD_WPA_SUPPLICANT,strlen(CMD_WPA_SUPPLICANT))) {
				printf("match \n");
				// echo run result
				//send(sock, buffer, len, 0);
			}

		}

		close(sock);
	}

	close(listen_sock);
	return 0;
}
