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
	char buffer[BUFSIZE];
	unsigned int short type, length;
	memset(buffer, 0, BUFSIZE));
	while(1){
		size = read(tp->ethfd, buffer, sizeof(buffer));
		if(size < 1){
			fprintf(stderr, "error, not connected");
			exit(-1);
		}
		type = ntohs(type);
		if(type != 0xABCD){
			fprintf(stderr, "error, incorrect type");
			exit(-1);
		}
		read(tp->ethfd, &size, 2);
		length = ntohs(size);
		read(tp->ethfd, buffer, length);
		write(tp->tapfd, buffer, length);
	}
}

void *tap_thread(thread_param *tp){
	ssize_t size;
	char buffer[BUFSIZE];
	unsigned int short type, length;
	memset(buffer, 0, BUFSIZE);
	while(1){
		size = read(tp->tapfd, buffer, sizeof(buffer));
		if(size < 1){	
			fprintf(stderr, "error, not connected");
			exit(-1);
		}
		buffer[size] = '\0';
		type = htons(type);  
		length = htons(size);
		if(type != 0xABCD){
			fprintf(stderr, "error, type not 16 bit");
			exit(-1);
		}
		write(tp->ethfd, &type, 2); //size of unsigned int short is 2
		write(tp->ethfd, &length, 2);
		write(tp->ethfd, buffer, size); 
	}
} 

/*not used may need to delete later*/
ssize_t write_to_tap(int client_fd, char* buffer, size_t length){
	ssize_t written, counter=0;
	while(length > 0){
		written = write(client_fd, buffer, length);
		if(written == -1){
			fprintf(stderr, "error, failed to write");
			return -1;
		}
		counter = counter + written;
		length = length - written;
	}
	return counter;
}
/*not used may need to delete later*/
ssize_t read_from_tap(int socket_fd, char* buffer, size_t length){
	ssize_t currRead, counter=0;
	while(length > 0){
		currRead = read(socket_fd, buffer, length);
		if(currRead == -1){
			fprintf(stderr, "error, failed to read from tap");
			return -1;	
		}
		counter = counter + currRead;
		length = length - currRead;
	}
	return counter;
}
