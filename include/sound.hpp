#ifndef ACONN__SOUND__
#define ACONN__SOUND__

#include <iostream>
#include "portaudio.h"
#include "cursor.hpp"
#include "config.hpp"
#include "ringbuffer.hpp"

struct UserData{
	RingBuffer *outdata,*indata;
};

class Sound{
public:
	Sound(int sampleRate=SAMPLE_RATE,int framesPerBuffer=FRAMES_PER_BUFFER){
		this->sampleRate=sampleRate;
		this->framesPerBuffer=framesPerBuffer;
		userData.outdata=NULL;
		userData.indata=NULL;
		
		error=Pa_Initialize();
		checkError(error);
		outputParameters.channelCount=NUM_CHANNELS;
		outputParameters.sampleFormat=PA_SAMPLE_TYPE;
		inputParameters.channelCount=NUM_CHANNELS;
		inputParameters.sampleFormat=PA_SAMPLE_TYPE;
	}
	~Sound(){
		Pa_Terminate();
	}
	void open(PaDeviceIndex deviceOpt=-1,PaDeviceIndex deviceIpt=-1){
		// -1 stands for default device
		if(deviceOpt==-1)deviceOutput=Pa_GetDefaultOutputDevice();
		else deviceOutput=deviceOpt;
		if(deviceIpt==-1)deviceInput=Pa_GetDefaultInputDevice();
		else deviceInput=deviceIpt;
		if(Pa_GetDeviceInfo(deviceOutput)==NULL||Pa_GetDeviceInfo(deviceInput)==NULL)
			checkError(paInvalidDevice);
		
		outputParameters.device=deviceOutput;
		outputParameters.suggestedLatency=Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo=NULL;
		inputParameters.device=deviceInput;
		inputParameters.suggestedLatency=Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
		inputParameters.hostApiSpecificStreamInfo=NULL;
		
		// open device only if corresponding data is provided
		if(userData.outdata==NULL){
			if(userData.indata==NULL){
				fprintf(stderr,"No data provided, open failed!\n");
				checkError(paBadBufferPtr);
			}else{
				error=Pa_OpenStream(&stream,&inputParameters,NULL,sampleRate,framesPerBuffer,paClipOff,callback,&userData);
			}
		}else{
			if(userData.indata==NULL){
				error=Pa_OpenStream(&stream,NULL,&outputParameters,sampleRate,framesPerBuffer,paClipOff,callback,&userData);
			}else{
				error=Pa_OpenStream(&stream,&inputParameters,&outputParameters,sampleRate,framesPerBuffer,paClipOff,callback,&userData);
			}
		}
		checkError(error);
		
		error=Pa_StartStream(stream);
		checkError(error);
	}
	void close(){
		error=Pa_StopStream(stream);
		checkError(error);
		error=Pa_CloseStream(stream);
		checkError(error);
	}
	void printDeviceInfo(PaDeviceIndex outdevice,PaDeviceIndex indevice){
		if(outdevice==-1)outdevice=Pa_GetDefaultOutputDevice();
		if(indevice==-1)indevice=Pa_GetDefaultInputDevice();
		printDeviceInfo(outdevice);
		printDeviceInfo(indevice);
	}
	void printDeviceInfo(PaDeviceIndex device=-1){
		if(device==-1)device=Pa_GetDefaultOutputDevice();
		const PaDeviceInfo *deviceInfo;
		deviceInfo=Pa_GetDeviceInfo(device);
		if(deviceInfo==NULL)checkError(paInvalidDevice);
		printf( "--------------------------------------- device #%d\n", device );
		printf( "Name                         = %s\n", deviceInfo->name );
		printf( "Host API                     = %s\n",  Pa_GetHostApiInfo( deviceInfo->hostApi )->name );
		printf( "Max inputs = %d,  Max outputs = %d\n", deviceInfo->maxInputChannels , deviceInfo->maxOutputChannels);
		printf( "Default low input latency    = %8.4f\n", deviceInfo->defaultLowInputLatency );
		printf( "Default low output latency   = %8.4f\n", deviceInfo->defaultLowOutputLatency  );
		printf( "Default high input latency   = %8.4f\n", deviceInfo->defaultHighInputLatency  );
		printf( "Default high output latency  = %8.4f\n", deviceInfo->defaultHighOutputLatency  );
		printf("----------------------------------------------\n");
	}
	void listDevices(){
		int numDevices=Pa_GetDeviceCount();
		for(int i=0;i<numDevices;i++){
			printDeviceInfo(i);
		}
	}
	inline void checkError(PaError error){
		if(error!=paNoError){
			Pa_Terminate();
			fprintf(stderr,"An error occurred while using the portaudio stream\n");
			fprintf(stderr,"Error number: %d\n",error);
			fprintf(stderr,"Error message: %s\n",Pa_GetErrorText(error));
			getch();
			exit(error);
		}
	}
	static int defaultCallback(	const void *inputBuffer, void *outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags,
								void *userData ){
		// ACLOG("Sound:defaultCallback %f %f %f\n",timeInfo->currentTime,timeInfo->inputBufferAdcTime,timeInfo->outputBufferDacTime);
		UserData* user=(UserData*)userData;
		// ACLOG("Sound:defaultCallback readAvailable=%d writeAvailable=%d\n",user->outdata->readAvailable(),user->indata->writeAvailable());
		int length=framesPerBuffer*NUM_CHANNELS;
		if(user->outdata!=NULL){
			if(user->outdata->readAvailable()<length){
				for(int i=0;i<length;i++){
					*(SAMPLE*)((char*)outputBuffer+i*sizeof(SAMPLE))=SAMPLE_SILENCE;
				}
			}else{
				user->outdata->read(outputBuffer,length);
			}
			
			// printf("sound outdata length = %d bytes\n",length*sizeof(SAMPLE));
			// for(int i=0;i<length;i++){
				// printf("%d ",*(SAMPLE*)((char*)outputBuffer+i*sizeof(SAMPLE)));
			// }
			// printf("\n");
		}
		if(user->indata!=NULL){
			int len=user->indata->write(inputBuffer,length);
			if(len<length){
				printf("Sound:defaultCallback ringbuffer indata is full, %d/%d\n",user->indata->size(),user->indata->getCapacity());
			}
			
			// printf("sound indata length = %d bytes\n",length*sizeof(SAMPLE));
			// for(int i=0;i<length;i++){
				// printf("%d ",*(SAMPLE*)((char*)inputBuffer+i*sizeof(SAMPLE)));
			// }
			// printf("\n");
		}
		return paContinue;
	}
	void setData(RingBuffer *outdata,RingBuffer *indata){
		userData.outdata=outdata;
		userData.indata=indata;
	}
public:
	int sampleRate;
	int framesPerBuffer;
	int (*callback)(	const void *inputBuffer, void *outputBuffer,
						unsigned long framesPerBuffer,
						const PaStreamCallbackTimeInfo* timeInfo,
						PaStreamCallbackFlags statusFlags,
						void *userData )=defaultCallback;
private:
	PaStream *stream;
	PaError error;
	PaDeviceIndex deviceOutput,deviceInput;
	PaStreamParameters outputParameters,inputParameters;
	UserData userData;
};

#endif // ACONN__SOUND__