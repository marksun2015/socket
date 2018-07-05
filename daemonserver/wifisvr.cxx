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



#define WPA_TMP_SUPP_CONF "/tmp/wpa_supplicant.conf"
#define WIFI_ENABLE_FLAG "/etc/flags/flag_wifi"
#define CMD_WPA_SUPPLICANT  ("wpa_supplicant")
#define WPA_PROCESS "wpa_supplicant"
#define WPA_DAEMON_PATH "/var/run/wpa_supplicant" 

sem_t mutex;

#if 0
void init_daemon(void) 
{ 
    int pid; 
    int i;
    if(pid=fork()) 
       exit(0); 
    else if(pid< 0) 
       exit(1);

    setsid();

    if(pid=fork()) 
       exit(0);
    else if(pid< 0) 
       exit(1); 

    for(i=0;i< NOFILE;++i)
        close(i); 
    chdir("/");
    umask(0); 
    return;
}
#endif

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
#if 0
void fork_exec(const std::string& path, const std::string& para)
{
    pid_t pid = fork();
    if (pid != 0)
    {
        if (pid > 0) waitpid(pid, 0, 0);
    }
    else
    {  //child1
        pid = fork();
        if (pid != 0)
        {
            exit(0);
        }
        else
        {//child2
            struct rlimit rlim;
            if (0 == getrlimit(RLIMIT_NOFILE, &rlim))
            {
                int maxFD = rlim.rlim_cur;
                for (int i = 0; i < maxFD; ++i)
                {
                    fcntl(i, F_SETFD, fcntl(i, F_GETFD) | FD_CLOEXEC);
                }
            }
            if (para.empty())
            {
                if (-1 == execl(path.c_str(), path.c_str(), NULL))
                {
                    exit(0);
                }
            }
            else
            {
                if (-1 == execl(path.c_str(), path.c_str(), para.c_str(), NULL))
                {
                    exit(0);
                }
            }

        }
    }
}
#endif

int cmd_wpa_supplicant(char *pbuffer)
{
	sem_wait(&mutex);
	FILE *fr, *fw;
	int val = 0, ret;
	char cmd[256];//test
	
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

	fw = fopen( WPA_TMP_SUPP_CONF, "w+"); 
	fprintf(fw,"ctrl_interface=%s\nupdate_config=1\n", WPA_DAEMON_PATH);
	fclose(fw);

	//snprintf(cmd, 256,"wpa_supplicant -d -Dnl80211 -c%s -iwlan0 -B", WPA_TMP_SUPP_CONF);
	//ret = system(cmd);

	//snprintf(cmd,sizeof(pbuffer),"%s", pbuffer);
	//snprintf(cmd,65,"%s", pbuffer);
	//ret = system(cmd);

	//snprintf(cmd,256,"echo %s >> /home/log.txt", pbuffer);
	snprintf(cmd,256,"echo startwifi >> /home/log.txt");
	ret = system(cmd);

	sem_post(&mutex);
	return 0;
}

int main(int argc, char *argv[]) {
	sem_init(&mutex, 0, 1);
	int SERVER_PORT = 8877;
	//int ret;
	char cmd[256];//test

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
	//int client_address_len = 0;
	socklen_t client_address_len = 0;

					sprintf(cmd,"echo startwifi >> /home/log1.txt");
					system(cmd);
	// run indefinitely
	while (true) {
		int sock;
		if ((sock = accept(listen_sock, (struct sockaddr *)&client_address, 
						&client_address_len)) < 0) {
			printf("could not open a socket to accept data\n");
			return 1;
		}

		int n = 0;
		int ret,maxlen = 256;
		char buffer[maxlen];
		char *pbuffer = buffer;
		printf("client connected with ip address: %s\n",
		       inet_ntoa(client_address.sin_addr));

		while ((n = recv(sock, pbuffer, maxlen, 0)) > 0) {
			if (!memcmp(pbuffer,CMD_WPA_SUPPLICANT,strlen(CMD_WPA_SUPPLICANT))) {
					snprintf(cmd,256,"echo startwifi_%s >> /home/log1.txt",pbuffer);
					ret = system(cmd);
				ret = cmd_wpa_supplicant(pbuffer);
			}
		}
		close(sock);
	}
	sem_destroy(&mutex);
	close(listen_sock);
	return 0;
}
