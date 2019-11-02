#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>


void error_handling(char* message);

int main(int argc, char **argv){
	int serv_sock;
	char check[10];
	char reACK[] = "reACK";
	char okACK[] = "okACK";
	int str_len, num = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	struct timeval optVal = {10, 0};

	int optLen = sizeof(optVal);
	int clnt_addr_size;
	int cnt = 0;

	if(argc!=3){
		printf("Usage : %s <port> <buffer size>\n", argv[0]);
		exit(1);
	}
	int buf_size = atoi(argv[2]);
	char message[buf_size];

	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1)
		error_handling("UDP socket 생성 error");

	//set timeout
	setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	int check_len;
	sleep(5);
	while(1){
		clnt_addr_size = sizeof(clnt_addr);
		str_len = recvfrom(serv_sock, message, buf_size, 0,
				(struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(str_len == -1){
			printf("[Timeout] process done\n", stderr);
			break;
		}

		if(message[0] == 'q' && str_len == 1){
			printf("[client terminated!] process done\n", stderr);
			break;
		}

		write(1, message, str_len);
	}

	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
