#include "chmbmanager.hpp"

CChmbManager::CChmbManager(QObject *parent) : QObject(parent)
{
	Init();
}

CChmbManager::~CChmbManager()
{
	DeleteInstance();
}

void CChmbManager::DeleteInstance()
{
	if( m_timerPlcCon->isActive() ) m_timerPlcCon->stop();
	m_timerPlcCon->deleteLater();

	QThread::msleep( 100 );
	if( m_pIpcSvr )
	{
		m_pIpcSvr->close();
		m_pIpcSvr->deleteLater();
	}

	if( m_pYkgPlcTh )
	{
		m_pYkgPlcTh->quit();
		m_pYkgPlcTh->wait();
	}

	for( CChmbClient* client : m_mapIpc )
	{
		client->deleteLater();
	}
	m_mapIpc.clear();

	if( m_pLogger )
	{
		m_pLogger->deleteLater();
	}
}

void CChmbManager::Init()
{
	RegisterMetaType();
	StandByDirectory();
	LoadChmbIni();
	InitTimer();

    m_nMyKey = MY_UNIQUE_ID;
	m_mapIpc.clear();
	m_pIpcSvr   = new CLocalServer( this );
	m_pLogger   = new CLogger( PROTOCOL::SOURCE_TYPE::_ATCHMB_COMM );
	m_pYkgPlc   = new CYokogawaPlc();
	m_pYkgPlcTh = new QThread;
	m_pYkgPlc->moveToThread( m_pYkgPlcTh );

	m_pLogger->SetLogDir( m_sCurrentDir );

	m_pYkgPlc->SetPlcTcpInfo( m_sChamberIp, m_uChamberPort );
	m_pYkgPlc->SetChamberTempLimit( -40.0, 140.0 );

	connect( this     , &CChmbManager::SigSaveLog             , m_pLogger, &CLogger::SlotLogMessage             );
	connect( m_pIpcSvr, &CLocalServer::SigIncommingConnection , this     , &CChmbManager::SlotIpcConnected      );

	connect( m_pYkgPlcTh , &QThread::finished                  , m_pYkgPlc, &QObject::deleteLater               );
	connect( this        , &CChmbManager::SigPlcSocketControl  , m_pYkgPlc, &CYokogawaPlc::SlotPlcSocketControl );
	connect( m_pYkgPlc   , &CYokogawaPlc::SigPlcConnStatus     , this     , &CChmbManager::SlotPlcConnStatus    );
	connect( m_pYkgPlc   , &CYokogawaPlc::SigPlcLogMessage     , m_pLogger, &CLogger::SlotLogMessage            );
	connect( m_pYkgPlc   , &CYokogawaPlc::SigPlcChmbResponse   , this     , &CChmbManager::SlotPlcChmbResponse  );

	m_pYkgPlcTh->start();

	emit SigPlcSocketControl( PLC_ONLINE );
	m_timerPlcCon->start( 3000 );
}

void CChmbManager::InitTimer()
{
	m_timerPlcCon = new QTimer(this);
	connect(m_timerPlcCon         , &QTimer::timeout, this, &CChmbManager::SlotTimeoutPlcCon           );
}

void CChmbManager::LoadChmbIni()
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

	//CHAMBER IP
	QString ini = QObject::tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(CHAMBER_INI);
	section_key = QObject::tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_IP);
	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		UTIL::INI::writeSettings( ini, section_key, DEFAULT_CHMB_IP );
		m_sChamberIp = DEFAULT_CHMB_IP;
		bOk = false;
	}
	else
	{
		m_sChamberIp = sValue;
	}
	//CHAMBER IP
	ini = QObject::tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(CHAMBER_INI);
	section_key = QObject::tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_PORT);
	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		UTIL::INI::writeSettings( ini, section_key, DEFAULT_CHMB_PORT );
		m_uChamberPort = DEFAULT_CHMB_PORT;
		bOk = false;
	}
	else
	{
		m_uChamberPort = sValue.toUInt( &bOk, 10);
	}
}

void CChmbManager::RegisterMetaType()
{
	qRegisterMetaType<stTcpElement>("stTcpElement");
	qRegisterMetaType<stIpcElement>("stIpcElement");
	qRegisterMetaType<LOG_TYPE>("LOG_TYPE");
	qRegisterMetaType<PLC_ON_OFFLINE>("PLC_ON_OFFLINE");
	qRegisterMetaType<PLC_ADDRESS_MAP>("PLC_ADDRESS_MAP");
	qRegisterMetaType<PLC_COMM_RESULT_CODE>("PLC_COMM_RESULT_CODE");
	qRegisterMetaType<PLC_EXIT_ERROR_CODE>("PLC_EXIT_ERROR_CODE");
	qRegisterMetaType<stPlcResponse>("stPlcResponse");
}

void CChmbManager::StandByDirectory()
{
	char* env_run = getenv(ATRUN_DIR_ENV);
	QString run_path = "";
	if( env_run != NULL )
	{
		run_path = QString(env_run);
	}
	else
	{
		run_path = tr("%1/%2").arg(QDir::home().absolutePath()).arg("atsystem");
	}

	QDir::setCurrent( run_path );
	m_sCurrentDir = run_path;
}

int CChmbManager::run(int argc, char* argv[])
{
	m_bDebugPrint = false;
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
		if( m_bDebugPrint ) fprintf( stdout, "Debug Print On\n" );
	}
	m_pLogger->SetDebugPrint( m_bDebugPrint );
	RunLocalServer();
	return 0;
}


bool CChmbManager::RunLocalServer()
{
	const int RETRY_COUNT = 10;
	bool bListen = false;
	int retry = 0;

	if( m_pIpcSvr->isListening() ) return true;

	do
	{
		QLocalServer::removeServer( IPC_INFO::IPC_CHMBCOMM_SOCK );
		if( !m_pIpcSvr->listen( IPC_INFO::IPC_CHMBCOMM_SOCK ) )
		{
			m_pIpcSvr->close();
			bListen = false;
		}
		else
		{
			bListen = true;
			break;
		}
		QThread::usleep( 500 );
	}while( retry++ < RETRY_COUNT );

	if( !m_pIpcSvr->isListening() )
	{
		emit SigSaveLog( LOG_TYPE::_ERROR_LOG , tr("[INFO] IPC Server(%1) Listen NG!").arg(IPC_INFO::IPC_CHMBCOMM_SOCK) );
	}
	else
	{
		emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("[INFO] IPC Server(%1) Listen OK!").arg(IPC_INFO::IPC_CHMBCOMM_SOCK) );
	}
	return m_pIpcSvr->isListening();
}

void CChmbManager::SlotIpcLogSave( uint id, LOG_TYPE t, QString s )
{
	emit SigSaveLog( t, tr("[%1]%2").arg(id,4,10,QChar('0')).arg(s) );
}

uint CChmbManager::GenerateUniqueId()
{
    int nKey = m_nMyKey;
	do
	{
        nKey += 1;
	}while( m_mapIpc.contains(nKey) );

    //m_nKey = nKey;
    return nKey;
}

void CChmbManager::SlotIpcConnected(quintptr handle)
{
	uint nMapKey = GenerateUniqueId();
	CChmbClient* pChmbClient = new CChmbClient( nMapKey, handle );
	m_mapIpc[nMapKey] = pChmbClient;

	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("[INFO] IPC(%1) Connected!").arg(nMapKey,4,10,QChar('0')) );

	connect( pChmbClient, &CChmbClient::SigIpcLogMessage      , this       , &CChmbManager::SlotIpcLogSave          );
	connect( pChmbClient, &CChmbClient::SigIpcConnectionStatus, this       , &CChmbManager::SlotIpcConnectionStatus );
	connect( pChmbClient, &CChmbClient::SigIpcRecvPacket      , m_pYkgPlc  , &CYokogawaPlc::SlotPlcControlByEmt     );
	connect( this       , &CChmbManager::SigIpcSendPacket     , pChmbClient, &CChmbClient::SlotSendPacket           );
}

void CChmbManager::SlotIpcConnectionStatus(uint id, PLC_ON_OFFLINE onoff )
{
	if( onoff == PLC_OFFLINE )
	{
		if( m_mapIpc.contains( id ) )
		{
			CChmbClient* p = m_mapIpc[id];
			m_mapIpc.remove( id );
			p->deleteLater();
			emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("[INFO] IPC(%1) Disconnected!").arg(id,4,10,QChar('0')) );
		}
	}
}

void CChmbManager::SlotPlcConnStatus( PLC_ON_OFFLINE onoff )
{
    //PLC Status send to connected IPCs
	QMap<uint, CChmbClient*>::const_iterator it = m_mapIpc.constBegin();
	while (it != m_mapIpc.constEnd())
	{
		uint id = it.key();
		SendPlcStatusToIpcs( id, onoff );
		it++;
	}
    if( onoff == PLC_ONLINE ) //ONLINE
    {
        //Watchdog Timer Enable
    }
}

bool CChmbManager::SendPlcStatusToIpcs( uint id, PLC_ON_OFFLINE onoff )
{
	char* pData = nullptr;
	uint size   = sizeof(stIpcHeader);

	stIpcHeader head;
	head.cmd     = PROTOCOL::PLC_COMMAND::CMD_SYS_PLC_CONN;
	head.dest    = PROTOCOL::_ATCHMB_UI;
	head.src     = PROTOCOL::_ATCHMB_COMM;
	head.datalen = 0;
	head.slot    = 0x0;
	head.runner  = 0x0;
	head.error   = (onoff == PLC_OFFLINE)?0x1:0x0;
	pData = new char[size];
	memcpy( pData, (char*)&head, size );
	emit SigIpcSendPacket( id, pData, size );
	emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , tr("[SEND] IPC(%1) %2")
					 .arg(id,4,10,QChar('0')).arg(UTIL::CHAMBER::PLCCMD_STR(PROTOCOL::PLC_COMMAND::CMD_SYS_PLC_CONN)) );
	return true;
}

void CChmbManager::SlotTimeoutPlcCon()
{
	emit SigPlcSocketControl( PLC_ONLINE );
}

void CChmbManager::SlotPlcChmbResponse( uint id, stPlcResponse response )
{
    //response.datalen;

	char* pData = nullptr;
    uint dsize  = response.datalen;
    uint size   = sizeof(stIpcHeader) + dsize;	//1 or 2

	stIpcHeader head;
	head.cmd     = response.plc_cmd;
	head.dest    = PROTOCOL::_ATCHMB_UI;
	head.src     = PROTOCOL::_ATCHMB_COMM;
    head.datalen = dsize;
	head.slot    = 0x0;
	head.runner  = 0x0;
	head.error   = response.bPass?0x0:0x1;
	pData = new char[size];
	memcpy( pData, (char*)&head, size );
    if( dsize > 0 )
    {
        memcpy( pData+sizeof(stIpcHeader), (char*)&response.szdata, dsize );
    }
	emit SigIpcSendPacket( id, pData, size );
    //QString log = QString("").asprintf( "[SEND] RESPONSE(ID:%04X) %s : [DATA:0x%02X%02X]",
    //                                    id, UTIL::CHAMBER::PLCCMD_STR(response.plc_cmd).toStdString().c_str(), *(pData+sizeof(stIpcHeader))&0xFF, *(pData+sizeof(stIpcHeader)+1)&0xFF);
    //emit SigSaveLog( LOG_TYPE::_NORMAL_LOG , log );
}
