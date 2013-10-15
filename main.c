#include "cs352proxy.h"

int main(int argc, char **argv){
	char *c;
	int ethfd, tapfd;
	unsigned short port;
	thread_param tp;
	pthread_t eth_tid, tap_tid;
	
	char *remote_host;
	int remote_port;
	char *local_interface;

	if(argc != 4 || argc != 3){
		printf("Error\n\t Usage for first proxy:\n\t\t cs352proxy <port> <local interface> \n\t Usage for second proxy: \n\t\t cs352proxy <remote host> <remote port> <local interface>\n");
	return -1;
	}

	switch(argc){
		// 1st proxy
		case 3:
			for(c=argv[1]; isdigit(*c)||*c=='\0'; c++);
			if(*c=='\0'){
				ethfd=atoi(argv[1]);
				if(ethfd<1024||ethfd>65535){
					perror("ERROR: port must be from "
						"1024-65535.\n");
					exit(1);
				}
				port=(unsigned short)ethfd;
			}
			else{
				perror("ERROR: port parameter "
					"not a decimal number.\n");
				exit(1);
			}
			if((ethfd=open_listenfd(port))<0){
				perror("error opening ethernet device\n");
				exit(1);
			}
			if((tapfd=allocate_tunnel(argv[2], IFF_TAP|IFF_NO_PI))<0){
				perror("error opening tap device\n");
				exit(1);
			}

			/**
			  * 1st thread listens to TCP socket
			  * 2nd thread listens to tap device
			  */

			tp.ethfd=ethfd;
			tp.tapfd=tapfd;
			Pthread_create(&eth_tid, NULL, eth_thread, &tp);
			Pthread_create(&tap_tid, NULL, tap_thread, &tp);
			Pthread_join(eth_tid, NULL);
			Pthread_join(tap_tid, NULL);
			close(ethfd);
			close(tapfd);
			break;

		// 2nd proxy
		case 4:
			remote_host = argv[1];
			remote_port = atoi(argv[2]);
			local_interface = argv[3];

			tp.ethfd = open_clientfd(remote_host, remote_port);
			tp.tapfd = allocate_tunnel(local_interface, IFF_TAP|IFF_NO_PI);

			tp.ethfd=ethfd;
			tp.tapfd=tapfd;
			Pthread_create(&eth_tid, NULL, eth_thread, &tp);
			Pthread_create(&tap_tid, NULL, tap_thread, &tp);
			Pthread_join(eth_tid, NULL);
			Pthread_join(tap_tid, NULL);
			close(ethfd);
			close(tapfd);
			
			break;
		default:
			perror("ERROR: invalid parameters.\n");
			exit(1);
	}
	return 0;
}
