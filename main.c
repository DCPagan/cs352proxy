int main(int argc, char **argv){
	char *c;
	int ethfd, tapfd;
	unsigned short port;
	struct sockaddr_in *eth, *tap;
	eth=malloc(sizeof(struct sockaddr_in));
	memset(*eth, 0, sizeof(struct sockaddr_in));
	tap=malloc(sizeof(struct sockaddr_in));
	memset(*tap, 0, sizeof(struct sockaddr_in));
	ethfd=socket(AF_INET, SOCK_STREAM, 0);
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
		// 2nd proxy
		case 4:
			break;
		default:
			perror("ERROR: invalid parameters.\n");
			exit(1);
	}
}
