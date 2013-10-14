#include"cs352proxy.h"

int main(int argc, char **argv){
	char *c;
	int ethfd, tapfd;
	unsigned short port;
	switch(argc){
		// 1st proxy
		case 3:
			for(c=argv[1]; isdigit(*c)||*c=='\0'; c++);
			if(*c='\0'){
				port=(unsigned short)atoi(argv[1]);
				if(port<1024){
					perror("ERROR: port must be from "
						"1024-65535.\n");
					exit(1);
				}
			}
			else{
				perror("ERROR: port parameter "
					"not an unsigned short.\n");
				exit(1);
			}
			if((tapfd=allocate_tunnel(argv[2], IFF_TAP|IFF_NO_PI))
				<0){
				perror("Opening tap interface failed!\n");
				exit(1);
			}
			break;

			/**
			  * 1st thread listens to TCP socket
			  * 2nd thread listens to tap device
			  */
			eth->sin_addr.s_addr=htonl(INADDR_ANY);
			eth->sin_port=htons(port);
			if(bind(ethfd, (struct sockaddr *)&eth, sizeof(eth))<0){
				perror("error binding ethfd to port\n");
				exit(1);
			}
			if(listen(ethfd, BACKLOG)<0){
				perror("error converting ethfd into listening socket\n");
				exit(1);
			}

			tap->sin_addr.s_addr=htonl(INADDR_ANY);
			tap->sin_port=htons(port);
			if(bind(tapfd, (struct sockaddr *)&tap, sizeof(tap))<0){
				perror("error binding tapfd to port\n");
				exit(1);
			}
			if(listen(tapfd, BACKLOG)<0){
				perror("error converting tapfd into listening socket\n");
				exit(1);
			}
		// 2nd proxy
		case 4:
			break;
		default:
			perror("ERROR: invalid parameters.\n");
			exit(1);
	}
}
