#include "client_codec.hpp"
#include "sound_file.hpp"
#include "anony.hpp"
#include <thread>
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
	RingBuffer inbuffer=RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	RingBuffer upbuffer=RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	RingBuffer downbuffer=RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	
	// setup network
	Client client;
	client.setBuffer(&upbuffer,&downbuffer);
	if(client.connect(serverAddr,serverPort,EN_SELF_LISTENER)==-1){
		return -1;
	}
	
	// setup audio interface
	SoundFile soundfile;
	soundfile.setBuffer(&downbuffer,&inbuffer);
	
	// setup anonymization
	Anony anony;
	anony.setBuffer(&upbuffer,&inbuffer);
	
	std::thread t_socket(&Client::start,&client);
	std::thread t_anony(&Anony::start,&anony);
	std::thread t_soundfile(&SoundFile::start,&soundfile);
	t_soundfile.join();
	t_socket.join();
	t_anony.join();
	return 0;
}