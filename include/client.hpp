#ifndef ACONN__CLIENT__
#define ACONN__CLIENT__

#include "socket.hpp"
#include "utils.hpp"
#include "ringbuffer.hpp"
#include "config.hpp"
#include "packet.hpp"

class Client{
public:
	Client(){
		m_id=-1;
		updata=NULL;
		downdata=NULL;
		seq=0;
	}
	void setBuffer(RingBuffer *updata,RingBuffer *downdata){
		this->updata=updata;
		this->downdata=downdata;
	}
	int connect(const char* addr,unsigned int port,int type=EN_BROADCAST){
		m_sock.set_addr(addr,port);
		Packet pack;
		pack.type=EN_NEW_USER;
		pack.client_id=type;
		int len=pack.enc(sendBuffer);
		m_sock.send(sendBuffer,len);
		
		int ret=m_sock.recv(recvBuffer);
		if(ret<0){
			fprintf(stderr,"Client connection failed\n");
			return -1;
		}
		pack.dec(recvBuffer);
		m_id=pack.client_id;
		printf("Successfully registed as id=%d\n",m_id);
		return 0;
	}
	int start(){
		if(updata==NULL&&downdata==NULL){
			fprintf(stderr,"Buffer not set up, start failed!\n");
			return -1;
		}
		if(m_id==-1){
			fprintf(stderr,"Not connected to server, start failed!\n");
		}
		m_sock.setNonblocking();
		Packet pack;
		while(true){
			if(updata!=NULL){
				if(updata->readAvailable()>=SAMPLES_PER_PACK){
					pack.type=EN_PACK_SPEAK;
					pack.client_id=m_id;
					pack.seq=seq++;
					pack.content_size=updata->read(pack.content,SAMPLES_PER_PACK);
					int len=pack.enc(sendBuffer);
					// printf("Send seq=%d\n",pack.seq);
					m_sock.send(sendBuffer,len);
				}
			}
			if(downdata!=NULL){
				int recvlen=m_sock.recv(recvBuffer);
				if(recvlen>0){
					pack.dec(recvBuffer);
					// printf("Recv seq=%d\n",pack.seq);
					downdata->write(pack.content,pack.content_size);
				}
			}
		}
		return 0;
	}
private:
	int m_id; // unique client_id
	int seq;
	Socket m_sock;
	char sendBuffer[SOCKET_BUFFER_MAX];
	char recvBuffer[SOCKET_BUFFER_MAX];
	RingBuffer *updata,*downdata;
};

#endif // ACONN__CLIENT__