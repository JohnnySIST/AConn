#include "socket.hpp"
#include "utils.hpp"
int main(int argc,char* args[]){
	Socket sock;
	char serverAddr[100];
	unsigned int serverPort;
	printf("Please enter server address:");
	my_gets(serverAddr,SOCKET_BUFFER_MAX);
	printf("Please enter server port:");
	scanf("%d",&serverPort);
	sock.set_addr(serverAddr,serverPort);
	char send[SOCKET_BUFFER_MAX],recv[SOCKET_BUFFER_MAX];
	while(true){
		my_gets(send,SOCKET_BUFFER_MAX);
		sock.send(send,strlen(send));
		sock.recv(recv);
		printf("%s\n",recv);
	}
	return 0;
}