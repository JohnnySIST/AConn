#ifndef ACONN__CODEC__
#define ACONN__CODEC__

#include "ringbuffer.hpp"
#include <opus.h>
#include <cassert>

class Codec{
public:
	Codec(){
		encoder=opus_encoder_create(SAMPLE_RATE,NUM_CHANNELS,OPUS_APPLICATION_VOIP,&error);
		decoder=opus_decoder_create(SAMPLE_RATE,NUM_CHANNELS,&error);
	}
	~Codec(){
		opus_encoder_destroy(encoder);
		opus_decoder_destroy(decoder);
	}
	void setBuffer(RingBuffer *encIn,RingBuffer *encOut,RingBuffer *decIn,RingBuffer *decOut){
		encDataIn=encIn;
		encDataOut=encOut;
		decDataIn=decIn;
		decDataOut=decOut;
	}
	int start(){
		if(encDataOut->getItemsize()!=1||decDataIn->getItemsize()!=1){
			fprintf(stderr,"Ringbuffer itemsize does not match codec\n");
			return -1;
		}
		while(true){
			if(encDataIn!=NULL&&encDataOut!=NULL){
				while(encDataIn->readAvailable()>=SAMPLES_PER_ENC){
					int numsamples=encDataIn->read(encBuffer,SAMPLES_PER_ENC);
					int numbytes=opus_encode(encoder,encBuffer,numsamples,decBuffer,CODEC_BUFFER_MAX);
					printf("encode: %d -> %d\n",numsamples,numbytes);
					printf_hex(decBuffer,numbytes);
					if(numbytes<0){
						fprintf(stderr,"Failed to encode frame: %s\n",opus_strerror(numbytes));
						return -1;
					}
					encDataOut->write(decBuffer,numbytes);
				}
			}
			if(decDataIn!=NULL&&decDataOut!=NULL){
				while(decDataIn->readAvailable()>=SAMPLES_PER_DEC){
					int numbytes=decDataIn->read(decBuffer,SAMPLES_PER_DEC);
					printf_hex(decBuffer,numbytes);
					int numsamples=opus_decode(decoder,decBuffer,numbytes,encBuffer,CODEC_BUFFER_MAX,0);
					printf("decode: %d -> %d\n",numbytes,numsamples);
					if(numsamples<0){
						fprintf(stderr,"Failed to decode frame: %s\n",opus_strerror(numsamples));
						return -1;
					}
					decDataOut->write(decBuffer,numsamples);
				}
			}
		}
		return 0;
	}
private:
	SAMPLE encBuffer[CODEC_BUFFER_MAX];
	unsigned char decBuffer[CODEC_BUFFER_MAX];
	RingBuffer *encDataIn,*encDataOut;
	RingBuffer *decDataIn,*decDataOut;
	OpusEncoder *encoder;
	OpusDecoder *decoder;
	int error;
};

#endif // ACONN__CODEC__