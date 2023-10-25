#ifndef ACONN__SOCKET__
#define ACONN__SOCKET__

#include <cstring>
#ifdef _WIN32
  #include <winsock2.h>
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #define SOCKET_ERROR -1
#endif
#include "config.hpp"
#include "cursor.hpp"
#define BUFFER_MAX 2048

// winsock wrapper
class Socket{
public:
	Socket(){
#ifdef _WIN32
		WSAStartup(MAKEWORD(2,2),&wsaData);//请求winsock版本2.2
#endif
		sock_udp=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
		memset(&remoteAddr,0,sizeof(sockaddr_in));
		remoteAddr.sin_family=AF_INET;
		localAddr.sin_family=AF_INET;
		localAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	~Socket(){
#ifdef _WIN32
		closesocket(sock_udp);
		WSACleanup();
#else
		close(sock_udp);
#endif
	}
	void set_addr(const char* addr,unsigned int port=0){
		remoteAddr.sin_addr.s_addr=inet_addr(addr);
		remoteAddr.sin_port=htons(port);
	}
	void set_addr(unsigned int addr,unsigned int port=0){
		remoteAddr.sin_addr.s_addr=addr;
		remoteAddr.sin_port=htons(port);
	}
	int send(const char* data,int len=-1){
		if(len==-1)len=strlen(data);
		int iResult=sendto(sock_udp,data,len,0,(sockaddr*)&remoteAddr,sizeof(sockaddr_in));
		if(iResult==SOCKET_ERROR){
			printf("Socket bind Error Code : %d\n",getError());
			return -1;
		}
		// ACLOG("send to %s:%d\n",inet_ntoa(remoteAddr.sin_addr),ntohs(remoteAddr.sin_port));
		return iResult;
	}
	int bind(unsigned int port){
		localAddr.sin_port=htons(port);
		int iResult=::bind(sock_udp,(sockaddr*)&localAddr,sizeof(sockaddr_in));
		if(iResult==SOCKET_ERROR){
			printf("Socket bind Error Code : %d\n",getError());
			return -1;
		}
		return iResult;
	}
	int recv(char* data){ // store received bytes into data
		int iResult=recvfrom(sock_udp,data,BUFFER_MAX,0,(sockaddr*)&recvAddr,&recvlen);
		if(iResult==SOCKET_ERROR){
			printf("Socket recv Error Code : %d\n",getError());
			return -1;
		}
		data[iResult]=0;// terminate character
		// ACLOG("from %s:%d receive %s\n",inet_ntoa(recvAddr.sin_addr),ntohs(recvAddr.sin_port),data);
		return iResult;
	}
	int recv(char* data,unsigned int& srcAddr,unsigned int& port){ // store source address & port
		int iResult=recvfrom(sock_udp,data,BUFFER_MAX,0,(sockaddr*)&recvAddr,&recvlen);
		if(iResult==SOCKET_ERROR){
			printf("Socket recv Error Code : %d\n",getError());
			return -1;
		}
		data[iResult]=0;// terminate character
		// ACLOG("from %s:%d receive %s\n",inet_ntoa(recvAddr.sin_addr),ntohs(recvAddr.sin_port),data);
		srcAddr=recvAddr.sin_addr.s_addr;
		port=ntohs(recvAddr.sin_port);
		return iResult;
	}
	static int getError(){
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno;
#endif
	}
	static const char* getAddrStr(unsigned int addr){
		sockaddr_in address;
		address.sin_addr.s_addr=addr;
		return inet_ntoa(address.sin_addr);
	}
public:
	sockaddr_in remoteAddr,localAddr,recvAddr; // for sending, local binding, receiving respectively
#ifdef _WIN32
	WSADATA wsaData;
	unsigned int sock_udp;
	int recvlen=sizeof(sockaddr_in);
#else
	int sock_udp;
	unsigned int recvlen=sizeof(sockaddr_in);
#endif
};

#endif // ACONN__SOCKET__
