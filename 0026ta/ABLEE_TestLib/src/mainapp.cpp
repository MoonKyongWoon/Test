#include <QCoreApplication>
#include "mainapp.hpp"

CMainApp* pMainPtr;
CMainApp::CMainApp() : m_pCtrlSock(nullptr)
{
	pMainPtr = this;
	m_sEwsIp    = "";
	m_bUiAutoMode = false;
	m_bHwOpen     = false;
	m_bAutoRun    = false;
	m_bEwsConn    = false;
	m_bToolPgm    = false;
	m_bValidVer   = false;
	m_nSlot       = -1;
	m_nRunner     = -1;
	RegisterMetaType();
}

CMainApp::~CMainApp()
{
    if( m_tmrConn.isActive() ) m_tmrConn.stop();
	if( m_tmrKeepAlive.isActive() ) m_tmrKeepAlive.stop();	//2023.08.10 - KWMOON
    if( m_pDevTh )
    {
        m_pDevTh->quit();
        m_pDevTh->wait();
    }
    CloseSocket();
}

void CMainApp::RegisterMetaType()
{
	qRegisterMetaType<stTcpHeader>    ( "stTcpHeader"    );
	qRegisterMetaType<stTcpElement>   ( "stTcpElement"   );
	qRegisterMetaType<QByteArray>     ( "QByteArray"     );
	qRegisterMetaType<LOG_TYPE>       ( "LOG_TYPE"       );
	qRegisterMetaType<stUiInitInfo>   ( "stUiInitInfo"   );
	qRegisterMetaType<stUiTestInfo>   ( "stUiTestInfo"   );
	qRegisterMetaType<stTestItem>     ( "stTestItem"     );
}

void CMainApp::SetDeviceIf( const char* program, CApi * pApi)
{
	m_pDevInst = new CDeviceInstance();
    m_pDevTh   = new QThread();
	pApi->SetProgramName( program );
	pApi->moveToThread( m_pDevTh );
    m_pDevInst->moveToThread( m_pDevTh );
	m_pDevInst->SetDeviceIf( pApi );
	connect( m_pDevTh  , &QThread::finished          , m_pDevInst, &QObject::deleteLater                );
	connect( this      , &CMainApp::SigOpenDevice    , m_pDevInst, &CDeviceInstance::SlotOpenDevice     );
	connect( this      , &CMainApp::SigPcieValidCheck, m_pDevInst, &CDeviceInstance::SlotPcieValidCheck );
	connect( this      , &CMainApp::SigConnectToHost , m_pDevInst, &CDeviceInstance::SlotConnectToHost  );
	connect( this      , &CMainApp::SigInitProgram   , m_pDevInst, &CDeviceInstance::SlotInitProgram    );
	connect( this      , &CMainApp::SigInitTest      , m_pDevInst, &CDeviceInstance::SlotInitTest       );
	connect( this      , &CMainApp::SigTest          , m_pDevInst, &CDeviceInstance::SlotTest           );
	connect( this      , &CMainApp::SigJumpTest      , m_pDevInst, &CDeviceInstance::SlotJumpTest       );
    //connect( this      , &CMainApp::SigStopTest      , m_pDevInst, &CDeviceInstance::SlotStopTest       );
	connect( this      , &CMainApp::SigManualRun     , m_pDevInst, &CDeviceInstance::SlotManualRun      );

	connect( m_pDevInst, &CDeviceInstance::SigOpenDeviceResult    , this , &CMainApp::SlotOpenDeviceResult     );
	connect( m_pDevInst, &CDeviceInstance::SigPcieValidCheckResult, this , &CMainApp::SlotPcieValidCheckResult );
	m_pDevTh->start();
}

void CMainApp::ShowHelp()
{
    printf(" %s usage : \n", qApp->applicationName().toStdString().c_str() );
	printf(" -auto               ----- Auto mode\n");
	printf(" -id {no}            ----- Slot id\n");
	printf(" -cpu {no}           ----- CPU(PGM) no \n");
	printf(" -tool               ----- if DIAG/CAL and AUTO MODE, use this option\n");
	printf(" -d                  ----- Debug print mode\n");
}

void CMainApp::SignalHandler( int signum )
{
	switch( signum )
	{
		case SIGINT:     //Ctrl+C
		case SIGQUIT:    //Ctrl+'\'
		case SIGSEGV:    //Segment fault
		case SIGTERM:	//SIG TERM
		{
			//printf("[SignalHandler] SIGNUM:%d\n", signum );
			pMainPtr->SetTerminate();
		}
		break;
	}
}

//2023.04.03 - kwmoon
void CMainApp::SetTerminate()
{
	if( !m_pDevInst->IsStopMode() )
	{
		m_pDevInst->SetTerminate();
	}
	m_pDevInst->WaitTerminated();
	exit(1);
}

////////////////////////////////////////////////////////////////////
/// \brief CMainApp::parseArgument
/// \param argc
/// \param argv
/// \return if manual mode return value, auto mode message loop(a.exec)
////////////////////////////////////////////////////////////////////
int  CMainApp::parseArguments( int argc, char* argv[] )
{
    QCoreApplication a(argc,argv);

	bool bSlot  = false;
	bool bCpu   = false;
	bool bDebug = false;
	bool bTool  = false;

	signal( SIGINT ,  CMainApp::SignalHandler );
	signal( SIGQUIT,  CMainApp::SignalHandler );
	signal( SIGSEGV,  CMainApp::SignalHandler );
	signal( SIGTERM,  CMainApp::SignalHandler );

	m_bAutoRun = false;
    for (int i = 1; i < argc; i++)
    {
			 if ( strncasecmp(argv[i], "-auto", 5) == 0 ) { m_bAutoRun = true; }
		else if ( strncasecmp(argv[i], "-id"  , 3) == 0 ) { m_nSlot    = atoi(argv[++i]); bSlot = true; }
		else if ( strncasecmp(argv[i], "-cpu" , 4) == 0 ) { m_nRunner  = atoi(argv[++i]); bCpu  = true; }
		else if ( strncasecmp(argv[i], "-tool", 5) == 0 ) { bTool = true;  }
		else if ( strncasecmp(argv[i], "-d"   , 2) == 0 ) { bDebug = true; }
        else
        {
            //m_bAutoMode = true;
        }
    }
	if( !bSlot )
	{
		fprintf( stderr, "SLOT NO is not set\n");
		ShowHelp();
		return -1;
	}
	if( !bCpu )
	{
		fprintf( stderr, "CPU(PGM) NO is not set\n");
		ShowHelp();
		return -1;
	}
	if( (m_nSlot<1) || (m_nSlot>MAX_SLOT) )
	{
		fprintf( stderr, " SLOT NO %d is invalid, use [ %d ~ %d]\n", m_nSlot, 1, MAX_SLOT );
		ShowHelp();
		return -1;
	}
	if( (m_nRunner<CPU00+1) || (m_nRunner>CPU01+1) )
	{
		fprintf( stderr, " CPU NO %d is invalid, use [ %d ~ %d]\n", m_nRunner, CPU00+1, CPU01+1 );
		ShowHelp();
		return -1;
	}
	if( bTool )
	{
		m_bToolPgm = true;
	}

	if( m_bAutoRun )
	{
		PrintLog( _NORMAL_LOG, "AUTO MODE" );
		LoadFtpIni();
		CreateStartSocket();
	}
	else
	{
		PrintLog( _NORMAL_LOG, "MANUAL MODE" );
	}
	emit SigOpenDevice( m_nSlot, m_nRunner );
    return a.exec();
}

void CMainApp::SlotOpenDeviceResult(int slot, int runner, bool result )
{
	m_bHwOpen = true;
	if( !result )
	{
		m_bHwOpen = false;
		if( !m_bAutoRun )
		{
			fprintf( stderr, "SLOT%d CPU%d H/W Open Error!\n", slot, runner );
			exit(-1);
		}
	}

	emit SigPcieValidCheck( m_nSlot, m_nRunner );
}

void CMainApp::SlotPcieValidCheckResult(int slot, int runner, int nChip, bool result )
{
	m_bValidVer = true;
	if( !result )
		m_bValidVer = false;

	bool bOk = true;
	std::string msg = "";
	if( m_bAutoRun )
	{
		if( !m_bHwOpen || !m_bValidVer )
		{
			bOk = true;
			if( !m_bHwOpen  )
			{
				bOk =false;
				msg = "H/W open error!\n";
				SendPgmLoadStatus( msg, bOk );
			}
			else if( !m_bValidVer )
			{
				bOk = false;
				atstring asmsg = "";
				asmsg.Format("SLOT%d RUN%d EXPECT IOA CHIP_VER:%d, READ:%d", slot, runner, CHIP1_IOA_VER, nChip );
				SendPgmLoadStatus( asmsg, bOk );
			}
		}
		else
		{
			bOk =true;
			msg = "OK";
			SendPgmLoadStatus( msg, bOk );
			RunAutoMode();
		}
	}
	else
	{
		if( !m_bValidVer )
		{
			fprintf( stderr, "SLOT%d CPU%d EXPECT IOA CHIP VER:0x%X / READ:0x%X!\n", slot, runner, CHIP1_IOA_VER, nChip );
			exit(-1);
		}
		RunManualMode();
	}
}

void CMainApp::SendPgmLoadStatus(std::string msg, bool bOk)
{
	fprintf( stdout, "[SEND][LOAD STAT] OK:%d, MSG:%s --- EWS CONN:%d\n", bOk, msg.c_str(), m_bEwsConn );

	if( m_bEwsConn )
	{
		stTcpHeader header;
		uint msgsize = msg.length();
		header.cmd    = PROTOCOL::CMD_PGM_LOAD_STATUS_SEND;
		header.dest   = PROTOCOL::_ATMGR;
		header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + (m_nSlot-1) );
		header.slot   = m_nSlot - 1;
		header.runner = m_nRunner - 1;
		header.datalen = msgsize;
		header.error = bOk?0x0:0x1;

		uint size = sizeof(stTcpHeader) + msgsize;
		char* pBuffer = new char[ size ];
		memcpy( pBuffer                    , &header    , sizeof(stTcpHeader) );
		memcpy( pBuffer+sizeof(stTcpHeader), msg.c_str(), msgsize );
		emit SigSendPacket( m_nSlot-1, pBuffer, size );
	}
}

void CMainApp::SendInitTestAck()
{
	if( m_bEwsConn )
	{
		stTcpHeader header;
		header.cmd    = PROTOCOL::CMD_INIT_TEST_START_ACK;
		header.dest   = PROTOCOL::_ATMGR;
		header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + (m_nSlot-1) );
		header.slot   = m_nSlot - 1;
		header.runner = m_nRunner - 1;
		header.datalen = 0x0;
		header.error = 0x0;

		uint size = sizeof(stTcpHeader);
		char* pBuffer = new char[ size ];
		memcpy( pBuffer                    , &header    , sizeof(stTcpHeader) );
		emit SigSendPacket( m_nSlot-1, pBuffer, size );
	}
	else
	{
		fprintf( stderr, "[SEND] CMD_INIT_TEST_START_ACK error!\n" );
	}
	PrintLog( _NORMAL_LOG, tr("DO %1").arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_INIT_TEST_START_ACK)) );
}

void CMainApp::SendTestAck()
{
	if( m_bEwsConn )
	{
		stTcpHeader header;
		header.cmd    = PROTOCOL::CMD_TEST_START_ACK;
		header.dest   = PROTOCOL::_ATMGR;
		header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + (m_nSlot-1) );
		header.slot   = m_nSlot - 1;
		header.runner = m_nRunner - 1;
		header.datalen = 0x0;
		header.error = 0x0;

		uint size = sizeof(stTcpHeader);
		char* pBuffer = new char[ size ];
		memcpy( pBuffer                    , &header    , sizeof(stTcpHeader) );
		emit SigSendPacket( m_nSlot-1, pBuffer, size );
	}
	else
	{
		fprintf( stderr, "[SEND] CMD_INIT_TEST_START_ACK error!\n" );
	}
}

void CMainApp::SendJumpTestAck()
{
	if( m_bEwsConn )
	{
		stTcpHeader header;
		header.cmd    = PROTOCOL::CMD_ITEM_TEST_START_ACK;
		header.dest   = PROTOCOL::_ATMGR;
		header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + (m_nSlot-1) );
		header.slot   = m_nSlot - 1;
		header.runner = m_nRunner - 1;
		header.datalen = 0x0;
		header.error = 0x0;

		uint size = sizeof(stTcpHeader);
		char* pBuffer = new char[ size ];
		memcpy( pBuffer                    , &header    , sizeof(stTcpHeader) );
		emit SigSendPacket( m_nSlot-1, pBuffer, size );
	}
	else
	{
		fprintf( stderr, "[SEND] CMD_ITEM_TEST_START_ACK error!\n" );
	}
}

void CMainApp::SendStopAck()
{
	if( m_bEwsConn )
	{
		stTcpHeader header;
		header.cmd    = PROTOCOL::CMD_TEST_STOP_ACK;
		header.dest   = PROTOCOL::_ATMGR;
		header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + (m_nSlot-1) );
		header.slot   = m_nSlot - 1;
		header.runner = m_nRunner - 1;
		header.datalen = 0x0;
		header.error = 0x0;

		uint size = sizeof(stTcpHeader);
		char* pBuffer = new char[ size ];
		memcpy( pBuffer                    , &header    , sizeof(stTcpHeader) );
		emit SigSendPacket( m_nSlot-1, pBuffer, size );
	}
	else
	{
		fprintf( stderr, "[SEND] CMD_TEST_STOP_ACK error!\n" );
	}
}

//2023.08.10 - KWMOON
void CMainApp::SendKeepAlive()
{
	if( m_bEwsConn )
	{
		stTcpHeader header;
		header.cmd    = PROTOCOL::CMD_KEEP_ALIVE;
		header.dest   = PROTOCOL::_ATMGR;
		header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + (m_nSlot-1) );
		header.slot   = m_nSlot - 1;
		header.runner = m_nRunner - 1;
		header.datalen = 0x0;
		header.error = 0x0;

		uint size = sizeof(stTcpHeader);
		char* pBuffer = new char[ size ];
		memcpy( pBuffer                    , &header    , sizeof(stTcpHeader) );
		emit SigSendPacket( m_nSlot-1, pBuffer, size );
	}
	else
	{
		fprintf( stderr, "[SEND] CMD_KEEP_ALIVE error!\n" );
	}
}

void CMainApp::RunManualMode()
{
	if( !m_bHwOpen )
		exit(-1);
	if( !m_bValidVer )
		exit(-1);
	emit SigManualRun();
}

void CMainApp::RunAutoMode()
{
	emit SigConnectToHost( m_nSlot, m_nRunner, m_bToolPgm );
    emit SigInitProgram();
}

//Get EWS IP
bool CMainApp::LoadFtpIni()
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
		m_sEwsIp = DEFAULT_EWS_FTP_IP;
		bOk = false;
	}
	else
	{
		m_sEwsIp = sValue;
	}
	return bOk;
}

//Main UI <-> Device PGM
void CMainApp::CreateStartSocket()
{
	m_pCtrlSock = new CTcpSocket( m_nSlot-1, m_nRunner-1 );
	if( m_bToolPgm )
		m_pCtrlSock->SetHostInfo( m_sEwsIp, TCP_PORT::EWS_MAINUI_DIAG_CTRL_PORT );	//DIAG UI
	else
		m_pCtrlSock->SetHostInfo( m_sEwsIp, TCP_PORT::EWS_TESTMGR_PGM_CTRL_PORT );	//TESTMGR

	connect( m_pCtrlSock, &CTcpSocket::SigReceivePacket , this       , &CMainApp::SlotTcpReceivePacket   );
	connect( m_pCtrlSock, &CTcpSocket::SigStatusChanged , this       , &CMainApp::SlotTcpStatusChanged   );
	connect( m_pCtrlSock, &CTcpSocket::SigLogMessage    , this       , &CMainApp::SlotTcpLogMessage      );
	connect( &m_tmrConn , &QTimer::timeout              , this       , &CMainApp::SlotTimerConnectToHost );
	connect( this       , &CMainApp::SigConnectToHostMgr, m_pCtrlSock, &CTcpSocket::SlotConnectToHost    );
	connect( this       , &CMainApp::SigSendPacket      , m_pCtrlSock, &CTcpSocket::SlotSendPacket       );
	connect( &m_tmrKeepAlive , &QTimer::timeout         , this       , &CMainApp::SlotTimerKeepAlive );
	m_tmrConn.start( 1000 );
	PrintLog( _NORMAL_LOG, "Start Tcp Socket Conn Timer" );
}

void CMainApp::SlotTimerConnectToHost()
{
	if( m_pCtrlSock->IsConn() ) return;
	//PrintLog( _NORMAL_LOG, tr("TCP start socket connect to %1[%2]").arg(m_sEwsIp).arg(TCP_PORT::EWS_TESTMGR_PGM_PORT) );
	emit SigConnectToHostMgr( m_nSlot-1 );
}

//2023.08.10 - KWMOON
void CMainApp::SlotTimerKeepAlive()
{
	if( !m_pCtrlSock->IsConn() ) return;
	SendKeepAlive();
}

void CMainApp::CloseSocket()
{
    if( m_pCtrlSock )
    {
		if( m_pCtrlSock->IsConn() )
        {
			m_pCtrlSock->CloseSocket();
        }
        m_pCtrlSock->deleteLater();
    }
}

void CMainApp::SlotTcpStatusChanged( int slot, int runner, bool bConn )
{
	m_bEwsConn = bConn;
	if( m_bEwsConn )
	{
		PrintLog( _NORMAL_LOG, "Start socket connected!" );
		SendSlotRunnerNo();
		if( m_tmrConn.isActive() ) m_tmrConn.stop();
		m_tmrKeepAlive.start( PROTOCOL::EN_KEEP_ALIVE::EN_COMM_PERIOD * 1000 );	//2023.08.10 - KWMOON
	}
	else
	{
		PrintLog( _NORMAL_LOG, "Start socket disconnected!" );
		if( !m_tmrConn.isActive() ) m_tmrConn.start();
		m_tmrKeepAlive.stop();	//2023.08.10 - KWMOON
	}
}

void CMainApp::SendSlotRunnerNo()
{
	stTcpHeader header;
	header.cmd    = PROTOCOL::CMD_SLOT_RUN_NO_SEND;
	header.dest   = PROTOCOL::_ATMGR;
	header.src    = static_cast<PROTOCOL::SOURCE_TYPE>( PROTOCOL::SOURCE_TYPE::_ATSLOT01 + (m_nSlot-1) );
	header.slot   = m_nSlot - 1;
	header.runner = m_nRunner - 1;
	header.datalen = 0;

	char* pBuffer = new char[ sizeof(stTcpHeader) ];
	memcpy( pBuffer, &header, sizeof(stTcpHeader) );
	emit SigSendPacket( m_nSlot-1, pBuffer, sizeof(stTcpHeader) );
}

void CMainApp::SlotTcpReceivePacket( int slot, int runner, stTcpElement element )
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(element.tcpHeader.cmd);
	PrintLog( _NORMAL_LOG, tr("[CTRL][RECV] %1 SLOT:%2, CPU:%3").arg(UTIL::TCPIP::CMD_STR(cmd)).arg(slot+1).arg(runner+1) );
	switch ( cmd )
	{
		case PROTOCOL::CMD_INIT_TEST_START_REQ:
		{
			stUiInitInfo infoInfo;
			memset( &infoInfo, 0x0, sizeof(stUiInitInfo) );
			if( element.tcpHeader.datalen == sizeof(stUiInitInfo) )
			{
				memcpy( &infoInfo, element.pData, sizeof(stUiInitInfo) );
				m_bUiAutoMode = infoInfo.ui_automode;
			}
			emit SigInitTest( infoInfo );
			SendInitTestAck();
			PrintLog( _NORMAL_LOG, tr("DO %1").arg(UTIL::TCPIP::CMD_STR(cmd)) );
		}
		break;

		case PROTOCOL::CMD_TEST_START_REQ:
		{
			stUiTestInfo testinfo;
			memset( &testinfo, 0x0, sizeof(stUiTestInfo) );
			if( element.tcpHeader.datalen == sizeof(stUiTestInfo) )
			{
				memcpy( &testinfo, element.pData, sizeof(stUiTestInfo) );
			}
			emit SigTest( testinfo );
			SendTestAck();
			PrintLog( _NORMAL_LOG, tr("DO %1").arg(UTIL::TCPIP::CMD_STR(cmd)) );
		}
		break;

		case PROTOCOL::CMD_ITEM_TEST_START_REQ:		//jump
		{
			stTestItem Items;
			if( element.tcpHeader.datalen == sizeof(stTestItem) )
			{
				memcpy( &Items, element.pData, sizeof(stTestItem) );
				emit SigJumpTest(Items);
				SendJumpTestAck();
				PrintLog( _NORMAL_LOG, tr("DO %1").arg(UTIL::TCPIP::CMD_STR(cmd)) );
			}
			else
			{
				//NACK
			}
		}
		break;

		case PROTOCOL::CMD_TEST_STOP_REQ:
			m_pDevInst->SetStop( true );			
			while( !m_pDevInst->IsStopMode() )
			{
				PrintLog( _NORMAL_LOG, tr("[SLOT%1-RUN%2] WAIT STOP...").arg(slot).arg(runner) );
				usleep( 1000000 );
			}
			SendStopAck();
			PrintLog( _NORMAL_LOG, tr("DO %1").arg(UTIL::TCPIP::CMD_STR(cmd)) );
			break;
		default:
			break;
	}

	if( element.pData )
	{
		delete[] element.pData;
	}
}

void CMainApp::SlotTcpLogMessage( int slot, int runner, LOG_TYPE t, QString s )
{
	PrintLog( t, tr("SLOT:%1, RUNNER:%2 %3").arg(slot).arg(runner).arg(s) );
}

void CMainApp::PrintLog( LOG_TYPE t, QString msg )
{
    QString logmsg  = "";
	if( t == _NORMAL_LOG ) logmsg = UTIL::DATE::yyMMDDHHmmss( true ) + "[INFO] " + msg;
	else                   logmsg = UTIL::DATE::yyMMDDHHmmss( true ) + "[ ERR] " + msg;

	fprintf( stdout, "%s\n", logmsg.toStdString().c_str() );
	fflush(stdout);
	//QTextStream out(stdout);
	//if( t == _NORMAL_LOG )
	//    out << COLOR_LIGHT_BLUE + logmsg + COLOR_NC + LF;
	//else
	//    out << COLOR_RED        + logmsg + COLOR_NC + LF;

    //SaveLog( t, logmsg );
}

void CMainApp::SaveLog( LOG_TYPE t, QString msg )
{
    QString sEnvPath = qgetenv( "ATRUN_DIR" );
    QString sLogPath = QDir::current().currentPath();
    QString sLogDir  = "log";

    if( !sEnvPath.isEmpty() ) sLogPath = tr("%1/%2").arg(sEnvPath).arg(sLogDir);
    else                      sLogPath = tr("%1/%2").arg(sLogPath).arg(sLogDir);

    UTIL::FILE::AppendFile( sLogPath, msg+LF );
}
