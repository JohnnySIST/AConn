#include "socket.hpp"
int main(int argc,char* args[]){
	Socket sock;
	sock.set_addr("127.0.0.1",1234);
	sock.send("hello world",12);
	return 0;
}