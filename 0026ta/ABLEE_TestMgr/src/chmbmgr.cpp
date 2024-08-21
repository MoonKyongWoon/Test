#include "chmbmgr.hpp"

CChmbMgr::CChmbMgr(QObject *parent) : QObject(parent)
{
	m_bChamberRun = false;
	m_bChamberUse = false;
	m_uTestSlots  = 0;
	LoadChmbIni();
	InitChmbSync();
}

CChmbMgr::~CChmbMgr()
{
	if( m_timerChmb->isActive() )
	{
		m_timerChmb->stop();
	}
	if( m_pYkgPlc != nullptr )
	{
		m_pYkgPlc->PlcClose();
		m_pYkgPlc->deleteLater();
	}
}

void CChmbMgr::SetSlotInfo( stSlotInfo slotinfo )
{
	memcpy ( &m_stSlotInfo, &slotinfo, sizeof(stSlotInfo) );
	//for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
	//{
	//	bool bMask = m_stSlotInfo.SlotIp[s].bMask;
	//	for( int c=0;c<m_stSlotInfo.SlotCpu.nCpuPerSlot;c++ )
	//	{
	//		if( bMask )
	//			m_SlotLive.slot_status[s][c] = EN_SLOT_MASK;
	//		else
	//			m_SlotLive.slot_status[s][c] = EN_SLOT_OFFLINE;
	//	}
	//}
}

//2023.08.18 - KWMOON
void CChmbMgr::SetTestSlots(uint slot_bits)
{
	m_uTestSlots = slot_bits;
}

bool CChmbMgr::SetChamberUse(bool usage)
{
	m_bChamberUse = usage;
	//bool bConn = false;
	//if( usage )
	//{
	//	bConn = m_pYkgPlc->PlcConnect();
	//}
	//else
	//{
	//	if( m_pYkgPlc!= nullptr )
	//		m_pYkgPlc->PlcClose();
	//}
	//emit SigChmbConnStatus( bConn );
	return true;
}

void CChmbMgr::LoadChmbIni()
{
	QString os_path   = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
		os_path = QString(env_os);
	else
		os_path = DIR_ATOS_PATH;

	QString section_key = "";
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(CHAMBER_INI);
	//comment 2023.06.25 section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_USE);
	//comment 2023.06.25 if ( !UTIL::INI::readSettings( ini, section_key, m_bChamberUse ) )
	//comment 2023.06.25 	m_bChamberUse = false;
	m_bChamberUse = true;

	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_IP);
	if ( !UTIL::INI::readSettings( ini, section_key, m_sChamberIp ) )
		m_sChamberIp = DEFAULT_CHMB_IP;

	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_PORT);
	if ( !UTIL::INI::readSettings( ini, section_key, m_nChamberPort ) )
		m_nChamberPort = DEFAULT_CHMB_PORT;

	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_AMB_SV);
	if ( !UTIL::INI::readSettings( ini, section_key, m_dAmbSV ) )
		m_dAmbSV = DEFAULT_CHMB_AMB_SV;

	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_LOW);
	if ( !UTIL::INI::readSettings( ini, section_key, m_dTempLimitLow ) )
		m_dTempLimitLow = DEFAULT_CHMB_TEMP_LIMIT_LOW;

	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_HIGH);
	if ( !UTIL::INI::readSettings( ini, section_key, m_dTempLimitHigh ) )
		m_dTempLimitHigh = DEFAULT_CHMB_TEMP_LIMIT_HIGH;

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_WATCHDOG_PERIOD);
    if ( !UTIL::INI::readSettings( ini, section_key, m_nWatchdogPeriod ) )
        m_nWatchdogPeriod = WATCHDOG_DEFAULT_PERIOD;
}

void CChmbMgr::SlotChamberUse(bool use)
{
	SetChamberUse( use );
}

void CChmbMgr::SlotChmbMgrStart()
{
	Init();
	InitTimer();
	emit SigChmbMgrLogMessage( _NORMAL_LOG, "Started chmb mgr" );
}

void CChmbMgr::Init()
{
	m_nId = PROTOCOL::SOURCE_TYPE::_ATMGR;
	m_pYkgPlc = new CYokogawaPlc;
	m_pYkgPlc->SetMsgLoopMode( false );
	m_pYkgPlc->SetPlcTcpInfo( m_sChamberIp, m_nChamberPort );
	m_pYkgPlc->SetChamberTempLimit( m_dTempLimitLow, m_dTempLimitHigh );

	if( m_bChamberUse )
	{
        emit SigChmbMgrLogMessage( _NORMAL_LOG, tr("Init chmb mgr %1:%2").arg(m_sChamberIp).arg(m_nChamberPort) );
		m_pYkgPlc->PlcConnect();
	}
	else
	{
		m_pYkgPlc->PlcClose();
	}
    m_bChamberRun = false;
	m_bChmbMonSet = false;
	m_bChmbMonEnd = false;
}

//not use
void CChmbMgr::InitTimer()
{
    m_timerChmb = new QTimer(this);
    connect(m_timerChmb         , &QTimer::timeout, this, &CChmbMgr::SlotTimeoutChmb           );
	m_timerChmb->start( 5000 );
}

//not use
void CChmbMgr::SlotTimeoutChmb()
{
	bool bConn = false;
	if( !m_pYkgPlc->IsPlcConn() )
		m_pYkgPlc->PlcConnect();
	bConn = m_pYkgPlc->IsPlcConn();

	//qDebug() << "[TESTMGR-CHMBMBR] CHMB CONN : " << bConn;
	emit SigChmbConnStatus( bConn );
}

void CChmbMgr::InitChmbSync( unsigned int bitslot )
{
	SetTestSlots( bitslot );		//2023.08.18 - KWMOON
	for( uint s=0;s<MAX_SLOT;s++ )
	{
		//if( (1<<s) & bitslot )
		{
			for( uint c=0;c<MAX_SYS_CPU;c++ )
			{
				InitSlotSync( s,c );
				SetTestStat( s, c, EN_CHMB_STANDBY );
			}
		}
	}
}

void CChmbMgr::SetTestStat(uint s, uint c, EN_CHMB_TEST_STAT stat )
{
    m_bTest[s][c] = stat;
    QString str = "";
    switch ( stat ) {
        case EN_CHMB_TEST_STAT::EN_CHMB_STANDBY:
        str = "STANDBY";
        break;
        case EN_CHMB_TEST_STAT::EN_CHMB_TESTING:
        str = "TESTING";
        break;
        case EN_CHMB_TEST_STAT::EN_CHMB_TESTDONE:
        str = "TESTDONE";
        break;
		case EN_CHMB_TEST_STAT::EN_CHMB_HEATING_AMB:
		str = "HEATING-AMB";
		break;
	}
    emit SigChmbMgrLogMessage( _NORMAL_LOG, tr("[%1][S#%2-R#%3] STATUS : %4")
                          .arg("CHMBMGR", 10, QChar(' ')).arg(s+1, 2, 10, QChar('0')).arg(c+1, 2, 10, QChar('0')).arg(str) );
}

void CChmbMgr::InitSlotSync(uint s, uint c)
{
	m_stChmbSv[s][c].bSvSet   = false;
	m_stChmbSv[s][c].bRecvRun = false;
    m_stChmbSv[s][c].bEnter   = false;
    m_stChmbSv[s][c].bLeave   = true;
	m_stChmbSv[s][c].dSv = 0.0;
}

void CChmbMgr::SlotSetLivePgmList( stPgmLive pgmlive )
{
	memcpy( (char*)&m_PgmLive, (char*)&pgmlive, sizeof(stPgmLive) );
	if( 0 )
	{
		printf("MAX SLOT : %d\n", m_stSlotInfo.SlotCpu.nMaxSlot );
		for( int s=0;s<m_stSlotInfo.SlotCpu.nMaxSlot;s++ )
		{
			if( m_PgmLive.pgm_status[s][CPU00][PROTOCOL::EN_DATA_SOCKET] ==  EN_SLOT_MASK )
			{
				printf("SLOT%02d  mask\n", s+1 );
			}
			else
			{
				if( m_PgmLive.pgm_status[s][CPU00][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_ONLINE )
					printf("SLOT%02d CPU00 offline\n", s+1 );
				else
					printf("SLOT%02d CPU00 online\n", s+1 );

				if( m_stSlotInfo.SlotCpu.nPgmPerSlot == 2 )
				{
					if( m_PgmLive.pgm_status[s][CPU01][PROTOCOL::EN_DATA_SOCKET] != EN_SLOT_ONLINE )
					{
						printf("SLOT%02d CPU01 offline\n", s+1 );
					}
				}
			}
		}
	}
}

void CChmbMgr::SlotTestDone()
{
	DoChmbAmb();
	//SetChamberCmd( PROTOCOL::CMD_WW_SV_WRITE        , nSv, 2 );
	//SetChamberCmd( PROTOCOL::CMD_WB_TEMP_RUN_TRIGGER,   0, 0 );
}

//2023.05.31 - KWMOON( TEST END - AMBIENT OR 25DEGREE / TEST START - VARIABLE SET )
void CChmbMgr::SlotRecvTestCmd( char* pData, uint size )
{
    stIpcElement element;
    uint head_size = sizeof(stIpcHeader);
    if( size < head_size ) return;

    memcpy( &element.ipcHeader, pData, head_size );
    element.pData = nullptr;
    if( element.ipcHeader.datalen > 0 )
    {
        element.pData = new char[element.ipcHeader.datalen];
        memcpy( element.pData, pData + head_size, element.ipcHeader.datalen );
    }
    uint slot = element.ipcHeader.slot;
    uint cpu  = element.ipcHeader.runner;
    PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(element.ipcHeader.cmd);
    switch (cmd)
    {
        case PROTOCOL::CMD_TEST_START_ACK:
        case PROTOCOL::CMD_ITEM_TEST_START_ACK:
			SetTestStat( slot, cpu, EN_CHMB_TEST_STAT::EN_CHMB_TESTING );
            break;
        case PROTOCOL::CMD_TEST_STOP_ACK:
        case PROTOCOL::CMD_TEST_DONE:
			SetTestStat( slot, cpu, EN_CHMB_TEST_STAT::EN_CHMB_TESTDONE );
            DoChmbTestDone();
            break;
        case PROTOCOL::CMD_PGM_RUNTIME_ERROR:
            InitSlotSync( slot, cpu );
			SetTestStat( slot, cpu, EN_CHMB_TEST_STAT::EN_CHMB_TESTDONE );
            break;
        default:
            break;
    }

    if( element.ipcHeader.datalen > 0 )
    {
        delete[] element.pData;
        element.pData = nullptr;
    }
}

bool CChmbMgr::DoChmbTestDone()
{
    bool bOk = false;
    bool bAllDone = true;
    for( uint s=0;s<MAX_SLOT;s++ )
    {
        if( m_stSlotInfo.SlotIp[s].bMask ) continue;
        for( uint c=0;c<MAX_SYS_CPU;c++ )
        {
			if( m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE ) continue;
            if( m_bTest[s][c] != EN_CHMB_TESTDONE )
            {
                bAllDone = false;
                break;
            }
        }
        if( !bAllDone ) break;
    }
    if( bAllDone )
    {
        for( uint s=0;s<MAX_SLOT;s++ )
        {
            for( uint c=0;c<MAX_SYS_CPU;c++ )
            {
				SetTestStat( s, c, EN_CHMB_STANDBY );
            }
        }
		bOk = DoChmbAmb();
	}
    emit SigChmbMgrLogMessage( _NORMAL_LOG, tr("[%1] TEST DONE : %2")
                          .arg("CHMBMGR", 10, QChar(' ')).arg(bAllDone?"TRUE":"FALSE") );
    return bOk;
}

void CChmbMgr::SlotRecvChamberCmd( char* pData, uint size )
{
	stIpcElement element;
	uint head_size = sizeof(stIpcHeader);
	memcpy( &element.ipcHeader, pData, head_size );
	if( element.ipcHeader.datalen > 0 )
	{
		element.pData = new char[element.ipcHeader.datalen];
		memcpy( element.pData, pData + head_size, element.ipcHeader.datalen );
	}
	PROTOCOL::TCP_COMMAND cmd = static_cast<PROTOCOL::TCP_COMMAND>(element.ipcHeader.cmd);
	switch (cmd)
	{
		case PROTOCOL::CMD_CHMB_INIT_TEMP_REQ:
			DoDevInitTemp( cmd, element );
			break;
		case PROTOCOL::CMD_CHMB_ENTER_SYNC_REQ:
			DoDevEnter( cmd, element );
			break;
		case PROTOCOL::CMD_CHMB_SETSV_VALID_REQ:
			DoDevSetTempnValid( cmd, element );
			break;
		case PROTOCOL::CMD_CHMB_RUN_SYNC_REQ:
			DoDevChamberRun( cmd, element );
			break;
		case PROTOCOL::CMD_CHMB_READ_TEMP_REQ:
		case PROTOCOL::CMD_CHMB_READ_ALARM_REQ:
		case PROTOCOL::CMD_CHMB_READ_STAT1_REQ:
		case PROTOCOL::CMD_CHMB_READ_STAT2_REQ:
		case PROTOCOL::CMD_CHMB_DOOR_CLOSE_REQ:
		case PROTOCOL::CMD_CHMB_DOOR_OPEN_REQ:
		case PROTOCOL::CMD_CHMB_RACKIN_REQ:
		case PROTOCOL::CMD_CHMB_RACKOUT_REQ:
		case PROTOCOL::CMD_CHMB_SET_SV_REQ:
		case PROTOCOL::CMD_CHMB_SET_AMB_REQ:
		case PROTOCOL::CMD_CHMB_RESET_ALARM_REQ:
		case PROTOCOL::CMD_CHMB_RUN_REQ:
		case PROTOCOL::CMD_CHMB_STOP_REQ:
		case PROTOCOL::CMD_CHMB_MASK_ONOFF_REQ:
			DoDevControlChamber( cmd, element );
			break;
		default:
			break;
	}
	if( element.ipcHeader.datalen > 0 )
	{
		delete[] element.pData;
		element.pData = nullptr;
	}
}

void CChmbMgr::DoDevInitTemp( PROTOCOL::TCP_COMMAND reqcmd, stIpcElement emt )
{
	stTcpElement tcpemt;
	stChmbCmdResponse response;
	memset( &response, 0x0, sizeof(stChmbCmdResponse) );
	memcpy( &tcpemt.tcpHeader, &emt.ipcHeader, sizeof(stIpcHeader) );

	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_ACK;
	uint slot  = tcpemt.tcpHeader.slot;
	uint cpu   = tcpemt.tcpHeader.runner;
	bool bSlot = true;
	bool bCpu  = true;
	if( slot>(MAX_SLOT-1) )
	{
		bSlot = false;
		ack = PROTOCOL::PGM_CHMB_NACK;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "%s slot no %d is invalid", UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}
	if( cpu>(MAX_SYS_CPU-1) )
	{
		bCpu = false;
		ack = PROTOCOL::PGM_CHMB_NACK;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "%s cpu no %d is invalid", UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}
	if( bSlot && bCpu )
		InitSlotSync( slot, cpu );

	uint datalen = sizeof(stChmbCmdResponse);
	response.nAck = ack;
	response.nData = 0;
	response.nLen  = 0;

	tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_INIT_TEMP_ACK;
	tcpemt.tcpHeader.datalen = datalen;
	tcpemt.pData = new char[datalen];
	memcpy( tcpemt.pData, &response, datalen );
	emit SigChmbMgrAck( tcpemt );
}

void CChmbMgr::DoDevEnter( PROTOCOL::TCP_COMMAND reqcmd, stIpcElement emt )
{
	stTcpElement tcpemt;
	stChmbCmdResponse response;
	memset( &response, 0x0, sizeof(stChmbCmdResponse) );
	memcpy( &tcpemt.tcpHeader, &emt.ipcHeader, sizeof(stIpcHeader) );

	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_TRUE;
	uint slot  = emt.ipcHeader.slot;
	uint cpu   = emt.ipcHeader.runner;
	bool bSlot = true;
	bool bCpu  = true;
	if( slot>(MAX_SLOT-1) )
	{
		bSlot = false;
		ack = PROTOCOL::PGM_CHMB_FALSE;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "%s slot no %d is invalid", UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}

	if( cpu>(MAX_SYS_CPU-1) )
	{
		bCpu = false;
		ack = PROTOCOL::PGM_CHMB_FALSE;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "%s cpu no %d is invalid", UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}

    //bool bDoorClose = false;
    //DoChmbDoorCloseRead( bDoorClose );
    //emit SigChmbMgrLogMessage( _NORMAL_LOG, tr("[INFO] Door Close : %1").arg(bDoorClose) );
    //if( bDoorClose )
	{
		if( bSlot && bCpu )
		{
			bool bEnter = true;
			QString msg = "";
			bool bAllLeave = true;
			for( uint s=0;s<MAX_SLOT;s++ )
			{
				if( !( m_uTestSlots & (0x1<<s)) ) continue;		//2023.08.18 - KWMOON(Not Test Slot)
				for( uint c=0;c<MAX_SYS_CPU;c++ )
				{
					if( m_stSlotInfo.SlotIp[s].bMask ) continue;
					if( m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE ) continue;
					if( !m_stChmbSv[s][c].bLeave )
					{
						bAllLeave = false;
						ack = PROTOCOL::PGM_CHMB_FALSE;
						break;
					}
				}
				if( !bAllLeave ) break;
			}
			if( bAllLeave )
			{
				m_stChmbSv[slot][cpu].bEnter = true;
				for( uint s=0;s<MAX_SLOT;s++ )
				{
					if( m_stSlotInfo.SlotIp[s].bMask ) continue;
					for( uint c=0;c<MAX_SYS_CPU;c++ )
					{
						if( (s==slot) && (c==cpu) ) continue;			//my slot - cpu
						if( !( m_uTestSlots & (0x1<<s)) ) continue;		//2023.08.18 - KWMOON(Not Test Slot)
						if( m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE ) continue;			//2023.08.17 - KWMOON

						//2023.08.17 - KWMOON(COMMENT)
						//if( !m_stSlotInfo.SlotIp[s].bMask && (m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE) )
						//{
						//    tcpemt.tcpHeader.error = 1;
						//    sprintf( response.szMsg, "%s SLOT%02d-CPU%02d is not mask slot, but offline",
						//             UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), s+1, c+1  );
						//    bEnter = false;
						//    ack = PROTOCOL::PGM_CHMB_FAULT;
						//    break;
						//}
						if( !m_stChmbSv[s][c].bEnter )
						{
							msg = "";
							bEnter = false;
							ack = PROTOCOL::PGM_CHMB_FALSE;
						}
					}
					if( !bEnter ) break;
				}
				if( bEnter ) ack = PROTOCOL::PGM_CHMB_TRUE;
			}
		}
	}
    //else
    //{
    //	DoChmbDoorClose();
    //	ack = PROTOCOL::PGM_CHMB_FALSE;
    //}

	uint datalen = sizeof(stChmbCmdResponse);
	response.nAck = ack;
	response.nData = 0;
	response.nLen = 0;

	tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_ENTER_SYNC_ACK;
	tcpemt.tcpHeader.datalen = datalen;
	tcpemt.pData = new char[datalen];
	memcpy( tcpemt.pData, &response, datalen );
	emit SigChmbMgrAck( tcpemt );
}

void CChmbMgr::DoDevSetTempnValid( PROTOCOL::TCP_COMMAND reqcmd, stIpcElement emt )
{
	stSetTemp SetTemp;
	stTcpElement tcpemt;
	stChmbCmdResponse response;
	memset( &SetTemp, 0x0, sizeof(stSetTemp) );
	memset( &response, 0x0, sizeof(stChmbCmdResponse) );
	memcpy( &tcpemt.tcpHeader, &emt.ipcHeader, sizeof(stIpcHeader) );

	PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_TRUE;
	uint slot  = emt.ipcHeader.slot;
	uint cpu   = emt.ipcHeader.runner;
	uint len   = sizeof(stSetTemp);
	bool bSlot = true;
	bool bCpu  = true;
	if( slot>(MAX_SLOT-1) )
	{
		bSlot = false;
		ack = PROTOCOL::PGM_CHMB_FAULT;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "[S#%02d-R#%02d] %s slot no %d is invalid", slot+1, cpu+1, UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}
	if( cpu>(MAX_SYS_CPU-1) )
	{
		bCpu = false;
		ack = PROTOCOL::PGM_CHMB_FAULT;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "[S#%02d-R#%02d] %s cpu no %d is invalid",  slot+1, cpu+1, UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}
	if( sizeof(stSetTemp) != emt.ipcHeader.datalen )
	{
		bSlot = false;
		bCpu  = false;
		ack = PROTOCOL::PGM_CHMB_FAULT;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "[S#%02d-R#%02d] %s data size error, expect[%d] recv[%d]",
				 slot+1, cpu+1, UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), len, emt.ipcHeader.datalen );
	}

	if( bSlot && bCpu )
	{
		memcpy( &SetTemp, emt.pData, sizeof(stSetTemp) );
		double sv      = SetTemp.dTemp;
		bool bNotSync  = false;

		m_stChmbSv[slot][cpu].dSv    = sv;
		m_stChmbSv[slot][cpu].bSvSet = true;
		for( uint s=0;s<MAX_SLOT;s++ )
		{
			if( m_stSlotInfo.SlotIp[s].bMask ) continue;
			for( uint c=0;c<MAX_SYS_CPU;c++ )
			{
				if( (s==slot) && (c==cpu) ) continue;		//my slot - cpu
				if( m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE ) continue;		//2023.08.17 - KWMOON

				//2023.08.17 - KWMOON(COMMENT)
				//if( !m_stSlotInfo.SlotIp[s].bMask && (m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE) )
				//{
				//	ack = PROTOCOL::PGM_CHMB_FAULT;
				//	sprintf( response.szMsg, "%s S#%02d-R#%02d is not mask slot, but offline", UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), s+1, c+1 );
				//	bNotSync = true;
				//	break;
				//}

                if( m_stChmbSv[s][c].bEnter && m_stChmbSv[s][c].bSvSet )
				{
                    double diff = fabs( fabs( m_stChmbSv[s][c].dSv ) -  fabs( sv ) );
                    if( diff > DEFAULT_SV_GAP )
                    {
                        ack = PROTOCOL::PGM_CHMB_FAULT;
                        sprintf( response.szMsg, "[S#%02d-R#%02d] %s SV value(%6.2f) compare error with S#%02d-CPU%02d (%6.2f)",
                                 slot+1, cpu+1, UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), sv, s+1, c+1, m_stChmbSv[s][c].dSv );
						bNotSync = true;
                        break;
                    }
				}
				else
				{
                    ack = PROTOCOL::PGM_CHMB_FALSE;
					sprintf( response.szMsg, "%s S#%02d-R#%02d is not Sync Status!",
							 UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot+1, cpu+1 );
					bNotSync = true;
					break;
				}
			}
			if( bNotSync ) break;
		}
	}
    //ALL SV SET SYNC DONE
    if( ack == PROTOCOL::PGM_CHMB_TRUE )
    {
        m_bChamberRun = false;
    }

	uint datalen = sizeof(stChmbCmdResponse);
	response.nAck = ack;
	response.nData = 0;
	response.nLen  = 0;

	tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_SETSV_VALID_ACK;
	tcpemt.tcpHeader.datalen = datalen;
	tcpemt.pData = new char[datalen];
	memcpy( tcpemt.pData, &response, datalen );
	emit SigChmbMgrAck( tcpemt );
}

void CChmbMgr::DoDevChamberRun( PROTOCOL::TCP_COMMAND reqcmd, stIpcElement emt )
{
	stTcpElement tcpemt;
	stChmbCmdResponse response;
	memset( &response, 0x0, sizeof(stChmbCmdResponse) );
	memcpy( &tcpemt.tcpHeader, &emt.ipcHeader, sizeof(stIpcHeader) );

    PROTOCOL::PGM_CHMB_REQ_ACK ack = PROTOCOL::PGM_CHMB_TRUE;
	uint slot  = emt.ipcHeader.slot;
	uint cpu   = emt.ipcHeader.runner;
	bool bSlot = true;
	bool bCpu  = true;

	if( !m_pYkgPlc->IsPlcConn() )
	{
		bSlot = false;
		bCpu  = false;
		ack = PROTOCOL::PGM_CHMB_FAULT;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "[S#%02d-R#%02d] %s Chamber is not connected", slot+1, cpu+1, UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str() );
	}

	if( slot>(MAX_SLOT-1) )
	{
		bSlot = false;
		ack = PROTOCOL::PGM_CHMB_FAULT;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "[S#%02d-R#%02d] %s slot no %d is invalid", slot+1, cpu+1, UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}
	if( cpu>(MAX_SYS_CPU-1) )
	{
		bCpu = false;
		ack = PROTOCOL::PGM_CHMB_FAULT;
		tcpemt.tcpHeader.error = 1;
		sprintf( response.szMsg, "[S#%02d-R#%02d] %s cpu no %d is invalid",  slot+1, cpu+1, UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot );
	}

	bool bFault  = false;
	bool bAllSet = true;
	if( bSlot && bCpu )
	{
		m_stChmbSv[slot][cpu].bRecvRun = true;
		m_stChmbSv[slot][cpu].bLeave   = false;
		for( uint s=0;s<MAX_SLOT;s++ )
		{
			if( m_stSlotInfo.SlotIp[s].bMask ) continue;
			for( uint c=0;c<MAX_SYS_CPU;c++ )
			{
				if( (s==slot) && (c==cpu) ) continue;		//my slot - cpu
				if( m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE ) continue;		//2023.08.17 - KWMOON

				//2023.08.17 - KWMOON(COMMENT)
				//if( !m_stSlotInfo.SlotIp[s].bMask && (m_PgmLive.pgm_status[s][c][PROTOCOL::EN_DATA_SOCKET] == EN_SLOT_OFFLINE) )
				//{
				//	ack = PROTOCOL::PGM_CHMB_FAULT;
				//	tcpemt.tcpHeader.error = 1;
				//	sprintf( response.szMsg, "%s S#%02d-R#%02d is not mask slot, buf offline",
				//			 UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), slot+1, cpu+1 );
				//	bFault = true;
				//    bAllSet = false;
				//	break;
				//}
				//Not Enter
                if( !m_stChmbSv[s][c].bEnter )// || (!m_stChmbSv[s][c].bSvSet) )
				{
					ack = PROTOCOL::PGM_CHMB_FAULT;
					tcpemt.tcpHeader.error = 1;
					sprintf( response.szMsg, "%s S#%02d-R#%02d is not ENTER status",
							 UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), s, c );
					bFault = true;
					bAllSet = false;
					break;
				}
				//Not SV Set
				if( !m_stChmbSv[s][c].bSvSet )
				{
					ack = PROTOCOL::PGM_CHMB_FAULT;
					tcpemt.tcpHeader.error = 1;
					sprintf( response.szMsg, "%s S#%02d-R#%02d SV is not set",
							 UTIL::TCPIP::CMD_STR(reqcmd).toStdString().c_str(), s, c );
					bFault = true;
					bAllSet = false;
					break;
				}
				//No Chamber Run receive
				if( !m_stChmbSv[s][c].bRecvRun )
				{
					bAllSet = false;
					break;
				}
			}
			if( bFault ) break;
		}
		if( !bFault )
		{
            if( bAllSet )
			{
                if( !m_bChamberRun )
                {
					DoChmbMaskOnOff( true );
					bool bRun  = DoChmbRun();
					bool bSv   = DoChmbSvSet( m_stChmbSv[slot][cpu].dSv );
					if( bRun && bSv )
					{
						emit SigChmbMgrLogMessage( _NORMAL_LOG, tr("CHAMBER SV:%1 RUN OK!").arg(m_stChmbSv[slot][cpu].dSv) );
						m_bChamberRun = true;

						//2024.08.14 - KWMOON(TMEP MON SET)
						double sv = 0.0, pv = 0.0;
						if( DoChmbTempRead( sv, pv ) )
						{
							stGetTemp temp_set;
							memset( &temp_set, 0x0, sizeof(stGetTemp) );
							temp_set.dSv = sv;
							temp_set.dPv = pv;
							emit SigChmbTempMonSet( temp_set );
							m_bChmbMonSet = true;
							m_bChmbMonEnd = false;
							emit SigChmbMgrLogMessage( _NORMAL_LOG, tr("CHAMBER TEMP_MON SET SV[%1] PV[%2]").arg(sv).arg(pv) );
						}
						ack = PROTOCOL::PGM_CHMB_TRUE;
					}
					else
					{
						emit SigChmbMgrLogMessage( _ERROR_LOG, tr("CHAMBER SV:%1 RUN NG!").arg(m_stChmbSv[slot][cpu].dSv) );
						m_bChamberRun = false;
						m_bChmbMonSet = false;
						m_bChmbMonEnd = false;
						ack = PROTOCOL::PGM_CHMB_FALSE;
					}
                }
			}
			else
			{
				ack = PROTOCOL::PGM_CHMB_FALSE;
			}
		}
	}

	uint datalen = sizeof(stChmbCmdResponse);
	response.nAck = ack;
	response.nData = 0;
	response.nLen = 0;

	tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_RUN_SYNC_ACK;
	tcpemt.tcpHeader.datalen = datalen;
	tcpemt.pData = new char[datalen];
	memcpy( tcpemt.pData, &response, datalen );
	emit SigChmbMgrAck( tcpemt );
}

bool CChmbMgr::DoChmbSvSet(double sv)
{
	short sSv = static_cast<short>( sv * 10 );
	int nSv   = static_cast<int>(sSv&0xFFFF);
	bool bSet = DoChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE, nSv );
	if( bSet )
	{
		m_dCurrentSV = sv;	//2023.08.17 - KWMOON
		return true;
	}
	return false;

	//QMutexLocker locker(&m_mutexChmb);      //lock
	//stPlcResponse response;
	//bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE, response, nSv, 2 );
	//if( !bReq )
	//{
	//	//sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
	//	return false;
	//}
	//else
	//{
	//	if( !response.bPass )
	//		return false;
	//}
	//m_dCurrentSV = sv;	//2023.08.17 - KWMOON
	//return true;
}

bool CChmbMgr::DoChmbAlarmReset()
{
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET, 0x1 );
}

bool CChmbMgr::DoChmbRun()
{
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER, 0x1 );
}

bool CChmbMgr::DoChmbStop()
{
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER, 0x1 );
}

bool CChmbMgr::DoChmbAmb()
{
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER, 0x1 );
}

//2023.09.04 - KWMOON( MASK ON/OFF Change )
bool CChmbMgr::DoChmbMaskOnOff(bool onoff)
{
	int nOnOff = (onoff)?0x0:0x1;
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, nOnOff );
}

bool CChmbMgr::DoChmbRackOprTester(bool tester)
{
	int nData = (tester)?RACK_NOP_TESTER:RACK_NOP_CHAMBER;
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER, nData );
}

bool CChmbMgr::DoWatchdogTimeSet(uint seconds)
{
	return DoChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TIME_SET, seconds );
}

bool CChmbMgr::DoWatchdogOnoff(bool onoff)
{
	int nData = onoff?0x1:0x0;
	return DoChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB, nData );
}

bool CChmbMgr::DoWatchdog()
{
    static int nToggle = 0x0;
    int nData = nToggle&0x1;
	bool bWritten = DoChmbWordWrite( PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TOGGLE_TRIGGER, nData );
    nToggle=~nToggle;
	return bWritten;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChmbMgr::DoChmbDoorOpen
/// \return
/// \date 2023.08.17
/// \details chmb door open
bool CChmbMgr::DoChmbDoorOpen()
{
	DoChmbMaskOnOff( true );
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER, 0x1 );
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChmbMgr::DoChmbDoorClose
/// \return
/// \date 2023.08.17
/// \details chmb door close
bool CChmbMgr::DoChmbDoorClose()
{
	DoChmbMaskOnOff( true );
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER, 0x1 );
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChmbMgr::DoChmbRackIn
/// \return
/// \date 2023.08.17
/// \details chmb rack in
bool CChmbMgr::DoChmbRackIn()
{
	DoChmbMaskOnOff( true );
	DoChmbRackOprTester( true );
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER , 0x1 );
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChmbMgr::DoChmbRackIn
/// \return
/// \date 2023.08.17
/// \details chmb rack out
bool CChmbMgr::DoChmbRackOut()
{
	DoChmbMaskOnOff( true );
	DoChmbRackOprTester( true );
	return DoChmbBitWrite( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER, 0x1 );
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChmbMgr::DoChmbDoorCloseRead
/// \param rStat
/// \date 2023.08.17
/// \details chmb rack in / out set
bool CChmbMgr::DoChmbDoorCloseRead(bool& bClose)
{
	uint rWord = 0x0;
	bool bRtn = DoChmbWordRead( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1, rWord );
	bClose = false;
	bool bit_open  = false;
	bool bit_close = false;
	if( rWord & (0x1<<CHMB_STAT1_DOOR_OPEN) )
	{
		bit_open = true;
	}
	if( rWord & (0x1<<CHMB_STAT1_DOOR_CLOSE) )
	{
		bit_close = true;
	}
	(void)bit_open;
	bClose = bit_close;
	return bRtn;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChmbMgr::DoChmbRackStatusRead
/// \param rStat
/// \return
/// \date 2023.08.17
/// \details chmb rack in / out set
bool CChmbMgr::DoChmbRackInRead(bool& bRackIn)
{
	uint rWord = 0x0;
	bool bRtn = DoChmbWordRead( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1, rWord );
	bRackIn = false;
	bool bit_rackin  = false;
	bool bit_rackout = false;
	if( rWord & (0x1<<CHMB_STAT1_RACK_IN_COMPLETE) )
	{
		bit_rackin = true;
	}
	if( rWord & (0x1<<CHMB_STAT1_RACK_OUT_COMPLETE) )
	{
		bit_rackout = true;
	}
	if( bit_rackin && bit_rackout )
		bRackIn = false;
	else
		bRackIn = bit_rackin;
	return bRtn;
}

//2024.08.14 - KWMOON
bool CChmbMgr::DoChmbTempRead( double& sv, double& pv )
{
	int nData = 0x0;
	uint uLen = 0x0;
	uint nPv = 0x0, nSv = 0x0;
	short  uPv = 0x0, uSv = 0x0;
	stPlcResponse response;
	memset( &response, 0x0, sizeof(stPlcResponse) );

	bool bReqPv = m_pYkgPlc->RequestCmd( PROTOCOL::CMD_RW_PV, response, nData, uLen );
	if( bReqPv )
	{
		if( response.bPass )
		{
			if( response.datalen == 2 )
			{
				nPv  = (static_cast<int>(response.szdata[0])<<8)&0xFF00;
				nPv |= (static_cast<int>(response.szdata[1])<<0)&0x00FF;
				uPv  = static_cast<short>(nPv&0xFFFF);
				pv = static_cast<double>(uPv) / 10.0;
			}
		}
	}

	bool bReqSv = m_pYkgPlc->RequestCmd( PROTOCOL::CMD_RW_SV, response, nData, uLen );
	if( bReqSv )
	{
		if( response.bPass )
		{
			if( response.datalen == 2 )
			{
				nSv  = (static_cast<int>(response.szdata[0])<<8)&0xFF00;
				nSv |= (static_cast<int>(response.szdata[1])<<0)&0x00FF;
				uSv  = static_cast<short>(nSv&0xFFFF);
				sv = static_cast<double>(uSv) / 10.0;
			}
		}
	}

	return ( bReqPv & bReqSv );
}

bool CChmbMgr::DoChmbBitWrite ( PROTOCOL::PLC_COMMAND cmd, uint  nBit)
{
	if( !m_bChamberUse ) return true;

	QMutexLocker locker(&m_mutexChmb);      //lock
	stPlcResponse response;
	int nData = nBit&0x1;
	bool bReq = m_pYkgPlc->RequestCmd( cmd, response, nData, 1 );

	if( !bReq )
	{
		//sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
		return false;
	}
	else
	{
		if( !response.bPass )
			return false;
	}
	return true;
}

bool CChmbMgr::DoChmbBitRead  ( PROTOCOL::PLC_COMMAND cmd, uint& rBit)
{
	rBit = 0x0;
	if( !m_bChamberUse ) return true;

	QMutexLocker locker(&m_mutexChmb);      //lock
	int nData = 0x0;
	uint uLen = 0x0;
	stPlcResponse response;
	bool bReq = m_pYkgPlc->RequestCmd( cmd, response, nData, uLen );
	if( bReq )
	{
		if( response.bPass )
		{
			rBit  = static_cast<int>(response.szdata[0]&0x1);
			return true;
		}
	}
	return false;
}

bool CChmbMgr::DoChmbWordWrite( PROTOCOL::PLC_COMMAND cmd, uint nWord)
{
	if( !m_bChamberUse ) return true;

	QMutexLocker locker(&m_mutexChmb);      //lock
	stPlcResponse response;
	int nData = nWord;
	bool bReq = m_pYkgPlc->RequestCmd( cmd, response, nData, 2 );
	if( !bReq )
	{
		//sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
		return false;
	}
	else
	{
		if( !response.bPass )
			return false;
	}
	return true;
}

bool CChmbMgr::DoChmbWordRead ( PROTOCOL::PLC_COMMAND cmd, uint& rWord)
{
	rWord = 0x0;
	if( !m_bChamberUse ) return true;

	QMutexLocker locker(&m_mutexChmb);      //lock
	int nData = 0x0;
	uint uLen = 0x0;

	stPlcResponse response;
	bool bReq = m_pYkgPlc->RequestCmd( cmd, response, nData, uLen );
	if( bReq )
	{
		if( response.bPass )
		{
			rWord  = (static_cast<int>(response.szdata[0])<<8)&0xFF00;
			rWord |= (static_cast<int>(response.szdata[1])<<0)&0x00FF;
		}
	}
	//switch( cmd )
	//{
	//	case PROTOCOL::CMD_RW_PV                           :
	//	case PROTOCOL::CMD_RW_SV                           :
	//		break;
	//	case PROTOCOL::CMD_RW_PLC_VER                      :
	//	case PROTOCOL::CMD_RW_PCW_TEMP                     :
	//	case PROTOCOL::CMD_RW_WPS_PRESSURE                 :
	//	case PROTOCOL::CMD_RW_PCW_FLOW                     :
	//		break;
	//	case PROTOCOL::CMD_RW_STATUS1                      :
	//	case PROTOCOL::CMD_RW_STATUS2                      :
	//		break;
	//	case PROTOCOL::CMD_RW_ALARM0                       :
	//	case PROTOCOL::CMD_RW_ALARM1                       :
	//	case PROTOCOL::CMD_RW_ALARM2                       :
	//	case PROTOCOL::CMD_RW_ALARM3                       :
	//	case PROTOCOL::CMD_RW_ALARM4                       :
	//	case PROTOCOL::CMD_RW_ALARM5                       :
	//	case PROTOCOL::CMD_RW_ALARM6                       :
	//	case PROTOCOL::CMD_RW_ALARM7                       :
	//	case PROTOCOL::CMD_RW_ALARM8                       :
	//	case PROTOCOL::CMD_RW_ALARM9                       :
	//		break;
	//	default:
	//		break;
	//}
	return bReq;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CChmbMgr::DoDevControlChamber
/// \param reqcmd
/// \param element
/// \details Chamber Control Command
void CChmbMgr::DoDevControlChamber( PROTOCOL::TCP_COMMAND reqcmd, stIpcElement emt )
{
    QMutexLocker locker(&m_mutexChmb);      //lock
	int nData = 0x0;
	uint uLen = 0x0;
	stTcpElement tcpemt;
	memcpy( &tcpemt.tcpHeader, &emt.ipcHeader, sizeof(stIpcHeader) );
	uLen = emt.ipcHeader.datalen;
	if( uLen > 0 )
	{
		memcpy( (char*)&nData, emt.pData, uLen );
	}

	switch (reqcmd)
	{
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_TEMP_REQ:
		{
			bool bReq  = true;
			uint nPcw = 0x0;
			short  uPcw = 0x0;
			double dPv = 0.0, dSv = 0.0, dPcw = 0.0;
			stPlcResponse response;
			stGetTemp GetTemp;
			memset( &response, 0x0, sizeof(stPlcResponse) );
			memset( &GetTemp, 0x0, sizeof(stGetTemp) );

			GetTemp.nAck = PROTOCOL::PGM_CHMB_ACK;

			if( ! DoChmbTempRead( dSv, dPv ) )
			{
				GetTemp.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( GetTemp.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}

			//2024.08.14 - KWMOON
			if( GetTemp.nAck == PROTOCOL::PGM_CHMB_ACK)
			{
				if( m_bChmbMonSet && !m_bChmbMonEnd )
				{
					if( fabs( fabs(dSv) - fabs(dPv) )  < DEFAULT_CHMB_TEMP_MARGIN )
					{
						double sv = 0.0, pv = 0.0;
						if( DoChmbTempRead( sv, pv ) )
						{
							stGetTemp temp_set;
							memset( &temp_set, 0x0, sizeof(stGetTemp) );
							temp_set.dSv = sv;
							temp_set.dPv = pv;
							emit SigChmbTempMonEnd( temp_set );
							m_bChmbMonEnd = true;
							m_bChmbMonSet = false;
							emit SigChmbMgrLogMessage( _NORMAL_LOG, tr("CHAMBER TEMP_MON END SV[%1] PV[%2]").arg(sv).arg(pv) );
						}
					}
				}
			}
			/*
			bReq = m_pYkgPlc->RequestCmd( PROTOCOL::CMD_RW_PV, response, nData, uLen );
			if( !bReq )
			{
				GetTemp.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( GetTemp.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( response.bPass )
				{
					if( response.datalen == 2 )
					{
						nPv  = (static_cast<int>(response.szdata[0])<<8)&0xFF00;
                        nPv |= (static_cast<int>(response.szdata[1])<<0)&0x00FF;
						uPv  = static_cast<short>(nPv&0xFFFF);
						dPv = static_cast<double>(uPv) / 10.0;
					}
				}
				else
				{
					GetTemp.nAck = PROTOCOL::PGM_CHMB_NACK;
				}
			}

			if( GetTemp.nAck != PROTOCOL::PGM_CHMB_FAULT )
			{
				bReq = m_pYkgPlc->RequestCmd( PROTOCOL::CMD_RW_SV, response, nData, uLen );
				if( !bReq )
				{
					GetTemp.nAck = PROTOCOL::PGM_CHMB_FAULT;
					sprintf( GetTemp.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
				}
				else
				{
					if( response.bPass )
					{
						if( response.datalen == 2 )
						{
							nSv  = (static_cast<int>(response.szdata[0])<<8)&0xFF00;
                            nSv |= (static_cast<int>(response.szdata[1])<<0)&0x00FF;
							uSv  = static_cast<short>(nSv&0xFFFF);
							dSv = static_cast<double>(uSv) / 10.0;
						}
					}
					else
					{
						GetTemp.nAck = PROTOCOL::PGM_CHMB_NACK;
					}
				}
			}
			*/

			if( GetTemp.nAck != PROTOCOL::PGM_CHMB_FAULT )
			{
				bReq = m_pYkgPlc->RequestCmd( PROTOCOL::CMD_RW_PCW_TEMP, response, nData, uLen );
				if( !bReq )
				{
					GetTemp.nAck = PROTOCOL::PGM_CHMB_FAULT;
					sprintf( GetTemp.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
				}
				else
				{
					if( response.bPass )
					{
						if( response.datalen == 2 )
						{
							nPcw  = (static_cast<int>(response.szdata[0])<<8)&0xFF00;
                            nPcw |= (static_cast<int>(response.szdata[1])<<0)&0x00FF;
							uPcw  = static_cast<short>(nPcw&0xFFFF);
							dPcw = static_cast<double>(uPcw) / 10.0;
						}
					}
					else
					{
						GetTemp.nAck = PROTOCOL::PGM_CHMB_NACK;
					}
				}
			}

			GetTemp.dPv  = dPv;
			GetTemp.dSv  = dSv;
			GetTemp.dPcw = dPcw;

			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_TEMP_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stGetTemp);
			tcpemt.tcpHeader.error = ( GetTemp.nAck == PROTOCOL::PGM_CHMB_FAULT )?0x1:0x0;
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &GetTemp, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		//RFU
		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_ALARM_REQ:
		{

		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_STAT1_REQ:
		{
			uint uStatus = 0x0;
			stPlcResponse response;
			stGetChmbWord GetWord;
			memset( &response, 0x0, sizeof(stPlcResponse) );
			memset( &GetWord, 0x0, sizeof(stGetChmbWord) );
			GetWord.nAck = PROTOCOL::PGM_CHMB_ACK;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1, response, nData, uLen );
			if( !bReq )
			{
				GetWord.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( GetWord.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( response.bPass )
				{
					if( response.datalen == 2 )
					{
						uStatus |= (static_cast<int>(response.szdata[0])<<8);
						uStatus |= (static_cast<int>(response.szdata[1])<<0);
					}
				}
				else
				{
					GetWord.nAck = PROTOCOL::PGM_CHMB_NACK;
				}
			}

			GetWord.nStatus = uStatus;
			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_STAT1_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stGetChmbWord);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &GetWord, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_STAT2_REQ:
		{
			uint uStatus = 0x0;
			stPlcResponse response;
			stGetChmbWord GetWord;
			memset( &response, 0x0, sizeof(stPlcResponse) );
			memset( &GetWord, 0x0, sizeof(stGetChmbWord) );
			GetWord.nAck = PROTOCOL::PGM_CHMB_ACK;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_RW_STATUS2, response, nData, uLen );
			if( !bReq )
			{
				GetWord.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( GetWord.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( response.bPass )
				{
					if( response.datalen == 2 )
					{
						uStatus |= (static_cast<int>(response.szdata[0])<<8);
						uStatus |= (static_cast<int>(response.szdata[1])<<0);
					}
				}
				else
				{
					if( !response.bPass )
						GetWord.nAck = PROTOCOL::PGM_CHMB_NACK;
				}
			}

			GetWord.nStatus = uStatus;
			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_READ_STAT2_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stGetChmbWord);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &GetWord, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_DOOR_CLOSE_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nAck = PROTOCOL::PGM_CHMB_ACK;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( !response.bPass )
					CmdRes.nAck = PROTOCOL::PGM_CHMB_NACK;
			}
			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_DOOR_CLOSE_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_DOOR_OPEN_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( !response.bPass )
					CmdRes.nAck = PROTOCOL::PGM_CHMB_NACK;
			}
			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;

			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_DOOR_OPEN_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RACKIN_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( !response.bPass )
					CmdRes.nAck = PROTOCOL::PGM_CHMB_NACK;
			}
			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;

			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_RACKIN_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RACKOUT_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( !response.bPass )
					CmdRes.nAck = PROTOCOL::PGM_CHMB_NACK;
			}
			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;

			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_RACKOUT_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_SET_SV_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nAck = PROTOCOL::PGM_CHMB_ACK;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( !response.bPass )
					CmdRes.nAck = PROTOCOL::PGM_CHMB_NACK;
			}
			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_RUN_SYNC_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_SET_AMB_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( !response.bPass )
					CmdRes.nAck = PROTOCOL::PGM_CHMB_NACK;
			}
			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;

			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_SET_AMB_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RESET_ALARM_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			else
			{
				if( !response.bPass )
					CmdRes.nAck = PROTOCOL::PGM_CHMB_NACK;
			}
			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;

			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_RESET_ALARM_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_RUN_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nAck = PROTOCOL::PGM_CHMB_ACK;
			CmdRes.nData = 0x0;

			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;
			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_RUN_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_STOP_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nData = 0x0;
			nData = 0x1;
			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}
			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;
			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_STOP_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		case PROTOCOL::TCP_COMMAND::CMD_CHMB_MASK_ONOFF_REQ:
		{
			stPlcResponse response;
			stChmbCmdResponse CmdRes;
			CmdRes.nData = 0x0;
			nData = 0x1;
			bool bReq = m_pYkgPlc->RequestCmd( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, response, nData, uLen );
			if( !bReq )
			{
				CmdRes.nAck = PROTOCOL::PGM_CHMB_FAULT;
				sprintf( CmdRes.szMsg, "%s", m_pYkgPlc->GetErrMsg().toStdString().c_str() );
			}

			CmdRes.nAck = response.bPass?PROTOCOL::PGM_CHMB_ACK:PROTOCOL::PGM_CHMB_NACK;

			tcpemt.tcpHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_CHMB_STOP_ACK;
			tcpemt.tcpHeader.datalen = sizeof(stChmbCmdResponse);
			tcpemt.pData = new char[tcpemt.tcpHeader.datalen];
			memcpy( tcpemt.pData, &CmdRes, tcpemt.tcpHeader.datalen );
			emit SigChmbMgrAck( tcpemt );
		}
		break;

		default:
			break;
	}
	if( uLen > 0 )
	{
		delete[] emt.pData;
		emt.pData = nullptr;
	}
}
