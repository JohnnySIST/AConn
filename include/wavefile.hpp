#ifndef ATNET__WAVEFILE__
#define ATNET__WAVEFILE__

#include <fstream>
#include "config.hpp"

namespace WaveFile{

int sampleRate=SAMPLE_RATE;
struct WaveFileInfo{
	int sampleRate;
	int size;
	short bitdepth;
};

template<typename T>
inline void writeByte(char *s,T x){
	*(T*)s=x;
}
template<typename T>
void writeWaveFile(const char* path,int size,const T* data){
	std::ofstream file(path,std::ios_base::binary);
	char cache[45]="RIFF    WAVEfmt                     data    ";
	int byteDepth=sizeof(T);
	int dataSize=size*byteDepth;
	writeByte(cache+4,36+dataSize);
	writeByte(cache+16,16);
	writeByte(cache+20,(short)1);//1-PCM
	writeByte(cache+22,(short)1);//channel:1
	writeByte(cache+24,sampleRate);
	writeByte(cache+28,byteDepth*sampleRate);//bytes per second
	writeByte(cache+32,(short)byteDepth);//bytes per sample
	writeByte(cache+34,(short)(byteDepth*8));//bits per sample
	writeByte(cache+40,dataSize);
	file.write(cache,44);
	file.write((char*)data,dataSize);
	file.close();
}
void getWaveFileInfo(const char* path,WaveFileInfo* info){
	std::ifstream file(path,std::ios_base::binary);
	file.seekg(24);
	file.read((char*)&info->sampleRate,sizeof(int));
	file.seekg(34);
	file.read((char*)&info->bitdepth,sizeof(short));
	file.seekg(40);
	file.read((char*)&info->size,sizeof(int));
	file.close();
}
template<typename T>
int readWaveFile(const char* path,T* data){
	std::ifstream file(path,std::ios_base::binary);
	int size;
	file.seekg(40);
	file.read((char*)&size,sizeof(int));
	file.read((char*)data,size);
	file.close();
	return size;
}

} // end of namespace WaveFile

#endif // ATNET__WAVEFILE__
