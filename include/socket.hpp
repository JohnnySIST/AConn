#ifndef ACONN__SOCKET__
#define ACONN__SOCKET__

#ifdef _WIN32

#include <winsock2.h>
#include "config.hpp"
#include "cursor.hpp"
#define BUFFER_MAX 2048

// winsock wrapper
class Socket{
public:
	Socket(){
		WSAStartup(MAKEWORD(2,2),&wsaData);//请求winsock版本2.2
		sock_udp=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
		memset(&remoteAddr,0,sizeof(sockaddr));
		remoteAddr.sin_family=AF_INET;
		localAddr.sin_family=AF_INET;
		localAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	~Socket(){
		closesocket(sock_udp);
		WSACleanup();
	}
	void set_addr(const char* addr,int port=0){
		remoteAddr.sin_addr.s_addr=inet_addr(addr);
		remoteAddr.sin_port=htons(port);
	}
	void set_addr(unsigned int addr,int port=0){
		remoteAddr.sin_addr.s_addr=addr;
		remoteAddr.sin_port=htons(port);
	}
	void send(const char* data,int len=-1){
		if(len==-1)len=strlen(data);
		sendto(sock_udp,data,len,0,(sockaddr*)&remoteAddr,sizeof(remoteAddr));
		ACLOG("send to %s:%d\n",inet_ntoa(remoteAddr.sin_addr),ntohs(remoteAddr.sin_port));
	}
	void bind(int port){
		localAddr.sin_port=htons(port);
		::bind(sock_udp,(sockaddr*)&localAddr,sizeof(localAddr));
	}
	int recv(char* data){
		int iResult=recvfrom(sock_udp,data,BUFFER_MAX,0,(sockaddr*)&recvAddr,&recvlen);
		if(iResult==SOCKET_ERROR){
			printf("Error Code : %d\n",WSAGetLastError());
			return -1;
		}
		data[iResult]=0;// terminate character
		ACLOG("from %s:%d receive %s\n",inet_ntoa(recvAddr.sin_addr),ntohs(recvAddr.sin_port),data);
		return iResult;
	}
public:
	WSADATA wsaData;
	sockaddr_in remoteAddr,localAddr,recvAddr; // for sending, local binding, receiving respectively
	int recvlen=sizeof(recvAddr);
	SOCKET sock_udp;
};

#else // #ifdef _WIN32

#endif // #ifdef _WIN32

#endif // ACONN__SOCKET__