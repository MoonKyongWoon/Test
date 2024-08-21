#ifndef CDEVSOCKET_HPP
#define CDEVSOCKET_HPP

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>

#include "commstruct.hpp"
#include "protocol.hpp"
#include "sysdef.hpp"
#include "sysini.hpp"
#include "util.hpp"

//2023.08.13 - KWMOON
enum EN_DEVSOCK_TYPE
{
	EN_DEV_DATA_SOCKET =0x1 ,
	EN_DEV_LOG_SOCKET       ,
};

class CDevSocket
{
public:
	CDevSocket();
	~CDevSocket();
public:
	void SetDevice( bool bTool, EN_DEVSOCK_TYPE );
	bool ConnectToHost();
	bool CloseSocket();
	bool IsConn();
	bool GetToolMode();
public:
	bool SendSlotRunNumber();
	bool SendLogPrint(char*,uint);
	bool SendRuntimeError(char*,uint);
public:
	bool WriteElement (PROTOCOL::TCP_COMMAND, char*, uint);
	bool WriteTestData( PROTOCOL::TCP_COMMAND, char* head, uint headsize, char* data, uint datasize);
public:
	bool WritePacket(char*,uint);
	bool ReadPacket (char*,uint);
	bool ReadElement (stTcpElement&);
private:
	bool LoadFtpIni();
	bool InitSocket();
	void ErrorHandling( const char*, ... );
private:
	char m_szHostIp[17];
	uint m_uHostPort;
	bool m_bInitSocket;
	bool m_bConn;
	bool m_bSentMyInfo;
	EN_DEVSOCK_TYPE m_SockType;	//2023.08.13 - KWMOON
private:
	bool m_bDiagCalTool;
	int m_socket;	
	struct sockaddr_in m_sockaddr_in;
};

#endif // CDEVSOCKET_HPP
