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
		memset(&remoteAddr,0,sizeof(sockaddr));
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
#ifdef _WIN32
		if(iResult==SOCKET_ERROR){
			printf("Error Code : %d\n",WSAGetLastError());
			return -1;
		}
#else
		if(iResult==-1){
			printf("Error Code : %d\n",errno);
			return -1;
		}
#endif
		data[iResult]=0;// terminate character
		ACLOG("from %s:%d receive %s\n",inet_ntoa(recvAddr.sin_addr),ntohs(recvAddr.sin_port),data);
		return iResult;
	}
public:
	sockaddr_in remoteAddr,localAddr,recvAddr; // for sending, local binding, receiving respectively
#ifdef _WIN32
	WSADATA wsaData;
	unsigned int sock_udp;
	int recvlen=sizeof(recvAddr);
#else
	int sock_udp;
	unsigned int recvlen=sizeof(recvAddr);
#endif
};

#endif // ACONN__SOCKET__
