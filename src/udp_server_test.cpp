#include "socket.hpp"
int main(int argc,char* args[]){
	Socket sock;
	unsigned int listenPort;
	printf("Please enter listening port:");
	scanf("%d",&listenPort);
	if(sock.bind(listenPort)==-1){
		printf("Socket listening port bind failed.\n");
		return 0;
	}
	char recv[SOCKET_BUFFER_MAX];
	unsigned int srcAddr,port;
	while(true){
		sock.recv(recv,srcAddr,port);
		printf("From %s:%d receive %s\n",Socket::getAddrStr(srcAddr),port,recv);
		sock.set_addr(srcAddr,port);
		sock.send("msg received");
	}
	return 0;
}