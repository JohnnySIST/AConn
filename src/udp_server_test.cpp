#include "socket.hpp"
int main(int argc,char* args[]){
	Socket sock;
	if(sock.bind(1234)==-1){
		printf("Socket listening port bind failed.\n",recv);
		return 0;
	}
	char recv[BUFFER_MAX];
	unsigned int srcAddr,port;
	while(true){
		sock.recv(recv,srcAddr,port);
		printf("%s\n",recv);
		sock.set_addr(srcAddr,port);
		sock.send("msg received");
	}
	return 0;
}