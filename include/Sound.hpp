#ifndef ACONN__SOUND__
#define ACONN__SOUND__

#include <iostream>
#include <portaudio.h>
#include "Cursor.hpp"

struct UserData{
	int16_t *outdata,*indata;
	int *outcnt,*incnt;
};

class Sound{
public:
	Sound(int samplerate=48000,int buffersize=256){
		sampleRate=samplerate;
		bufferSize=buffersize;
		outdata=NULL;
		indata=NULL;
		userData.outdata=NULL;
		userData.indata=NULL;
		userData.outcnt=&outcnt;
		userData.incnt=&incnt;
		
		error=Pa_Initialize();
		checkError(error);
		outputParameters.channelCount=1;
		outputParameters.sampleFormat=paInt16;
		inputParameters.channelCount=1;
		inputParameters.sampleFormat=paInt16;
	}
	~Sound(){
		Pa_Terminate();
	}
	void open(PaDeviceIndex deviceOpt=-1,PaDeviceIndex deviceIpt=-1){
		deviceOutput=deviceOpt;
		if(deviceIpt==-1)deviceInput=deviceOutput;
		else deviceInput=deviceIpt;
		if(deviceOutput==-1){
			int out=(outdata!=NULL);
			int in=(indata!=NULL);
			if(out==0&&in==0){
				fprintf(stderr,"No data provided, open failed!\n");
				checkError(paBadBufferPtr);
			}
			error=Pa_OpenDefaultStream(&stream,in,out,paInt16,sampleRate,bufferSize,callback,&userData);
			checkError(error);
		}else{
			/* if(Pa_GetDeviceInfo(deviceOutput)==NULL||Pa_GetDeviceInfo(deviceInput)==NULL)checkError(paInvalidDevice); */
			
			outputParameters.device=deviceOutput;
			outputParameters.suggestedLatency=Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
			outputParameters.hostApiSpecificStreamInfo=NULL;
			inputParameters.device=deviceInput;
			inputParameters.suggestedLatency=Pa_GetDeviceInfo(inputParameters.device)->defaultLowOutputLatency;
			inputParameters.hostApiSpecificStreamInfo=NULL;
			
			if(outdata==NULL){
				if(indata==NULL){
					fprintf(stderr,"No data provided, open failed!\n");
					checkError(paBadBufferPtr);
				}else error=Pa_OpenStream(&stream,&inputParameters,NULL,sampleRate,bufferSize,paClipOff,callback,&userData);
			}else if(indata==NULL)error=Pa_OpenStream(&stream,NULL,&outputParameters,sampleRate,bufferSize,paClipOff,callback,&userData);
			else error=Pa_OpenStream(&stream,&inputParameters,&outputParameters,sampleRate,bufferSize,paClipOff,callback,&userData);
			checkError(error);
		}
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
		fprintf(stderr,"%f %f %f\n",timeInfo->currentTime,timeInfo->inputBufferAdcTime,timeInfo->outputBufferDacTime);
		UserData* user=(UserData*)userData;
		int16_t* in=(int16_t*)inputBuffer;
		int16_t* out=(int16_t*)outputBuffer;
		int16_t* indata=user->indata;
		int16_t* outdata=user->outdata;
		int stout=(*user->outcnt),stin=(*user->incnt);
		fprintf(stderr,"out: %d in: %d frames: %d\n",stout,stin,framesPerBuffer);
		if(outdata!=NULL){
			for(int i=0;i<framesPerBuffer;i++){
				out[i]=outdata[stout+i];
			}
		}
		if(indata!=NULL){
			for(int i=0;i<framesPerBuffer;i++){
				indata[stin+i]=in[i];
			}
		}
		(*user->outcnt)+=framesPerBuffer;
		(*user->incnt)+=framesPerBuffer;
		return 0;
	}
	void setData(int16_t* out,int16_t* in){
		outcnt=incnt=0;
		outdata=out;
		indata=in;
		userData.outdata=outdata;
		userData.indata=indata;
	}
public:
	int sampleRate;
	int bufferSize;
	int16_t *outdata,*indata;
	int outcnt,incnt;
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