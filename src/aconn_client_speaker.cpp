#include "client.hpp"
#include "sound.hpp"
int main(int argc,char* args[]){
	// input server address
	char serverAddr[30];
	unsigned int serverPort;
	printf("Please enter server address:");
	if(my_gets(serverAddr,30)==1){ // only null character
		strcpy(serverAddr,"127.0.0.1");
		printf("Using default address %s\n",serverAddr);
	}
	printf("Please enter server port:");
	scanf("%d",&serverPort);
	
	// create upload and download buffers
	RingBuffer upbuffer=RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	
	// setup network
	Client client;
	client.setBuffer(&upbuffer,NULL);
	if(client.connect(serverAddr,serverPort)==-1){
		return -1;
	}
	
	// setup audio interface
	Sound sound;
	sound.setData(NULL,&upbuffer);
	sound.open();
	
	client.start();
	return 0;
}