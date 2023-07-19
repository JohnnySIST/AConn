#include "sound.hpp"
using namespace std;
int main(int argc,char* args[]){
	Sound sound;
	// buffer can store 1-second signal
	RingBuffer *buffer=new RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	sound.setData(buffer,buffer);
	sound.open();
	rlutil::msleep(10000); // last 10 seconds
	sound.close();
	delete buffer;
	return 0;
}