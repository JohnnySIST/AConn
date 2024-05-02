#include "server.hpp"
int main(int argc,char* args[]){
	// input listening port
	unsigned int listenPort;
	printf("Please enter listening port:");
	scanf("%d",&listenPort);
	
	// setup network
	Server server;
	server.listen(listenPort);
	return 0;
}