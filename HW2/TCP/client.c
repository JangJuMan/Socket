#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#define BUFSIZE 100
void error_handling(char* message);

int main(int argc, char** argv){
	int sock;
	char message[BUFSIZE];
	int str_len;
	struct sockaddr_in serv_addr;

	if(argc != 3){
		printf("Usage : %s <IP> <Port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	
	send(sock, "Connection\n", sizeof("Connection\n"), 0);
	send(sock, "_Establish\n", sizeof("_Establish\n"), 0);
	send(sock, "ment__DONE\n", sizeof("ment__DONE\n"), 0);

	while(1){
		fputs("전송할 메시지를 입력하세요 (q to quit) : ", stdout);
		fgets(message, BUFSIZE, stdin);

		if(!strcmp(message, "q\n"))
			break;
		send(sock, message, strlen(message), 0);
	}

	close(sock);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

