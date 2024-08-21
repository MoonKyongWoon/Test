#include "eapsimm.hpp"
#include "ui_eapsimm.h"

CEapSimm::CEapSimm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CEapSimm)
{
	ui->setupUi(this);
	initVariable();
	initUi();
	LoadIni();
}

CEapSimm::~CEapSimm()
{
	if (this->server_simm)
		this->server_simm->deleteLater();
	if (this->client_simm)
		this->client_simm->deleteLater();
	if (this->socket)
		delete this->socket;
	delete ui;
}

void CEapSimm::initVariable()
{
	this->m_pMsgDlg = new CMsgDlg(this);
	this->server_simm = nullptr;
	this->client_simm = nullptr;
	this->socket = nullptr;
	this->hsms_status = HSMS_STATUS::NOT_CONNECTED;
	this->system_bytes = 0;
}

void CEapSimm::initUi()
{
	connect(this->ui->btnSetMode,			&QPushButton::clicked,	this,	&CEapSimm::slotSetMode);
	connect(this->ui->tblSml    ,	        &QTableWidget::clicked,	this,	&CEapSimm::slotSelectStreamFunction);
	connect(this->ui->btnSmlSave,			&QPushButton::clicked,	this,	&CEapSimm::slotSmlSave);
	connect(this->ui->btnSmlReload,			&QPushButton::clicked,	this,	&CEapSimm::slotSmlReload);
	connect(this->ui->btnHsmsSend,			&QPushButton::clicked,	this,	&CEapSimm::slotSendDataMessage);
//	connect(this->ui->pbConvTemp,			&QPushButton::clicked,	this,	&CEapSimm::slotConvertFromBinToSml);//temp

	ui->tblSml->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->teCommLog->setContextMenuPolicy( Qt::CustomContextMenu );
	connect(this->ui->tblSml,				&QTableWidget::customContextMenuRequested, this, &CEapSimm::SlotCustomContextMenuSFList );
	connect(this->ui->teCommLog,			&QPlainTextEdit::customContextMenuRequested, this, &CEapSimm::SlotCustomContextMenuSMLView );

	connect( m_pMsgDlg, &CMsgDlg::SigSave, this, &CEapSimm::SlotSaveMsg );
}


void CEapSimm::SlotCustomContextMenuSFList( const QPoint& point )
{
	// Start with the standard menu.
	auto item = ui->tblSml->itemAt(point);
	if( !item ) return;

	QTableWidget* pSender = static_cast<QTableWidget*>(QObject::sender());
	QMenu * pMenu = new QMenu(this);
	QAction * pAction;

	if( pSender != ui->tblSml )
	{
		delete pMenu;
		return;
	}

	// Clear.
	// Because QPlainTextEdit::clear() is a slot method, I can connect directly to it.
	pAction = new QAction( "Add" );
	connect( pAction, &QAction::triggered , this, &CEapSimm::AddStreamFunction );
	pMenu->addAction( pAction );

	pAction = new QAction( "Copy" );
	connect( pAction, &QAction::triggered , this, &CEapSimm::CopyStreamFunction );
	pMenu->addAction( pAction );

	pAction = new QAction( "Remove" );
	connect( pAction, &QAction::triggered , this, &CEapSimm::RemoveStreamFunction );
	pMenu->addAction( pAction );

	// Show the menu.
	//pMenu->popup( ui->tblSml->viewport()->mapToGlobal(point) );
	pMenu->exec( ui->tblSml->viewport()->mapToGlobal(point) );

	// The menu's ownership is transferred to me, so I must destroy it.
	pMenu->deleteLater();
}

void CEapSimm::SlotCustomContextMenuSMLView( const QPoint& point )
{
	// Start with the standard menu.
	QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
	if( !pSender ) return;

	QMenu * pMenu = pSender->createStandardContextMenu();
	QAction * pAction;

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


void CEapSimm::slotSendDataMessage()
{
	QString sml = ui->teSml->document()->toPlainText();

	uint sysbyte = 0;
	if( ui->chkSystemByte )
	{
		sysbyte = ui->spinSystemByte->value();
	}
	CSecsMsg* pMsg = m_secsgem.CompileSML( sml.toStdString(), sysbyte, device_id );
	if( pMsg == nullptr )
	{
		std::vector<atstring> vecErrors = m_secsgem.getCompileErrors();
		appendSystemLog( tr("%1").arg(m_sCurrentSF) );
		for( atstring msg : vecErrors )
		{
			appendSystemLog( QString(msg) );
		}
	}
	else
	{
		if( m_secsgem.GetMsgHSMS( pMsg, m_baHsmsPacket ) < 0 )
		{
			appendHsmsInfo( tr("[SEND] %1 error, SML check please!").arg(m_sCurrentSF) );
		}
		else
		{
			uint length = this->m_baHsmsPacket.length();
			char* temp = new char[ length ];
			memset( temp, 0, length );
			memcpy( temp, this->m_baHsmsPacket.constData(), length );

			atstring rSML = "";
			short stream =0, function = 0, wbit = 0;
			int deviceid = 0, rbit = 0;
			uint sysbytes = 0;
			m_secsgem.GetMsgInfo( pMsg, stream, function, wbit );
			m_secsgem.GetMsgSML( pMsg, rSML );
			m_secsgem.GetDeviceId(pMsg, deviceid);
			m_secsgem.GetRBit(pMsg, rbit);
			m_secsgem.GetSystemByte( pMsg, sysbytes );

			if (this->server_simm && this->server_simm->getConnect()) {
				emit this->server_simm->sigSendPacket( temp, length );

				appendHsmsInfo( tr("[SEND]S%1F%2 Wbit:%3 SystemBytes:%4 DeviceId:%5 Rbit:%6").arg(stream).arg(function).arg(wbit).arg(sysbytes).arg(deviceid).arg(rbit) );
				appendSmlLog( QString(rSML) );
			}
			else if (this->client_simm) {
				emit this->client_simm->sigSendPacket( temp, length );

				appendHsmsInfo( tr("[SEND]S%1F%2 Wbit:%3 SystemBytes:%4 DeviceId:%5 Rbit:%6").arg(stream).arg(function).arg(wbit).arg(sysbytes).arg(deviceid).arg(rbit) );
				appendSmlLog( QString(rSML) );
			}
			else {
				appendHsmsInfo( tr("[ ERR]SEND S%1F%2 Wbit:%3 SystemBytes:%4 DeviceId:%5 Rbit:%6").arg(stream).arg(function).arg(wbit).arg(sysbytes).arg(deviceid).arg(rbit) );
				appendSmlLog( QString(rSML) );
				appendSystemLog( "Simulator mode is not setted" );
				delete[] temp;
			}
		}
		delete pMsg;
	}

/*
    this->ui->teBin->append("[SEND]\n");
	for (int i = 0; i < this->m_baHsmsPacket.length(); i++)
	{
		this->ui->teBin->append(QString("").asprintf("[%02d] %02X", i, this->m_baHsmsPacket.at(i) & 0xFF));
        if( i == 3 )
            this->ui->teBin->append( "-------------------- " );
    }
*/
}

void CEapSimm::slotRecvSystemBytes(uint sysbytes)
{
	ui->spinSystemByte->setValue( static_cast<int>(sysbytes) );
	//ui->leSystemBytes->setText( tr("%1").arg(sysbytes) );
}

void CEapSimm::slotConvertFromBinToSml()
{
/*
	this->ui->teLog->clear();
	this->ui->teBin->clear();

	for (int i = 0; i < this->m_baHsmsPacket.length(); i++)
        this->ui->teBin->append(QString("").asprintf("%02X",ba.at(i) & 0xFF));

	atstring sml;

	//CHsmsCnv converter;
	CSecsGem secs;
	CSecsMsg* pMsg =  secs.CreateMsg( ba.data(), ba.length() );
	if( pMsg != nullptr )
	{
		secs.GetMsgSML( pMsg, sml );
	}
	secs.CloseMsg( pMsg );

	//converter.HsmsToSml(ba.data(), ba.length(), sml);
	this->ui->teLog->appendPlainText(QString(sml).replace("\t", "    "));
*/
}

/*
void CEapSimm::slotConvertFromSmlToBin()
{
	qDebug() << __func__;
	if (this->client_simm)
		this->system_bytes = this->client_simm->getSystemByte();
	if (this->server_simm)
		this->system_bytes = this->server_simm->getSystemByte();

	//uint session_id = this->ui->leSessionId->text().toUInt();
	//this->system_bytes = this->ui->leSystemBytes->text().toUInt();
	// TODO
	// Convert Sml To Bin
	// Display to teSml

	atstring sml = this->ui->teSml->toPlainText().toStdString();

	CSecsGem secsgem;
	//CHsmsCnv converter;
	//converter.SetSessionId(session_id);
	//converter.SetSystemBytes(this->system_bytes);
	CSecsMsg* pSecsMsg =  secsgem.CompileSML( sml );
	if ( pSecsMsg == nullptr )
	{
		std::vector<atstring> vecError = secsgem.getCompileErrors();
		for( atstring err : vecError )
		{
			//printf("%s\n", err.c_str() );
			qDebug() << "ERROR : " << err.c_str();
			appendSmlLog(err.c_str());
		}
		return;
	}
	else
	{
		m_baHsmsPacket.clear();
		if( secsgem.GetMsgHSMS( pSecsMsg, m_baHsmsPacket ) > 0 )
		{
			qDebug() << "HEX RESULT : " << m_baHsmsPacket.toHex();
			qDebug() << "BIN RESULT : " << m_baHsmsPacket;
		}
	}
}
*/

void CEapSimm::slotSelectStreamFunction( const QModelIndex& index)
{
	if ( this->ui->tblSml->rowCount() < 1 )
		return;	
	QString selected_sect = ui->tblSml->item( index.row(), 0 )->text().trimmed();	//this->ui->listSml->currentItem()->text();
	QString selected_desc = ui->tblSml->item( index.row(), 1 )->text().trimmed();	//this->ui->listSml->currentItem()->text();
	m_sCurrentSF = selected_sect;

	ui->leSection->setText( selected_sect );
	ui->leDesc->setText( selected_desc );

	LoadStreamFunction( m_sCurrentSF );
}

void CEapSimm::LoadStreamFunction(QString section)
{
	QString os_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	//QString ini = "/opt/at6200/config/simul.sml";
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg("simul.sml");

	this->ui->teSml->clear();
	QString rStr = "";

	if( UTIL::INI::readSettings( ini, tr("%1/%2").arg(section).arg(INI_KEY_SML), rStr ) )
	{
		rStr.replace("	","    ");
		ui->teSml->appendPlainText( rStr );
	}
	else
	{
		ui->teSml->appendPlainText( tr("%1 load error!").arg(INI_KEY_SML) );
	}

	if( UTIL::INI::readSettings( ini, tr("%1/%2").arg(section).arg(INI_KEY_DESCRIPTION), rStr ) )
	{
		rStr.replace("	","    ");
		ui->leDesc->setText( rStr );
	}
	else
	{
		//ui->teSml->appendPlainText( tr("%1 load error!").arg(INI_KEY_DESCRIPTION) );
	}
}

void CEapSimm::slotSmlReload()
{
	LoadStreamFunction( m_sCurrentSF );
}

void CEapSimm::slotSmlSave()
{
	QString os_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	//QString ini = "/opt/at6200/config/simul.sml";
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg("simul.sml");
	QString sml = ui->teSml->document()->toPlainText();
	QString desc = ui->leDesc->text();

	if( UTIL::INI::writeSettings( ini, tr("%1/%2").arg(m_sCurrentSF).arg(INI_KEY_SML), sml ) )
	{
	}
	else
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 save error!").arg(m_sCurrentSF), QMessageBox::Ok );
	}

	if( UTIL::INI::writeSettings( ini, tr("%1/%2").arg(m_sCurrentSF).arg(INI_KEY_DESCRIPTION), desc ) )
	{
	}
	else
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 save error!").arg(m_sCurrentSF), QMessageBox::Ok );
	}
	LoadIni();
}

void CEapSimm::SlotSaveMsg( QString section, QString description, QString sml )
{
	QString os_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	//QString ini = "/opt/at6200/config/simul.sml";
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg("simul.sml");

	if( UTIL::INI::writeSettings( ini, tr("%1/%2").arg(section).arg(INI_KEY_SML), sml ) )
	{
	}
	else
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 save error!").arg(m_sCurrentSF), QMessageBox::Ok );
	}

	if( UTIL::INI::writeSettings( ini, tr("%1/%2").arg(section).arg(INI_KEY_DESCRIPTION), description ) )
	{
	}
	else
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 save error!").arg(m_sCurrentSF), QMessageBox::Ok );
	}
	LoadIni();
}


void CEapSimm::setButtonEnable(bool enable)
{
	this->ui->btnConnect->setEnabled(enable);
	this->ui->btnDisconnect->setEnabled(enable);
	this->ui->btnSelect->setEnabled(enable);
	this->ui->btnDeselect->setEnabled(enable);
	this->ui->btnLinktest->setEnabled(enable);
	this->ui->btnReject->setEnabled(enable);
	this->ui->btnSeperate->setEnabled(enable);
}

void CEapSimm::appendBinLog(QString log)
{
	//this->ui->teBin->append(UTIL::DATE::yyyyMMDDHHmmsszzz(true));	this->ui->teBin->append(log);
}

void CEapSimm::appendSystemLog(QString log)
{
	this->ui->teSystemLog->append(tr("%1 %2").arg(UTIL::DATE::yyMMDDHHmmss(true)).arg(log));
//	this->ui->teSystemLog->append(log);
}

void CEapSimm::appendHsmsInfo(QString log)
{
	QString info = UTIL::DATE::yyyyMMDDHHmmsszzz(true) + log;
	this->ui->teCommLog->appendPlainText( info );
}

void CEapSimm::appendSmlLog(QString log)
{
	QString sml = log.replace("	","    " );
	this->ui->teCommLog->appendPlainText(sml+"\n");
}

void CEapSimm::setTcpStatus(bool bConn)
{
	if (bConn)
	{
		this->ui->leTcpStat->setText("CONNECTED");
		this->hsms_status = HSMS_STATUS::CONNECTED;
		this->hsms_status = HSMS_STATUS::NOT_SELECTED;
	}
	else
	{
		this->ui->leTcpStat->setText("DISCONNECTED");
		this->ui->leSelectStat->setText("DESELECTED");
		this->hsms_status = HSMS_STATUS::NOT_CONNECTED;
	}
}

void CEapSimm::setHsmsStatus(bool bSelect)
{
	if (bSelect)
	{
		this->ui->leSelectStat->setText("SELECTED");
		this->hsms_status = HSMS_STATUS::SELECTED;
	}
	else
	{
		this->ui->leSelectStat->setText("DESELECTED");
		this->hsms_status = HSMS_STATUS::NOT_SELECTED;
	}
}

void CEapSimm::slotSetMode()
{
//	QString ip = this->ui->leIP->text().isEmpty() ? TEST_IP : this->ui->leIP->text();
//	quint16 port = this->ui->lePort->text().isEmpty() ? TEST_PORT : this->ui->lePort->text().toUInt();

	if (this->ui->cbMode->currentText().compare("SERVER") == 0)
	{
		this->appendSystemLog("SET SERVER MODE");
		this->setButtonEnable(false);

		if (this->client_simm)
			this->client_simm->deleteLater();

		this->server_simm = new CServerSimm;
		connect(this->server_simm,			&CServerSimm::sigAppendBinLog,		this,				&CEapSimm::slotAppendBinLog);
		connect(this->server_simm,			&CServerSimm::sigAppendSystemLog,	this,				&CEapSimm::slotAppendSystemLog);
		connect(this->server_simm,			&CServerSimm::sigAppendHsmsInfo,    this,				&CEapSimm::slotAppendHsmsInfo);
		connect(this->server_simm,			&CServerSimm::sigAppendSmlLog,		this,				&CEapSimm::slotAppendSmlLog);
		connect(this->server_simm,			&CServerSimm::sigSetTcpStatus,		this,				&CEapSimm::slotSetTcpStatus);
		connect(this->server_simm,			&CServerSimm::sigSetHsmsStatus,		this,				&CEapSimm::slotSetHsmsStatus);
        connect(this->server_simm,			&CServerSimm::sigRecvSystemBytes,   this,				&CEapSimm::slotRecvSystemBytes);
        return ;
	}
	if (this->ui->cbMode->currentText().compare("CLIENT") == 0)
	{
		this->appendSystemLog("SET CLIENT MODE");
		this->setButtonEnable(true);

		if (this->server_simm)
			this->server_simm->deleteLater();

		this->client_simm = new CClientSimm;
		connect(this->ui->btnConnect,		&QPushButton::clicked,				this->client_simm,	&CClientSimm::slotConnectToHost);
		connect(this->ui->btnDisconnect,	&QPushButton::clicked,				this->client_simm,	&CClientSimm::slotDisconnectFromHost);
		// TODO
		// add parameter for session id
		connect(this->ui->btnSelect,		&QPushButton::clicked,				[this](){this->client_simm->slotSendControlMessage(SType::SELECT_REQ);});
		connect(this->ui->btnDeselect,		&QPushButton::clicked,				[this](){this->client_simm->slotSendControlMessage(SType::DESELECT_REQ);});
		connect(this->ui->btnSeperate,		&QPushButton::clicked,				[this](){this->client_simm->slotSendControlMessage(SType::SEPARATE_REQ);});
		connect(this->ui->btnReject,		&QPushButton::clicked,				[this](){this->client_simm->slotSendControlMessage(SType::REJECT_REQ);});
		connect(this->ui->btnLinktest,		&QPushButton::clicked,				[this](){this->client_simm->slotSendControlMessage(SType::LINKTEST_REQ);});
//		connect(this->ui->btnSend,			&QPushButton::clicked,				[this](){this->client_simm->slotSendMessage(SType::DATA_MESSAGE);});
		connect(this->client_simm,			&CClientSimm::sigAppendBinLog,		this,				&CEapSimm::slotAppendBinLog);
		connect(this->client_simm,			&CClientSimm::sigAppendSystemLog,	this,				&CEapSimm::slotAppendSystemLog);
		connect(this->client_simm,			&CClientSimm::sigAppendSmlLog,		this,				&CEapSimm::slotAppendSmlLog);
		connect(this->client_simm,			&CClientSimm::sigSetTcpStatus,		this,				&CEapSimm::slotSetTcpStatus);
		connect(this->client_simm,			&CClientSimm::sigSetHsmsStatus,		this,				&CEapSimm::slotSetHsmsStatus);
		return ;
	}
}

void CEapSimm::slotAppendBinLog(QString log)
{
	this->appendBinLog(log);
}

void CEapSimm::slotAppendSystemLog(QString log)
{
	this->appendSystemLog(log);
}
void CEapSimm::slotAppendHsmsInfo( QString log )
{
	this->appendHsmsInfo(log);
}

void CEapSimm::slotAppendSmlLog(QString log)
{
	this->appendSmlLog(log);
}

void CEapSimm::slotSetTcpStatus(bool bConn)
{
	this->setTcpStatus(bConn);
}

void CEapSimm::slotSetHsmsStatus(bool bSelect)
{
	this->setHsmsStatus(bSelect);
}

void CEapSimm::LoadIni()
{
	QString os_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg("simul.sml");
	QString initms = tr("%1/%2/%3.ini").arg(os_path).arg(CONFIG_DIR).arg(ATTMSUI_NAME);

	m_listSetions = UTIL::INI::getAllGroups(ini);
	//this->ui->listSml->addItems(ini_list);
	ui->tblSml->clear();
	ui->tblSml->setRowCount( m_listSetions.count() );

	QTableWidgetItem* initItem = nullptr;
	//HeaderView
	{
		ui->tblSml->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("SECTION")) );
		ui->tblSml->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("DESCRIPTION")) );
	}
	for( int r =0;r<m_listSetions.count();r++ )
	{
		initItem = new QTableWidgetItem;
		initItem->setText(m_listSetions[r]);
		initItem->setFlags(initItem->flags()^Qt::ItemIsEditable);
		initItem->setTextAlignment( Qt::AlignVCenter | Qt::AlignLeft );
		ui->tblSml->setItem( r, 0, initItem );

		QString desc = "";
		UTIL::INI::readSettings( ini, tr("%1/%2").arg(m_listSetions[r]).arg(INI_KEY_DESCRIPTION), desc );

		initItem = new QTableWidgetItem;
		initItem->setText(desc);
		initItem->setFlags(initItem->flags()^Qt::ItemIsEditable);
		initItem->setTextAlignment( Qt::AlignVCenter | Qt::AlignLeft );
		ui->tblSml->setItem( r, 1, initItem );
	}
	//ui->tblSml->resizeColumnsToContents();
	//if( m_listSetions.count() > 0 ) ui->tblSml->setCurrentCell( 0, 0 );

	device_id = 0;
	UTIL::INI::readSettings( initms, tr("%1/%2").arg(SECTION_COMM).arg(KEY_TMS_DEVID), device_id );
}


void CEapSimm::AddStreamFunction()
{
	if( m_pMsgDlg == nullptr ) return;

	m_pMsgDlg->SetSectionList( m_listSetions );
	m_pMsgDlg->Clear();
	m_pMsgDlg->setModal( true );
	m_pMsgDlg->show();
}

void CEapSimm::CopyStreamFunction()
{
	if( m_pMsgDlg == nullptr ) return;

	QString sml = ui->teSml->document()->toPlainText();
	QString desc = ui->leDesc->text();
	QString sect = ui->leSection->text();

	m_pMsgDlg->SetSectionList( m_listSetions );
	m_pMsgDlg->SetSection( sect );
	m_pMsgDlg->SetDescription( desc );
	m_pMsgDlg->SetSML( sml );
	m_pMsgDlg->setModal( true );
	m_pMsgDlg->show();
}

void CEapSimm::RemoveStreamFunction()
{
	QString os_path = "";
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}

	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, qApp->applicationName(), tr("Do you want to remove %1?").arg(m_sCurrentSF),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		QString ini = tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg("simul.sml");
		UTIL::INI::removeSection( ini, tr("%1").arg(m_sCurrentSF) );
		LoadIni();
	}
	else
	{

	}
}
