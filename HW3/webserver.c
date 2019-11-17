#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

void error_handling(char* message);

int main(int argc, char* argv[]){
	int serv_sock, clnt_sock, clnt_addr_size;;
	int str_len, bufSize = 2048, file_size, file_read;

	char message[bufSize];
	char temp[bufSize];
	char response_header[bufSize];
	char post_response[bufSize];
	char* ptr;
	char* type;
	char* request;
	char* response = malloc(sizeof(char) * bufSize);
	char name[1000];
	char snumber[1000];

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	FILE* fp;

	if(argc != 2){
		printf("Usage : %s <port_number>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1){
		error_handling("socket error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind error");

	if(listen(serv_sock, 5) == -1)
		error_handling("listen error");

	while(1){
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept error");
	
		str_len = recv(clnt_sock, message, bufSize-1, 0);
		write(1, message, str_len);
		strcpy(temp, message);
		ptr = strtok(message, " ");
		type = malloc(sizeof(char) * (strlen(message) + 1));
		strcpy(type, message);
		ptr = strtok(NULL, " ");
		request = malloc(sizeof(char) * (strlen(message) +1));
		strcpy(request, ptr);
	
		if(!strcmp("GET", type)){
			if(!strcmp(request, "/index.html") || !strcmp(request, "/")){
				strcpy(response_header,"HTTP/1.1 200 OK\n\n");
				fp = fopen("index.html", "rb");
			}
			else if(!strcmp(request, "/query.html")){
				strcpy(response_header, "HTTP/1.1 200 OK\n\n");
				fp = fopen("query.html", "rb");
			}
			else {
				char err_message[1000] = "HTTP/1.1 404 NOT FOUND\n\n";
				send(clnt_sock, err_message, strlen(err_message), 0);
				memset(message, 0, sizeof(message));
				free(type);
				free(request);
				close(clnt_sock);
				continue;
			}
			
			if(fp == NULL){
				printf("file open error\n");
			}

			fseek(fp, 0, SEEK_END);
			file_size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			strcpy(response, response_header);
			memset(message, 0, sizeof(message));
			while((file_read = fread(message, 1, bufSize, fp)) > 0){
				response = realloc(response ,sizeof(char) * (strlen(response) + strlen(message) + 1));
				strcat(response, message);
			}
			send(clnt_sock, response, strlen(response), 0);
			fclose(fp);
		}
		else if(!strcmp("POST", type)){
			ptr = strstr(temp, "name=");
			strcpy(post_response, "HTTP/1.1 200 OK\n\n<body><h2>");
			strcat(post_response, ptr);
			strcat(post_response, "</h2></body>");
			send(clnt_sock, post_response, strlen(post_response), 0);
			memset(temp, 0, sizeof(temp));
			memset(post_response, 0, sizeof(post_response));
			free(type);
			free(request);
			close(clnt_sock);
			continue;
		}
		memset(message, 0, sizeof(message));
		free(type);
		free(request);
		memset(response, 0, sizeof(response));
		memset(response_header, 0, sizeof(response_header));

		close(clnt_sock);
	}

	free(response);
	close(serv_sock);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
