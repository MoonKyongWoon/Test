#include "tmsui.hpp"
#include "ui_tmsui.h"

CTmsUi::CTmsUi(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CTmsUi)
{
	ui->setupUi(this);
	InitVar();
	InitLog();
	InitUi();
	InitTimeOutChecker();
	InitTimer();
	InitSharedMemory();
}

CTmsUi::~CTmsUi()
{
	if (m_pShmMainUi->isAttached())
	{
		m_pShmMainUi->detach();
		m_pShmMainUi = nullptr;
	}
	if (m_pTmrTimeOut != nullptr)
	{
		StopTimer();

		delete m_pTmrTimeOut;
		m_pTmrTimeOut = nullptr;
	}
	if (m_pTimeOutCheckerTh)
	{
		m_pTimeOutCheckerTh->quit();
		m_pTimeOutCheckerTh->wait();
	}
	if (m_pTimeOutChecker != nullptr)
	{
		delete m_pTimeOutChecker;
		m_pTimeOutChecker = nullptr;
	}
	if( m_pLoggerSys != nullptr )
	{
		delete m_pLoggerSys;
		m_pLoggerSys = nullptr;
	}
	if( m_pLoggerHsms != nullptr )
	{
		delete m_pLoggerHsms;
		m_pLoggerHsms = nullptr;
	}
	if( m_pHsmsClient != nullptr )
	{
		delete m_pHsmsClient;
		m_pHsmsClient = nullptr;
	}
	if( m_pIpcClient != nullptr)
	{
		delete m_pIpcClient;
		m_pIpcClient = nullptr;
	}
	if (m_pTcpSvr != nullptr)
	{
		CloseTcpServer();
		DeleteTcpServer();
	}
	if (m_pIpcSvr != nullptr)
	{
		CloseIpcServer();
		DeleteIpcServer();
	}
	delete ui;
}

void CTmsUi::InitVar()
{
	m_pIpcSvr = nullptr;
	m_pTcpSvr = nullptr;
	m_pIpcClient = nullptr;
	m_pHsmsClient = nullptr;
	m_uPort = HSMS_DEFAULT_PORT;
	m_uDevId = 0;
	m_bAutoScrollSys  = true;
	m_bAutoScrollHsms = true;
	m_HsmsStatus = HSMS_STATUS::NOT_CONNECTED;
	m_bTcpStatus = false;
	m_bOnlineMode = false;
}

void CTmsUi::InitUi()
{
	ui->teSysLog->setContextMenuPolicy(Qt::CustomContextMenu );
	ui->teSysLog->setMaximumBlockCount( MAX_LOG_LINE_NUM );

	ui->teCommLog->setContextMenuPolicy(Qt::CustomContextMenu );
	ui->teCommLog->setMaximumBlockCount( MAX_LOG_LINE_NUM );

	connect( ui->teSysLog     , &QPlainTextEdit::customContextMenuRequested, this, &CTmsUi::SlotCustomContextMenuRequested );
	connect( ui->teCommLog    , &QPlainTextEdit::customContextMenuRequested, this, &CTmsUi::SlotCustomContextMenuRequested );
	connect( ui->splitterTmsUi, &QSplitter::splitterMoved                  , this, &CTmsUi::SlotSplitterMoved       );

	QSettings settings( CONFIG_INI_KEY, ui->splitterTmsUi->objectName() );
	QByteArray saved_geometry = settings.value("geometry").toByteArray();
	if ( !saved_geometry.isEmpty() )
		ui->splitterTmsUi->restoreGeometry( saved_geometry );
	QByteArray saved_state = settings.value("windowState").toByteArray();
	if ( !saved_state.isEmpty() )
		ui->splitterTmsUi->restoreState(saved_state);
}

void CTmsUi::InitLog()
{
	m_pLoggerSys  = new CLogger( PROTOCOL::_ATTMS_UI   );
	m_pLoggerHsms = new CLogger( PROTOCOL::_ATTMS_HSMS );

	connect( this, &CTmsUi::SigSaveSysLog , m_pLoggerSys , &CLogger::SlotLogMessage );
	connect( this, &CTmsUi::SigSaveHsmsLog, m_pLoggerHsms, &CLogger::SlotLogMessage );
}

void CTmsUi::InitTimeOutChecker()
{
	m_pTimeOutChecker = new CHsmsTimeOutChecker;
	m_pTimeOutCheckerTh = new QThread;
	m_pTimeOutChecker->moveToThread(m_pTimeOutCheckerTh);

	connect(m_pTimeOutCheckerTh, &QThread::finished, m_pTimeOutChecker, &QObject::deleteLater);
	connect(m_pTimeOutChecker, &QObject::destroyed, [this](){this->m_pTimeOutChecker = nullptr;});
	connect(m_pTimeOutChecker, &CHsmsTimeOutChecker::SigResendHsmsPacket, this, &CTmsUi::SlotResendHsmsPacket);

	m_pTimeOutCheckerTh->start();
}

void CTmsUi::InitTimer()
{
	m_pTmrTimeOut = new QTimer();

	connect(m_pTmrTimeOut, &QTimer::timeout, this, &CTmsUi::SlotHsmsTimeOut);
}

bool CTmsUi::StartTimer()
{
	m_pTmrTimeOut->start(1000);

	return m_pTmrTimeOut->isActive();
}

bool CTmsUi::StopTimer()
{
	m_pTmrTimeOut->stop();

	if (m_pTimeOutChecker->IsTimeOutEmpty())
	{
		m_pTimeOutChecker->RemoveAllTimeOut();
	}

	return !m_pTmrTimeOut->isActive();
}

void CTmsUi::SlotHsmsTimeOut()
{
	if (m_pTimeOutChecker->IsTimeOutEmpty())
	{
		switch (m_HsmsStatus)
		{
			case HSMS_STATUS::NOT_CONNECTED:
				m_pTmrTimeOut->stop();

				break;
			case HSMS_STATUS::CONNECTED:

				break;
			case HSMS_STATUS::NOT_SELECTED:

				break;
			case HSMS_STATUS::SELECTED:

				break;
			default:
				break;
		}

		return;
	}

	m_pTimeOutChecker->CheckTimeOut();
}

void CTmsUi::InitSharedMemory()
{
	m_pShmMainUi = new QSharedMemory(SHM_KEY::SHM_HSMS_KEY, this);

	if(!m_pShmMainUi->create(sizeof(stShmHsms), QSharedMemory::ReadWrite))
	{
		if(m_pShmMainUi->error() == QSharedMemory::AlreadyExists)
		{
			m_pShmMainUi->attach(QSharedMemory::ReadWrite);
		}
	}
	else
	{
		this->m_pShmMainUi->lock();
		stShmHsms* to = static_cast<stShmHsms*>(this->m_pShmMainUi->data());
		to->system_byte = 6200;
		this->m_pShmMainUi->unlock();
	}
}

bool CTmsUi::CloseIpcServer()
{
	disconnect(m_pIpcSvr, &CLocalServer::SigIncommingConnection, this, &CTmsUi::SlotIpcConnected);

	if(m_pIpcSvr->isListening())
	{
		m_pIpcSvr->close();
	}

	return true;
}

bool CTmsUi::DeleteIpcServer()
{
	delete m_pIpcSvr;
	m_pIpcSvr = nullptr;

	return true;
}

bool CTmsUi::ListenTcpServer()
{
	const int RETRY_COUNT = 10;
	int retry = 0;

	if (!m_pTcpSvr->isListening())
	{
		do
		{
			if(!m_pTcpSvr->listen(QHostAddress::AnyIPv4, m_uPort))
			{
				m_pTcpSvr->close();
			}
			else
			{
				break;
			}

			usleep(1000);
		}while(retry++ < RETRY_COUNT);

		if( retry >= RETRY_COUNT )
		{
			DisplaySysLog(LOG_TYPE::_ERROR_LOG , tr("[ERR ] EAP TCP Server(%1) Listen NG!").arg(m_uPort));
			return false;
		}
		else
		{
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, tr("[INFO] EAP TCP Server(%1) Listen OK!").arg(m_uPort));
			return true;
		}
	}
	else
	{
		DisplaySysLog(LOG_TYPE::_NORMAL_LOG, tr("[INFO] EAP TCP Server(%1) Listen OK!").arg(m_uPort));
		return true;
	}
}

bool CTmsUi::CloseTcpServer()
{
	if( m_pTcpSvr->isListening() )
	{
		m_pTcpSvr->close();
	}

	return true;
}

bool CTmsUi::DeleteTcpServer()
{
	delete m_pTcpSvr;
	m_pTcpSvr = nullptr;

	return true;
}

void CTmsUi::SlotCustomContextMenuRequested( QPoint point )
{
	bool bSender = false;
	bool bScroll = true;
	QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
	QMenu * pMenu = pSender->createStandardContextMenu();
	QAction * pAction;

	if( pSender == ui->teSysLog ) bScroll = m_bAutoScrollSys;
	else                	      bScroll = m_bAutoScrollHsms;

	bSender = true;
	pAction = new QAction( tr("Auto Scroll On"), pSender );
	pAction->setCheckable( true );
	if( bScroll ) pAction->setChecked(true);
	else          pAction->setChecked(false);
	connect( pAction, &QAction::triggered, this, &CTmsUi::SlotScrollStatusChanged );
	pMenu->addAction( pAction );

	// Clear.
	// Because QPlainTextEdit::clear() is a slot method, I can connect directly to it.
	pAction = new QAction( "Clear", pSender );
	connect( pAction, SIGNAL( triggered() ), pSender, SLOT( clear() ) );
	pMenu->addAction( pAction );

	// Show the menu.
	QPoint q = pSender->mapToGlobal( point );
	pMenu->exec( q );

	// The menu's ownership is transferred to me, so I must destroy it.
	delete pMenu;
}

void CTmsUi::SlotScrollStatusChanged()
{
	QAction* pSender = static_cast<QAction*>(QObject::sender());
	if( pSender->parent() == ui->teSysLog )
	{
		m_bAutoScrollSys = !m_bAutoScrollSys;
	}
	else if( pSender->parent() == ui->teCommLog )
	{
		m_bAutoScrollHsms = !m_bAutoScrollHsms;
	}
	else
	{

	}
}

void CTmsUi::SlotSplitterMoved( int pos, int index )
{
	(void)pos;
	(void)index;

	//ResizeView();
	QSplitter* pSender = static_cast<QSplitter*>(QObject::sender());
	//qDebug() << __FUNCTION__ << " : " << ui->splitterRun->objectName();
	QSettings settings( CONFIG_INI_KEY, pSender->objectName() );
	settings.setValue( "geometry"     , pSender->saveGeometry() );
	settings.setValue( "windowState"  , pSender->saveState()    );
	settings.sync();

	QByteArray saved_geometry = settings.value("geometry").toByteArray();
	if ( !saved_geometry.isEmpty() )
		pSender->restoreGeometry( saved_geometry );
	QByteArray saved_state = settings.value("windowState").toByteArray();
	if ( !saved_state.isEmpty() )
		pSender->restoreState(saved_state);
}

bool CTmsUi::LoadTmsIni()
{
	bool bOk = true;
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		m_sOsDir = QString(env_os);
	}
	else
	{
		m_sOsDir = DIR_ATOS_PATH;
	}
	m_sTmsIni = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(TMS_INI);

	int nValue = 0;
	QString sValue = 0;
	QString section_key = "";

	//EQUIPMENT ID
	section_key = tr("%1/%2").arg(SECTION_SYSTEM).arg(KEY_TMS_EQUIPID);
	if( !UTIL::INI::readSettings( m_sTmsIni, section_key, sValue ) )
	{
		bOk = false;
		sValue = DEFAULT_TMS_EQUIPID;
	}
	ui->leEqpId->setText( sValue );

	//TCP PORT
	section_key = tr("%1/%2").arg(SECTION_COMM).arg(KEY_TMS_PORT);
	if( !UTIL::INI::readSettings( m_sTmsIni, section_key, nValue ) )
	{
		bOk = false;
		nValue = DEFAULT_TMS_PORT;
	}
	m_uPort = nValue;
	ui->leTcpPort->setText( tr("%1").arg(nValue) );

	//DEVICE ID
	section_key = tr("%1/%2").arg(SECTION_COMM).arg(KEY_TMS_DEVID);
	if( !UTIL::INI::readSettings( m_sTmsIni, section_key, nValue ) )
	{
		bOk = false;
		nValue = DEFAULT_TMS_DEVID;
	}
	m_uDevId = nValue;
	ui->leDeviceId->setText( tr("%1").arg(nValue) );

	//T3, T5 ~ T8
	section_key = tr("%1/%2").arg(SECTION_COMM).arg("KEY_T3_TIME");
	UTIL::INI::readSettings(m_sTmsIni, section_key, m_pTimeOutChecker->m_uTimeOut[0]);

	for( int t = 1; t < KEY_TIMEOUT_MAX; t++ )
	{
		QString sKey = tr("KEY_T%1_TIME").arg(t + 4);
		section_key = tr("%1/%2").arg(SECTION_COMM).arg(sKey);
		UTIL::INI::readSettings( m_sTmsIni, section_key, m_pTimeOutChecker->m_uTimeOut[t]);
	}

	return bOk;
}

bool CTmsUi::RunIpcServer()
{
	if(m_pIpcClient != nullptr)
	{
		m_pIpcClient->deleteLater();
	}

	if(m_pIpcSvr != nullptr)
	{
		CloseIpcServer();
		DeleteIpcServer();
	}

	if(m_pIpcSvr == nullptr)
	{
		m_pIpcSvr = new CLocalServer( this );

		connect(m_pIpcSvr, &CLocalServer::SigIncommingConnection, this, &CTmsUi::SlotIpcConnected);
	}

	const int RETRY_COUNT = 10;
	int retry = 0;

	QLocalServer::removeServer(IPC_INFO::IPC_TMS_SOCK);

	do
	{
		if(!m_pIpcSvr->listen(IPC_INFO::IPC_TMS_SOCK))
		{
			m_pIpcSvr->close();
		}
		else
		{
			break;
		}

		usleep(5000);
	}while(retry++ < RETRY_COUNT);

	if(!m_pIpcSvr->isListening())
	{
		DisplaySysLog(LOG_TYPE::_ERROR_LOG , tr("[ERR ] IPC Server(%1) Listen NG!").arg(IPC_INFO::IPC_TMS_SOCK));
	}
	else
	{
		DisplaySysLog(LOG_TYPE::_NORMAL_LOG , tr("[INFO] IPC Server(%1) Listen OK!").arg(IPC_INFO::IPC_TMS_SOCK));
	}

	return m_pIpcSvr->isListening();
}

bool CTmsUi::RunTcpServer()
{
	if( m_pHsmsClient != nullptr )
	{
		m_pHsmsClient->deleteLater();
	}


	if(m_pTcpSvr != nullptr)
	{
		StopTimer();
		CloseTcpServer();
		DeleteTcpServer();
	}

	if( m_pTcpSvr == nullptr )
	{
		m_pTcpSvr = new CTcpServer();

		connect(m_pTcpSvr, &CTcpServer::SigIncommingConnection, this, &CTmsUi::SlotHsmsConnected);
	}

	bool bListen = false;

	bListen = ListenTcpServer();

	return bListen;
}

void CTmsUi::DisplaySysLog( LOG_TYPE t, QString s )
{
	QPlainTextEdit* pEditor = ui->teSysLog;
	if( pEditor == nullptr ) return;

	if( pEditor->document()->blockCount()> MAX_LOG_LINE_NUM )
		pEditor->clear();
	QTextCharFormat tf;
	tf = pEditor->currentCharFormat();
	tf.setForeground( FAIL_TEXT_QCOLOR );

	QString sLog = UTIL::DATE::yyyyMMDDHHmmss(true) + s;
	if( t == _NORMAL_LOG )
	{
		tf.setForeground( PASS_TEXT_QCOLOR );
		 pEditor->setCurrentCharFormat( tf );
	}
	else
	{
		tf.setForeground( FAIL_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	pEditor->appendPlainText( sLog );

	if( m_bAutoScrollSys )
	{
		QTextCursor c = pEditor->textCursor();
		c.movePosition(QTextCursor::End);
		pEditor->setTextCursor(c);
	}

	emit SigSaveSysLog( t, s );

}

void CTmsUi::DisplayHsmsLog( LOG_TYPE t, QString s )
{
	QPlainTextEdit* pEditor = ui->teCommLog;
	if( pEditor == nullptr ) return;

	if( pEditor->document()->blockCount()> MAX_LOG_LINE_NUM )
		pEditor->clear();
	QTextCharFormat tf;
	tf = pEditor->currentCharFormat();
	tf.setForeground( FAIL_TEXT_QCOLOR );

	QString sLog = UTIL::DATE::yyyyMMDDHHmmss(true) + s;
	if( t == _NORMAL_LOG )
	{
		tf.setForeground( PASS_TEXT_QCOLOR );
		 pEditor->setCurrentCharFormat( tf );
	}
	else
	{
		tf.setForeground( FAIL_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	pEditor->appendPlainText( sLog );

	if( m_bAutoScrollHsms )
	{
		QTextCursor c = pEditor->textCursor();
		c.movePosition(QTextCursor::End);
		pEditor->setTextCursor(c);
	}
	emit SigSaveHsmsLog( t, s );

}

void CTmsUi::SlotStart()
{
	DisplaySysLog(LOG_TYPE::_NORMAL_LOG , tr("[INFO] Starting IPC Server(%1).").arg(IPC_INFO::IPC_TMS_SOCK));
    RunIpcServer();
}

void CTmsUi::SlotRestart()
{
	if(m_pIpcClient == nullptr)
	{
		DisplaySysLog(LOG_TYPE::_NORMAL_LOG , tr("[INFO] Restarting IPC Server(%1).").arg(IPC_INFO::IPC_TMS_SOCK));
		RunIpcServer();
	}
	else
	{
		DisplaySysLog(LOG_TYPE::_ERROR_LOG , tr("[ERR ] IPC Server(%1) is already in use.").arg(IPC_INFO::IPC_TMS_SOCK));
	}

	if(m_pHsmsClient == nullptr)
	{
		if(LoadTmsIni())
		{
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG , tr("[INFO] Restarting EAP TCP Server(%1).").arg(m_uPort));
			RunTcpServer();
		}
		else
		{
			DisplaySysLog(LOG_TYPE::_ERROR_LOG , tr("[ERR ] Failed to load %1.").arg(m_sTmsIni));
		}
	}
	else
	{
		DisplaySysLog(LOG_TYPE::_ERROR_LOG , tr("[ERR ] EAP TCP Server(%1) is already in use.").arg(m_uPort));
	}
}

void CTmsUi::SlotSysLog( LOG_TYPE t, QString s )
{
	DisplaySysLog( t, s );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CTmsUi::SlotIpcConnected
/// \param handle
/// \details MAIN UI  connected
void CTmsUi::SlotIpcConnected( quintptr handle )
{
	if (m_pIpcClient != nullptr)
	{
		CLocalClient* pClient = new CLocalClient(handle);
		pClient->DisconnectServer();

		return;
	}

	m_pIpcClient = new CLocalClient(handle);

	DisplaySysLog(LOG_TYPE::_NORMAL_LOG, tr("[CONN] IPC Client connected."));
	emit SigIpcStatusChanged(true);

	connect(m_pIpcClient,	&CLocalClient::SigIpcConnectionStatus,	this,			&CTmsUi::SlotIpcStatusChanged);
	connect(m_pIpcClient,	&CLocalClient::SigIpcRecvPacket,		this,			&CTmsUi::SlotIpcRecvPacket);
	connect(m_pIpcClient,	&CLocalClient::SigIpcLogMessage,		this,			&CTmsUi::SlotSysLog);
	connect(m_pIpcClient,	&CLocalClient::destroyed       ,       [this](){this->m_pIpcClient = nullptr;});
	connect(this,			&CTmsUi::SigIpcSendPacket,				m_pIpcClient,	&CLocalClient::SlotSendPacket);

	SendTcpStatus();
    if( LoadTmsIni() )
    {
        DisplaySysLog(LOG_TYPE::_NORMAL_LOG , tr("[INFO] Starting EAP TCP Server(%1).").arg(m_uPort));
        RunTcpServer();
    }
    else
    {
        DisplaySysLog(LOG_TYPE::_ERROR_LOG , tr("[ERR ] Failed to load %1.").arg(m_sTmsIni));
    }
}

void CTmsUi::SlotIpcStatusChanged(bool bIpcConnected)
{
	if (!bIpcConnected)
	{
		m_pIpcClient->deleteLater();
		DisplaySysLog(LOG_TYPE::_ERROR_LOG, tr("[CONN] IPC Client disconnected."));
	}

	emit SigIpcStatusChanged(bIpcConnected);
}

void CTmsUi::SlotIpcRecvPacket(stIpcElement ipcElement)
{
	if (ipcElement.ipcHeader.cmd == PROTOCOL::TCP_COMMAND::CMD_TMS_SET_ONLINE_MODE)
	{
		memcpy(&m_bOnlineMode, ipcElement.pData, ipcElement.ipcHeader.datalen);

		if (ipcElement.pData != nullptr)
		{
			delete[] ipcElement.pData;
			ipcElement.pData = nullptr;
		}

		SetOnlineMode(m_bOnlineMode);
	}
	else if (ipcElement.ipcHeader.cmd == PROTOCOL::TCP_COMMAND::CMD_TMSUI_WITH_MAIN_MSG)
	{
		if (!m_bTcpStatus)
		{
			SendTcpStatus();

			return;
		}

		CSecsGem secsgem;
		atstring data_str;
		unsigned int size = ipcElement.ipcHeader.datalen;
		char *data = nullptr;
		data = new char[size];
		stHsmsHeader hsmsHeader;
		unsigned int systemBytes = 0x00;
		unsigned int deviceId = 0x00;

		memcpy(data, ipcElement.pData, size);
		memcpy(&hsmsHeader, ipcElement.pData, sizeof(hsmsHeader));

		systemBytes |= (hsmsHeader.stMsgHeader.cSystemBytes[0] << 24);
		systemBytes |= (hsmsHeader.stMsgHeader.cSystemBytes[1] << 16);
		systemBytes |= (hsmsHeader.stMsgHeader.cSystemBytes[2] << 8);
		systemBytes |= (hsmsHeader.stMsgHeader.cSystemBytes[3] << 0);

		deviceId |= (hsmsHeader.stMsgHeader.cDevidMsb << 7);
		deviceId |= (hsmsHeader.stMsgHeader.cDevidLsb << 0);

		DisplaySysLog(LOG_TYPE::_NORMAL_LOG, tr("[RECV] S%1F%2 from MAINUI (Wbit: %3, SystemBytes: %4, DeviceId: %5, Rbit: %6).").arg(hsmsHeader.stMsgHeader.cStream).arg(hsmsHeader.stMsgHeader.cFunction).arg(hsmsHeader.stMsgHeader.cWbit).arg(systemBytes).arg(deviceId).arg(hsmsHeader.stMsgHeader.cRbit));

		if (ipcElement.pData != nullptr)
		{
			delete[] ipcElement.pData;
			ipcElement.pData = nullptr;
		}
		CSecsMsg* pMsg = secsgem.CreateMsg( data, size );
		if( pMsg != nullptr )
		{
			pMsg->MsgToSML( data_str );
			secsgem.CloseMsg( pMsg );
		}

		if (hsmsHeader.stMsgHeader.cWbit)
		{
			m_pTimeOutChecker->AddTimeOut(systemBytes, TIMEOUT_TYPE::_T3, size, data);

			if (!m_pTmrTimeOut->isActive())
			{
				StartTimer();
			}
		}

		emit SigSendHsmsPacket(data, size);
		DisplayHsmsLog(LOG_TYPE::_NORMAL_LOG, tr("\n%1").arg(QString(data_str).replace("\t", "    ")));
		DisplaySysLog(LOG_TYPE::_NORMAL_LOG, tr("[SEND] S%1F%2 to EAP (Wbit: %3, SystemBytes: %4, DeviceId: %5, Rbit: %6).").arg(hsmsHeader.stMsgHeader.cStream).arg(hsmsHeader.stMsgHeader.cFunction).arg(hsmsHeader.stMsgHeader.cWbit).arg(systemBytes).arg(deviceId).arg(hsmsHeader.stMsgHeader.cRbit));
	}
}

void CTmsUi::SetTcpStatus(bool stat)
{
	m_bTcpStatus = stat;
	emit SigTcpStatusChanged(stat);
	SendTcpStatus();

	if(stat)
	{
		ui->leTcpStatus->setText( "CONNECTED" );
	}
	else
	{
		ui->leTcpStatus->setText( "NOT CONNECTED" );
	}
}

void CTmsUi::SetSelectStatus(bool selected )
{
	emit SigHsmsStatusChanged(selected);

	if( selected )
	{
		ui->leEqpSelect->setText( "SELECTED" );
	}
	else
	{
		ui->leEqpSelect->setText( "NOT SELECTED" );
	}
}

void CTmsUi::SetHsmsStatus(HSMS_STATUS status)
{
	this->m_HsmsStatus = status;

	switch(status)
	{
		case HSMS_STATUS::NOT_CONNECTED:
			DisplaySysLog(LOG_TYPE::_ERROR_LOG, "[CONN] EAP disconnected.");
			SetTcpStatus( false );
			break;
		case HSMS_STATUS::CONNECTED:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[CONN] EAP connected.");
			SetTcpStatus( true );
			break;
		case HSMS_STATUS::NOT_SELECTED:
			SetSelectStatus(false);
			break;
		case HSMS_STATUS::SELECTED:
			SetSelectStatus(true);
			break;
		default:
			break;
	}
}

void CTmsUi::SetOnlineMode(bool bOnline)
{
	if (bOnline)
	{
		ui->leEqpMode->setText("ONLINE");
	}
	else
	{
		ui->leEqpMode->setText("OFFLINE");
	}
}

void CTmsUi::SendTcpStatus()
{
	stIpcHeader ipcHeader;

	ipcHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_TMSUI_EAP_CONN_STAT;
	ipcHeader.src = PROTOCOL::SOURCE_TYPE::_ATTMS_UI;
	ipcHeader.dest = PROTOCOL::SOURCE_TYPE::_ATMAIN_UI;
	ipcHeader.slot = 0x0;
	ipcHeader.runner = 0x0;
	ipcHeader.error = 0;
	ipcHeader.datalen = sizeof(bool);

	unsigned int size = sizeof(stIpcHeader) + ipcHeader.datalen;
	char* pBuffer = nullptr;
	pBuffer = new char[size];

	memcpy(pBuffer, (char*)&ipcHeader, sizeof(stIpcHeader));
	memcpy(pBuffer + sizeof(stIpcHeader), (char*)&m_bTcpStatus, sizeof(bool));

	emit SigIpcSendPacket(pBuffer, size);
	DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[SEND] EAP connection status to Main UI.");
}

void CTmsUi::SendHsmsSelectRsp(stHsmsElement hsmsElement)
{
	stHsmsElement rsp;

	memcpy(&rsp, &hsmsElement, sizeof(stHsmsElement));

	//TODO: Add Condition for CONNECT_EXHAUST
	if (this->m_HsmsStatus == HSMS_STATUS::NOT_CONNECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(SELECT_STATUS::CONNECTION_NOT_READY);
	}
	else if (this->m_HsmsStatus == HSMS_STATUS::NOT_SELECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(SELECT_STATUS::COMMUNICATION_ESTABLISHED);
		SetHsmsStatus(HSMS_STATUS::SELECTED);
	}
	else if (this->m_HsmsStatus == HSMS_STATUS::SELECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(SELECT_STATUS::COMMUNICATION_ALREADY_ACTIVE);
	}

	rsp.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::SELECT_RSP);

	char *data = nullptr;
	data = new char[sizeof(stHsmsHeader)];

	memcpy(data, &rsp, sizeof(stHsmsHeader));

	emit SigSendHsmsPacket(data, sizeof(stHsmsHeader));
	DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[SEND] SELECT Response to EAP.");
}

void CTmsUi::SendHsmsDeselectRsp(stHsmsElement hsmsElement)
{
	stHsmsElement rsp;

	memcpy(&rsp, &hsmsElement, sizeof(stHsmsElement));

	//TODO: Add Condition for COMMUNICATION_BUSY
	if (this->m_HsmsStatus != HSMS_STATUS::SELECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(DESELECT_STATUS::COMMUNICATION_NOT_ESTABLISHED);
	}
	else if (this->m_HsmsStatus == HSMS_STATUS::SELECTED)
	{
		rsp.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(DESELECT_STATUS::COMMUNICATION_ENDED);
		SetHsmsStatus(HSMS_STATUS::NOT_SELECTED);
	}

	rsp.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::DESELECT_RSP);

	char *data = nullptr;
	data = new char[sizeof(stHsmsHeader)];

	memcpy(data, &rsp, sizeof(stHsmsHeader));

	emit SigSendHsmsPacket(data, sizeof(stHsmsHeader));
	DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[SEND] DESELECT Response to EAP.");
}

void CTmsUi::SendHsmsLinkTestRsp(stHsmsElement hsmsElement)
{
	if (this->m_HsmsStatus != HSMS_STATUS::SELECTED)
	{
		return;
	}

	stHsmsElement rsp;

	memcpy(&rsp, &hsmsElement, sizeof(stHsmsElement));

	rsp.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::LINKTEST_RSP);

	char *data = nullptr;
	data = new char[sizeof(stHsmsHeader)];

	memcpy(data, &rsp, sizeof(stHsmsHeader));

	emit SigSendHsmsPacket(data, sizeof(stHsmsHeader));
	DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[SEND] LINKTEST Response to EAP.");
}

void CTmsUi::SendHsmsRejectReq(stHsmsElement hsmsElement)
{
	stHsmsElement req;

	memcpy(&req, &hsmsElement, sizeof(stHsmsElement));

	req.hsmsHeader.stMsgHeader.cByte2 = hsmsElement.hsmsHeader.stMsgHeader.cSType;
	req.hsmsHeader.stMsgHeader.cFunction = static_cast<char>(REASON_CODE::TRANSACTION_NOT_OPEN);
	req.hsmsHeader.stMsgHeader.cSType = static_cast<char>(SType::REJECT_REQ);

	char *data = nullptr;
	data = new char[sizeof(stHsmsHeader)];

	memcpy(data, &req, sizeof(stHsmsHeader));

	emit SigSendHsmsPacket(data, sizeof(stHsmsHeader));
	DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[SEND] REJECT Request to EAP.");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CTmsUi::SlotHsmsConnected
/// \param handle
/// \details EAP client connected
void CTmsUi::SlotHsmsConnected( qintptr handle )
{
	if (m_pHsmsClient != nullptr)
	{
		CHsmsClient* pClient = new CHsmsClient(handle);

		pClient->CloseSocket();
		delete pClient;

		return;
	}

	this->m_pHsmsClient = new CHsmsClient(handle);
	this->SetHsmsStatus(HSMS_STATUS::CONNECTED);
	this->SetHsmsStatus(HSMS_STATUS::NOT_SELECTED);

	connect(this->m_pHsmsClient,	&CHsmsClient::SigStatusChanged, this               , &CTmsUi::SlotHsmsStatusChanged );
	connect(this->m_pHsmsClient,	&CHsmsClient::SigReceivePacket, this               , &CTmsUi::SlotHsmsReceivePacket );
	connect(this->m_pHsmsClient,	&CHsmsClient::destroyed       , [this](){this->m_pHsmsClient = nullptr;});
	connect(this               ,	&CTmsUi::SigSendHsmsPacket    ,	this->m_pHsmsClient, &CHsmsClient::SlotSendPacket   );

	StartTimer();
}

void CTmsUi::SlotHsmsStatusChanged( bool bConn )
{
	if (bConn)
	{
		this->SetHsmsStatus(HSMS_STATUS::CONNECTED);
		this->SetHsmsStatus(HSMS_STATUS::NOT_SELECTED);
	}
	else
	{
		this->SetHsmsStatus(HSMS_STATUS::NOT_SELECTED);
		this->SetHsmsStatus(HSMS_STATUS::NOT_CONNECTED);
		this->m_pHsmsClient->deleteLater();
	}
}

void CTmsUi::SlotHsmsReceivePacket(stHsmsElement hsmsElement)
{
	CSecsGem secsgem;
	atstring data_str;
	unsigned int systemBytes = 0x00;
	unsigned int deviceId = 0x00;
	uint uSType = hsmsElement.hsmsHeader.stMsgHeader.cSType;

	systemBytes |= (hsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[0] << 24);
	systemBytes |= (hsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[1] << 16);
	systemBytes |= (hsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[2] << 8);
	systemBytes |= (hsmsElement.hsmsHeader.stMsgHeader.cSystemBytes[3] << 0);

	deviceId |= (hsmsElement.hsmsHeader.stMsgHeader.cDevidMsb << 7);
	deviceId |= (hsmsElement.hsmsHeader.stMsgHeader.cDevidLsb << 0);

	if (hsmsElement.hsmsHeader.stMsgHeader.cWbit)
	{
		m_pShmMainUi->lock();
		stShmHsms* to = static_cast<stShmHsms*>(this->m_pShmMainUi->data());
		to->system_byte = systemBytes;
		m_pShmMainUi->unlock();
	}
	else
	{
		if(m_pTimeOutChecker->RemoveTimeOut(systemBytes))
		{
			if(m_pTimeOutChecker->IsTimeOutEmpty())
			{
				m_pTmrTimeOut->stop();
			}
		}
	}

	switch(uSType) {
		case 0:
		{
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, tr("[RECV] S%1F%2 from EAP (Wbit: %3, SystemBytes: %4, DeviceId: %5, Rbit: %6).").arg(hsmsElement.hsmsHeader.stMsgHeader.cStream).arg(hsmsElement.hsmsHeader.stMsgHeader.cFunction).arg(hsmsElement.hsmsHeader.stMsgHeader.cWbit).arg(systemBytes).arg(deviceId).arg(hsmsElement.hsmsHeader.stMsgHeader.cRbit));

			stIpcHeader ipcHeader;
			unsigned int msgLength = 0;
			char *dataMsg = nullptr;
			unsigned int dataMsgSize = 0;

			msgLength |= (hsmsElement.hsmsHeader.szMsgLength[0] << 24);
			msgLength |= (hsmsElement.hsmsHeader.szMsgLength[1] << 16);
			msgLength |= (hsmsElement.hsmsHeader.szMsgLength[2] << 8);
			msgLength |= (hsmsElement.hsmsHeader.szMsgLength[3] << 0);

			dataMsgSize = msgLength + sizeof(hsmsElement.hsmsHeader.szMsgLength);
			dataMsg = new char[dataMsgSize];

			memcpy(dataMsg, hsmsElement.hsmsHeader.szMsgLength, sizeof(hsmsElement.hsmsHeader.szMsgLength));
			memcpy(dataMsg + sizeof(hsmsElement.hsmsHeader.szMsgLength), &hsmsElement.hsmsHeader.stMsgHeader, sizeof(hsmsElement.hsmsHeader.stMsgHeader));

			if (dataMsgSize > 14)
			{
				memcpy(dataMsg + sizeof(hsmsElement.hsmsHeader.szMsgLength) + sizeof(hsmsElement.hsmsHeader.stMsgHeader), hsmsElement.pData, msgLength - sizeof(hsmsElement.hsmsHeader.stMsgHeader));
			}

			CSecsMsg* pMsg = secsgem.CreateMsg( dataMsg, dataMsgSize );

			if( pMsg != nullptr )
			{
				pMsg->MsgToSML( data_str );
				secsgem.CloseMsg( pMsg );
			}

			DisplayHsmsLog(LOG_TYPE::_NORMAL_LOG, tr("\n%1").arg(QString(data_str).replace("\t", "    ")));

			if (m_pIpcClient == nullptr)
			{
//				SendHsmsRejectReq(hsmsElement);
			}
			else
			{
				ipcHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_TMSUI_WITH_MAIN_MSG;
				ipcHeader.src = PROTOCOL::SOURCE_TYPE::_ATTMS_UI;
				ipcHeader.dest = PROTOCOL::SOURCE_TYPE::_ATMAIN_UI;
				ipcHeader.slot = 0x0;
				ipcHeader.runner = 0x0;
				ipcHeader.error = 0;
				ipcHeader.datalen = dataMsgSize;

				unsigned int size = sizeof(stIpcHeader) + ipcHeader.datalen;
				char* pBuffer = nullptr;
				pBuffer = new char[size];

				memcpy(pBuffer, (char*)&ipcHeader, sizeof(stIpcHeader));
				memcpy(pBuffer + sizeof(stIpcHeader), dataMsg, ipcHeader.datalen);

				emit SigIpcSendPacket(pBuffer, size);
				DisplaySysLog(LOG_TYPE::_NORMAL_LOG, tr("[SEND] S%1F%2 to Main UI (Wbit: %3, SystemBytes: %4, DeviceId: %5, Rbit: %6).").arg(hsmsElement.hsmsHeader.stMsgHeader.cStream).arg(hsmsElement.hsmsHeader.stMsgHeader.cFunction).arg(hsmsElement.hsmsHeader.stMsgHeader.cWbit).arg(systemBytes).arg(deviceId).arg(hsmsElement.hsmsHeader.stMsgHeader.cRbit));
			}

			if (dataMsg != nullptr)
			{
				delete[] dataMsg;
				dataMsg = nullptr;
			}

			if (hsmsElement.pData != nullptr)
			{
				delete[] hsmsElement.pData;
				hsmsElement.pData = nullptr;
			}

			break;
		}
		case 1:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] SELECT Request from EAP.");
			SendHsmsSelectRsp(hsmsElement);

			break;
		case 2:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] SELECT Response from EAP.");

			break;
		case 3:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] DESELECT Request from EAP.");
			SendHsmsDeselectRsp(hsmsElement);

			break;
		case 4:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] DESELECT Response from EAP.");
			break;
		case 5:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] LINKTEST Request from EAP.");
			SendHsmsLinkTestRsp(hsmsElement);

			break;
		case 6:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] LINKTEST Response from EAP.");

			break;
		case 7:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] REJECT Request from EAP.");

			break;
		case 9:
			DisplaySysLog(LOG_TYPE::_NORMAL_LOG, "[RECV] Separate Request from EAP.");

			if (m_HsmsStatus == HSMS_STATUS::SELECTED)
			{
				SetHsmsStatus(HSMS_STATUS::NOT_SELECTED);
			}

			break;
		default:
			break;
	}
}

void CTmsUi::SlotResendHsmsPacket(char* hsmsMsg, unsigned int size)
{
	char *pBuffer = nullptr;
	pBuffer = new char[size];

	memcpy(pBuffer, hsmsMsg, size);

	emit SigSendHsmsPacket(pBuffer, size);
}
