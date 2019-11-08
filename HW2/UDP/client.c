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
	char check[10];
	char message[BUFSIZE];
	int str_len, addr_size, i;

	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;

	if(argc != 3){
		printf("Usage : %s <IP> <Port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_handling("UDP socket initialize error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//	sendto(sock, "Connection\n", sizeof("Connection\n"), 0);
  //  sendto(sock, "_Establish\n", sizeof("_Establish\n"), 0);
//    sendto(sock, "ment__DONE\n", sizeof("ment__DONE\n"), 0);
	sendto(sock, "Connection\n", sizeof("Connection\n"), 0,  
                    (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	sendto(sock, "_Establish\n", sizeof("_Establish\n"), 0,        
                    (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	sendto(sock, "ment__DONE\n", sizeof("ment__DONE\n"), 0,
                    (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	while(1){
		fputs("전송할 메시지를 입력하세요 (q to quit) : ", stdout);
		fgets(message, sizeof(message), stdin);
		if(!strcmp(message, "q\n")){
//			write(sock, "q", strlen("q"));
			sendto(sock, message, strlen(message), 0,
					(struct sockaddr*)&serv_addr, sizeof(serv_addr));
			break;
		}
		sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	}
	while(write(sock, "q", strlen("q")) != -1){
	//		sendto(sock, "q", strlen("q"), 0,
      //              (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != -1){
	//	printf("1\n");
	}


	close(sock);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

