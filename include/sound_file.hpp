#ifndef ACONN__SOUND_FILE__
#define ACONN__SOUND_FILE__

#include "wavefile.hpp"

const int SOUND_FILE_BUFFER_MAX = SAMPLE_RATE * 60;
const int SAMPLES_PER_FILE = 480;

class SoundFile{
public:
	SoundFile(){
		inbuffer=new SAMPLE[SOUND_FILE_BUFFER_MAX];
		outbuffer=new SAMPLE[SOUND_FILE_BUFFER_MAX];
		insize=WaveFile::readWaveFile("input.wav",inbuffer)/sizeof(SAMPLE);
        insize=insize/SAMPLES_PER_FILE*SAMPLES_PER_FILE;
		outsize=insize;
		
	}
    void writeToFile(){
        WaveFile::writeWaveFile("output.wav",outsize,outbuffer);
        printf("output audio file recorded.\n");
    }
	~SoundFile(){
		delete[] inbuffer;
		delete[] outbuffer;
	}
	void setBuffer(RingBuffer *out,RingBuffer *in){
		datain=in;
		dataout=out;
	}
	int start(){
		incnt=outcnt=0;
		while(incnt<insize||outcnt<outsize){
			if(incnt<insize&&datain->writeAvailable()>=SAMPLES_PER_FILE&&incnt<outcnt+5*SAMPLES_PER_FILE){
				int len=insize-incnt;
				if(len>SAMPLES_PER_FILE)len=SAMPLES_PER_FILE;
				datain->write(inbuffer+incnt,len);
				incnt+=len;
                // printf("%d/%d %d/%d\n",incnt,insize,outcnt,outsize);
			}
			if(outcnt<outsize&&dataout->readAvailable()>=SAMPLES_PER_FILE){
				int len=outsize-outcnt;
				if(len>SAMPLES_PER_FILE)len=SAMPLES_PER_FILE;
				int nsamples=dataout->read(outbuffer+outcnt,len);
				outcnt+=nsamples;
                // printf("%d/%d %d/%d\n",incnt,insize,outcnt,outsize);
			}
		}
        writeToFile();
		return 0;
	}
private:
	SAMPLE *inbuffer,*outbuffer;
	int insize,incnt,outsize,outcnt;
	RingBuffer *datain,*dataout;
};

#endif // ACONN__SOUND_FILE__