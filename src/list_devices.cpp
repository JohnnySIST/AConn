#include "sound.hpp"
int main(int argc,char* args[]){
	Sound sound;
	if(argc>1)freopen("DeviceList.txt","w",stdout);
	sound.listDevices();
	getch();
	return 0;
}