#include"cs352proxy.h"
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

int open_listenfd(int port){
	int listenfd;
	struct sockaddr_in *serveraddr;
	if((listenfd=socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("error creating socket\n");
		exit(1);
	}
	if(listen(listenfd, BACKLOG)<0){
		perror("error making socket a listening socket\n");
		exit(1);
	}
	serveraddr=(struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr->sin_family=AF_INET;
	serveraddr->sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr->sin_port=port;
	if(bind(listenfd, (struct sockaddr *)serveraddr,
		sizeof(struct sockaddr_in))<0){
		perror("error binding socketfd to port\n");
		exit(1);
	}
	return listenfd;
}
