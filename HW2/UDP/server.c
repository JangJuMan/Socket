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
	int buf_size = 2048, clnt_addr_size, recv_offset, check = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	char* file_name;
	char message[buf_size];

	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1)
		error_handling("UDP socket 생성 error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");


	clnt_addr_size = sizeof(clnt_addr);
	printf("[ downloading file... ]\n");
	recv_offset = recvfrom(serv_sock, message, buf_size, 0,
            (struct sockaddr*)&clnt_addr, &clnt_addr_size);

	file_name = malloc(sizeof(char) * (strlen(message)));
	strcpy(file_name, message);
	memset(message, 0, sizeof(message));

	sendto(serv_sock, "YES", strlen("YES"), 0,
	    (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));

	FILE* fp = fopen(file_name, "wb");

	while(1){
		memset(message, 0, sizeof(message));
		recv_offset = recvfrom(serv_sock, message, buf_size, 0,
				(struct sockaddr*)&clnt_addr, &clnt_addr_size);
		
		if(!strcmp(file_name, message) && check == 0){
			sendto(serv_sock, "YES", strlen("YES"), 0,
                (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
		}
		else{
			check = 1;
			fprintf(fp, "%s", message);
		}
	}
	printf("\n\n[ downloading done..! ]\n");
	fclose(fp);
	return 0;


}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
