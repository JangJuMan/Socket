#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#define BUFSIZE 1024
void error_handling(char* message);

int main(int argc, char** argv){
	int sock, cnt, str_len, file_size;
	int temp, curr_send;

	char message[BUFSIZE];
	char* file_name;
	char* file_buf;
	
	FILE* fp;

	struct sockaddr_in serv_addr;

	if(argc != 4){
		printf("Usage : %s <IP> <Port> <file name to send>\n", argv[0]);
		exit(1);
	}

	// Connection Setting
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	
	file_name = malloc(sizeof(char) * (strlen(argv[3]) + 2));
	strcpy(file_name, argv[3]);
	strcat(file_name, " \0");

	fp = fopen(argv[3], "rb");
	if(fp == NULL){
		printf("file open error\n");
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	send(sock, file_name, strlen(file_name), 0);

	memset(message, 0, sizeof(message));
	while((temp = fread(message, 1, BUFSIZE, fp)) > 0){
		if((curr_send = send(sock, message, temp, 0)) < 0)
			error_handling("\nsend error\n");
		cnt += curr_send;
		//memset(message, 0, sizeof(message));
		printf("%d / %d\r", cnt, file_size);
	}
	printf("\n");
	fclose(fp);
	close(sock);

	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

