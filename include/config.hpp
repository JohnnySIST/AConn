#ifndef ACONN__CONFIG__
#define ACONN__CONFIG__

/* Audio Configurations */

#define NUM_CHANNELS (1)
#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (512)

/* Socket Configurations */

#define SOCKET_BUFFER_MAX (2048)
#define SAMPLES_PER_PACK (128)

/* Definitions */
const int EN_NEW_USER=0;
const int EN_NEW_USER_ACK=1;
const int EN_PACK_SPEAK=2;
const int EN_PACK_HEAR=3;

const int EN_BROADCAST=0;
const int EN_SELF_LISTENER=1;
struct Address{
	Address(){
		addr=0;
		port=0;
	}
	Address(unsigned int addr,unsigned int port,int type){
		this->addr=addr;
		this->port=port;
		this->type=type;
	}
	unsigned int addr,port;
	int type;
};

/* Select sample format. */
#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

#include<iostream>
#define ACLOG(X, ...) printf(X __VA_OPT__(,) __VA_ARGS__)
//#define ACLOG(X, ...) // Log off

#endif // ACONN__CONFIG__