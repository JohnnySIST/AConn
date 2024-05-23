#ifndef ACONN__RINGBUFFER__
#define ACONN__RINGBUFFER__

#include <cstring>
#include "config.hpp"

class RingBuffer{ // parallel secure: one thread read, one thread write
public:
	RingBuffer(int capacity,int itemsize){
		this->capacity=capacity;
		this->itemsize=itemsize; // size in bytes
		head=tail=0;
		data=new char[capacity*itemsize];
	}
	int size(){ // atomic
		int tmptail=tail,tmphead=head;
		if(tmptail>=tmphead)return tmptail-tmphead;
		else return tmptail+capacity-tmphead;
	}
	int readAvailable(){
		return size();
	}
	int read(void *toread,int len){
		int ret;
		int available=readAvailable(); // to guarantee atomicity
		if(len>available)ret=len=available;
		else ret=len;
		if(capacity-head<=len){
			memcpy(toread,data+head*itemsize,(capacity-head)*itemsize);
			len-=capacity-head;
			toread=(char*)toread+(capacity-head)*itemsize;
			head=0;
		}
		if(len==0)return ret;
		memcpy(toread,data+head*itemsize,len*itemsize);
		head+=len;
		return ret;
	}
	int writeAvailable(){
		return capacity-1-size();
	}
	int write(const void *towrite,int len){
		int ret;
		int available=writeAvailable(); // to guarantee atomicity
		if(len>available)ret=len=available;
		else ret=len;
		if(capacity-tail<=len){
			memcpy(data+tail*itemsize,towrite,(capacity-tail)*itemsize);
			len-=capacity-tail;
			towrite=(char*)towrite+(capacity-tail)*itemsize;
			tail=0;
		}
		if(len==0)return ret;
		memcpy(data+tail*itemsize,towrite,len*itemsize);
		tail+=len;
		return ret;
	}
	~RingBuffer(){
		delete[] data;
	}
	int getCapacity(){
		return capacity;
	}
	int getItemsize(){
		return itemsize;
	}
private:
	volatile int head,tail;
	int capacity,itemsize;
	char *data;
};

#endif // ACONN__RINGBUFFER__