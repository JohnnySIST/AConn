#include "socket.hpp"
#include "utils.hpp"
int main(int argc,char* args[]){
	Socket sock;
	char serverAddr[100];
	unsigned int serverPort;
	printf("Please enter server address:");
	my_gets(serverAddr,BUFFER_MAX);
	printf("Please enter server port:");
	scanf("%d",&serverPort);
	sock.set_addr(serverAddr,serverPort);
	char send[BUFFER_MAX],recv[BUFFER_MAX];
	while(true){
		my_gets(send,BUFFER_MAX);
		sock.send(send,strlen(send));
		sock.recv(recv);
		printf("%s\n",recv);
	}
	return 0;
}