#include<stdio.h>
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
#include<sys/ioctl.h>
#include<sys/select.h>
#include<sys/time.h>

#define BACKLOG 16
extern int allocate_tunnel(char *, int);
<<<<<<< HEAD
extern int open_listenfd(unsigned short);
extern int open_clientfd(char *, unsigned short);
extern void *eth_thread(int ethfd);
=======
extern int open_listenfd(int);
extern int open_clientfd(char *, int);
//extern void *eth_thread(int ethfd);
extern void *eth_thread();
>>>>>>> b473439a4d6ffb7f584557aa03f09a87c4da2fc5
extern void *tap_thread(int tapfd);
ssize_t write_to_tap(int client_fd, char* buffer, size_t length);
ssize_t read_from_tap(int socket_fd, char* buffer, size_t length);
//void *tcp_handle();
