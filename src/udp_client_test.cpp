#include "socket.hpp"
int main(int argc,char* args[]){
	Socket sock;
	sock.set_addr("127.0.0.1",1234);
	char send[BUFFER_MAX],recv[BUFFER_MAX];
	while(true){
		gets(send);
		sock.send(send,strlen(send));
		sock.recv(recv);
		printf("%s\n",recv);
	}
	return 0;
}