#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>
#include<errno.h>
#include<fcntl.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<net/if.h>
#include<arpa/inet.h>
#include<linux/if_tun.h>
#include<sys/socket.h>
#include<netdb.h>
#include<sys/ioctl.h>
#include<sys/select.h>
#include<sys/time.h>

#define BACKLOG 16
//	MTU for eth0 and tap2 for both machines are 1500.
#define BUFSIZE 1500

typedef struct thread_param{
	int ethfd;
	int tapfd;
} thread_param;

extern int allocate_tunnel(char *, int);
extern int open_listenfd(unsigned short);
extern int open_clientfd(char *, unsigned short);
extern void *eth_thread(thread_param *tp);
extern void *tap_thread(thread_param *tp);
extern ssize_t write_to_tap(int client_fd, char* buffer, size_t length);
extern ssize_t read_from_tap(int socket_fd, char* buffer, size_t length);
