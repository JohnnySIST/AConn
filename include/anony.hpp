#ifndef ACONN__ANONY__
#define ACONN__ANONY__

class Anony{
public:
	Anony(){}
	~Anony(){}
	void setBuffer(RingBuffer *out,RingBuffer *in){
		datain=in;
		dataout=out;
	}
	int start(){
		while(true){
			if(datain->readAvailable()>=SAMPLES_PER_ANONY){
				int nsamples=datain->read(inbuffer,SAMPLES_PER_ANONY);
				dataout->write(inbuffer,nsamples);
			}
		}
		return 0;
	}
private:
	unsigned char inbuffer[ANONY_BUFFER_MAX],outbuffer[ANONY_BUFFER_MAX];
	RingBuffer *datain,*dataout;
};

#endif // ACONN__ANONY__