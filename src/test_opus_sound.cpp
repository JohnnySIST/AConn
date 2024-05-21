#include "sound.hpp"
#include <opus.h>
int main(int argc,char* args[]){
	Sound sound;
	// buffer can store 1-second signal
	RingBuffer outbuffer=RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	RingBuffer inbuffer=RingBuffer(SAMPLE_RATE*NUM_CHANNELS,sizeof(SAMPLE));
	sound.setData(&outbuffer,&inbuffer);
	
	OpusEncoder *encoder;
	OpusDecoder *decoder;
	int error;
	encoder=opus_encoder_create(SAMPLE_RATE,NUM_CHANNELS,OPUS_APPLICATION_VOIP,&error);
	decoder=opus_decoder_create(SAMPLE_RATE,NUM_CHANNELS,&error);
	
	SAMPLE samples[CODEC_BUFFER_MAX];
	unsigned char buffer[CODEC_BUFFER_MAX];
	sound.open();
	while(true){
		if(inbuffer.readAvailable()>=SAMPLES_PER_ENC){
			int numsamples=inbuffer.read(samples,SAMPLES_PER_ENC);
			int numbytes=opus_encode(encoder,samples,numsamples,buffer,CODEC_BUFFER_MAX);
			printf("encode: %d -> %d\n",numsamples,numbytes);
			numsamples=opus_decode(decoder,buffer,numbytes,samples,CODEC_BUFFER_MAX,0);
			printf("decode: %d -> %d\n",numbytes,numsamples);
			outbuffer.write(samples,numsamples);
		}else{ // 这里不加sleep就会失败，可能是编译器优化问题
			rlutil::msleep(10);
		}
	}
	sound.close();
	opus_encoder_destroy(encoder);
	opus_decoder_destroy(decoder);
	return 0;
}