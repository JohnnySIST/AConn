#include <iostream>
#include "ringbuffer.hpp"
using namespace std;
int main(int argc,char* args[]){
	RingBuffer rbuf=RingBuffer(100,1);
	int a[4]={1,2,3,4},b[4];
	for(int i=0;i<10;i++){
		rbuf.write(&a,16);
		rbuf.read(&b,16);
	}
	cout<<"a[]: ";
	for(int i=0;i<4;i++){
		cout<<a[i]<<" \n"[i==3];
	}
	cout<<"b[]: ";
	for(int i=0;i<4;i++){
		cout<<b[i]<<" \n"[i==3];
	}
	return 0;
}