#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

void error_handling(char* message);

int main(int argc, char **argv){
	int serv_sock, clnt_sock, clnt_addr_size;
	int str_len;
	int buffer_size = 1024;

	char message[buffer_size];
	char* ptr;
	char* file_name;
	FILE* fp;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	// Connection Setting
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock=accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
	if(clnt_sock == -1)
		error_handling("accecpt() error");


	// Receieve Data
	str_len = recv(clnt_sock, message, buffer_size-1, 0);
	ptr = strtok(message, " \0");
	file_name = malloc(sizeof(char) * (strlen(message)+1));
	strcpy(file_name, message);
	fp = fopen(file_name, "wb");
	ptr = strtok(NULL, " \0");

	printf("[ downloading file... ]\n");
	while((str_len = recv(clnt_sock, message, buffer_size-1, 0)) != 0){
		fprintf(fp, "%s", message);
		memset(message, 0, sizeof(message));
	}
	printf("\n\n[ downloading done..! ]\n");
	free(file_name);
	close(clnt_sock);
	fclose(fp);

	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
