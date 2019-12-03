#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<math.h>

#define BUFSIZE 100
void error_handling(char* message);

int main(int argc, char** argv){
	int sock;
	char send_message[BUFSIZE];
	char recv_message[BUFSIZE];
	int str_len, i;
	char op;
	int start_addr, start_addr_1st, start_addr_2nd, number_of_coils, number_of_coils_1st, number_of_coils_2nd;
	int loop = 1, control_data, control_data_1st, control_data_2nd, bc, send_size;
	struct sockaddr_in serv_addr;

	if(argc != 2){
		printf("Usage : %s <Modbus-TCP_server_IP_address> \n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(502);
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	

	send_message[0] = 0;	
	send_message[1] = 0;	
	send_message[2] = 0;	
	send_message[3] = 0;
	send_message[4] = 0;	
	

	while(loop == 1){
		printf("\n========================================================\n");
		printf("\t\tPlease Choose MODBUS Menu  \n");
		printf("========================================================\n\n");
		printf("\t[r] : Read Coils\n");
		printf("\t[w] : Write Multiple Coils\n");
		printf("\t[R] : Read Holding registers\n");
		printf("\t[W] : Write multiple Holding Registers\n");
		printf("\t[q] : Quit\n");
		printf("\n >>> INPUT : ");
		scanf(" %c", &op);
		switch (op)
		{
		case 'r':
			printf("> Please Input <Start address> and <Number of coils to read>! : ");
			scanf("%d %d", &start_addr, &number_of_coils);
			start_addr = start_addr % 65536;
			start_addr_1st = start_addr / 256;
			start_addr_2nd = start_addr % 256;
			number_of_coils = number_of_coils % 65536;
			number_of_coils_1st = number_of_coils / 256;
			number_of_coils_2nd = number_of_coils % 256;

			send_message[5] = 6;
			send_message[6] = 1;
			send_message[7] = 1;
			send_message[8] = start_addr_1st;	// 읽어올 주소 시작 1Byte = 8bits = 256경우의 수
			send_message[9] = start_addr_2nd;	
			send_message[10] = number_of_coils_1st;	// 몇개나 읽을 것인가?
			send_message[11] = number_of_coils_2nd;	
			send(sock, send_message, 12, 0);
			str_len = recv(sock, recv_message, BUFSIZE, 0);
			

			printf("\n\n\t--------< DATA RXed in HEX(read) >--------\n");
			printf("\t>\t[FC] : 0x%02x  \n\t>\t[BC] : 0x%02x\t\n", recv_message[7], recv_message[8]);
			printf("\t>\t[DATA] : ");
			for(i=9; i<9 + recv_message[8]; i++){
				printf("%02x  ", recv_message[i]);
			}
			printf("\n\t------------------------------------------\n\n\n");
			memset(recv_message, 0, sizeof(recv_message));
			break;

		case 'w':
			printf("> Please Input <Starting Control Address> <Boundary of Control Address> and <Control Data>! : ");
			scanf("%d %d %d", &start_addr, &number_of_coils, &control_data);
			start_addr = start_addr % 65536;
			start_addr_1st = start_addr / 256;
			start_addr_2nd = start_addr % 256;
			number_of_coils = number_of_coils % 65536;
			number_of_coils_1st = number_of_coils / 256;
			number_of_coils_2nd = number_of_coils % 256;
			// bc = ceil((float)number_of_coils / 8);
			bc = number_of_coils;
			if(number_of_coils % 8 > 0){
				bc++;
			}
			send_size = 13;
			

			send_message[5] = 8;
			send_message[6] = 1;
			send_message[7] = 15;	// 함수코드 15 : Write multiple coils
			send_message[8] = start_addr_1st;	// 제어할 주소 시작 1Byte = 8bits = 256경우의 수
			send_message[9] = start_addr_2nd;	
			send_message[10] = number_of_coils_1st;	// 몇개나 제어할 것인가?
			send_message[11] = number_of_coils_2nd;	
			send_message[12] = bc;	//1; 	// ??? 
			for(i=13; bc != 0; i++){
				send_message[i] = control_data%256;	//control_data : 제어할 코일들
				control_data /= 256;
				bc--;
				send_size++;
			}
			send(sock, send_message, send_size, 0);

			str_len = recv(sock, recv_message, BUFSIZE, 0);

			printf("\n\n\t--------< DATA RXed in HEX(write) >--------\n");
			printf("\t>\t[FC] : 0x%02x  \n\t>\t[SA] : 0x%02x%02x  [QO] : 0x%02x%02x\t\n", recv_message[7], recv_message[8], recv_message[9], recv_message[10], recv_message[11]);
			printf("\n\t-------------------------------------------\n\n\n");
			memset(recv_message, 0, sizeof(recv_message));
			break;

		case 'R':
			printf("> Please Input <Start register address> and <Number of registers to read>! : ");
			scanf("%d %d", &start_addr, &number_of_coils);
			start_addr = start_addr % 65536;
			start_addr_1st = start_addr / 256;
			start_addr_2nd = start_addr % 256;
			number_of_coils = number_of_coils % 65536;
			number_of_coils_1st = number_of_coils / 256;
			number_of_coils_2nd = number_of_coils % 256;

			send_message[5] = 6;
			send_message[6] = 1;
			send_message[7] = 3;
			send_message[8] = start_addr_1st;
			send_message[9] = start_addr_2nd;
			send_message[10] = number_of_coils_1st;
			send_message[11] = number_of_coils_2nd;
			send(sock, send_message, 12, 0);
			str_len = recv(sock, recv_message, BUFSIZE, 0);
			
			printf("\n\n\t--------< DATA RXed in HEX(READ) >--------\n");
			printf("\t>\t[FC] : 0x%02x  \n\t>\t[BC] : 0x%02x\t\n", recv_message[7], recv_message[8]);
			printf("\t>\t[RV] : ");
			for(i=9; i<9 + recv_message[8]; i+=2){
				printf("0x%02x%02x  ", recv_message[i], recv_message[i+1]);
			}
			printf("\n\t------------------------------------------\n\n\n");
			memset(recv_message, 0, sizeof(recv_message));
			break;

		case 'W':
			printf("> Please Input <Location to change register value> <Boundary of Control Address> and <Control Data>! : ");
			scanf("%d %d %d", &start_addr, &number_of_coils, &control_data);
			start_addr = start_addr % 65536;
			start_addr_1st = start_addr / 256;
			start_addr_2nd = start_addr % 256;
			number_of_coils_1st = number_of_coils / 256;
			number_of_coils_2nd = number_of_coils % 256;
			bc = 2*number_of_coils;
			control_data_1st = control_data / 256;
			control_data_2nd = control_data % 256;
			send_size=13;
			

			send_message[5] = 9;
			send_message[6] = 1;
			send_message[7] = 16;	// 함수코드 16 : Write multiple coils
			send_message[8] = start_addr_1st;	// 제어할 주소 시작 1Byte = 8bits = 256경우의 수
			send_message[9] = start_addr_2nd;	
			send_message[10] = number_of_coils_1st;	// 몇개나 제어할 것인가?
			send_message[11] = number_of_coils_2nd;	
			send_message[12] = bc;	

			for(i=13; i<13+bc; i+=2){
				send_message[i] = control_data_1st;
				send_message[i+1] = control_data_2nd;
				send_size+=2;
			}
			send(sock, send_message, send_size, 0);

			str_len = recv(sock, recv_message, BUFSIZE, 0);
			for(i=0; i<str_len; i++){
				printf(" [%d. %d]\n", i, recv_message[i]);
			}

			printf("\n\n\t--------< DATA RXed in HEX(WRITE) >--------\n");
			printf("\t>\t[FC] : 0x%02x  \n\t>\t[SA] : 0x%02x%02x  [QO] : 0x%02x%02x\t\n", recv_message[7], recv_message[8], recv_message[9], recv_message[10], recv_message[11]);
			printf("\n\t-------------------------------------------\n\n\n");
			memset(recv_message, 0, sizeof(recv_message));
			break;

		case 'q': 
			loop = -1;
			printf("\n\n");
			break;
		}
	}

	close(sock);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

