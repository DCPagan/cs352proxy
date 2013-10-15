#include"cs352proxy.h"

int tap_fd = -1;
int tcp_fd = -1;

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
	struct sockaddr_in serveraddr;
	if((listenfd=socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("error creating socket\n");
		return -1;
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
		hp=gethostbyaddr((const char *)&addr, sizeof(in_addr), AF_INET);
	else if((hp=gethostbyname(hostname))==NULL){
		perror("error retrieving host information\n");
		return -1;
	}
	memset(&serveraddr, 0, sizeof(sockaddr_in));
	serveraddr.sin_family=AF_INET;
	memcpy(hp->h_addr_list[0], &serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port=htons(port);
	if(connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(sockaddr_in))<0){
		perror("error connecting to server\n");
		return -1;
	}
	return clientfd;
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

void *eth_thread(){
	ssize_t size;
	char buffer[1500]; //Do we just pick a buffer size?
	memset(buffer, '0', sizeof(buffer));
	while(1){
		size = read(tap_fd, buffer, sizeof(buffer));
		if(size < 1){	
			fprintf(stderr, "error, not connected");
			exit(-1);
		}
		buffer[size] = '\0';
		unsigned int short type, length;
		type = htons(type);   //or is it ntohs()?
		length = htons(length);
		if(type != 0xABCD){
			fprintf(stderr, "error, type not 16 bit");
			exit(-1);
		}
		write(tcp_fd, &type, 2); //size of unsigned int short is 2
		write(tcp_fd, &length, 2);
		write(tcp_fd, buffer, size); 
		
	}
} 
/*
void *eth_thread(int ethfd){
	while(1){

	}
}
*/
void *tap_thread(int tapfd){
	while(1){
		/**
		  * Insert tap device handling code here.
		  */
	}
}
