#ifndef ACONN__CLIENT__
#define ACONN__CLIENT__

#include "socket.hpp"
#include "utils.hpp"
#include "ringbuffer.hpp"
#include "config.hpp"
#include "packet.hpp"
#include <queue>
#include <opus.h>

struct PacketCompare {
	bool operator()(const Packet* a,const Packet* b)const{
		return a->seq>b->seq;
	}
};

class Client{
public:
	Client(){
		m_id=-1;
		updata=NULL;
		downdata=NULL;
		encoder=opus_encoder_create(SAMPLE_RATE,NUM_CHANNELS,OPUS_APPLICATION_VOIP,&error);
		decoder=opus_decoder_create(SAMPLE_RATE,NUM_CHANNELS,&error);
	}
	~Client(){
		opus_encoder_destroy(encoder);
		opus_decoder_destroy(decoder);
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
		printf("Successfully registered as id=%d\n",m_id);
		return 0;
	}
	int start(){
		if(updata==NULL&&downdata==NULL){
			fprintf(stderr,"Buffer not set up, start failed!\n");
			return -1;
		}
		if(m_id==-1){
			fprintf(stderr,"Not connected to server, start failed!\n");
			return -1;
		}
		m_sock.setNonblocking();
		Packet *pack=new Packet;
		send_seq=0;
		recv_seq=-1;
		while(true){
			if(updata!=NULL){
				if(updata->readAvailable()>=SAMPLES_PER_ENC){
					// encoding
					int numsamples=updata->read(encBuffer,SAMPLES_PER_ENC);
					int numbytes=opus_encode(encoder,encBuffer,numsamples,(unsigned char*)pack->content,SOCKET_BUFFER_MAX);
					printf("encode: %d -> %d\n",numsamples,numbytes);
					if(numbytes<0){
						fprintf(stderr,"Failed to encode frame: %s\n",opus_strerror(numbytes));
						return -1;
					}
					
					pack->type=EN_PACK_SPEAK;
					pack->client_id=m_id;
					pack->seq=send_seq++;
					pack->content_size=numbytes;
					// rlutil::setColor(rlutil::MAGENTA);
					// printf_hex(sendBuffer,numbytes);
					// printf("END%dEND",numbytes);
					// rlutil::resetColor();
					int len=pack->enc(sendBuffer);
					// rlutil::setColor(rlutil::BROWN);
					// printf_hex(pack->content,numbytes);
					// printf("END%dEND",numbytes);
					// rlutil::resetColor();
					
					// printf("Send seq=%d\n",pack->seq);
					m_sock.send(sendBuffer,len);
				}
			}
			if(downdata!=NULL){
				int recvlen=m_sock.recv(recvBuffer);
				if(recvlen>0){
					pack->dec(recvBuffer);
					// printf("Recv type=%d client_id=%d\n",pack->type,pack->client_id);
					// printf("Recv seq=%d\n",pack->seq);
					
					if(recv_seq==-1||pack->seq==recv_seq+1){
						// decoding
						int numbytes=pack->content_size;
						// rlutil::setColor(rlutil::BLUE);
						// printf_hex(pack->content,numbytes);
						// printf("END%dEND",numbytes);
						// rlutil::resetColor();
						int numsamples=opus_decode(decoder,(unsigned char*)pack->content,numbytes,encBuffer,CODEC_BUFFER_MAX,0);
						printf("decode: %d -> %d\n",numbytes,numsamples);
						if(numsamples<0){
							fprintf(stderr,"Failed to decode frame: %s\n",opus_strerror(numsamples));
							return -1;
						}
					
						downdata->write(encBuffer,numsamples);
						recv_seq=pack->seq;
					}else{
						Packet *t;
						// printf("recv_seq=%d packseq=%d queuesize=%d\n",recv_seq,pack->seq,packet_queue.size());
						// if(!packet_queue.empty())printf("topseq=%d\n",packet_queue.top()->seq);
						int pack_seq=pack->seq;
						packet_queue.push(pack);
						while(!packet_queue.empty()&&
							((packet_queue.top()->seq+SEQ_BUFFER_MAX<=pack_seq)||
							(packet_queue.top()->seq==recv_seq+1))){
							t=packet_queue.top();
							packet_queue.pop();
							// printf("poping packet %d\n",t->seq);
							
							// decoding
							int numbytes=pack->content_size;
							int numsamples=opus_decode(decoder,(unsigned char*)pack->content,numbytes,encBuffer,CODEC_BUFFER_MAX,0);
							printf("decode: %d -> %d\n",numbytes,numsamples);
							if(numsamples<0){
								fprintf(stderr,"Failed to decode frame: %s\n",opus_strerror(numsamples));
								return -1;
							}
							
							downdata->write(encBuffer,numsamples);
							recv_seq=t->seq;
							delete t;
						}
						pack=new Packet;
					}
				}
			}
		}
		return 0;
	}
private:
	int m_id; // unique client_id
	int send_seq;
	Socket m_sock;
	char sendBuffer[SOCKET_BUFFER_MAX];
	char recvBuffer[SOCKET_BUFFER_MAX];
	RingBuffer *updata,*downdata;
	
	// For packet sequence preservation
	int recv_seq;
	std::priority_queue<Packet*,std::vector<Packet*>,PacketCompare>packet_queue;
	
	// For encoding and decoding
	SAMPLE encBuffer[CODEC_BUFFER_MAX];
	OpusEncoder *encoder;
	OpusDecoder *decoder;
	int error;
};

#endif // ACONN__CLIENT__