#ifndef ACONN__SERVER__
#define ACONN__SERVER__

#include "socket.hpp"
#include "utils.hpp"
#include "ringbuffer.hpp"
#include "config.hpp"
#include <vector>
#include "packet.hpp"

class Server{
public:
	Server(){
		m_counter=0;
	}
	int listen(unsigned int listenPort){
		if(m_sock.bind(listenPort)==-1){
			fprintf(stderr,"Socket listening port bind failed.\n");
			return -1;
		}
		unsigned int srcAddr,srcPort;
		int recvlen;
		Packet pack;
		while(true){
			recvlen=m_sock.recv(recvBuffer,srcAddr,srcPort);
			if(recvlen==-1){
				continue;
			}
			// printf("From %s:%d receive length %d\n",Socket::getAddrStr(srcAddr),srcPort,recvlen);
			pack.dec(recvBuffer);
			if(pack.type==EN_NEW_USER){
				addresses.push_back(Address(srcAddr,srcPort,pack.client_id));
				
				pack.type=EN_NEW_USER_ACK;
				pack.client_id=m_counter;
				int len=pack.enc(sendBuffer);
				respond(m_counter,len);
				printf("User %d successfully registered\n",m_counter);
				m_counter+=1;
			}else if(pack.type==EN_PACK_SPEAK){
				int client_id=findClientid(srcAddr,srcPort);
				if(client_id==-1){
					fprintf(stderr,"Client not found!\n");
					continue;
				}
				printf("From user %d receive seq=%d\n",client_id,pack.seq);
				
				pack.type=EN_PACK_HEAR;
				int len=pack.enc(sendBuffer);
				if(addresses[client_id].type==EN_BROADCAST){
					for(int i=0;i<m_counter;i++){
						if(i!=client_id)respond(i,len);
					}
				}else if(addresses[client_id].type==EN_SELF_LISTENER){
					respond(client_id,len);
				}
			}
		}
	}
	void respond(int client_id,int len=-1){
		// printf("Respond to client_id=%d\n",client_id);
		m_sock.set_addr(addresses[client_id].addr,addresses[client_id].port);
		m_sock.send(sendBuffer,len);
	}
	int findClientid(unsigned int addr,unsigned int port){
		for(int i=0;i<m_counter;i++){
			if(addresses[i].addr==addr&&addresses[i].port==port){
				return i;
			}
		}
		return -1;
	}
private:
	int m_counter; // client_id incrementer
	Socket m_sock;
	std::vector<Address> addresses;
	char sendBuffer[SOCKET_BUFFER_MAX];
	char recvBuffer[SOCKET_BUFFER_MAX];
};

#endif // ACONN__SERVER__