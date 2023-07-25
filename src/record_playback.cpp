#include "sound.hpp"
int main(int argc,char* args[]){
	Sound sound;
	// buffer can store 1-second signal
	RingBuffer buffer=RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	sound.setData(&buffer,&buffer);
	sound.open();
	printf("Press any key to stop . . .\n");
	getch();
	sound.close();
	return 0;
}