#include "../include/cpumon.hpp"

CCpuMon::CCpuMon(QObject *parent) : QObject(parent)
{
	this->m_server = new CTcpServer(this);
	this->m_client = nullptr;
	runTcpServer();
	initIniFile();
}

bool	CCpuMon::runTcpServer() {
	bool bListen = true;
	if (m_server->isListening()) return bListen;
	if (!m_server->listen(QHostAddress::Any, TCP_PORT::SYSTEM::PFM_PORT)) {
		m_server->close();
		bListen = false;
	}
	if (bListen) {
		connect(m_server, &CTcpServer::SigIncommingConnection, this, &CCpuMon::SlotIncommingConnection);
	}
	return bListen;
}

void CCpuMon::setSlotCpuInfo(stTcpElement& tcp_element) {
	if( tcp_element.tcpHeader.datalen > 0 )
	{
		if( tcp_element.tcpHeader.datalen == sizeof(stSlotCpu) )
		{
			stSlotCpu slot_cpu;
			memcpy( &slot_cpu, tcp_element.pData, tcp_element.tcpHeader.datalen );
			this->m_slot = slot_cpu.nSlotNo;
		}
	}
}

void CCpuMon::SlotStatusChanged( bool bConn )
{
	if( !bConn )
	{
		CTcpClient* pClient = qobject_cast<CTcpClient*>(QObject::sender());
		if( pClient )
		{
			pClient->deleteLater();
		}
	}
}

void CCpuMon::SlotReceivePacket( stTcpElement tcpelement )
{
	int nCmd = tcpelement.tcpHeader.cmd;
	switch( nCmd )
	{
		case PROTOCOL::TCP_COMMAND::CMD_SLOT_CPU_INFO_SEND:
		{
			setSlotCpuInfo(tcpelement);
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

void CCpuMon::SlotIncommingConnection( qintptr handle )
{
	this->m_client = new CTcpClient(handle);
	connect( m_client,		&CTcpClient::SigStatusChanged,	this,			&CCpuMon::SlotStatusChanged   );
	connect( m_pMonitor[0], &CMonitor::sigTCPSend,			this->m_client,	&CTcpClient::SendPacket );
	connect( m_pMonitor[1], &CMonitor::sigTCPSend,			this->m_client,	&CTcpClient::SendPacket );
	connect( this,			&CCpuMon::sigSendPacket,		m_client,		&CTcpClient::SlotSendPacket   );
	connect( m_client,		&CTcpClient::SigReceivePacket,	this,			&CCpuMon::SlotReceivePacket	);
}

void	CCpuMon::initIniFile() {
	std::string run_path = "";
	char* env_run  = getenv(ATRUN_DIR_ENV);
	if( env_run != NULL )
	{
		run_path = std::string( env_run );
	}
	else
	{
		run_path = std::string( DIR_ATRUN_PATH );
	}
	QString pfm_fault_ini = tr("%1/%2/%3").arg(run_path.c_str()).arg(CONFIG_DIR).arg(PFM_FAULT_INI);

	UTIL::FILE::WriteFile(pfm_fault_ini, "", false);
	for (int j = 0; j < 2; j += 1) {
		UTIL::FILE::AppendFile(pfm_fault_ini, tr("[CPU%1]").arg(j + 1), true);
		for (int i = 0; i < 8; i += 1)
			UTIL::FILE::AppendFile(pfm_fault_ini, tr("PPS%1=0").arg(i + 1), true);
		for (int i = 0; i < 8; i += 1)
			UTIL::FILE::AppendFile(pfm_fault_ini, tr("S1PS%1=0").arg(i + 1), true);
		UTIL::FILE::AppendFile(pfm_fault_ini, "", true);
	}
}

CCpuMon::~CCpuMon()
{
	if( m_pMonitorTh[0] )
	{
		m_pMonitorTh[0]->quit();
		m_pMonitorTh[0]->wait();
	}
	if( m_pMonitorTh[1] )
	{
		m_pMonitorTh[1]->quit();
		m_pMonitorTh[1]->wait();
	}

	if( m_pLogger )
	{
		delete m_pLogger;
		m_pLogger = nullptr;
	}
}

void CCpuMon::CreateInstance()
{
	fprintf( stdout, "Create Instance\n" );
	m_pMonitor[0] = new CMonitor( this->m_slot, 0 );
	m_pMonitorTh[0] = new QThread;
	m_pMonitor[0]->moveToThread( m_pMonitorTh[0] );

	connect( m_pMonitorTh[0] , &QThread::finished , m_pMonitor[0], &QObject::deleteLater  );
	connect( this, &CCpuMon::SigOpenHW, m_pMonitor[0], &CMonitor::SlotOpenHW );
	connect( this, &CCpuMon::SigRun   , m_pMonitor[0], &CMonitor::SlotRun    );

	connect( m_pMonitor[0], &CMonitor::SigOpenHWResult  , this, &CCpuMon::SlotOpenHWResult  );

	connect( m_pMonitor[0], &CMonitor::SigRunResult     , this, &CCpuMon::SlotRunResult     );
	connect( m_pMonitor[0], &CMonitor::SigLogMessage    , this, &CCpuMon::SlotLogMessage    );
	m_pMonitorTh[0]->start();
	if( 1 )
	{
		m_pMonitor[1] = new CMonitor( this->m_slot, 1 );
		m_pMonitorTh[1] = new QThread;
		m_pMonitor[1]->moveToThread( m_pMonitorTh[1] );

		connect( m_pMonitorTh[1] , &QThread::finished , m_pMonitor[1], &QObject::deleteLater  );
		connect( this, &CCpuMon::SigOpenHW, m_pMonitor[1], &CMonitor::SlotOpenHW );
		connect( this, &CCpuMon::SigRun   , m_pMonitor[1], &CMonitor::SlotRun    );

		connect( m_pMonitor[1], &CMonitor::SigOpenHWResult  , this, &CCpuMon::SlotOpenHWResult  );
		connect( m_pMonitor[1], &CMonitor::SigRunResult     , this, &CCpuMon::SlotRunResult     );
		connect( m_pMonitor[1], &CMonitor::SigLogMessage    , this, &CCpuMon::SlotLogMessage    );
		m_pMonitorTh[1]->start();
	}
}

void CCpuMon::Start()
{
	CreateInstance();
	emit SigOpenHW();
}

void CCpuMon::SlotOpenHWResult( uint cpu, bool result)
{
	if( result ) PRINTF( _NORMAL_LOG, tr("CPU%1 %2").arg(cpu+1).arg("Open H/W OK!") );
	else         PRINTF( _ERROR_LOG , tr("CPU%1 %2").arg(cpu+1).arg("Open H/W NG!") );
	emit SigRun(cpu);
}

void CCpuMon::SlotRunResult( uint cpu, bool result )
{
	if( result ) PRINTF( _NORMAL_LOG, tr("CPU%1 %2").arg(cpu+1).arg("Run Monitoring OK!") );
	else         PRINTF( _ERROR_LOG , tr("CPU%1 %2").arg(cpu+1).arg("Run Monitoring NG!") );
}

void CCpuMon::SlotLogMessage( uint cpu, LOG_TYPE t, QString msg )
{
	QString log = tr("[CPU%1] %2").arg(cpu+1).arg(msg);
	PRINTF( t, log );
}

void CCpuMon::PRINTF(LOG_TYPE t, QString msg)
{
	if( t == _ERROR_LOG )
		fprintf( stdout, "[ ERR] %s\n", msg.toStdString().c_str() );
	else
		fprintf( stdout, "[ MSG] %s\n", msg.toStdString().c_str() );
}
