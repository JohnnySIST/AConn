#include "socket.hpp"
int main(int argc,char* args[]){
	Socket sock;
	sock.bind(1234);
	char recv[100];
	sock.recv(recv);
	printf("%s\n",recv);
	return 0;
}