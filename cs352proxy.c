#include"cs352proxy.h"

//int tap_fd = -1;
//int eth_fd = -1;

/**************************************************
  * allocate_tunnel:
  * open a tun or tap device and returns the file
  * descriptor to read/write back to the caller
  *****************************************/
int allocate_tunnel(char *dev, int flags) {
	int fd, error;
	struct ifreq ifr;
	char *device_name = "/dev/net/tun";
	if( (fd = open(device_name , O_RDWR)) < 0 ) {
		perror("error opening /dev/net/tun");
		return fd;
	}
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if (*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}if( (error = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
		perror("ioctl on tap failed");
		close(fd);
		return error;
	}
	strcpy(dev, ifr.ifr_name);
	return fd;
}

int open_listenfd(unsigned short port){
	int listenfd;
	int optval = 1;
	struct sockaddr_in serveraddr;
	if((listenfd=socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("error creating socket\n");
		return -1;
	}	
	/* avoid EADDRINUSE error on bind() */
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
		(char *)&optval, sizeof(optval)) < 0) {
		perror("setsockopt()");
		exit(-1);
	}
	if(listen(listenfd, BACKLOG)<0){
		perror("error making socket a listening socket\n");
		return -1;
	}
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=port;
	if(bind(listenfd, (struct sockaddr *)&serveraddr,
		sizeof(struct sockaddr_in))<0){
		perror("error binding socketfd to port\n");
		return -1;
	}
	return listenfd;
}

int open_clientfd(char *hostname, unsigned short port){
	int clientfd;
	struct hostent *hp;
	struct in_addr addr;
	struct sockaddr_in serveraddr;
	if((clientfd=socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("error opening socket\n");
		return -1;
	}
	if((inet_aton(hostname, &addr))!=0)
		hp=gethostbyaddr((const char *)&addr, sizeof(struct in_addr), AF_INET);
	else if((hp=gethostbyname(hostname))==NULL){
		perror("error retrieving host information\n");
		return -1;
	}
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family=AF_INET;
	memcpy(&serveraddr.sin_addr, hp->h_addr_list, hp->h_length);
	serveraddr.sin_port=htons(port);
	if(connect(clientfd, (struct sockaddr *)&serveraddr,
		sizeof(struct sockaddr_in))<0){
		perror("error connecting to server\n");
		return -1;
	}
	return clientfd;
}

void *eth_thread(thread_param *tp){
	ssize_t size;
	unsigned int short type, length;
	char buffer[BUFSIZE];
	memset(buffer, 0, BUFSIZE);
	while(1){
		size = read(tp->ethfd, buffer, BUFSIZE);
		if(size < 1){
			fprintf(stderr, "error, not connected");
			close(tp->ethfd);
			close(tp->tapfd);
			return -1;
		}
		type = ntohs((unsigned short *)(buffer[0]));
		length=ntohs((unsigned short *)(buffer[1]));
		if(type != 0xABCD){
			fprintf(stderr, "error, incorrect type");
			close(tp->ethfd);
			close(tp->tapfd);
			return -1;
		}
		write(tp->tapfd, buffer+4, BUFSIZE-4);
	}
}

void *tap_thread(thread_param *tp){
	ssize_t size;
	unsigned int short type, length;
	char buffer[BUFSIZE];
	memset(buffer, 0, BUFSIZE);
	while(1){
		size = read(tp->tapfd, buffer, sizeof(buffer));
		if(size < 1){	
			fprintf(stderr, "error, not connected");
			close(tp->ethfd);
			close(tp->tapfd);
			return -1;
		}
		buffer[size] = '\0';
		type = htons(0xABCD);  
		length = htons(size);
		/**
		  * type, length and buffer are stored in the stack
		  * adjacent to each other in order.
		  */
		write(tp->ethfd, &type, BUFSIZE+4); //size of unsigned int short is 2
	}
} 
