#ifndef ACONN__PACKET__
#define ACONN__PACKET__

#include "config.hpp"

struct Packet{
	int type; // type id of packet
	int client_id; // type/id of client
	int seq; // sequence number 
	int content_size; // number of bytes in content
	char content[PACKET_SIZE_MAX];
	int enc(void *buffer){
		char *buf=(char*)buffer;
		*(int*)buf=type;
		buf+=sizeof(int);
		
		*(int*)buf=client_id;
		buf+=sizeof(int);
		if(type==EN_NEW_USER||type==EN_NEW_USER_ACK)return buf-(char*)buffer;
		
		*(int*)buf=seq;
		buf+=sizeof(int);
		*(int*)buf=content_size;
		buf+=sizeof(int);
		memcpy(buf,content,content_size);
		buf+=content_size;
		return buf-(char*)buffer;
	}
	void dec(const void *buffer){
		const char *buf=(const char *)buffer;
		type=*(int*)buf;
		buf+=sizeof(int);
		
		client_id=*(int*)buf;
		buf+=sizeof(int);
		if(type==EN_NEW_USER||type==EN_NEW_USER_ACK)return;
		
		seq=*(int*)buf;
		buf+=sizeof(int);
		content_size=*(int*)buf;
		buf+=sizeof(int);
		memcpy(content,buf,content_size);
		buf+=content_size;
		return;
	}
};

#endif // ACONN__PACKET__