#include "testmgr.hpp"

CTestMgr::CTestMgr() : m_pLogger(nullptr), m_pIpcSvr(nullptr), m_pLoaderMgr(nullptr)
 , m_pDevCtrlMgr(nullptr), m_pDevDataMgr(nullptr), m_pDevLogMgr(nullptr), m_pDevCtrlMgrTh(nullptr), m_pLoaderMgrTh(nullptr)
 , m_pDevDataMgrTh(nullptr), m_pDevLogMgrTh(nullptr), m_pChmbMgr(nullptr)
{
	m_bUiCon = false;
	m_bDebugPrint = false;
	m_bIniValid = true;
	m_vecIpcs.clear();

	memset( &m_stSlotInfo , 0x0, sizeof(m_stSlotInfo)  );
	memset( &m_stSlotLive , 0x0, sizeof(m_stSlotLive)  );
	memset( &m_stPgmLive  , 0x0, sizeof(m_stPgmLive )  );

	RegisterMetaType();
	StandByDirectory();
}

CTestMgr::~CTestMgr()
{
	DeleteInstance();
}

void CTestMgr::StandByDirectory()
{
	char* env_run  = getenv(ATRUN_DIR_ENV);
	char* env_data = getenv(ATDATA_DIR_ENV);

	QString os_path   = "";
	QString run_path  = "";
	QString data_path = "";
	if( env_run != NULL )
	{
		run_path = QString(env_run);
	}
	else
	{
		run_path = tr("%1/%2").arg(QDir::home().absolutePath()).arg("atsystem");
	}

	if( env_data != NULL )
	{
		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
	}
	else
	{
		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);
	}

	QDir dir(run_path);
	if( !dir.exists( run_path)  )	dir.mkdir( run_path  );
	if( !dir.exists( data_path) )	dir.mkdir( data_path );

	QDir::setCurrent( run_path );	
	m_sCurrentDir = run_path;
	m_sDataDir    = data_path;
}

void CTestMgr::Terminate(int code)
{
	QThread::msleep( 100 );
	qApp->exit( code );
}

void CTestMgr::DeleteInstance()
{
	emit SigStopLoaderMgr();

	if( m_pLoaderMgrTh )
	{
		m_pLoaderMgrTh->quit();
		m_pLoaderMgrTh->wait();
		m_pLoaderMgrTh->deleteLater();
	}

	if( m_pIpcSvr )
	{
		if( m_pIpcSvr->isListening() )
			m_pIpcSvr->close();		
		m_pIpcSvr->deleteLater();
		QLocalServer::removeServer( IPC_INFO::IPC_TESTMGR_SOCK );
	}

	//Device Program Ctrl Thread
	if( m_pDevCtrlMgrTh )
	{
		m_pDevCtrlMgrTh->quit();
		m_pDevCtrlMgrTh->wait();
		m_pDevCtrlMgrTh->deleteLater();
	}

	//Device Program Data Thread
	if( m_pDevDataMgrTh )
	{
		m_pDevDataMgrTh->quit();
		m_pDevDataMgrTh->wait();
		m_pDevDataMgrTh->deleteLater();
	}

	//Device Program Log Thread - 2023.08.13
	if( m_pDevLogMgrTh )
	{
		m_pDevLogMgrTh->quit();
		m_pDevLogMgrTh->wait();
		m_pDevLogMgrTh->deleteLater();
	}

	if( m_pLogger )
	{
		m_pLogger->deleteLater();
	}
	if( m_pTestDataMgr )
	{
		delete m_pTestDataMgr;
	}
	for( CLocalClient* pClient : m_vecIpcs )
	{
		m_vecIpcs.removeOne( pClient );
		pClient->deleteLater();
	}
}

void CTestMgr::RegisterMetaType()
{
	qRegisterMetaType<stTcpElement>("stTcpElement");
	qRegisterMetaType<stIpcElement>("stIpcElement");
	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");
	qRegisterMetaType<stSlotIp>("stSlotIp");
	qRegisterMetaType<stSlotCpu>("stSlotCpu");
	qRegisterMetaType<stSlotInfo>("stSlotInfo");
	qRegisterMetaType<stSlotLive>("stSlotLive");
	qRegisterMetaType<stPgmLive>("stPgmLive");
	qRegisterMetaType<stVersion>("stVersion");
	qRegisterMetaType<stBibId>("stBibId");
	qRegisterMetaType<QLocalSocket::LocalSocketError>("QLocalSocket::LocalSocketError");
	//2023.05.15 - KWMOON
	qRegisterMetaType<PROTOCOL::PGM_CHMB_REQ_ACK>("PROTOCOL::PGM_CHMB_REQ_ACK");
	qRegisterMetaType<stChmbMgrSv>("stChmbMgrSv");
	qRegisterMetaType<stSetTemp>("stSetTemp");
	qRegisterMetaType<stGetTemp>("stGetTemp");
}

void CTestMgr::CreateInstance()
{
	m_pLogger      = new CLogger( PROTOCOL::SOURCE_TYPE::_ATMGR );
	m_pIpcSvr      = new CLocalServer( this );
	m_pDevCtrlMgr  = new CDevCtrlMgr;
	m_pDevDataMgr  = new CDevDataMgr;
	m_pDevLogMgr   = new CDevLogMgr;
	m_pLoaderMgr   = new CLoaderMgr;
	m_pChmbMgr     = new CChmbMgr;		//2023.05.13 - kwmoon
	m_pTestDataMgr = new CFileMap( m_sDataDir.toStdString().c_str() );
	m_pLogger->SetLogDir( m_sCurrentDir );
	m_pLogger->SetDebugPrint( m_bDebugPrint );
	connect( this     , &CTestMgr::SigSaveLog                  , m_pLogger, &CLogger::SlotLogMessage    );
	connect( m_pIpcSvr, &CLocalServer::SigIncommingConnection  , this     , &CTestMgr::SlotIpcConnected );		//ui

	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("%1").arg(ATMGR_VERSION) );
    emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("%1:%2").arg(ATRUN_DIR_ENV).arg(m_sCurrentDir) );
    emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("%1:%2").arg(ATDATA_DIR_ENV).arg(m_sDataDir) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CTestMgr::run
/// \param argc
/// \param argv
/// \return
/// \details TestMgr Main Method
int CTestMgr::run( int argc, char* argv[] )
{
	QCoreApplication app(argc, argv);
	if( argc > 1 )
	{
		for (int i = 1; i < argc; i++)
		{
			if ( strncasecmp(argv[i], "-D"  , 2) == 0 )
			{
				m_bDebugPrint  = true;
			}
			else if ( strncasecmp(argv[i], "/D"  , 2) == 0 )
			{
				m_bDebugPrint  = true;
			}
		}
		if( m_bDebugPrint ) fprintf( stdout, "%s Debug Print On\n", RC_ATMGR_VERSION );
	}

	CreateInstance();
	RunLocalServer();
	if( LoadIni() )
	{
		RunTcpLoaderManager();
		RunPgmCtrlManager();
		RunPgmDataManager();
		RunPgmLogManager();			//2023.08.13 - kwmoon
		RunChamberManager();
	}
	else
	{
		m_bIniValid = false;
		emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("%1 read error!").arg(SITE_INI) );
	}

	return app.exec();
}

bool CTestMgr::LoadIni()
{
	char* env_os  = getenv(ATOS_DIR_ENV);
	bool bOk = true;
	QString os_path = "";
	QString section_key = "";
	QString sValue = 0x0;
	int nValue = 0x0;

	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(SITE_INI);

	//MAX_SLOT
	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_MAX_SLOT);
	if( !UTIL::INI::readSettings( ini, section_key, nValue ) )
	{
		m_stSlotInfo.SlotCpu.nMaxSlot = MAX_SLOT;
		bOk = false;
	}
	else
	{
		m_stSlotInfo.SlotCpu.nMaxSlot = nValue;
	}

	//CPU_PER_SLOT
	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_CPU_PER_SLOT);
	if( !UTIL::INI::readSettings( ini, section_key, nValue ) )
	{
		m_stSlotInfo.SlotCpu.nCpuPerSlot = 1;
		bOk = false;
	}
	else
	{
		m_stSlotInfo.SlotCpu.nCpuPerSlot = nValue;
	}

	//PGM_PER_SLOT
	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_PGM_PER_SLOT);
	if( !UTIL::INI::readSettings( ini, section_key, nValue ) )
	{
		m_stSlotInfo.SlotCpu.nPgmPerSlot = 2;
		bOk = false;
	}
	else
	{
		m_stSlotInfo.SlotCpu.nPgmPerSlot = nValue;
	}

	for( int i=0;i<m_stSlotInfo.SlotCpu.nMaxSlot;i++ )
	{
		QString key_mask = tr("SLOT%1/%2").arg(i+1).arg(KEY_SITE_SLOT_MASK);
		QString key_ip1  = tr("SLOT%1/%2").arg(i+1).arg(KEY_SITE_CPU1_IP);
		QString key_ip2  = tr("SLOT%1/%2").arg(i+1).arg(KEY_SITE_CPU2_IP);

		//SLOT MASK
		if( !UTIL::INI::readSettings( ini, key_mask, nValue ) )
		{
			m_stSlotInfo.SlotIp[i].bMask = true;
			m_stSlotLive.slot_status[i][CPU00] = EN_SLOT_MASK;
			m_stSlotLive.slot_status[i][CPU01] = EN_SLOT_MASK;
		}
		else
		{
			m_stSlotInfo.SlotIp[i].bMask = (nValue>0)?true:false;
			if( m_stSlotInfo.SlotIp[i].bMask )
			{
				m_stSlotLive.slot_status[i][CPU00] = EN_SLOT_MASK;
				m_stSlotLive.slot_status[i][CPU01] = EN_SLOT_MASK;
			}
			else
			{
				m_stSlotLive.slot_status[i][CPU00] = EN_SLOT_OFFLINE;
				m_stSlotLive.slot_status[i][CPU01] = EN_SLOT_OFFLINE;
			}
		}

		//CPU IP1
		if( !UTIL::INI::readSettings( ini, key_ip1, sValue ) )
		{
			snprintf( m_stSlotInfo.SlotIp[i].szCpu1Ip, sizeof(m_stSlotInfo.SlotIp[i].szCpu1Ip), "%s.%d", KEY_SITE_CPU_IP_START, i+1 );
			UTIL::INI::writeSettings( ini, key_ip1, QString(m_stSlotInfo.SlotIp[i].szCpu1Ip) );
		}
		else
		{
			snprintf( m_stSlotInfo.SlotIp[i].szCpu1Ip, sizeof(m_stSlotInfo.SlotIp[i].szCpu1Ip), "%s", sValue.toStdString().c_str() );
		}

		//if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
		{
			//CPU IP2
			if( !UTIL::INI::readSettings( ini, key_ip2, sValue ) )
			{
				snprintf( m_stSlotInfo.SlotIp[i].szCpu2Ip, sizeof(m_stSlotInfo.SlotIp[i].szCpu2Ip), "%s.%d", KEY_SITE_CPU_IP_START, i+1+m_stSlotInfo.SlotCpu.nMaxSlot );
				UTIL::INI::writeSettings( ini, key_ip2, QString(m_stSlotInfo.SlotIp[i].szCpu2Ip) );
			}
			else
			{
				snprintf( m_stSlotInfo.SlotIp[i].szCpu2Ip, sizeof(m_stSlotInfo.SlotIp[i].szCpu2Ip), "%s", sValue.toStdString().c_str() );
			}
		}
		if( 0 )
		{
			qDebug() << tr("SLOT%1 MASK:%2 , IP1:%3, IP2:%4")
						.arg(i+1).arg(m_stSlotInfo.SlotIp[i].bMask).arg(m_stSlotInfo.SlotIp[i].szCpu1Ip).arg(m_stSlotInfo.SlotIp[i].szCpu2Ip);
		}
	}

	return bOk;
}

//Main Ui
bool CTestMgr::RunLocalServer()
{
	if( m_pIpcSvr->isListening() ) return true;

	const int RETRY_COUNT = 10;
	int retry = 0;

	QLocalServer::removeServer( IPC_INFO::IPC_TESTMGR_SOCK );
	do
	{
		if( !m_pIpcSvr->listen( IPC_INFO::IPC_TESTMGR_SOCK ) )
			m_pIpcSvr->close();
		else
			break;
		usleep( 5000 );
	}while( retry++ < RETRY_COUNT );

	if( !m_pIpcSvr->isListening() )
	{
		emit SigSaveLog( LOG_TYPE::_ERROR_LOG , tr("IPC Server(%1) Listen NG!").arg(IPC_INFO::IPC_TESTMGR_SOCK) );
	}
	else
	{
		emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("IPC Server(%1) Listen OK!").arg(IPC_INFO::IPC_TESTMGR_SOCK) );
	}
	return m_pIpcSvr->isListening();
}

//Test Pgm contrl
bool CTestMgr::RunPgmCtrlManager()
{
	m_pDevCtrlMgrTh = new QThread;
	m_pDevCtrlMgr->moveToThread( m_pDevCtrlMgrTh );
	m_pDevCtrlMgr->SetSlotInfo( m_stSlotInfo );
	connect( m_pDevCtrlMgrTh , &QThread::finished                  , m_pDevCtrlMgr, &QObject::deleteLater              );
	connect( m_pDevCtrlMgr   , &CDevCtrlMgr::SigDevLogMessage      , this         , &CTestMgr::SlotDevCtrlLogMessage    );
	connect( m_pDevCtrlMgr   , &CDevCtrlMgr::SigDevRecvPacket      , this         , &CTestMgr::SlotDevCtrlRecvPacket    );
	connect( m_pDevCtrlMgr   , &CDevCtrlMgr::SigDevStatusChanged   , this         , &CTestMgr::SlotDevCtrlStatusChanged );
	connect( this            , &CTestMgr::SigRunDevCtrlMgr         , m_pDevCtrlMgr, &CDevCtrlMgr::SlotRunService        );
	connect( this            , &CTestMgr::SigSendPacketToDevCtrlMgr, m_pDevCtrlMgr, &CDevCtrlMgr::SlotDevMgrSendPacket  );

	m_pDevCtrlMgrTh->start();
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("Device Program Ctrl Manager Start!") );
	emit SigRunDevCtrlMgr();
	return m_pDevCtrlMgrTh->isRunning();
}

//2023.05.13 - kwmoon(pgm chmb ctrl)
bool CTestMgr::RunChamberManager()
{
	m_pChmbMgrTh = new QThread;
	m_pChmbMgr->SetSlotInfo( m_stSlotInfo );
	m_pChmbMgr->moveToThread( m_pChmbMgrTh );

	connect( m_pChmbMgrTh , &QThread::finished                , m_pChmbMgr, &QObject::deleteLater             );
	connect( this         , &CTestMgr::SigLivePgmList         , m_pChmbMgr, &CChmbMgr::SlotSetLivePgmList     );
	connect( this         , &CTestMgr::SigRunChmbMgr          , m_pChmbMgr, &CChmbMgr::SlotChmbMgrStart       );
	connect( this         , &CTestMgr::SigChamberUse          , m_pChmbMgr, &CChmbMgr::SlotChamberUse         );
	connect( this         , &CTestMgr::SigSendChamberCmd      , m_pChmbMgr, &CChmbMgr::SlotRecvChamberCmd     );
    connect( this         , &CTestMgr::SigSendTestCmd         , m_pChmbMgr, &CChmbMgr::SlotRecvTestCmd        );
    connect( m_pChmbMgr   , &CChmbMgr::SigChmbMgrLogMessage   , this      , &CTestMgr::SlotChmbMgrLogMessage  );
	connect( m_pChmbMgr   , &CChmbMgr::SigChmbMgrAck          , this      , &CTestMgr::SlotRecvChmbMgrAck     );
	connect( m_pChmbMgr   , &CChmbMgr::SigChmbConnStatus      , this      , &CTestMgr::SlotRecvChmbConnStatus );
	connect( m_pChmbMgr   , &CChmbMgr::SigChmbTempMonSet      , this      , &CTestMgr::SlotChmbTempMonSet     );	//2024.08.14 - KWMOON
	connect( m_pChmbMgr   , &CChmbMgr::SigChmbTempMonEnd      , this      , &CTestMgr::SlotChmbTempMonEnd     );	//2024.08.14 - KWMOON
	m_pChmbMgrTh->start();
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("Test PGM Chamber Manager Start!") );
	emit SigRunChmbMgr();
	return m_pChmbMgrTh->isRunning();
}

void CTestMgr::SlotDevCtrlLogMessage( LOG_TYPE t, QString s)
{
	emit SigSaveLog( t, s );
}

void CTestMgr::SlotDevCtrlStatusChanged( uint slot, uint runner, bool stat )
{
	EN_SLOT_STAT pgm_stat = EN_SLOT_OFFLINE;
	PROTOCOL::EN_PGM_SOCKET_TYPE sock_type= PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET;
	if( stat == true ) pgm_stat = EN_SLOT_ONLINE;

	m_stPgmLive.pgm_status[slot][runner][sock_type] = pgm_stat;
}

//device program ctrl socket
void CTestMgr::SlotDevCtrlRecvPacket( stTcpElement tcpemt )
{
	uint bufsize = sizeof(stIpcHeader) + tcpemt.tcpHeader.datalen;
	uint datalen = tcpemt.tcpHeader.datalen;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( tcpemt.tcpHeader.cmd );
    char* pBuf     = new char [ bufsize ];
	char* pTestBuf = new char [ bufsize ];
    memcpy( pBuf    , &tcpemt.tcpHeader, sizeof(stIpcHeader) );
    memcpy( pTestBuf, &tcpemt.tcpHeader, sizeof(stIpcHeader) );
	if( datalen > 0 )
    {
		memcpy( pBuf    +sizeof(stIpcHeader), &tcpemt.pData, datalen );
		memcpy( pTestBuf+sizeof(stIpcHeader), &tcpemt.pData, datalen );
    }

	switch ( cmd )
	{
		case PROTOCOL::TCP_COMMAND::CMD_TEST_START_ACK:
		case PROTOCOL::TCP_COMMAND::CMD_ITEM_TEST_START_ACK:
        case PROTOCOL::TCP_COMMAND::CMD_TEST_STOP_ACK:
            emit SigSendTestCmd( pTestBuf, bufsize );           //2023.05.31 - KWMOON
			break;
		default:
			break;
	}

	emit SigIpcSendPacket( pBuf, bufsize );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1] %2").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(cmd)) );
	if( tcpemt.tcpHeader.datalen )
	{
		delete[] tcpemt.pData;
	}
}

//Test pgm data
bool CTestMgr::RunPgmDataManager()
{
	m_pDevDataMgrTh = new QThread;
	m_pDevDataMgr->moveToThread( m_pDevDataMgrTh );
	m_pDevDataMgr->SetSlotInfo( m_stSlotInfo );
	connect( m_pDevDataMgrTh , &QThread::finished                  , m_pDevDataMgr, &QObject::deleteLater               );
	connect( m_pDevDataMgr   , &CDevDataMgr::SigDevLogMessage      , this         , &CTestMgr::SlotDevDataLogMessage    );
	connect( m_pDevDataMgr   , &CDevDataMgr::SigDevRecvPacket      , this         , &CTestMgr::SlotDevDataRecvPacket    );
	connect( m_pDevDataMgr   , &CDevDataMgr::SigDevStatusChanged   , this         , &CTestMgr::SlotDevDataStatusChanged );
	connect( this            , &CTestMgr::SigRunDevDataMgr         , m_pDevDataMgr, &CDevDataMgr::SlotRunService        );
	connect( this            , &CTestMgr::SigSendPacketToDevDataMgr, m_pDevDataMgr, &CDevDataMgr::SlotDevDataSendPacket );

	m_pDevDataMgrTh->start();
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("Device Program Data Manager Start!") );
	emit SigRunDevDataMgr();
	return m_pDevDataMgrTh->isRunning();
}

//Test pgm log
bool CTestMgr::RunPgmLogManager()
{
	m_pDevLogMgrTh = new QThread;
	m_pDevLogMgr->moveToThread( m_pDevLogMgrTh );
	m_pDevLogMgr->SetSlotInfo( m_stSlotInfo );
	connect( m_pDevLogMgrTh , &QThread::finished                  , m_pDevLogMgr , &QObject::deleteLater              );
	connect( m_pDevLogMgr   , &CDevLogMgr::SigDevLogMessage       , this         , &CTestMgr::SlotDevLogLogMessage    );
	connect( m_pDevLogMgr   , &CDevLogMgr::SigDevRecvPacket       , this         , &CTestMgr::SlotDevLogRecvPacket    );
	connect( m_pDevLogMgr   , &CDevLogMgr::SigDevStatusChanged    , this         , &CTestMgr::SlotDevLogStatusChanged );
	connect( this           , &CTestMgr::SigRunDevLogMgr          , m_pDevLogMgr , &CDevLogMgr::SlotRunService        );

	m_pDevLogMgrTh->start();
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("Device Program Log Manager Start!") );
	emit SigRunDevLogMgr();
	return m_pDevLogMgrTh->isRunning();
}

void CTestMgr::SlotDevDataLogMessage( LOG_TYPE t, QString s)
{
	emit SigSaveLog( t, s );
}

void CTestMgr::SlotDevDataStatusChanged( uint slot, uint runner, bool stat )
{
	EN_SLOT_STAT pgm_stat = EN_SLOT_OFFLINE;
	PROTOCOL::EN_PGM_SOCKET_TYPE sock_type= PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET;
	if( stat == true ) pgm_stat = EN_SLOT_ONLINE;

	m_stPgmLive.pgm_status[slot][runner][sock_type] = pgm_stat;
	emit SigLivePgmList( m_stPgmLive );
}

//2023.08.13 - KWMOON(LOG SOCKET)
void CTestMgr::SlotDevLogLogMessage( LOG_TYPE t, QString s )
{
	emit SigSaveLog( t, s );
}

//2023.08.13 - KWMOON(LOG SOCKET)
void CTestMgr::SlotDevLogStatusChanged( uint slot, uint runner, bool stat )
{
	EN_SLOT_STAT pgm_stat = EN_SLOT_OFFLINE;
	PROTOCOL::EN_PGM_SOCKET_TYPE sock_type= PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET;
	if( stat == true ) pgm_stat = EN_SLOT_ONLINE;

	m_stPgmLive.pgm_status[slot][runner][sock_type] = pgm_stat;
	emit SigLivePgmList( m_stPgmLive );
}

//Device Program Log/Data Socket
void CTestMgr::SlotDevDataRecvPacket( stTcpElement element )
{
	bool bSentUi = false;
	uint slot    = element.tcpHeader.slot;
	uint runner  = element.tcpHeader.runner;
	uint datalen = element.tcpHeader.datalen;
	uint buflen  = sizeof(stTcpHeader) + datalen;
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>( element.tcpHeader.cmd );
	char* pSend = NULL;
    char* pTestBuf = NULL;
	//emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4[LEN:%5]")
	//				 .arg(" PACKET", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );

	switch ( cmd )
	{
		case PROTOCOL::TCP_COMMAND::CMD_DEV_PGM_VARIABLE:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( element.tcpHeader.datalen == sizeof(stDevPgmVariables) )
				{
					stDevPgmVariables pgmvar;
					memcpy(&pgmvar, element.pData, sizeof(stDevPgmVariables) );
					if( !m_pTestDataMgr->SET_DEVPGM_VAR( slot, runner, pgmvar ) )
					{
						emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
										 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					}
					else
					{
						emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done.")
										 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

						//2023.06.11 - KWMOON( PGM VARIABLE -> UI )
						pSend = new char [ buflen ];
						memcpy( pSend    , &element.tcpHeader, sizeof(stTcpHeader) );
						if( datalen > 0 )
						{
							memcpy( pSend+sizeof(stIpcHeader), element.pData, datalen );
						}
						emit SigIpcSendPacket( pSend, buflen);
						bSentUi = true;
					}
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 [EXPECT LEN:%5, RECV LEN:%6]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(sizeof(stDevPgmVariables)).arg(element.tcpHeader.datalen) );
				}
			}
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_INIT_TEST_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_INITFAIL_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.19 - KWMOON
		case PROTOCOL::TCP_COMMAND::CMD_CHIPID_INFO:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CHIPID_INFO( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.19 - KWMOON
		case PROTOCOL::TCP_COMMAND::CMD_CHIPID_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CHIPID_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_PGM_ITEM_LISTUP:
		{
			//qDebug() << "BEGIN " << __FUNCTION__;
			uint size = sizeof(stTestItem);
			if( element.tcpHeader.datalen != size )
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 [Expect Size:%5, Recv Size:%6]")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(size).arg(element.tcpHeader.datalen) );
				//Send error msg to ui
			}
			else
			{
				stTestItem items;
				memcpy( (char*)&items, element.pData, size );
				if( !m_pTestDataMgr->SET_TESTITEM( slot, runner, items ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("<-PGMDATA", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
				}
				else
				{
					//if( m_pTestDataMgr->IS_EXIST_TESTITEM(slot) )
					{
						stIpcHeader ipcheader;
						memcpy( &ipcheader , &element.tcpHeader, sizeof(stIpcHeader) );
						ipcheader.cmd = PROTOCOL::CMD_INIT_TEST_DONE;
						ipcheader.error = 0;
						ipcheader.datalen = 0;

						pSend = new char [ sizeof(stIpcHeader) ];
						memcpy( pSend, (char*)&ipcheader, sizeof(stIpcHeader) );
						emit SigIpcSendPacket( pSend, sizeof(stIpcHeader) );

						emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4")
										 .arg("->UI", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_INIT_TEST_DONE)) );
						bSentUi = false;
					}
				}
			}
			//qDebug() << "END " << __FUNCTION__;

		}
		break;

		//2023.04.25 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_STEP_IN_INFO:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPIN_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
					stStepInInfo stepin;
					memcpy( (char*)&stepin, element.pData, element.tcpHeader.datalen );
					//struct tm* t = localtime(&stepin.tStepBegin);
					//fprintf(stdout, "Step Begin : %04d-%02d-%02d %02d:%02d:%02d Step.%03d %s\n",
					//		t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, stepin.nStepNo, stepin.szStepName );

					//2023.06.09 - KWMOON
					//Send to Ui Cycle Info
					uint bufsize = sizeof(stTcpHeader) + element.tcpHeader.datalen;
					uint datalen = element.tcpHeader.datalen;
					char* pTestBuf = new char [ bufsize ];
					memcpy( pTestBuf, &element.tcpHeader, sizeof(stTcpHeader) );
					if( datalen > 0 )
					{
						memcpy( pTestBuf+sizeof(stTcpHeader), element.pData, datalen );
					}
					emit SigIpcSendPacket( pTestBuf, bufsize );
				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.25 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_STEP_OUT_INFO:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPOUT_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
					stStepOutInfo stepout;
					memcpy( (char*)&stepout, element.pData, element.tcpHeader.datalen );
					//struct tm* t = localtime(&stepout.tStepEnd);
					//fprintf(stdout, "Step Out : %04d-%02d-%02d %02d:%02d:%02d Step.%03d %s\n",
					//		t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, stepout.nStepNo, stepout.szStepName );

					//2023.06.09 - KWMOON
					//Send to Ui Cycle Info
					uint bufsize = sizeof(stTcpHeader) + element.tcpHeader.datalen;
					uint datalen = element.tcpHeader.datalen;
					char* pTestBuf = new char [ bufsize ];
					memcpy( pTestBuf, &element.tcpHeader, sizeof(stTcpHeader) );
					if( datalen > 0 )
					{
						memcpy( pTestBuf+sizeof(stTcpHeader), element.pData, datalen );
					}
					emit SigIpcSendPacket( pTestBuf, bufsize );
				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.26 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_CYCLE_IN_INFO:
		case PROTOCOL::TCP_COMMAND::CMD_CYCLE_OUT_INFO:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CYCLEINOUT_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
					stCycleInfo cycleinfo;
					memcpy( (char*)&cycleinfo, element.pData, element.tcpHeader.datalen );
					if( cmd == PROTOCOL::TCP_COMMAND::CMD_CYCLE_IN_INFO )
					{						
						//struct tm* t = localtime(&cycleinfo.tCycleBegin);
						//fprintf(stdout, "Cycle Begin : %04d-%02d-%02d %02d:%02d:%02d\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
					}
					else
					{
						//struct tm* t = localtime(&cycleinfo.tCycleEnd);
						//fprintf(stdout, "Cycle End : %04d-%02d-%02d %02d:%02d:%02d\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
					}
					//2023.06.09 - KWMOON
					//Send to Ui Cycle Info
					uint bufsize = sizeof(stTcpHeader) + element.tcpHeader.datalen;
					uint datalen = element.tcpHeader.datalen;
					char* pTestBuf = new char [ bufsize ];
					memcpy( pTestBuf, &element.tcpHeader, sizeof(stTcpHeader) );
					if( datalen > 0 )
					{
						memcpy( pTestBuf+sizeof(stTcpHeader), element.pData, datalen );
					}
					emit SigIpcSendPacket( pTestBuf, bufsize );
				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.25 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPFAIL_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.25 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_BLOCK_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPFAILBLOCK_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.25 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_CHIP_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPFAILCHIP_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

////////////////////////////////////////////////////////////////////////////////////////////////////
		//2024.07.22 fatric
		case PROTOCOL::TCP_COMMAND::CMD_STEP_FCATE_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPFCATE_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_STEP_PCATE_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPPCATE_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_STEP_FAIL_MODE:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPFAIL_MODE( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;
////////////////////////////////////////////////////////////////////////////////////////////////////

		//2024.08.18 fatric
		case PROTOCOL::TCP_COMMAND::CMD_STEP_USE_MODE:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_STEPUSE_MODE( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;
////////////////////////////////////////////////////////////////////////////////////////////////////

		//2023.04.26 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_BLOCK_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CUMFAILBLOCK_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.26 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_CHIP_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CUMFAILCHIP_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.26 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CUMFAIL_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.04.26 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_CUM_SORTBIN_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CUMSORTBIN_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

		//2023.06.24 - kwmoon
		case PROTOCOL::TCP_COMMAND::CMD_CUM_FAIL_CATEGORY:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_CUMFAILCATE_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );

				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;

/////////////////////////////////////////////////////////////////
		//2024.07.22 - fatric
		case PROTOCOL::TCP_COMMAND::CMD_BINMAP_DATA:
		{
			if( element.tcpHeader.datalen > 0 )
			{
				if( !m_pTestDataMgr->SET_BINMAP_DATA( slot, runner, element.pData, element.tcpHeader.datalen ) )
				{
					emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4[%5]")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(m_pTestDataMgr->GET_ERRMSG()) );
					//ipcheader.error = 1;
					//Send error msg to ui
				}
				else
				{
					emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 Done")
									 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)) );
				}
			}
			else
			{
				emit SigSaveLog( LOG_TYPE::_ERROR_LOG, tr("[%1][S#%2-R#%3] %4 Data length error")
								 .arg("TESTMGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
			}
		}
		break;
/////////////////////////////////////////////////////////////////

		case PROTOCOL::TCP_COMMAND::CMD_TEST_DONE:
		{
			uint size = sizeof(stIpcHeader) + element.tcpHeader.datalen;
            pSend    = new char[ size ];
            pTestBuf = new char[ size ];
			memcpy( pSend                    , (char*)&element.tcpHeader, sizeof(stIpcHeader) );
            memcpy( pTestBuf                 , (char*)&element.tcpHeader, sizeof(stIpcHeader) );
            if( element.tcpHeader.datalen > 0 )
			{
                memcpy( pSend+sizeof(stIpcHeader)   , element.pData, element.tcpHeader.datalen );
                memcpy( pTestBuf+sizeof(stIpcHeader), element.pData, element.tcpHeader.datalen );
            }
            emit SigIpcSendPacket( pSend   , size );        //To MainUi
            emit SigSendTestCmd  ( pTestBuf, size );        //To ChmbMgr - 20230531
            //chamber ambient or 25degree set.
			bSentUi = true;
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_LOG_PRINT:
		{
			uint size = sizeof(stIpcHeader) + element.tcpHeader.datalen;
			pSend = new char[ size ];
			memcpy( pSend                    , (char*)&element.tcpHeader, sizeof(stIpcHeader) );
			if( element.tcpHeader.datalen > 0 )
			{
				memcpy( pSend+sizeof(stIpcHeader), element.pData, element.tcpHeader.datalen );
			}
            emit SigIpcSendPacket( pSend, size );
			bSentUi = true;
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_PGM_RUNTIME_ERROR:
		{
			uint size = sizeof(stIpcHeader) + element.tcpHeader.datalen;
            pSend    = new char[ size ];
            pTestBuf = new char[ size ];
			memcpy( pSend                    , (char*)&element.tcpHeader, sizeof(stIpcHeader) );
            memcpy( pTestBuf                 , (char*)&element.tcpHeader, sizeof(stIpcHeader) );
			if( element.tcpHeader.datalen > 0 )
			{
                memcpy( pSend+sizeof(stIpcHeader)   , element.pData, element.tcpHeader.datalen );
                memcpy( pTestBuf+sizeof(stIpcHeader), element.pData, element.tcpHeader.datalen );
			}
            emit SigIpcSendPacket( pSend   , size );
            emit SigSendTestCmd  ( pTestBuf, size );        //2023.06.02 - KWMOON
			bSentUi = true;
		}
		break;

		//2023.05.18 - kwmoon(chamber ctrl command from devpgm)
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_INIT_TEMP_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_ENTER_SYNC_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_SETSV_VALID_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RUN_SYNC_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_TEMP_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_ALARM_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_STAT1_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_STAT2_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_DOOR_CLOSE_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_DOOR_OPEN_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RACKIN_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RACKOUT_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_SET_SV_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_SET_AMB_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RESET_ALARM_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RUN_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_STOP_REQ:
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_MASK_ONOFF_REQ:
		{
			uint size = sizeof(stIpcHeader) + element.tcpHeader.datalen;
			pSend = new char[ size ];
			memcpy( pSend , (char*)&element.tcpHeader, sizeof(stIpcHeader) );
			if( element.tcpHeader.datalen > 0 )
			{
				memcpy( pSend+sizeof(stIpcHeader), element.pData, element.tcpHeader.datalen );
			}
            emit SigSendChamberCmd( pSend, size );
			//bSentUi = true;		//Log Save
		}
		break;

		default:
			break;
	}
	if( bSentUi )
	{
		emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1][S#%2-R#%3] %4 [DataLen:%5]")
						 .arg("->UI", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(element.tcpHeader.datalen) );
	}

	if( element.tcpHeader.datalen > 0 )
	{
		if( element.pData )
		{
			delete[] element.pData;
		}
	}
}

void CTestMgr::SlotDevLogRecvPacket( stTcpElement emt)
{

}

//slot cpu loader
bool CTestMgr::RunTcpLoaderManager()
{
	m_pLoaderMgrTh = new QThread;
	m_pLoaderMgr->moveToThread( m_pLoaderMgrTh );
	m_pLoaderMgr->SetSlotInfo( m_stSlotInfo );
	connect( m_pLoaderMgrTh , &QThread::finished                 , m_pLoaderMgr, &QObject::deleteLater               );
	connect( m_pLoaderMgr   , &CLoaderMgr::SigLogMessage         , this        , &CTestMgr::SlotLoaderLogMessage     );
	connect( m_pLoaderMgr   , &CLoaderMgr::SigLoaderAckConnSlots , this        , &CTestMgr::SlotAckLoaderConnSlots   );
	connect( m_pLoaderMgr   , &CLoaderMgr::SigLoaderRecvPacket   , this        , &CTestMgr::SlotLoaderRecvPacket     );
	connect( this           , &CTestMgr::SigMakeSockLoaderMgr    , m_pLoaderMgr, &CLoaderMgr::SlotMakeSockets        );
	connect( this           , &CTestMgr::SigRunLoaderMgr         , m_pLoaderMgr, &CLoaderMgr::SlotRun                );
	connect( this           , &CTestMgr::SigStopLoaderMgr        , m_pLoaderMgr, &CLoaderMgr::SlotStop               );
	connect( this           , &CTestMgr::SigSendPacketToLoader   , m_pLoaderMgr, &CLoaderMgr::SlotSendPacketToLoader );

	m_pLoaderMgrTh->start();
	emit SigMakeSockLoaderMgr();
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("Loader Manager Start!") );
	emit SigRunLoaderMgr();
	return m_pLoaderMgrTh->isRunning();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CTestMgr::SlotIpcConnected
/// \param ptr
/// \details MAIN UI connected
void CTestMgr::SlotIpcConnected(quintptr handle)
{
	CLocalClient* pIpcClient = new CLocalClient( handle );
	//2024.07.25 - KWMOON
	if( m_vecIpcs.size() > 0 )
	{
		pIpcClient->deleteLater();
		return;
	}
	connect( pIpcClient, &CLocalClient::SigIpcConnectionStatus , this      , &CTestMgr::SlotIpcStatusChanged );
	connect( pIpcClient, &CLocalClient::SigIpcRecvPacket       , this      , &CTestMgr::SlotIpcRecvPacket    );
	connect( pIpcClient, &CLocalClient::SigIpcLogMessage       , this      , &CTestMgr::SlotIpcLogMessage    );
	connect( this      , &CTestMgr::SigIpcSendPacket           , pIpcClient, &CLocalClient::SlotSendPacket   );
	m_vecIpcs.push_back( pIpcClient );
	emit SigSaveLog( _NORMAL_LOG, tr("%1 Connected!").arg(ATMAIN_NAME) );
	m_bUiCon = true;

	//2024.07.25 - KWMOON
	if( m_pTestDataMgr != nullptr )
	{
		m_pTestDataMgr->LOAD_FILEMAP_INFO( m_sDataDir.toStdString().c_str() );
	}
}

void CTestMgr::SlotIpcStatusChanged( bool bConn )
{
	if( !bConn )
	{
		CLocalClient* pClient = qobject_cast<CLocalClient*>(QObject::sender());
		if( pClient != nullptr )
		{
			m_vecIpcs.removeOne( pClient );
			pClient->deleteLater();
			emit SigSaveLog( _NORMAL_LOG, tr("%1 Disconnected!").arg(ATMAIN_NAME) );
		}
		m_bUiCon = false;
	}
	else
	{
		m_pTestDataMgr->LOAD_FILEMAP_INFO();		//2024.06.19 - KWMOON
		emit SigSaveLog( _NORMAL_LOG, tr("%1 Connected!").arg(ATMAIN_NAME) );
	}
}

//MGR <- UI
void CTestMgr::SlotIpcRecvPacket( stIpcElement element )
{
	int nDataSize = element.ipcHeader.datalen;
	int nDest     = element.ipcHeader.dest;
	uint uSlots   = element.ipcHeader.slot;
	switch( element.ipcHeader.cmd )
	{
		case PROTOCOL::CMD_SLOT_INFO_REQ:		//UI -> MGR
			AckSlotInfo( element );
			break;

		case PROTOCOL::CMD_LIVE_SLOT_REQ:
			AckSlotLive(element);
			break;

		case PROTOCOL::CMD_LIVE_PGM_REQ:
			AckDevPgmLive(element);
			break;

		case PROTOCOL::CMD_READ_BIBID_REQ:		//2024.06.17 - kwmoon
			for( uint s =0;s<MAX_SLOT;s++ )
			{
				if( uSlots & (1<<s) )
				{
					m_pTestDataMgr->REMOVE_BIBID_FILE( s );
				}
			}
			ReqToLoader( element );
			break;
		case PROTOCOL::CMD_SLOT_CPU_INFO_SEND:
		case PROTOCOL::CMD_LOADER_INFO_REQ:
		//case PROTOCOL::CMD_SETUP_BIBID_REQ:	//2024.06.17 - kwmoon(not use)
		case PROTOCOL::CMD_PGM_LOAD_REQ:
		case PROTOCOL::CMD_PGM_RELOAD_REQ:
		case PROTOCOL::CMD_PGM_UNLOAD_REQ:
			ReqToLoader( element );
			break;

		case PROTOCOL::CMD_VERSION_REQ:
			if( nDest == PROTOCOL::_ATMGR )
				AckVersion( element );
			else if( nDest == PROTOCOL::_ATSLOTS )
				ReqToLoader( element );
			else
			{

			}
			break;

		//case PROTOCOL::CMD_TEST_DATA_INIT_REQ:		//2024.06.17 - kwmoon(not use)
		//	DoTestDataInit( element );
		//	break;

		case PROTOCOL::CMD_ENGR_SETUP_REQ:			//2024.06.17 - KWMOON
			DoEngrSetup( element );
			break;

		case PROTOCOL::CMD_AUTO_SETUP_REQ:			//2024.06.17 - KWMOON
			DoAutoSetup( element );
			break;

		case PROTOCOL::CMD_UI_CHMBUSE_REQ:			//2023.05.25 - KWMOON
			DoUiChamberUse( element );
			break;

		case PROTOCOL::CMD_INIT_TEST_START_REQ:
			DoInitTestDataInit( uSlots );
			SendToDevCtrlMgr( element );
			break;

		case PROTOCOL::CMD_TEST_START_REQ:
		case PROTOCOL::CMD_ITEM_TEST_START_REQ:
			m_uTestSlots = uSlots;
			m_pChmbMgr->InitChmbSync( uSlots );	//2023.05.24 - KWMOON
			DoStepTestDataInit( uSlots );
			SendToDevCtrlMgr( element );
			break;
		case PROTOCOL::CMD_TEST_STOP_REQ:
			SendToDevCtrlMgr( element );
			break;

		default:
			qInfo() << "CMD_UNKNOWN";
			break;
	}
	if( nDataSize > 0 )
	{
		if( element.pData )
		{
			delete[] element.pData;
			element.pData = nullptr;
		}
	}
}

void CTestMgr::ReqToLoader(stIpcElement& IpcEmt)
{
	stTcpElement element;
	memcpy( &element.tcpHeader, &IpcEmt.ipcHeader, sizeof(stTcpHeader) );

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(IpcEmt.ipcHeader.cmd);
	emit SigSaveLog( _NORMAL_LOG, tr("[%1] %2").arg("<-UI", 10, QChar(' ')).arg(UTIL::TCPIP::CMD_STR(cmd)) );

	uint nDataSize = element.tcpHeader.datalen;
	element.pData  = nullptr;
	if( element.tcpHeader.datalen > 0 )
	{
		element.pData = new char[nDataSize];
		memcpy( element.pData, IpcEmt.pData, nDataSize );
	}
	emit SigSendPacketToLoader( element );
	//don't remove IPcEmt pData...SlotIpcRecvPacket delete it.
}

void CTestMgr::SendToDevCtrlMgr(stIpcElement& IpcEmt)
{
	stTcpElement element;
	memcpy( &element.tcpHeader, &IpcEmt.ipcHeader, sizeof(stTcpHeader) );

	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(IpcEmt.ipcHeader.cmd);
	emit SigSaveLog( _NORMAL_LOG, tr("[%1] %2[0x%3]").arg("<-MAINUI", 10, QChar(' ')).arg(UTIL::TCPIP::CMD_STR(cmd)).arg(IpcEmt.ipcHeader.slot,8,16,QChar('0')) );

	uint nDataSize = element.tcpHeader.datalen;
	element.pData  = nullptr;
	if( element.tcpHeader.datalen > 0 )
	{
		element.pData = new char[nDataSize];
		memcpy( element.pData, IpcEmt.pData, nDataSize );
	}
	emit SigSendPacketToDevCtrlMgr( element );
}

//MGR <- UI
void CTestMgr::SlotIpcLogMessage( LOG_TYPE t, QString s)
{
	qInfo() << __FUNCTION__ <<  " : " << s;
}

//atslot.ini info ack
bool CTestMgr::AckSlotInfo( stIpcElement& unused )
{
	(void)unused;
	if( !m_bUiCon ) return false;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_SLOT_INFO_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = sizeof(stSlotInfo);
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &m_stSlotInfo , header.datalen      );
	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2 ").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_SLOT_INFO_ACK) ) );
	return true;
}

//cpu(loader) connection info ack
bool CTestMgr::AckSlotLive( stIpcElement& unused )
{
	(void)unused;
	if( !m_bUiCon ) return false;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_LIVE_SLOT_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = sizeof(stSlotLive);
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &m_stSlotLive , header.datalen      );
	emit SigIpcSendPacket( pBuf, size );
	//emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
	//				 tr("[%1][SEND] %2 ").arg("->UI", 7, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_LIVE_SLOT_ACK) ) );
	return true;
}

bool CTestMgr::AckDevPgmLive( stIpcElement& unused )
{
	(void)unused;
	if( !m_bUiCon ) return false;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_LIVE_PGM_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = sizeof(stPgmLive);
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &m_stPgmLive  , header.datalen      );
	emit SigIpcSendPacket( pBuf, size );
	if( 0 )
	{
		for( int s=0;s<MAX_SLOT;s++ )
		{
			if( m_stSlotInfo.SlotIp[s].bMask ) continue;
			for(int p=0;p<2;p++ )
			{
				qDebug() << tr("[PGM LIVE] SLOT%1-%2 CTRL : %3").arg(s).arg(p).arg(m_stPgmLive.pgm_status[s][p][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET]);
				qDebug() << tr("[PGM LIVE] SLOT%1-%2 DATA : %3").arg(s).arg(p).arg(m_stPgmLive.pgm_status[s][p][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET]);
				qDebug() << tr("[PGM LIVE] SLOT%1-%2 LOG  : %3").arg(s).arg(p).arg(m_stPgmLive.pgm_status[s][p][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET] );
			}
		}
	}
	return true;
}

//mgr version info ack
bool CTestMgr::AckVersion( stIpcElement& unused )
{
	(void)unused;
	if( !m_bUiCon ) return false;

	char szBuf[64] = {0x0,};
	memset( szBuf, 0x0, sizeof(szBuf) );
	snprintf( szBuf, sizeof(szBuf), "%s[%s]", ATMGR_VER, ATMGR_DATE );
	szBuf[strlen(szBuf)+1]='\0';
	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_VERSION_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = strlen(szBuf)+1;
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), szBuf         , header.datalen      );
	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2 %3").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_VERSION_ACK)).arg(szBuf) );
	return true;
}

bool CTestMgr::MakeDir(QString path)
{
	QDir dir(path);
	if( !dir.exists() )
	{
		return dir.mkdir( path );
	}
	return true;
}

bool CTestMgr::MakeTxtFile(QString f, QString s)
{
	UTIL::FILE::WriteFile( f, s );
	return true;
}

bool CTestMgr::MakeBinFile(QString f, uint len, char* p )
{
	QFile file(f);
	if( !file.open( QFile::WriteOnly ) )
	{
		return false;
	}
	file.write( p, len );
	file.close();
	return true;
}

/*
//2023.06.17 - kwmoon(not use)
bool CTestMgr::DoTestDataInit(stIpcElement& ipcemt)
{
	bool bError   = false;
	//Receive stSetupInfo
	if( ipcemt.ipcHeader.datalen != sizeof(stSetupInfo) )
	{
		bError = true;
		AckTestDataInit( bError );
		return true;
	}
	memcpy( &m_stSetupInfo, ipcemt.pData, sizeof(stSetupInfo) );
    emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
                     tr("[%1] DATA DIR:%2").arg(__FUNCTION__).arg(m_sDataDir) );
	if( m_stSetupInfo.auto_mode )
		m_sDataModeDir = tr("%1/%2").arg(m_sDataDir).arg(AUTO_DIR);
	else
		m_sDataModeDir = tr("%1/%2").arg(m_sDataDir).arg(ENGR_DIR);

	m_pTestDataMgr->SET_DEBUGMODE( true );
	bError = m_pTestDataMgr->INIT_SETUP( m_sDataModeDir.toStdString().c_str(), m_stSetupInfo.lot_number, m_stSetupInfo.part_number, m_stSetupInfo.operator_id, m_stSetupInfo.auto_mode );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_TEST_DATA_INIT_ACK)) );
	AckTestDataInit( !bError );
	DeletePfmFiles();				// 2023-09-21 hsbae delete all of pfm log file
    createInitialPfmFiles();
	return true;
}
*/

//2023.06.17 - kwmoon
bool CTestMgr::DoEngrSetup(stIpcElement& ipcemt)
{
	bool bError   = false;
	//Receive stSetupInfo
	if( ipcemt.ipcHeader.datalen != sizeof(stSetupInfo) )
	{
		bError = true;
		AckEngrSetup( bError );
		return true;
	}

	memcpy( &m_stSetupInfo, ipcemt.pData, sizeof(stSetupInfo) );
	//m_sDataModeDir = tr("%1/%2").arg(m_sDataDir).arg(ENGR_DIR);

	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG, tr("[%1] DATA DIR:%2").arg(__FUNCTION__).arg(m_sDataModeDir) );

	m_pTestDataMgr->SET_DEBUGMODE( true );
	m_pTestDataMgr->LOAD_FILEMAP_INFO( m_sDataDir.toStdString().c_str() );
	bError = m_pTestDataMgr->SET_ENGR_SETUP( m_stSetupInfo.lot_number, m_stSetupInfo.part_number, m_stSetupInfo.operator_id );

	DeletePfmFiles();			//2024.08.20 - KWMOON
	createInitialPfmFiles();	//2024.08.20 - KWMOON

	AckEngrSetup( !bError );
	return true;
}

//2023.06.17 - kwmoon
bool CTestMgr::DoAutoSetup(stIpcElement& ipcemt)
{
	bool bError   = false;
	int slot_with_bib = 0x0;
	//Receive stSetupInfo
	if( ipcemt.ipcHeader.datalen != sizeof(slot_with_bib) )
	{
		bError = true;
		AckAutoSetup( bError );
		return true;
	}

	memcpy( &slot_with_bib, ipcemt.pData, sizeof(slot_with_bib) );

	for( int i=0;i<MAX_SLOT;i++ )
	{
		m_pTestDataMgr->REMOVE_INITTEST_DATA(i);
		m_pTestDataMgr->REMOVE_STEPTEST_DATA(i);

		if ( slot_with_bib & (0x1 << i) )
		{
			QString sRcmdMainInfoFile = tr("%1/%2/%3/SLOT%4%5").arg(m_sDataDir).arg(DIR_AUTO_DATA_PATH).arg(BOARD_MAINT_DIR).arg(i).arg(EXT_RCMD);
			char * pReadMainInfo = new char[sizeof(stRcmdBoardMaintInfo)];
			if ( !m_pTestDataMgr->READ_FILE(sRcmdMainInfoFile.toStdString().c_str(), pReadMainInfo, sizeof(stRcmdBoardMaintInfo)) )
			{
				bError = true;
				break;
			}
			stRcmdBoardMaintInfo rcmdBoardMaintInfo;
            memcpy( (char*)&rcmdBoardMaintInfo, pReadMainInfo, sizeof(stRcmdBoardMaintInfo));
			delete[] pReadMainInfo;

			// TODO
			// RCMD FILE VALID CHECK
		}
	}
	DeletePfmFiles();			//2024.08.20 - KWMOON
	createInitialPfmFiles();	//2024.08.20 - KWMOON
	AckAutoSetup(bError);

	return true;
}

bool CTestMgr::AckUiChamberUse (bool bError, bool bConn)
{
	if( !m_bUiCon ) return false;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_UI_CHMBUSE_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = sizeof(char);
	header.error   = (bError?0x1:0x0);
	header.slot    = 0;
	header.runner  = 0;
	uint size = sizeof(stIpcHeader) + header.datalen;
    char* pBuf = new char[ size ];
	char cCon = bConn?0x1:0x0;
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), &cCon         , sizeof(char)        );
	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2 : %3").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_UI_CHMBUSE_ACK) ).arg(cCon, 1, 10, QChar('0')) );
	return true;
}

bool CTestMgr::DoUiChamberUse(stIpcElement& ipcemt)
{
	//bool bError   = false;
	if( ipcemt.ipcHeader.datalen < 1 )
	{
		//bError = true;
		AckUiChamberUse( true, false );
		return true;
	}
	bool bUse = false;
	memcpy( &bUse, ipcemt.pData, sizeof(bool) );

	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] UI Chamber use :%2").arg(__FUNCTION__).arg(bUse?"YES":" NO") );
	emit SigChamberUse( bUse );		//->chmbmgr
	return true;
}

bool CTestMgr::DoInitTestDataInit(uint uSlots)
{
	for( int s=0;s<MAX_SLOT;s++ )
	{
		if( (1<<s) & uSlots )
		{
			m_pTestDataMgr->REMOVE_INITTEST_DATA( s );
		}
	}
	return true;
}

bool CTestMgr::DoStepTestDataInit(uint uSlots)
{
	for( int s=0;s<MAX_SLOT;s++ )
	{
		if( (1<<s) & uSlots )
		{
			m_pTestDataMgr->REMOVE_STEPTEST_DATA( s );
		}
	}
	return true;
}

bool CTestMgr::AckEngrSetup(bool bError)
{
	if( !m_bUiCon ) return false;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_ENGR_SETUP_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.slot = 0;
	header.runner = 0;
	header.datalen = 0;
	header.error   = (bError)?0x1:0x0;
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_ENGR_SETUP_ACK)) );
	return true;
}

bool CTestMgr::AckAutoSetup(bool bError)
{
	if( !m_bUiCon ) return false;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_AUTO_SETUP_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.slot = 0;
	header.runner = 0;
	header.datalen = 0;
	header.error   = (bError)?0x1:0x0;
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );

	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_ENGR_SETUP_ACK)) );
	return true;
}

/*
bool CTestMgr::AckTestDataInit ( bool bError )
{
	if( !m_bUiCon ) return false;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_TEST_DATA_INIT_ACK;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = 0;
	header.error   = (bError)?0x1:0x0;
	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header, sizeof(stIpcHeader) );
	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_TEST_DATA_INIT_ACK)) );
	return true;
}
*/

//Receive from load manager - cpu loader status
void CTestMgr::SlotAckLoaderConnSlots( stSlotLive slotlive )
{
	memcpy( &m_stSlotLive, &slotlive, sizeof(stSlotLive) );
	emit SigLiveSlotInfo(m_stSlotLive);
	if( 0 )
	{
		printf("MAX SLOT : %d\n", m_stSlotInfo.SlotCpu.nMaxSlot );
		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
		{
			if( m_stSlotLive.slot_status[s][CPU00] ==  EN_SLOT_MASK )
			{
				printf("SLOT%02d  mask\n", s+1 );
			}
			else
			{
				if( m_stSlotLive.slot_status[s][CPU00] == EN_SLOT_ONLINE )
					printf("SLOT%02d CPU00 offline\n", s+1 );
				else
					printf("SLOT%02d CPU00 online\n", s+1 );

				if( m_stSlotInfo.SlotCpu.nCpuPerSlot == 2 )
				{
					if( m_stSlotLive.slot_status[s][CPU01] != EN_SLOT_ONLINE )
					{
						printf("SLOT%02d CPU01 offline\n", s+1 );
					}
				}
			}
		}
	}
}

//Receive from cpu loader
void CTestMgr::SlotLoaderRecvPacket( int slot, int runner, stTcpElement tcpEmt )
{
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(tcpEmt.tcpHeader.cmd);
	int nDataSize = tcpEmt.tcpHeader.datalen;
	//int nDest     = tcpEmt.tcpHeader.dest;
	switch( tcpEmt.tcpHeader.cmd )
	{
		case PROTOCOL::CMD_SLOT_CPU_INFO_ACK:
		case PROTOCOL::CMD_EWS_FTP_INFO_ACK:
			emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
							 tr("[%1][S#%2] %3").arg("<-CPU", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg( UTIL::TCPIP::CMD_STR(cmd)) );
			break;

		case PROTOCOL::CMD_LOADER_INFO_ACK:
		//case PROTOCOL::CMD_SETUP_BIBID_ACK:	//2023.06.17 - kwmoon(not use)
		case PROTOCOL::CMD_PGM_LOAD_STATUS:
		case PROTOCOL::CMD_PGM_LOAD_ACK:
		case PROTOCOL::CMD_PGM_RELOAD_ACK:
		case PROTOCOL::CMD_PGM_UNLOAD_ACK:
		case PROTOCOL::CMD_PGM_FINISHED:
		case PROTOCOL::CMD_VERSION_ACK:
		case PROTOCOL::CMD_READ_BIBID_ACK:		//2024.06.17 - KWMOON
		{
			emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
							 tr("[%1][S#%2] %3").arg("<-CPU", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg( UTIL::TCPIP::CMD_STR(cmd)) );

			//2024.06.17 - KWMOON
			if( tcpEmt.tcpHeader.cmd == PROTOCOL::CMD_READ_BIBID_ACK )
			{
				stBibId bibid;
				memset( &bibid, 0x0, sizeof(stBibId) );
				if( nDataSize == sizeof(stBibId) )
				{
					memcpy( &bibid, tcpEmt.pData, nDataSize );
					//BIB ID / CONN / PARITY  WRITE
					//tcpEmt.tcpHeader.error = bOk?0:1;
				}
				m_pTestDataMgr->WRITE_BIBID( slot, bibid );	//2024.06.17 - KWMOON
			}
			uint nSize = sizeof(stIpcHeader) + tcpEmt.tcpHeader.datalen;
			char* pBuf = new char[nSize];
			memcpy( pBuf, (char*)&tcpEmt.tcpHeader, sizeof(stIpcHeader) );
			if( tcpEmt.tcpHeader.datalen > 0 )
				memcpy( pBuf+sizeof(stIpcHeader), tcpEmt.pData, tcpEmt.tcpHeader.datalen );

			emit SigIpcSendPacket( pBuf, nSize );
			emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
							 tr("[%1][S#%2] %3").arg("->UI", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg( UTIL::TCPIP::CMD_STR(cmd)) );
		}
		break;

		default:
			emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
							 tr("[%1][S#%2-R#%3] %4").arg("<-CPU", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg( UTIL::TCPIP::CMD_STR(cmd)) );

			break;
	}
	if( nDataSize > 0 )
	{
		delete[] tcpEmt.pData;
		tcpEmt.pData = nullptr;
	}
}

void CTestMgr::SlotLoaderLogMessage( LOG_TYPE t, QString s )
{
	emit SigSaveLog( t, s );
}


void CTestMgr::SlotChmbMgrLogMessage( LOG_TYPE t, QString s )
{
	emit SigSaveLog( t, s );
}

void CTestMgr::SlotRecvChmbMgrAck( stTcpElement element)
{
	stTcpHeader header;
	memcpy( &header, &element.tcpHeader, sizeof(stTcpHeader) );

	PROTOCOL::TCP_COMMAND cmd  = static_cast<PROTOCOL::TCP_COMMAND>(header.cmd);
	uint slot = header.slot;
	uint runner = header.runner;

	if( header.src == PROTOCOL::_ATMAIN_UI )
	{
		emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
						 tr("[%1][S#%2-R#%3] %4").arg("->UI", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg( UTIL::TCPIP::CMD_STR(cmd)) );
	}
	else if( header.src == PROTOCOL::_ATMGR )
	{
		emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
						 tr("[%1][S#%2-R#%3] %4").arg("->MGR", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg( UTIL::TCPIP::CMD_STR(cmd)) );
		if( element.tcpHeader.datalen > 1 )
		{
			delete[] element.pData;
		}
	}
	else if( header.src == PROTOCOL::_ATTMS_UI )
	{

	}
	else if( (header.src > PROTOCOL::_ATSLOTS) && (header.src < PROTOCOL::_ATCHMB_PLC) )
	{
		emit SigSendPacketToDevDataMgr( element );
		//emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
		//				 tr("[%1][S#%2-R#%3] %4").arg("->PGM", 10, QChar(' ')).arg(slot+1,2,10,QChar('0')).arg(runner+1).arg( UTIL::TCPIP::CMD_STR(cmd)) );
	}
}

//2023.08.17 - KWMOON
void CTestMgr::SlotRecvChmbConnStatus( bool bConn )
{
	AckUiChamberUse( false, bConn );
}

//2024.08.14 - KWMOON
void CTestMgr::SlotChmbTempMonSet( stGetTemp temp_set )
{
	if( !m_bUiCon ) return;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_CHMB_TEMP_MON_SET;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = sizeof(stGetTemp);
	header.error   = 0;
	header.slot    = 0;
	header.runner  = 0;

	uint step_no = 0;
	for( uint s=0;s<MAX_SLOT;s++ )
	{
		if( (1<<s) & m_uTestSlots )
		{
			uint nMaxStep0 = m_pTestDataMgr->GET_MAX_STEP( s, CPU00 );
			uint nMaxStep1 = m_pTestDataMgr->GET_MAX_STEP( s, CPU01 );
			if( nMaxStep0 == nMaxStep1 )
			{
				step_no = nMaxStep0;
				break;
			}
		}
	}
	step_no += 2;	//2024.08.20 - KWMOON(0:ALL, 1:INIT, current step is not stepout)
	temp_set.nAck = step_no;

	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header  , sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), (char*)&temp_set, sizeof(stGetTemp)   );
	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2 : SV[%3] PV[%4]").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_TEMP_MON_SET) ).arg(temp_set.dSv).arg(temp_set.dPv) );
}

//2024.08.14 - KWMOON
void CTestMgr::SlotChmbTempMonEnd( stGetTemp temp_end )
{
	if( !m_bUiCon ) return;

	stIpcHeader header;
	header.cmd  = PROTOCOL::CMD_CHMB_TEMP_MON_END;
	header.src  = PROTOCOL::_ATMGR;
	header.dest = PROTOCOL::_ATMAIN_UI;
	header.datalen = sizeof(stGetTemp);
	header.error   = 0;
	header.slot    = 0;
	header.runner  = 0;

	uint step_no = 0;
	for( uint s=0;s<MAX_SLOT;s++ )
	{
		if( (1<<s) & m_uTestSlots )
		{
			uint nMaxStep0 = m_pTestDataMgr->GET_MAX_STEP( s, CPU00 );
			uint nMaxStep1 = m_pTestDataMgr->GET_MAX_STEP( s, CPU01 );
			if( nMaxStep0 == nMaxStep1 )
			{
				step_no = nMaxStep0;
				break;
			}
		}
	}
	step_no += 2;	//2024.08.20 - KWMOON(0:ALL, 1:INIT, current step is not stepout)
	temp_end.nAck = step_no;

	uint size = sizeof(stIpcHeader) + header.datalen;
	char* pBuf = new char[ size ];
	memcpy( pBuf                    , (char*)&header  , sizeof(stIpcHeader) );
	memcpy( pBuf+sizeof(stIpcHeader), (char*)&temp_end, sizeof(stGetTemp)   );
	emit SigIpcSendPacket( pBuf, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG,
					 tr("[%1] %2 : SV[%3] PV[%4]").arg("->UI", 10, QChar(' ')).arg( UTIL::TCPIP::CMD_STR(PROTOCOL::CMD_CHMB_TEMP_MON_END) ).arg(temp_end.dSv).arg(temp_end.dPv) );
}

//2023.09.21 - hsbae
void CTestMgr::DeletePfmFiles() {
    char* env_data  = getenv(ATDATA_DIR_ENV);
    QString data_dir;
    if( env_data != NULL )
    {
        data_dir = tr("%1/pfm").arg(env_data);
    }
    else
    {
        data_dir = tr("%1/pfm").arg(DIR_ATDATA_PATH);
    };
    QDir dir(data_dir);

    if (!dir.exists(data_dir)) {
        dir.mkdir(data_dir);
        return;
    }
    QFileInfoList filelist = dir.entryInfoList( QDir::Files | QDir::NoDotAndDotDot );
    //qDebug() << __FUNCTION__ << "dir : " << m_sDownloadPath;
    foreach( QFileInfo file, filelist )
    {
        //qDebug() << "remove file : " << file.absoluteFilePath();
        QFile::remove( file.absoluteFilePath() );
    }
    return;
}

void CTestMgr::createInitialPfmFiles() {
    char* env_data  = getenv(ATDATA_DIR_ENV);
    QString data_dir;
    if( env_data != NULL )
    {
        data_dir = tr("%1/pfm").arg(env_data);
    }
    else
    {
        data_dir = tr("%1/pfm").arg(DIR_ATDATA_PATH);
    };
    QDir dir(data_dir);

    if (!dir.exists(data_dir)) {
        dir.mkdir(data_dir);
    }

    for (int slot_number = 0; slot_number < 16; slot_number += 1) {
        QString header = "    time    ,  ch  ,  sv  ,  rv  ,  rc  ,   sw   ,  so  , FAULT ,  ch  ,  sv  ,  rv  ,  rc  ,   sw   ,  so  , FAULT \n";
        UTIL::FILE::WriteFile(tr("%1/SLOT%2-1.dat").arg(data_dir).arg(slot_number), header, true);
        UTIL::FILE::WriteFile(tr("%1/SLOT%2-2.dat").arg(data_dir).arg(slot_number), header, true);

        header = "";
        header += "[CPU1]\n";
        for (int i = 0; i < 8; i += 1)
            header += tr("PPS%1=0\n").arg(i + 1);
        for (int i = 0; i < 8; i += 1)
            header += tr("S1PS%1=0\n").arg(i + 1);
        header += "\n";

        header += "[CPU2]\n";
        for (int i = 0; i < 8; i += 1)
            header += tr("PPS%1=0\n").arg(i + 1);
        for (int i = 0; i < 8; i += 1)
            header += tr("S1PS%1=0\n").arg(i + 1);
        header += "\n";
        UTIL::FILE::WriteFile(tr("%1/SLOT%2_pfm_fault.ini").arg(data_dir).arg(slot_number), header, true);
    }
}

