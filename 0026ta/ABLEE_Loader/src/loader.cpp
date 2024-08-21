#include "loader.hpp"

CLoader::CLoader()
	: m_bDebug(false), m_pTcpMgrSvr(nullptr), m_pTcpToolSvr(nullptr), m_pIpcSvr(nullptr)
{
	m_pcieTpg0 = nullptr;
	m_pcieTpg1 = nullptr;
	m_pTcpMgrClient = nullptr;
	m_pTcpToolClient = nullptr;
	InitDirectory();
}

CLoader::~CLoader()
{
	CleanUp();
}

void CLoader::CleanUp()
{
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("Quit") );


	if( m_pTcpToolClient != nullptr )
	{
		m_pTcpToolClient->deleteLater();
	}
	if( m_pTcpMgrClient != nullptr )
	{
		m_pTcpMgrClient->deleteLater();
	}
	if( m_pTcpMgrSvr )
	{
		if( m_pTcpMgrSvr->isListening() )
			m_pTcpMgrSvr->close();
		delete m_pTcpMgrSvr;
		m_pTcpMgrSvr = nullptr;
	}
	if( m_pTcpToolSvr )
	{
		if( m_pTcpToolSvr->isListening() )
			m_pTcpToolSvr->close();
		delete m_pTcpToolSvr;
		m_pTcpToolSvr = nullptr;
	}
	if( m_pIpcSvr )
	{
		m_pIpcSvr->close();
		m_pIpcSvr->deleteLater();
		QLocalServer::removeServer( IPC_INFO::IPC_LOADER_SOCK );
	}
	if( m_pDownloaderTh )
	{
		m_pDownloaderTh->quit();
		m_pDownloaderTh->wait();
	}
	if( m_pDevMgrTh )
	{
		m_pDevMgrTh->quit();
		m_pDevMgrTh->wait();
	}
	if( m_pcieTpg0 )
	{
		if( m_pcieTpg0->pcie_isopen() )
			m_pcieTpg0->pcie_close();
		delete m_pcieTpg0;
		m_pcieTpg0 = nullptr;
	}
}

void CLoader::InitInstance()
{
	m_pcieTpg0      = new CPcie;
	m_pTcpMgrSvr    = new CTcpServer( this );
	m_pTcpToolSvr   = new CTcpServer( this );
	m_pIpcSvr       = new CLocalServer( this );
	m_pDevMgr       = new CDevMgr;
	m_pDevMgrTh     = new QThread;
	m_pDownloader   = new CDownloader;
	m_pDownloaderTh = new QThread;
	m_pLogger       = new CLogger( PROTOCOL::_ATLODER );

	m_pcieTpg0->SetPcieTPG( PCIE_DEVICE::PCIE_TPG0 );
	m_pLogger->SetLogDir( m_sCurrentRunPath );
	m_pLogger->SetDebugPrint( m_bDebug );

	connect( this      , &CLoader::SigSaveLog            , m_pLogger, &CLogger::SlotLogMessage );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("%1").arg(ATLOADER_VERSION) );
	//emit SigSaveLog( _NORMAL_LOG, tr("Current Working  Path : %1").arg(m_sCurrentWorkPath) );
	//emit SigSaveLog( _NORMAL_LOG, tr("Current Log      Path : %1").arg(m_sLogPath)         );
	//emit SigSaveLog( _NORMAL_LOG, tr("Current Download Path : %1").arg(m_sDownloadPath)    );
}

void CLoader::InitDirectory()
{
	char* env_os  = getenv(ATOS_DIR_ENV);
	char* env_run = getenv(ATRUN_DIR_ENV);
	QString os_path = "";
	QString run_path = "";

	// /opt/at6200
	if( env_os != NULL )
	{
		m_sCurrentOsPath = QString(env_os);
		m_sCurrentOsConfig = tr("%1/%2").arg(m_sCurrentOsPath).arg(DIR_CONFIG_PATH);
	}
	else
	{
		m_sCurrentOsPath = DIR_ATOS_PATH;
		m_sCurrentOsConfig = tr("%1/%2").arg(m_sCurrentOsPath).arg(DIR_CONFIG_PATH);
	}
	MakeDirectory( m_sCurrentOsConfig );		//atsystem

	// /home/ablee/atsystem
	if( env_run != NULL )
	{
		run_path = QString(env_run);
		m_sCurrentRunPath  = run_path;
		m_sCurrentWorkPath = tr("%1/%2").arg(run_path).arg(DIR_RUN_PATH);
		m_sLogPath         = tr("%1/%2").arg(run_path).arg(DIR_LOG_PATH);
		m_sDownloadPath    = tr("%1/%2").arg(run_path).arg(DIR_DOWN_PATH);
		m_sProgramPath     = tr("%1/%2").arg(run_path).arg(DIR_PROGRAM_PATH);
		m_sConfigPath      = tr("%1/%2").arg(run_path).arg(DIR_CONFIG_PATH);
		m_sCfgPath         = tr("%1/%2").arg(run_path).arg(DIR_CFG_PATH);
	}
	else
	{
		run_path = "/home/ablee/atsystem";
		m_sCurrentRunPath  = run_path;
		m_sCurrentWorkPath = tr("%1/%2").arg(run_path).arg(DIR_RUN_PATH);
		m_sLogPath         = tr("%1/%2").arg(run_path).arg(DIR_LOG_PATH);
		m_sDownloadPath    = tr("%1/%2").arg(run_path).arg(DIR_DOWN_PATH);
		m_sProgramPath     = tr("%1/%2").arg(run_path).arg(DIR_PROGRAM_PATH);
		m_sConfigPath      = tr("%1/%2").arg(run_path).arg(DIR_CONFIG_PATH);
		m_sCfgPath         = tr("%1/%2").arg(run_path).arg(DIR_CFG_PATH);
	}

	MakeDirectory( m_sCurrentRunPath );		//atsystem
	MakeDirectory( m_sCurrentWorkPath );	//atsystem/run
	MakeDirectory( m_sLogPath );			//atsystem/log
	MakeDirectory( m_sDownloadPath );		//atsystem/download
	MakeDirectory( m_sProgramPath );		//atsystem/program
	MakeDirectory( m_sConfigPath );			//atsystem/config
	MakeDirectory( m_sCfgPath );			//atsystem/cfg

	QDir::setCurrent( m_sCurrentWorkPath );
}

bool CLoader::MakeDirectory(QString path)
{
	QDir dirConfig( path );
	if( !dirConfig.exists() )
		dirConfig.mkdir( path );
	return true;
}

bool CLoader::LoadMyIp()
{
	bool bValidIp = false;
	m_sMyIp = LOOPBACK;
	foreach( QNetworkInterface interface, QNetworkInterface::allInterfaces() )
	{
		if ( !interface.flags().testFlag(QNetworkInterface::IsLoopBack) )
		{
			foreach (QNetworkAddressEntry entry, interface.addressEntries())
			{
				if ( interface.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains("192.168") )
				{
					m_sMyIp = entry.ip().toString();
					bValidIp = true;
					break;
				}
			}
			if( bValidIp ) break;
		}
	}
	return bValidIp;
}

void CLoader::showHelp()
{
	printf(" %s usage : \n", qApp->applicationName().toStdString().c_str() );
	printf(" -d or /d                ----- debug print\n");
}

int CLoader::parseArguments( int argc, char* argv[] )
{
	QCoreApplication app(argc, argv);

	qRegisterMetaType<stTcpElement>("stTcpElement");
	qRegisterMetaType<stIpcElement>("stIpcElement");
	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");
	qRegisterMetaType<QAbstractSocket::SocketError>("SocketError");
	qRegisterMetaType<PROTOCOL::EN_PGM_STATUS_CODE>("PROTOCOL::EN_PGM_STATUS_CODE");
	qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");

	m_bDebug = false;
	if( argc > 1 )
	{
		for (int i = 1; i < argc; i++)
		{
			if ( strncasecmp(argv[i], "-D"  , 2) == 0 )
			{
				m_bDebug  = true;
			}
			else if ( strncasecmp(argv[i], "/D"  , 2) == 0 )
			{
				m_bDebug  = true;
			}
			//else if ( strncasecmp(argv[i], "/S" , 2) == 0 )
			//{
			//	m_nSlot = strtol(argv[++i], NULL, 0);
			//}
			//else if ( strncasecmp(argv[i], "-S"  , 2) == 0 )
			//{
			//	m_nSlot = strtol(argv[++i], NULL, 0);
			//}
		}
	}
	InitInstance();
	Run();
	//if( (m_nSlot<1) || (m_nSlot>MAX_SLOT) )
	//{
	//	emit SigSaveLog( _FAULT_LOG, tr("Slot number '%1' error [usage:%2~%3]").arg(m_nSlot).arg(1).arg(MAX_SLOT) );
	//	showHelp();
	//	exit(-1);
	//}

	return app.exec();
}

bool CLoader::RunTcpMgrServer()
{
	bool bListen = true;
	if( m_pTcpMgrSvr->isListening() ) return bListen;
	if( !m_pTcpMgrSvr->listen( QHostAddress(m_sMyIp), TCP_PORT::SYSTEM::CPU_LOADER_MGR_PORT) )
	{
		m_pTcpMgrSvr->close();
		bListen = false;
	}
	if ( bListen )
	{
		connect( m_pTcpMgrSvr, &CTcpServer::acceptError           , this     , &CLoader::SlotMgrAcceptError         );
		connect( m_pTcpMgrSvr, &CTcpServer::SigIncommingConnection, this     , &CLoader::SlotMgrIncommingConnection );

		emit SigSaveLog( _NORMAL_LOG,
							tr("TCP(MGR) Server Listen %1[%2] --- %3").arg(m_sMyIp.toStdString().c_str()).arg(TCP_PORT::SYSTEM::CPU_LOADER_MGR_PORT).arg(bListen?"OK":"NG") );
	}
	else
	{
		emit SigSaveLog( _ERROR_LOG,
							tr("TCP(MGR) Server Listen %1[%2] --- %3").arg(m_sMyIp.toStdString().c_str()).arg(TCP_PORT::SYSTEM::CPU_LOADER_MGR_PORT).arg(bListen?"OK":"NG") );

	}
	return bListen;
}

bool CLoader::RunTcpToolServer()
{
	bool bListen = true;
	if( m_pTcpToolSvr->isListening() ) return bListen;
	if( !m_pTcpToolSvr->listen( QHostAddress(m_sMyIp), TCP_PORT::SYSTEM::CPU_LOADER_TOOL_PORT) )
	{
		m_pTcpToolSvr->close();
		bListen = false;
	}
	if ( bListen )
	{
		connect( m_pTcpToolSvr, &QTcpServer::acceptError           , this     , &CLoader::SlotToolAcceptError         );
		connect( m_pTcpToolSvr, &CTcpServer::SigIncommingConnection, this     , &CLoader::SlotToolIncommingConnection );

		emit SigSaveLog( _NORMAL_LOG,
							tr("TCP(TOOL) Server Listen %1[%2] --- %3").arg(m_sMyIp.toStdString().c_str()).arg(TCP_PORT::SYSTEM::CPU_LOADER_TOOL_PORT).arg(bListen?"OK":"NG") );
	}
	else
	{
		emit SigSaveLog( _ERROR_LOG,
							tr("TCP(TOOL) Server Listen %1[%2] --- %3").arg(m_sMyIp.toStdString().c_str()).arg(TCP_PORT::SYSTEM::CPU_LOADER_TOOL_PORT).arg(bListen?"OK":"NG") );

	}
	return bListen;
}

bool CLoader::RunLocalServer()
{
	bool bListen = true;
	if( m_pIpcSvr->isListening() ) return bListen;

	QLocalServer::removeServer( IPC_INFO::IPC_LOADER_SOCK );
	if( !m_pIpcSvr->listen( IPC_INFO::IPC_LOADER_SOCK ) )
	{
		m_pIpcSvr->close();
		bListen = false;
	}
	emit SigSaveLog( _NORMAL_LOG,
						tr("IPC Server Listen %1 --- %2").arg(IPC_INFO::IPC_LOADER_SOCK).arg(bListen?"OK":"NG") );
	return bListen;
}

bool CLoader::RunDownloadThread()
{
	m_pDownloader->moveToThread( m_pDownloaderTh );
	connect( m_pDownloaderTh , &QThread::finished               , m_pDownloader, &QObject::deleteLater             );
	connect( m_pDownloader   , &CDownloader::SigDownloadResult  , this         , &CLoader::SlotDownloadFtpResult   );
	connect( m_pDownloader   , &CDownloader::SigDownloadProgress, this         , &CLoader::SlotDownloadProgress    );
	connect( this            , &CLoader::SigDownloadProgram     , m_pDownloader, &CDownloader::SlotDownloadFile    );
	m_pDownloaderTh->start();
	emit SigSaveLog( _NORMAL_LOG, tr("Download thread start!") );
	return true;
}

bool CLoader::RunDevMgrThread()
{
	m_pDevMgr->SetRunDir( m_sCurrentWorkPath );

	m_pDevMgr->moveToThread( m_pDevMgrTh );
	connect( m_pDevMgrTh     , &QThread::finished                 , m_pDevMgr, &QObject::deleteLater            );
	connect( this            , &CLoader::SigDevMgrSlotCpuInfo     , m_pDevMgr, &CDevMgr::SlotSlotCpuInfo        );
	connect( this            , &CLoader::SigDevMgrRunDevPgm       , m_pDevMgr, &CDevMgr::SlotRunDevice          );
	connect( this            , &CLoader::SigDevMgrProgramInfo     , m_pDevMgr, &CDevMgr::SlotProgramInfo        );
	connect( this            , &CLoader::SigDevMgrTerminateDevPgm , m_pDevMgr, &CDevMgr::SlotTerminateDevice    );
	connect( m_pDevMgr       , &CDevMgr::SigPgmRunAck             , this     , &CLoader::SlotDevMgrRunAck       );
	connect( m_pDevMgr       , &CDevMgr::SigPgmRerunAck           , this     , &CLoader::SlotDevMgrReRunAck     );
	connect( m_pDevMgr       , &CDevMgr::SigPgmFinished           , this     , &CLoader::SlotDevMgrPgmFinished  );
	connect( m_pDevMgr       , &CDevMgr::SigPgmUnloadAck          , this     , &CLoader::SlotDevMgrUnloadAck  );
	connect( m_pDevMgr       , &CDevMgr::SigLogMessage            , this     , &CLoader::SlotDevMgrLogMessage   );

	m_pDevMgrTh->start();
	emit SigSaveLog( _NORMAL_LOG, tr("Device program manager start!") );
	return true;
}

void CLoader::Run()
{
	LoadMyIp();
	RunTcpMgrServer();
	RunTcpToolServer();
	RunLocalServer();
	RunDownloadThread();
	RunDevMgrThread();
}

void CLoader::SlotMgrAcceptError( QAbstractSocket::SocketError errCode )
{
	QString errMsg = UTIL::TCPIP::GetSocketErrDescription( errCode );
	emit SigSaveLog( _ERROR_LOG, tr("TCP(MGR) Server Accept Error : %1").arg(errMsg) );
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotTcpIncommingConnection
/// \param handle
/// \details EWS Test Mgr connected...
void CLoader::SlotMgrIncommingConnection( qintptr handle )
{
	if (m_pTcpMgrClient != nullptr)
	{
		CTcpClient* pClient = new CTcpClient(handle);

		pClient->CloseSocket();
		delete pClient;

		return;
	}

	m_pTcpMgrClient = new CTcpClient(handle);

	connect( m_pTcpMgrClient, &CTcpClient::SigStatusChanged, this,			  &CLoader::SlotMgrStatusChanged );
	connect( m_pTcpMgrClient, &CTcpClient::SigReceivePacket, this,			  &CLoader::SlotMgrReceivePacket );
	connect( m_pTcpMgrClient, &CTcpClient::SigLogMessage,	 this,			  &CLoader::SlotMgrLogMessage    );
	connect( m_pTcpMgrClient, &CTcpClient::destroyed,		[this](){this->m_pTcpMgrClient = nullptr;});
	connect( this,			  &CLoader::SigMgrSendPacket,	 m_pTcpMgrClient, &CTcpClient::SlotSendPacket    );
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotClientDisconnected
/// \details EWS Test Mgr disconnected...
void CLoader::SlotMgrStatusChanged( bool bConn )
{
	if( !bConn )
	{
		if( m_pTcpMgrClient )
		{
			m_pTcpMgrClient->deleteLater();
		}
		emit SigSaveLog( _NORMAL_LOG, tr("%1 disconnected!").arg(ATMGR_NAME) );
		emit SigDevMgrTerminateDevPgm( false );
		emit SigSaveLog( _NORMAL_LOG, tr("terminate test program!") );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotReceivePacket
/// \param tcpelement
/// \author KWMOON
/// \details From Client Socket <- EWS Test Manager
void CLoader::SlotMgrReceivePacket( stTcpElement tcpelement )
{
	QString sFrom = "<-MGR";
	int nCmd = tcpelement.tcpHeader.cmd;
	int nLen = tcpelement.tcpHeader.datalen;
	PROTOCOL::TCP_COMMAND scmd = static_cast<PROTOCOL::TCP_COMMAND>( nCmd );
	switch( nCmd )
	{
		case PROTOCOL::TCP_COMMAND::CMD_VERSION_REQ:
		{
			emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
			AckVersion();
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_SLOT_CPU_INFO_SEND:
		{
			emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
			SetSlotCpuInfo( tcpelement );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_LOADER_INFO_REQ:
		{
			emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
		}
		break;

		//2024.06.17 - KWMOON(NOT USE)
		//case PROTOCOL::TCP_COMMAND::CMD_SETUP_BIBID_REQ:
		//{
		//	emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
		//	//AckSetupBibId();
		//	AckReadBibId( PROTOCOL::TCP_COMMAND::CMD_SETUP_BIBID_ACK );
		//}
		//break;

		//2024.06.17 - KWMOON
		case PROTOCOL::TCP_COMMAND::CMD_READ_BIBID_REQ:
		{
			emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
			AckReadBibId( PROTOCOL::TCP_COMMAND::CMD_READ_BIBID_ACK );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_PGM_LOAD_REQ:
		{
			//emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
			SetPgmLoadInfo( tcpelement );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_PGM_RELOAD_REQ:
		{
			emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
			m_stPgmLoadStatus.bSuccess  = true;
			m_stPgmLoadStatus.nProgress = 50;
			AckPgmLoadStatus( m_stPgmLoadStatus.bSuccess, "RELOAD" );
			emit SigDevMgrRunDevPgm(true);
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_PGM_UNLOAD_REQ:
		{
			emit SigDevMgrTerminateDevPgm( true );
			emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_EWS_FTP_INFO_SEND:
		{
			//emit SigSaveLog( _NORMAL_LOG, tr("[%1][RECV] %2").arg(sFrom, 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(scmd)) );
			SetEwsFtpInfo( tcpelement );
		}
		break;
	}
	if( tcpelement.tcpHeader.datalen > 0 )
	{
		if( tcpelement.pData )
		{
			delete []  tcpelement.pData;
		}
	}
}

//TCP Socket Log Message
void CLoader::SlotMgrLogMessage( LOG_TYPE type, QString log )
{
	QString sFrom = "<-MGR";
	QString msg = tr("[%1][SOCK] MSG : %2").arg(sFrom, 7, QChar(' ') ).arg(log);
	emit SigSaveLog( type, msg );
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotToolAcceptError
/// \param errCode
/// \details Tool Server Accept
void CLoader::SlotToolAcceptError( QAbstractSocket::SocketError errCode )
{
	QString errMsg = UTIL::TCPIP::GetSocketErrDescription( errCode );
	emit SigSaveLog( _ERROR_LOG, tr("TCP(Tool) Server Accept Error : %1").arg(errMsg) );
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotToolIncommingConnection
/// \param handle
/// \details UI Tool connected
void CLoader::SlotToolIncommingConnection( qintptr handle )
{
	if (m_pTcpToolClient != nullptr)
	{
		CTcpClient* pClient = new CTcpClient(handle);

		pClient->CloseSocket();
		delete pClient;

		return;
	}

	m_pTcpToolClient = new CTcpClient(handle);

	connect( m_pTcpToolClient, &CTcpClient::SigStatusChanged, this, &CLoader::SlotToolStatusChanged   );
	connect( m_pTcpToolClient, &CTcpClient::SigReceivePacket, this, &CLoader::SlotToolReceivePacket   );
	connect( m_pTcpToolClient, &CTcpClient::SigLogMessage,	  this, &CLoader::SlotToolLogMessage      );
	connect( m_pTcpToolClient, &CTcpClient::destroyed,		 [this](){this->m_pTcpToolClient = nullptr;});
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotClientDisconnected
/// \details EWS Test Mgr disconnected...
void CLoader::SlotToolStatusChanged( bool bConn )
{
	if( !bConn )
	{
		if( m_pTcpToolClient )
		{
			m_pTcpToolClient->deleteLater();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotReceivePacket
/// \param tcpelement
/// \author KWMOON
/// \details From Client Socket <- EWS Tool(Diag/Cal)
void CLoader::SlotToolReceivePacket( stTcpElement tcpelement )
{
	int nCmd = tcpelement.tcpHeader.cmd;
	int nLen = tcpelement.tcpHeader.datalen;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotToolLogMessage
/// \param type
/// \param log
/// \details Tcp socket error or info message
void CLoader::SlotToolLogMessage( LOG_TYPE type, QString log )
{
	QString sFrom = "<-TOOL";
	QString msg = tr("[%1][SOCK] MSG : %2").arg(sFrom, 7, QChar(' ') ).arg(log);

	emit SigSaveLog( type, msg );
}


///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::SlotDownloadFtpResult
/// \param code
/// \details Ftp download program result
void CLoader::SlotDownloadFtpResult( bool bSuccess, QString msg )
{
	AckPgmLoadStatus( bSuccess, msg );

	if( !bSuccess  )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1/%2 download fail!").arg(m_stPgmLoad.szFilePath).arg(m_stPgmLoad.szFileName) );
		return;
	}
	else
	{
		emit SigSaveLog( _NORMAL_LOG, tr("%1/%2 download success!").arg(m_stPgmLoad.szFilePath).arg(m_stPgmLoad.szFileName) );
		if( !StandbyExecuteProgram() )
		{
			AckPgmLoad( false, false );
			return;
		}
		emit SigDevMgrRunDevPgm( false );
	}
}

bool CLoader::AckPgmLoad(bool bSuccess, bool bRerun)
{
	stTcpHeader tcpheader;
	if( !bRerun)
		tcpheader.cmd    = PROTOCOL::TCP_COMMAND::CMD_PGM_LOAD_ACK;
	else
		tcpheader.cmd    = PROTOCOL::TCP_COMMAND::CMD_PGM_RELOAD_ACK;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(tcpheader.cmd);
	tcpheader.dest   = PROTOCOL::_ATMAIN_UI;
	tcpheader.runner = 0x0;
	tcpheader.error  = bSuccess ? PROTOCOL::ACK_NORMAL : PROTOCOL::NACK_ERROR;
	tcpheader.slot   = m_SlotCpu.nSlotNo-1;
	tcpheader.datalen = 0x0;

	uint size = sizeof(stTcpHeader);
	char* pBuffer = new char [size ];
	memcpy( pBuffer, &tcpheader        , sizeof(stTcpHeader)     );

	QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("%1 %2:%3")
					 .arg(sHead).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(bSuccess?"SUCCESS":"FAIL") ) ;

	emit SigMgrSendPacket( pBuffer, size );

	return true;
}

bool CLoader::AckPgmUnload(bool bSuccess, QString msg )
{
	m_stPgmUnloadStatus.bSuccess = bSuccess;
	snprintf( m_stPgmUnloadStatus.szErrMsg, sizeof(m_stPgmUnloadStatus.szErrMsg), "%s", msg.toStdString().c_str() );

	stTcpHeader tcpheader;
	tcpheader.cmd    = PROTOCOL::TCP_COMMAND::CMD_PGM_UNLOAD_ACK;
	tcpheader.dest   = PROTOCOL::_ATMAIN_UI;
	tcpheader.runner = 0x0;
	tcpheader.error  = bSuccess ? PROTOCOL::ACK_NORMAL : PROTOCOL::NACK_ERROR;
	tcpheader.slot   = m_SlotCpu.nSlotNo-1;
	tcpheader.datalen = sizeof(stPgmLoadStatus);
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(tcpheader.cmd);

	uint size = sizeof(stTcpHeader) + sizeof(stPgmLoadStatus);
	char* pBuffer = new char [size ];
	memcpy( pBuffer                    , &tcpheader          , sizeof(stTcpHeader)     );
	memcpy( pBuffer+sizeof(stTcpHeader), &m_stPgmUnloadStatus, sizeof(stPgmLoadStatus) );

	QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("%1 %2:%3")
					 .arg(sHead).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(bSuccess?"SUCCESS":"FAIL") ) ;

	emit SigMgrSendPacket( pBuffer, size );

	return true;
}

bool CLoader::AckPgmLoadStatus( bool bSuccess, QString msg )
{
	m_stPgmLoadStatus.bSuccess = bSuccess;
	snprintf( m_stPgmLoadStatus.szErrMsg, sizeof(m_stPgmLoadStatus.szErrMsg), "%s", msg.toStdString().c_str() );

	stTcpHeader tcpheader;
	tcpheader.cmd    = PROTOCOL::TCP_COMMAND::CMD_PGM_LOAD_STATUS;
	tcpheader.dest   = PROTOCOL::_ATMAIN_UI;
	tcpheader.runner = 0x0;
	tcpheader.error  = PROTOCOL::ACK_NORMAL;
	tcpheader.slot   = m_SlotCpu.nSlotNo-1;
	tcpheader.datalen = sizeof(stPgmLoadStatus);

	uint size = sizeof(stTcpHeader) + sizeof(stPgmLoadStatus);
	char* pBuffer = new char [size ];
	memcpy( pBuffer                    , &tcpheader        , sizeof(stTcpHeader)     );
	memcpy( pBuffer+sizeof(stTcpHeader), &m_stPgmLoadStatus, sizeof(stPgmLoadStatus) );

	QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("%1 %2 : %3[%4]")
					 .arg(sHead).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_PGM_LOAD_STATUS)).arg(bSuccess).arg(msg) ) ;

	emit SigMgrSendPacket( pBuffer, size );

	return true;
}

void CLoader::SlotDownloadProgress(int progress)
{
	int nProgress = progress / 2;
	SetDownloadStatusProgress( nProgress );
}


///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::LoadFtpIni
/// \return
/// \details /opt/atos/config/ewsftp.ini
/// \details [EWS_FTP]
/// \details IP=192.168.62.100
/// \details USER=ablee
/// \details PORT=23
/// \details PASSWD=ablee
/// \details not use
bool CLoader::LoadFtpIni()
{
	QString ip, id, passwd;
	uint16_t port = 23;
	QString key = "";
	QString val = "";
	QString ini_file = tr("%1/%2/%3").arg(m_sCurrentOsPath).arg(DIR_CONFIG_PATH).arg(FTP_INI);

	//IP
	key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_IP);
	if( !UTIL::INI::readSettings( ini_file, key, ip ) )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1-%2 read error!").arg(ini_file).arg(key) );
		return false;
	}

	//PORT
	QVariant qvar;
	key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PORT);
	if( !UTIL::INI::readSettings( ini_file, key, qvar ) )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1-%2 read error!").arg(ini_file).arg(key) );
		return false;
	}
	bool bOk = false;
	port = static_cast<uint16_t>(qvar.toUInt(&bOk));

	//USER
	key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_USER);
	if( !UTIL::INI::readSettings( ini_file, key, id ) )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1-%2 read error!").arg(ini_file).arg(key) );
		return false;
	}

	//PASSWORD
	key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PASSWD);
	if( !UTIL::INI::readSettings( ini_file, key, passwd ) )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1-%2 read error!").arg(ini_file).arg(key) );
		return false;
	}
	//qDebug() << __FUNCTION__ << "ip : " << ip;
	//qDebug() << __FUNCTION__ << "port : " << port;
	//qDebug() << __FUNCTION__ << "id : " << id;
	//qDebug() << __FUNCTION__ << "passwd : " << passwd;
	m_pDownloader->SetFtpInfo( ip, port, id, passwd );
	return true;
}

bool CLoader::MakeFtpIni()
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
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(FTP_INI);

	//IP
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_IP);
	if( !UTIL::INI::writeSettings( ini, section_key, m_EwsFtpInfo.szServer) )
	{
		bOk = false;
	}

	//USER
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_USER);
	if( !UTIL::INI::writeSettings( ini, section_key, m_EwsFtpInfo.szId) )
	{
		bOk = false;
	}

	//PASSWD
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PASSWD);
	if( !UTIL::INI::writeSettings( ini, section_key, m_EwsFtpInfo.szPasswd) )
	{
		bOk = false;
	}

	//PASSWD
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PORT);
	QString sPort = tr("%1").arg(m_EwsFtpInfo.uPort);
	if( !UTIL::INI::writeSettings( ini, section_key, sPort) )
	{
		bOk = false;
	}

	return bOk;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::DeleteDownloadDirectory
/// \return
/// \details program download directory all file remove
bool CLoader::DeleteDownloadDirectoryFiles()
{
	bool bRemoved = true;
	QDir dir(m_sDownloadPath);
	QFileInfoList filelist = dir.entryInfoList( QDir::Files | QDir::NoDotAndDotDot );
	//qDebug() << __FUNCTION__ << "dir : " << m_sDownloadPath;
	foreach( QFileInfo file, filelist )
	{
		//qDebug() << "remove file : " << file.absoluteFilePath();
		bRemoved &= QFile::remove( file.absoluteFilePath() );
	}
	return bRemoved;
}

bool CLoader::PrepareDownload()
{
	DeleteDownloadDirectoryFiles();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::DownloadProgram
/// \param stPgmLoad
/// \return return
bool CLoader::DownloadProgram()
{
	if( !PrepareDownload() )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1 prepare download error!").arg(m_stPgmLoad.szFileName) );
		return false;
	}
	m_stPgmLoadStatus.bSuccess   = true;
	m_stPgmLoadStatus.nProgress  = 0x0;
	memset( m_stPgmLoadStatus.szErrMsg, 0x0, sizeof(m_stPgmLoadStatus.szErrMsg) );

	m_pDownloader->SetDownloadFileInfo( m_stPgmLoad.szFilePath, m_stPgmLoad.szFileName, m_stPgmLoad.nFileSize, m_stPgmLoad.bCompress );
	emit SigDownloadProgram();
	return true;
}

void CLoader::SetDownloadStatusProgress(int prg)
{
	m_stPgmLoadStatus.nProgress = prg;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// \brief CLoader::ExecuteProgram
/// \return
/// \details file executable check & execute device program
bool CLoader::StandbyExecuteProgram()
{
	bool bCopy  = true;
	bool bChmod = true;
	QString msg = "";
	QString org = tr("%1/%2").arg(m_sDownloadPath).arg(m_stPgmLoad.szFileName);
	QString run = tr("%1/%2").arg(m_sCurrentWorkPath).arg(m_stPgmLoad.szFileName);

	QFile::remove( run );
	if( QFile::copy  ( org, run ) )
	{
		m_stPgmLoadStatus.nProgress = 70;
		m_stPgmLoadStatus.bSuccess = true;
		msg = "Copy to run path success!";
	}
	else
	{
		bCopy = false;
		m_stPgmLoadStatus.bSuccess = false;
		msg = "Copy to run path error!";
	}
	AckPgmLoadStatus( m_stPgmLoadStatus.bSuccess, msg );
	if( !bCopy )
	{
		emit SigSaveLog( _ERROR_LOG, tr("Copy %1 to Run Directory(%2)").arg(m_stPgmLoad.szFileName).arg(m_sCurrentWorkPath) );
		return false;
	}
	emit SigSaveLog( _NORMAL_LOG, tr("Copy %1 to Run Directory(%2)").arg(m_stPgmLoad.szFileName).arg(m_sCurrentWorkPath) );

	emit SigDevMgrProgramInfo( m_sCurrentWorkPath, m_stPgmLoad.szFileName );
	if( ChmodSetX(run) )
	{
		m_stPgmLoadStatus.nProgress = 90;
		m_stPgmLoadStatus.bSuccess = true;
		msg = tr("%1 chmod succcess!").arg(run);
		emit SigSaveLog( _NORMAL_LOG, tr("%1/%2 permission change").arg(m_sCurrentWorkPath).arg(m_stPgmLoad.szFileName) );
	}
	else
	{
		bChmod = false;
		m_stPgmLoadStatus.nProgress = 70;
		m_stPgmLoadStatus.bSuccess  = false;
		msg = tr("%1 chmod error!").arg(run);
		emit SigSaveLog( _ERROR_LOG, tr("%1/%2 permission change").arg(m_sCurrentWorkPath).arg(m_stPgmLoad.szFileName) );
	}
	AckPgmLoadStatus( m_stPgmLoadStatus.bSuccess, msg );
	return bChmod;
}


bool CLoader::ChmodSetX( QString file )
{
	struct stat statbuf;

	if ( stat( file.toStdString().c_str(), &statbuf) < 0 )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1 chmod stat error!").arg(file) );
		return false;
	}
	if( chmod( file.toStdString().c_str(), 0755 ) < 0 )	//-rwx-r-x-r-x
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1 chmod run error!").arg(file) );
		return false;
	}
	return true;
}


bool CLoader::ReloadProgram()
{
	return true;
}

bool CLoader::SetSlotCpuInfo( stTcpElement& element )
{
	bool bOk = true;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.tcpHeader.cmd );
	if( element.tcpHeader.datalen > 0 )
	{
		QString sHead = tr("[%1][RECV]").arg("<-MGR", 7, QChar(' ') );
		if( element.tcpHeader.datalen == sizeof(stSlotCpu) )
		{
			memcpy( &m_SlotCpu, element.pData, element.tcpHeader.datalen );
			emit SigSaveLog( _NORMAL_LOG,
							 tr("%1 %2:[ SLOT:%3 / CPU_PER_SLOT:%4 / PGM_PER_CPU:%5 ]")
							 .arg(sHead).arg( UTIL::TCPIP::CMD_STR(cmd)).arg(m_SlotCpu.nSlotNo).arg(m_SlotCpu.nCpuPerSlot).arg(m_SlotCpu.nPgmPerSlot) );
			emit SigDevMgrSlotCpuInfo( m_SlotCpu.nSlotNo, m_SlotCpu.nCpuPerSlot, m_SlotCpu.nPgmPerSlot );
		}
		else
		{
			emit SigSaveLog( _ERROR_LOG, tr("%1 %2 Data length error").arg(sHead).arg( UTIL::TCPIP::CMD_STR(cmd)) );
			bOk = false;
		}
	}
	return AckSlotCpuInfo( bOk );
}

bool CLoader::SetPgmLoadInfo( stTcpElement& element )
{
	bool bOk = true;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.tcpHeader.cmd );
	int nDataLen = element.tcpHeader.datalen;

	QString sHead = tr("[%1][RECV]").arg("<-MGR", 7, QChar(' ') );
	emit SigSaveLog( _NORMAL_LOG, tr("%1 %2").arg(sHead).arg( UTIL::TCPIP::CMD_STR(cmd)) );
	if( nDataLen != sizeof(stPgmDownload) )
	{
		emit SigSaveLog( _ERROR_LOG, tr("%1 packet error expect[%2], receive[%3]").arg(UTIL::TCPIP::CMD_STR(cmd)).arg(sizeof(stPgmDownload)).arg(nDataLen) );
		bOk = false;
	}
	else
	{
		memset( &m_stPgmLoad, 0x0          , sizeof(stPgmDownload)     );
		memcpy( &m_stPgmLoad, element.pData, element.tcpHeader.datalen );
		DownloadProgram();
	}
	return bOk;
}

bool CLoader::SetEwsFtpInfo( stTcpElement& element )
{
	bool bOk = true;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.tcpHeader.cmd );
	if( element.pData )
	{
		QString sHead = tr("[%1][RECV]").arg("<-MGR", 7, QChar(' ') );
		if( element.tcpHeader.datalen == sizeof(stFtpInfo) )
		{
			memcpy( &m_EwsFtpInfo, element.pData, element.tcpHeader.datalen );
			emit SigSaveLog( _NORMAL_LOG,
							 tr("%1 %2:[ FTP_SVR:%3 / FTP_USER:%4 / FTP_PASSWD:%5 / FTP_PORT:%6 ]")
							 .arg(sHead).arg( UTIL::TCPIP::CMD_STR(cmd)).arg(m_EwsFtpInfo.szServer).arg(m_EwsFtpInfo.szId).arg(m_EwsFtpInfo.szPasswd).arg(m_EwsFtpInfo.uPort) );
			m_pDownloader->SetFtpInfo( m_EwsFtpInfo.szServer, m_EwsFtpInfo.uPort, m_EwsFtpInfo.szId, m_EwsFtpInfo.szPasswd );
			bOk = MakeFtpIni();
		}
		else
		{
			emit SigSaveLog( _ERROR_LOG, tr("%1 %2:Data length error [expect:%3, receive:%4]")
							 .arg(sHead).arg( UTIL::TCPIP::CMD_STR(cmd)).arg(sizeof(stFtpInfo)).arg(element.tcpHeader.datalen) );
			bOk = false;
		}
	}
	return AckEwsFtpInfo( bOk );
}

bool CLoader::AckSlotCpuInfo(bool ok)
{
	stTcpHeader header;
	header.cmd  = PROTOCOL::CMD_SLOT_CPU_INFO_ACK;
	header.src  = PROTOCOL::_ATLODER;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.error= ok ? PROTOCOL::ACK_NORMAL : PROTOCOL::NACK_ERROR;
	header.datalen = 0x0;
	uint size = sizeof(stTcpHeader);
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&header, sizeof(stTcpHeader) );
	QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
	emit SigMgrSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("%1 %2:[CODE:%3]").arg(sHead).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_SLOT_CPU_INFO_ACK)).arg(header.error) ) ;
	//qDebug() << tr("%1 %2 [CODE:%3]").arg(sHead).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_SLOT_CPU_INFO_ACK)).arg(header.error);

	return true;
}

bool CLoader::AckEwsFtpInfo(bool ok)
{
	stTcpHeader header;
	header.cmd  = PROTOCOL::CMD_EWS_FTP_INFO_ACK;
	header.src  = PROTOCOL::_ATLODER;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.error= ok ? PROTOCOL::ACK_NORMAL : PROTOCOL::NACK_ERROR;
	header.datalen = 0x0;
	uint size = sizeof(stTcpHeader);
	char* pBuf = new char[ size ];
	memcpy( pBuf, (char*)&header, sizeof(stTcpHeader) );
	emit SigMgrSendPacket( pBuf, size );
	QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("%1 %2:[CODE:%3]").arg(sHead).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_EWS_FTP_INFO_ACK)).arg(header.error) ) ;
	//qDebug() << tr("%1 %2[CODE:%3]").arg(sHead).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_EWS_FTP_INFO_ACK)).arg(header.error) ;
	return true;
}

bool CLoader::AckVersion()
{
	char szBuf[64] = {0x0,};
	memset( szBuf, 0x0, sizeof(szBuf) );
	snprintf( szBuf, sizeof(szBuf), "%s", ATLOADER_VERSION );
	szBuf[strlen(szBuf)+1]='\0';
	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_VERSION_ACK;
	header.src  = PROTOCOL::_ATLODER;
	header.slot    = m_SlotCpu.nSlotNo-1;
	header.runner  = CPU00;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = strlen(szBuf)+1;
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), szBuf         , header.datalen      );

	QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
	emit SigMgrSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("%1 %2:%3").arg(sHead).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_VERSION_ACK)).arg(szBuf) );
	return true;
}

bool CLoader::AckReadBibId(PROTOCOL::TCP_COMMAND ack_cmd)
{
	bool bIoErr = false;
	bool bib_exist = false;
	m_nBibId = 0x0;
	if( !m_pcieTpg0->pcie_open() )
	{
		emit SigSaveLog( _NORMAL_LOG, tr("Open H/W error!") );
		bIoErr = true;
	}
	else
	{
		//2023.08.04 - KWMOON
		int nConn = m_pcieTpg0->read_user( BITCHART::DRV_IOA::_BOARD_CONTACT );
		if( nConn & (0x1<<1) )
		{
			bib_exist = true;
		}
		EN_BIB_SEL sels[BIB_MAX_SEL] = { LBIB_SEL0, LBIB_SEL1, LBIB_SEL2, LBIB_SEL3 };
		for( int i=0;i<BIB_MAX_SEL;i++ )
		{
			m_pcieTpg0->write_user( BITCHART::DRV_IOA::_BIB_SEL, sels[i] );
			QThread::usleep( 1000 );                                            //2023.03.31 - KWMOON( 100 us -> 1 ms )
			int nRead = m_pcieTpg0->read_user( BITCHART::DRV_IOA::_BIB_ID );
			m_nBibId |= ( nRead<<(i*8) );
			QThread::usleep( 100 );
		}
		m_pcieTpg0->pcie_close();
	}
	stIpcHeader header;
	header.cmd     = ack_cmd;
	header.src     = PROTOCOL::_ATLODER;
	header.slot    = m_SlotCpu.nSlotNo-1;
	header.runner  = CPU00;
	header.dest    = PROTOCOL::_ATMAIN_UI;
	header.datalen = sizeof(stBibId);
	header.error   = EN_BIBIID_ACK;

	stBibId bibid;
	memset( &bibid, 0x0, sizeof(stBibId) );
	if( bIoErr )
	{
		header.error = EN_BIBID_IO_ERR;
		bibid.bBibConn = false;
		bibid.nBibId = 0x0;
		snprintf( bibid.szBibId, sizeof(bibid.szBibId), "%d", 0x0 );
		emit SigSaveLog( LOG_TYPE::_ERROR_LOG,"H/W open error!" );
		QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
		emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
						 tr("%1 %2:[BIB_ID:%3]")
						 .arg(sHead).arg(UTIL::TCPIP::CMD_STR(ack_cmd)).arg(bibid.szBibId) ) ;
	}
	else
	{
		bibid.bBibConn = bib_exist;	//2023.08.04 - KWMOON
		if( !IsValidBibIdParity( m_nBibId ) )
		{
			emit SigSaveLog( LOG_TYPE::_ERROR_LOG,"BIB ID parity error!" );
			if( bib_exist)
			{
				header.error = EN_BIBID_PARITY;
				bibid.bParityError = true;
			}
			else
			{
				header.error = EN_BIBID_EMPTY;
				bibid.bParityError = false;
			}
		}
		else
		{
			header.error = EN_BIBIID_ACK;
			bibid.bParityError = false;
		}
		bibid.nBibId  = m_nBibId;
		snprintf( bibid.szBibId, sizeof(bibid.szBibId), "%03X%05d", (m_nBibId>>16)&0xFFF, (m_nBibId>> 0)&0xFFFF );

		QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
		emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
						 tr("%1 %2:[BIB_ID:%3]")
						 .arg(sHead).arg(UTIL::TCPIP::CMD_STR(ack_cmd)).arg(bibid.szBibId) ) ;
	}

	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &bibid         , header.datalen      );

	emit SigMgrSendPacket( pBuf, size );
	return false;
}

bool CLoader::IsValidBibIdParity(uint bibid)
{
	bool bValid = true;
	int nTotalOneSum = 0x0;
	int nDiodeSel    = (bibid >> 28) & 0xF;
	int nBitSize     = 0x0;
	for( int i=0;i<BIB_MAX_SEL;i++ )
	{
		int nSelOneSum = 0x0;
		int nSelByte   = (bibid >> (i * 8)) & 0xFF;
		int nSelParity = (nDiodeSel >> i) & 0x1;

		if (i == 3) nBitSize = 0x4;
		else        nBitSize = 0x8;

		for (int bit = 0; bit<nBitSize; bit++)
		{
			if ((1 << bit) & nSelByte)
				nSelOneSum++;
		}
		if (nSelParity == 0)
		{
			if ((nSelOneSum % 2) == 0)
			{
				bValid = false;
				emit SigSaveLog( _ERROR_LOG, tr("BIBID SEL%1 parity error[parity:%2, diode count:%3]").arg(i).arg(nSelParity).arg(nSelOneSum) );
				//sLogMsg.sprintf("[ ERR] BIB ID SEL%d Parity Error [ Parity : %d, Diode Count : %d]", nSel, nSelParity, nSelOneSum);
			}
		}
		else
		{
			if ((nSelOneSum % 2) == 1)
			{
				bValid = false;
				emit SigSaveLog( _ERROR_LOG, tr("BIBID SEL%1 parity error[parity:%2, diode count:%3]").arg(i).arg(nSelParity).arg(nSelOneSum) );
			}
		}
		nTotalOneSum += nSelOneSum;
	}
	return bValid;
}

void CLoader::SlotDevMgrLogMessage( LOG_TYPE t, QString s )
{
	emit SigSaveLog( t, s );
}

void CLoader::SlotDevMgrRunAck( bool result, QString msg )
{
	m_stPgmLoadStatus.bSuccess  = result;
	m_stPgmLoadStatus.nProgress = result?100:90;
	AckPgmLoadStatus( m_stPgmLoadStatus.bSuccess, msg );
	QThread::usleep( 100 );
	AckPgmLoad( result, false  );
}

void CLoader::SlotDevMgrReRunAck( bool result, QString msg )
{
	m_stPgmLoadStatus.bSuccess  = result;
	m_stPgmLoadStatus.nProgress = result?100:90;
	AckPgmLoadStatus( m_stPgmLoadStatus.bSuccess, msg );
	QThread::usleep( 100 );
	AckPgmLoad( result, true  );
}

void CLoader::SlotDevMgrUnloadAck( bool result, QString msg )
{
	m_stPgmLoadStatus.bSuccess  = result;
	m_stPgmLoadStatus.nProgress = result?100:90;
	AckPgmLoadStatus( m_stPgmLoadStatus.bSuccess, msg );
	QThread::usleep( 100 );
	AckPgmUnload( result, msg );
}

void CLoader::SlotDevMgrPgmFinished( uint runner )
{
	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_PGM_FINISHED;
	header.src  = PROTOCOL::_ATLODER;
	header.dest = PROTOCOL::_ATMGR;
	header.slot = m_SlotCpu.nSlotNo-1;
	header.runner = runner;
	header.datalen = 0;
	uint size = sizeof(stIpcHeader);
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );

	QString sHead = tr("[%1][SEND]").arg("->MGR", 7, QChar(' ') );
	emit SigMgrSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("%1 %2 RUN:%3 finished!").arg(sHead).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_PGM_FINISHED)).arg(runner) );
}
