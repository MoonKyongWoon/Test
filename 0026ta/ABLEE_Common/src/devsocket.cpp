#include "devsocket.hpp"
#include "sys.hpp"

extern CSys g_sys;

CDevSocket::CDevSocket()
{
	m_socket = -1;
	m_bConn = false;
	m_bInitSocket = false;
	m_bSentMyInfo = false;
}

CDevSocket::~CDevSocket()
{
	CloseSocket();
}

void CDevSocket::ErrorHandling( const char* str, ... )
{
	char buf[4096];
	memset( buf, 0x0, sizeof(buf) );

	va_list	ap;
	va_start(ap, str);
	vsprintf(buf, str, ap);
	va_end(ap);
	fprintf( stderr, "%s\n", buf );
}

//Get Host Ip
bool CDevSocket::LoadFtpIni()
{
	QString os_path = "";
	QString section_key = "";
	QString sValue = 0x0;
	bool bOk = true;

	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	QString ini = QObject::tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(FTP_INI);

	//IP
	section_key = QObject::tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_IP);
	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		snprintf( m_szHostIp, sizeof(m_szHostIp), "%s", DEFAULT_EWS_FTP_IP );
		bOk = false;
	}
	else
	{
		snprintf( m_szHostIp, sizeof(m_szHostIp), "%s", sValue.toStdString().c_str() );
	}
	fprintf( stdout, "%s IP:%s\n", __FUNCTION__, m_szHostIp );
	return bOk;
}

bool CDevSocket::GetToolMode()
{
	return m_bDiagCalTool;
}

void CDevSocket::SetDevice( bool bTool, EN_DEVSOCK_TYPE type )
{
	m_bDiagCalTool = bTool;
	m_SockType     = type;		//2023.08.13 - KWMOON
	LoadFtpIni();
	if( bTool )	//Diag/cal
	{
		if( m_SockType == EN_DEV_DATA_SOCKET )
			m_uHostPort = TCP_PORT::EWS_MAINUI_DIAG_DATA_PORT;
		else
			m_uHostPort = TCP_PORT::EWS_MAINUI_DIAG_LOG_PORT;
	}
	else
	{
		if( m_SockType == EN_DEV_DATA_SOCKET )
			m_uHostPort = TCP_PORT::EWS_TESTMGR_PGM_DATA_PORT;
		else
			m_uHostPort = TCP_PORT::EWS_TESTMGR_PGM_LOG_PORT;
	}
	fprintf( stdout, "%s TYPE:%s  PORT:%d\n",
			 __FUNCTION__, (m_SockType == EN_DEV_DATA_SOCKET)?"DATA SOCK":" LOG SOCK", m_uHostPort );
	InitSocket();
}

bool CDevSocket::InitSocket()
{
	m_socket = socket( AF_INET, SOCK_STREAM, 0 );
	if( m_socket < 0 )
	{
		m_bInitSocket = false;
		ErrorHandling( "InitSocket(), socket create error!" );
		return false;
	}
	memset(&m_sockaddr_in,0,sizeof(m_sockaddr_in));
	m_sockaddr_in.sin_family = AF_INET;
	m_sockaddr_in.sin_addr.s_addr = inet_addr(m_szHostIp);
	m_sockaddr_in.sin_port = htons(m_uHostPort);
	m_bInitSocket = true;
	fprintf( stdout, "%s  m_bInitSocket : %d\n", __FUNCTION__, m_bInitSocket );
	return true;
}

bool CDevSocket::ConnectToHost()
{
	if( m_bConn ) return true;

	if( !m_bInitSocket )
	{
		ErrorHandling( "ConnectToHost(), host ip/port is not set!" );
		return false;
	}

	int flags;
	flags = fcntl(m_socket, F_GETFL);
	flags = (flags | O_NONBLOCK);
	if( fcntl( m_socket, F_SETFL, flags ) != 0 )
	{
		ErrorHandling( "ConnectToHost(), fntcl O_NONBLOCK error!" );
		return false;
	}

	if( connect(m_socket, (struct sockaddr*) &m_sockaddr_in, sizeof(m_sockaddr_in)) != 0 )
	{
		if ( errno != EINPROGRESS )
		 {
			 ErrorHandling( "connect() errorno:%d\n", errno );
			 close( m_socket );
			 m_socket = -1;
			 return false;
		 }
	}

	fd_set fdset;
	struct timeval tv;
	FD_ZERO(&fdset);
	FD_SET( m_socket, &fdset );
	tv.tv_sec  = 5;             /* 5 second timeout */
	tv.tv_usec = 0;
	if ( select( m_socket + 1, NULL, &fdset, NULL, &tv) < 1 )
	{
		ErrorHandling( "ConnectToHost(), select error!" );
		return false;
	}

	int so_error = 0;
	socklen_t len = sizeof(so_error);
	getsockopt( m_socket, SOL_SOCKET, SO_ERROR, &so_error, &len);

	if ( so_error )
	{
		ErrorHandling( "ConnectToHost(), ip:%s[%d] connect error!", m_szHostIp, m_uHostPort );
		return false;
	}
	fcntl( m_socket, F_GETFL );
	flags = fcntl(m_socket, F_GETFL);
	flags = (flags & ~O_NONBLOCK);
	if( fcntl( m_socket, F_SETFL, flags ) != 0 )
	{
		ErrorHandling( "ConnectToHost(), fntcl ~O_NONBLOCK error!" );
		return false;
	}
	//2023.05.22 - timeout
	tv.tv_sec  = 10;             /* 10 second timeout */
	tv.tv_usec =  0;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	//setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);

	m_bConn = true;
	fprintf( stdout, "SLOT%02d-%d Log/Data socket connected\n",	g_sys.GetCurrentSlotNo()+1, g_sys.GetCurrentCpuNo()+1 );

	SendSlotRunNumber();
	return m_bConn;
}

bool CDevSocket::CloseSocket()
{
	if( m_bConn ) close( m_socket );
	m_bConn = false;
	return true;
}

bool CDevSocket::IsConn()
{
	if( m_bConn ) return true;
	return false;
}

bool CDevSocket::SendSlotRunNumber()
{
	bool bSent = WriteElement( PROTOCOL::CMD_SLOT_RUN_NO_SEND, NULL, 0 );
	m_bSentMyInfo = bSent;
	return bSent;
}

bool CDevSocket::SendLogPrint(char* szLog, uint ulen )
{
	bool bSent = WriteElement( PROTOCOL::CMD_LOG_PRINT, szLog, ulen );
	return bSent;
}

bool CDevSocket::SendRuntimeError(char* szErrMsg, uint len)
{
	bool bSent = WriteElement( PROTOCOL::CMD_PGM_RUNTIME_ERROR, szErrMsg, len );
	return bSent;
}

bool CDevSocket::WriteElement(PROTOCOL::TCP_COMMAND cmd, char* buf, uint datalen )
{
	if( !ConnectToHost() ) return false;

	stTcpHeader header;
	memset( &header, 0x0, sizeof(stTcpHeader) );	//2023.07.31 - KWMOON

	header.cmd    = cmd;
	header.dest   = PROTOCOL::_ATMGR;
	header.slot   = g_sys.GetCurrentSlotNo();
	header.runner = g_sys.GetCurrentCpuNo();
	header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + header.slot );
	header.datalen = datalen;
	header.error   = 0x0;		//2023.07.31 - KWMOON
	uint size = sizeof(stTcpHeader) + datalen;
	char* pBuffer = new char[ size ];
	memcpy( pBuffer, &header, sizeof(stTcpHeader) );
	if( datalen > 0 )
	{
		if( buf != nullptr )
		{
			memcpy( pBuffer+sizeof(stTcpHeader), buf  , datalen );
		}
		else
		{
			header.datalen = 0x0;
			size = sizeof(stTcpHeader) + header.datalen;
		}
	}
	bool bSent = WritePacket( pBuffer, size );
	delete[] pBuffer;

	if( 0 )
	{
		fprintf( stdout, "[SLOT%02d-%d][SEND] %s [len:%d]\n",
				 header.slot, header.runner, UTIL::TCPIP::CMD_STR(cmd).toStdString().c_str(), size );
	}
	return bSent;
}

//2023.04.24 - kwmoon
bool CDevSocket::WriteTestData( PROTOCOL::TCP_COMMAND cmd, char* pHead, uint headlen, char* pData, uint datalen )
{
	if( !ConnectToHost() ) return false;

	stTcpHeader header;
	header.cmd    = cmd;
	header.dest   = PROTOCOL::_ATMGR;
	header.slot   = g_sys.GetCurrentSlotNo();
	header.runner = g_sys.GetCurrentCpuNo();
	header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + header.slot );
	header.datalen = headlen + datalen;
	uint size = sizeof(stTcpHeader) + header.datalen;
	char* pBuffer = new char[ size ];
	memcpy( pBuffer, &header, sizeof(stTcpHeader) );
	if( header.datalen > 0 )
	{
		if( pData != nullptr )
		{
			memcpy( pBuffer+sizeof(stTcpHeader)        , pHead, headlen );
			memcpy( pBuffer+sizeof(stTcpHeader)+headlen, pData, datalen );
		}
		else
		{
			header.datalen = 0x0;
			size = sizeof(stTcpHeader) + header.datalen;
		}
	}
	bool bSent = WritePacket( pBuffer, size );
	delete[] pBuffer;

	if( 0 )
	{
		char szLogMsg[PATH_MAX] = {0x0,};
		snprintf( szLogMsg, PATH_MAX, "[SLOT%02d-%d][SEND] %s [len:%d]\n",
				 header.slot, header.runner, UTIL::TCPIP::CMD_STR(cmd).toStdString().c_str(), size );
		SendLogPrint( szLogMsg, strlen(szLogMsg) );
	}
	return bSent;
}

bool CDevSocket::WritePacket(char* pBuffer, uint size)
{
	char* pBuf  = pBuffer;
	uint tosend = size;
	while ( tosend > 0 )
	{
		ssize_t nSendByte = write( m_socket, pBuf, tosend );
		if (nSendByte < 0 )
		{
			fprintf( stderr, "%s send packet error!\n", __FUNCTION__ );
			return false;
		}
		tosend -= nSendByte;
		pBuf   += nSendByte;
	}	
	return true;
}

bool CDevSocket::ReadPacket(char* pBuffer, uint size)
{
	uint toread = size;
	char* pBuf  = pBuffer;
	while( toread > 0 )
	{
		ssize_t nReadByte = read(m_socket, pBuf, toread );
		if( nReadByte < 0 )
		{
			fprintf( stderr, "%s read packet error!\n", __FUNCTION__ );
			return false;
		}
		toread -= nReadByte;
		pBuf   += nReadByte;
	}
	return true;
}

bool CDevSocket::ReadElement (stTcpElement& tcpelement)
{
	uint toread = sizeof(stTcpHeader);
	char* pBuf  = (char*)&tcpelement.tcpHeader;
	while( toread > 0 )
	{
		ssize_t nReadByte = read(m_socket, pBuf, toread );
		if( nReadByte < 0 )
		{
			fprintf( stderr, "%s read tcp header error!\n", __FUNCTION__ );
			return false;
		}
		toread -= nReadByte;
		pBuf   += nReadByte;
	}
	toread = tcpelement.tcpHeader.datalen;
	tcpelement.pData = nullptr;
	if( toread > 0 )
	{
		tcpelement.pData = new char[toread];
		char* pBuf  = tcpelement.pData;
		while( toread > 0 )
		{
			ssize_t nReadByte = read(m_socket, pBuf, toread );
			if( nReadByte < 0 )
			{
				fprintf( stderr, "%s read tcp data error!\n", __FUNCTION__ );
				return false;
			}
			toread -= nReadByte;
			pBuf   += nReadByte;
		}
	}
	return true;
}
