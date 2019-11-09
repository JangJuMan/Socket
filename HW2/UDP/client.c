#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#define BUFSIZE 2048

void error_handling(char* message);

int main(int argc, char** argv){
	int sock;
	int str_len, addr_size, serv_addr_size, recv_offset, file_size;
    int temp, cnt, curr_send;

	struct sockaddr_in serv_addr;
	struct sockaddr_in from_addr;
	struct timeval optVal = {5, 0};

	int optLen = sizeof(optVal);

	char message[BUFSIZE];	 

	if(argc != 4){
		printf("Usage : %s <IP> <Port> <filename>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		error_handling("UDP socket initialize error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &optVal, optLen);

	connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	serv_addr_size = sizeof(serv_addr);

	while(1){
		sendto(sock, argv[3], strlen(argv[3]), 0,
		            (struct sockaddr*)&serv_addr, sizeof(serv_addr));

		recv_offset = recv_offset = recvfrom(sock, message, BUFSIZE, 0,
                (struct sockaddr*)&serv_addr, &serv_addr_size);
		if(recv_offset == -1){
			continue;
		}
		if(!strcmp(message, "YES")){
			break;
		}
	}

	FILE* fp = fopen(argv[3], "rb");

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    while((temp = fread(message, 1, BUFSIZE, fp)) > 0){
        if((curr_send = send(sock, message, temp, 0)) < 0)
            error_handling("\nsend error\n");
        cnt += curr_send;
		usleep(100000);
        printf("%d / %d\r", cnt, file_size);
    }
    fclose(fp);
    close(sock);
    return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

