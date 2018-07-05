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

#include <fstream> 
#include <iostream>
#include <string>

#define WPA_TMP_SUPP_CONF "/tmp/wpa_supplicant.conf"
#define WIFI_ENABLE_FLAG "/etc/flags/flag_wifi"
#define WPA_DAEMON_PATH "/var/run/wpa_supplicant" 
#define CMD_WPA_SUPPLICANT  ("wpa_supplicant")
#define WPA_PROCESS "wpa_supplicant"
#define CMD_SIZE 256
#define WIFI_SVR_PORT 15858

sem_t mutex;

/*
 * get process pid by process name
 *
 * @param ProcName: process name
 * @param foundpid: return pid
 *
 * @return 0: Success
 *        1: Failed
 */
int w_find_pid_by_name(char* ProcName, int* foundpid)
{
        DIR             *dir;
        struct dirent   *d;
        int             pid, i; 
        char            *s;
        int pnlen;
        i = 0;
        foundpid[0] = 0;
        pnlen = strlen(ProcName);

        /* Open the /proc directory. */
        dir = opendir("/proc");
        if (!dir)
        {
                printf("cannot open /proc");
                return 1;
        }
        /* Walk through the directory. */
        while ((d = readdir(dir)) != NULL) {
                char exe [PATH_MAX+1];
                char path[PATH_MAX+1];
                int len;
                int namelen;
                /* See if this is a process */
                if ((pid = atoi(d->d_name)) == 0)       continue;

                snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
                if ((len = readlink(exe, path, PATH_MAX)) < 0)
                        continue;
                path[len] = '\0';

                /* Find ProcName */
                s = strrchr(path, '/');
                if(s == NULL) continue;
                s++;

                /* we don't need small name len */
                namelen = strlen(s);
                if(namelen < pnlen)     continue;

				if(!strncmp(ProcName, s, pnlen)) {
                        /* to avoid subname like search proc tao but proc taolinke matched */
                        if(s[pnlen] == ' ' || s[pnlen] == '\0') {
                                foundpid[i] = pid;
                                i++;
                        }
                }
        }
        foundpid[i] = 0;
        closedir(dir);
        return  0;
}


int w_status_program(char* name)
{
	int i = 0, ret, pid_t[256];
	ret = w_find_pid_by_name(name, pid_t);
	if(!ret) {
		for(i=0; pid_t[i] != 0; i++) {
			printf("find %s process pid id %d\n", name, pid_t[i]);
			return 0;
		}
	}
	return 1;
}

int cmd_wpa_supplicant(char *pbuffer, int data_len)
{
	sem_wait(&mutex);
	FILE *fr, *fw;
	int val = 0, ret;
	char cmd[CMD_SIZE];
	
	snprintf(cmd,256,"echo cmd_wpa_supplicant1:%d >> /home/log.txt",data_len);
	ret = system(cmd);
	
	if (data_len > CMD_SIZE){
		return 1;
	}

	fr = fopen(WIFI_ENABLE_FLAG, "r");
	if(fr){
		ret = fscanf(fr,"%d",&val);
		fclose(fr);
	}

	if(val == 0){
		return 1;
	}

	if(!w_status_program((char*)WPA_PROCESS)) {
		return 1;
	}

	fw = fopen(WPA_TMP_SUPP_CONF, "w+"); 
	fprintf(fw,"ctrl_interface=%s\nupdate_config=1\n", WPA_DAEMON_PATH);
	fclose(fw);

	snprintf(cmd,data_len+1,"%s", pbuffer); //snprintf in gcc need plus 1 for '\0'
	ret = system(cmd);

	snprintf(cmd,256,"echo cmd_wpa_supplicant2 >> /home/log.txt");
	ret = system(cmd);

	sem_post(&mutex);
	return 0;
}

int main(int argc, char *argv[]) {
	sem_init(&mutex, 0, 1);
	int SERVER_PORT = WIFI_SVR_PORT;

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
	socklen_t client_address_len = 0;

	while (true) {
		int sock;
		if ((sock = accept(listen_sock, (struct sockaddr *)&client_address, 
						&client_address_len)) < 0) {
			printf("could not open a socket to accept data\n");
			return 1;
		}

		int n = 0,ret = 0;
		char buffer[CMD_SIZE];
		char *pbuffer = buffer;
		printf("client connected with ip address: %s\n",
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
