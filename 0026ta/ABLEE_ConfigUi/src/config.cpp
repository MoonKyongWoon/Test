#include "config.hpp"
#include "ui_config.h"
#include "QDebug"

CConfig::CConfig(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CConfig)
{
	LoadOsEnv();
	InitUi();
    LoadIni();
}

CConfig::~CConfig()
{
	delete ui;
}

bool CConfig::LoadOsEnv()
{
	char* env_os  = getenv(ATOS_DIR_ENV);
	QString os_path = "";
	QString run_path = "";

	if( env_os != NULL )
	{
		m_sOsDir = QString(env_os);
	}
	else
	{
		m_sOsDir = DIR_ATOS_PATH;
    }
	m_sSiteIni    = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(SITE_INI);
	m_sSystemIni  = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(SYSTEM_INI);
	m_sFtpIni     = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(FTP_INI);
    m_sChamberIni = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(CHAMBER_INI);
	m_sServerIni  = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(SERVER_INI);
	m_sTmsIni     = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(TMS_INI);
	return true;
}

void CConfig::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent( event );
}

void CConfig::closeEvent(QCloseEvent *event)
{
	if(event->type() == QEvent::Close)
	{
		QMessageBox::StandardButton resBtn =
		QMessageBox::question( this, qApp->applicationName(), tr("Do you want to quit program?"),
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
		if (resBtn == QMessageBox::Yes)
		{
			event->accept();
		}
		else
		{
			event->ignore();
		}
	}
}

void CConfig::setQComboBoxStyle() {
    ui->comboBoxBaudRateCH_A->setStyleSheet("QComboBox {combobox-popup: 0;}");
    ui->comboBoxBaudRateCH_A->setMaxVisibleItems(10);
    ui->comboBoxBaudRateCH_A->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBoxBaudRateCH_B->setStyleSheet("QComboBox {combobox-popup: 0;}");
    ui->comboBoxBaudRateCH_B->setMaxVisibleItems(10);
    ui->comboBoxBaudRateCH_B->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void CConfig::InitUi()
{
	ui->setupUi(this);
    setQComboBoxStyle();
	InitSlotTab();
	InitSystemTab();
	InitChamberTab();
	InitFtpTab();
	InitServerTab();
	InitTmsTab();
	this->setMinimumSize( 800, 600 );
}

bool CConfig::InitSlotTab()
{
	ui->tblSlotIpMask->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->tblSlotIpMask->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tblSlotIpMask->setSelectionMode(QAbstractItemView::NoSelection);
	ui->tblSlotIpMask->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->tblSlotIpMask->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tblSlotIpMask->setAutoScroll(false);
	ui->tblSlotIpMask->setSortingEnabled(false);
	ui->tblSlotIpMask->horizontalHeader()->setVisible(true);
	ui->tblSlotIpMask->horizontalHeader()->setStretchLastSection(true);

	connect(ui->cboCpuPerSlot , QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CConfig::SlotSlotCpuPerSlotChanged );

	int nMaxSlot = ( ui->cboMaxSlot->currentIndex() == 0 )?16:32;
	ui->tblSlotIpMask->setRowCount(nMaxSlot);
	ui->tblSlotIpMask->clearContents();

	QFont font;
	font.setPointSize(9);
	QTableWidgetItem* initItem = nullptr;
    QWidget* pWidget = nullptr;
	QHBoxLayout* hLayout = nullptr;
	for( int i=0;i<nMaxSlot;i++ )
	{
		//SLOT
		initItem = new QTableWidgetItem;
		QString slot = tr("SLOT%1").arg(i+1);
		initItem->setFont(font);
		initItem->setText(slot);
		initItem->setFlags(initItem->flags()^Qt::ItemIsEditable);
		initItem->setTextAlignment(Qt::AlignCenter);
		initItem->setBackground( DISABLE_ROW_QCOLOR );
		ui->tblSlotIpMask->setItem(i, SITE_HEAD_SLOT, initItem );

		//MASK
		pWidget = new QWidget;
		pWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		pWidget->setFont(font);
		pWidget->setStyleSheet( tr("QWidget{ background-color:%1;}").arg(DISABLE_ROW_QSS_COLOR)  );
		hLayout = new QHBoxLayout(pWidget);
		hLayout->setMargin(0);
		hLayout->setAlignment(Qt::AlignCenter);
		hLayout->setContentsMargins( 1, 1, 1, 1 );
		m_pCheckMask[i] = new QCheckBox( STR_DISABLE );
		m_pCheckMask[i]->setChecked( true );
		m_pCheckMask[i]->setFont(font);
		hLayout->addWidget( m_pCheckMask[i] );
		connect( m_pCheckMask[i], &QCheckBox::clicked  , this, &CConfig::SlotSlotMaskChecked );
		ui->tblSlotIpMask->setCellWidget( i, SITE_HEAD_MASK, pWidget);

		//CPU1_IP
		initItem = new QTableWidgetItem;
		initItem->setFont(font);
		initItem->setText("");
		initItem->setFlags(initItem->flags()|Qt::ItemIsEditable);
		initItem->setTextAlignment(Qt::AlignCenter);
		initItem->setBackground( DISABLE_ROW_QCOLOR );
		ui->tblSlotIpMask->setItem(i, SITE_HEAD_CPU1_IP, initItem );

		//CPU1_IP
		initItem = new QTableWidgetItem;
		initItem->setFont(font);
		initItem->setText("");
		initItem->setFlags(initItem->flags()|Qt::ItemIsEditable);
		initItem->setTextAlignment(Qt::AlignCenter);
		initItem->setBackground( DISABLE_ROW_QCOLOR );
		ui->tblSlotIpMask->setItem(i, SITE_HEAD_CPU2_IP, initItem );

		//STATUS
		initItem = new QTableWidgetItem;
		initItem->setFont(font);
		initItem->setText("");
		initItem->setFlags(initItem->flags()^Qt::ItemIsEditable);
		initItem->setTextAlignment(Qt::AlignCenter);
		initItem->setBackground( DISABLE_ROW_QCOLOR );
		ui->tblSlotIpMask->setItem(i, SITE_HEAD_STATUS, initItem );

		//UPDATE
		initItem = new QTableWidgetItem;
		initItem->setFont(font);
		initItem->setText("");
		initItem->setFlags(initItem->flags()^Qt::ItemIsEditable);
		initItem->setTextAlignment(Qt::AlignCenter);
		initItem->setBackground( DISABLE_ROW_QCOLOR );
		ui->tblSlotIpMask->setItem(i, SITE_HEAD_UPDATE, initItem );
	}
	connect( ui->btnSaveSlot  , &QPushButton::clicked, this, &CConfig::SlotSlotBtnSaveClicked   );
	connect( ui->btnReloadSlot, &QPushButton::clicked, this, &CConfig::SlotSlotBtnReloadClicked );
	return true;
}

bool CConfig::InitSystemTab()
{
    QSerialPortInfo info;
    int port_count = info.availablePorts().count();

    for (int i = 0; i < port_count; i += 1) {
        ui->comboBoxPortNameCH_A->addItem(info.availablePorts()[i].portName());
        ui->comboBoxPortNameCH_B->addItem(info.availablePorts()[i].portName());
    }

    int baud_rate_count = info.standardBaudRates().count();

    for (int i = 0; i < baud_rate_count; i += 1) {
        ui->comboBoxBaudRateCH_A->addItem(QString::number(info.standardBaudRates()[i]));
        ui->comboBoxBaudRateCH_B->addItem(QString::number(info.standardBaudRates()[i]));
    }

    connect( ui->btnSaveSystem, &QPushButton::clicked, this, &CConfig::SystemBtnSaveClicked );
    connect( ui->btnReloadSystem, &QPushButton::clicked, this, &CConfig::SystemBtnReloadClicked );
	return true;
}

bool CConfig::InitChamberTab()
{
    connect( ui->btnChamberSave, &QPushButton::clicked, this, &CConfig::ChamberBtnSaveClicked );
    connect( ui->btnChamberReload, &QPushButton::clicked, this, &CConfig::ChamberBtnReloadClicked );
	return true;
}

bool CConfig::InitFtpTab()
{
	QStringList list = {"--- AVAIL IP LIST ---"};
	foreach( QNetworkInterface interface, QNetworkInterface::allInterfaces() )
	{
		if ( !interface.flags().testFlag(QNetworkInterface::IsLoopBack) )
		{
			foreach (QNetworkAddressEntry entry, interface.addressEntries())
			{
				if ( interface.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains("192.168") )
				{
					list.append( entry.ip().toString() );
				}
			}
		}
	}
	ui->cboHostAvailIp->clear();
	ui->cboHostAvailIp->addItems( list );
	connect( ui->cboHostAvailIp , QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CConfig::SlotFtpCboHostListChanged );
	connect( ui->btnFtpReload   , &QPushButton::clicked  , this, &CConfig::SlotFtpBtnReloadClicked );
	connect( ui->btnFtpSave     , &QPushButton::clicked  , this, &CConfig::SlotFtpBtnSaveClicked   );

	return true;
}

bool CConfig::InitServerTab()
{
	connect(ui->btnServerReload, &QPushButton::clicked, this, &CConfig::SlotServerBtnReloadClicked);
	connect(ui->btnServerSave, &QPushButton::clicked, this, &CConfig::SlotServerBtnSaveClicked);

	return true;
}

bool CConfig::InitTmsTab()
{
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CConfig::LoadIni
/// \details Load atsite.ini / atsystem.ini / atftp.ini / attms.ini /atserver.ini
void CConfig::LoadIni()
{
	LoadSiteIni();
	LoadSystemIni();
	LoadFtpIni();
    LoadChamberIni();
	LoadServerIni();
	LoadTmsIni();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CConfig::LoadSiteIni
/// \return
/// \details
bool CConfig::LoadSiteIni()
{
	QString siteini ="";
	QString section_key = "";
	QString sValue = 0x0;
	int nMaxSlot = 0;
	int nCpuPerSlot = 0;
	int nValue = 0x0;

	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_MAX_SLOT);
	if( !UTIL::INI::readSettings( m_sSiteIni, section_key, nValue ) )
	{
		UTIL::INI::writeSettings( m_sSiteIni, section_key, DEFAULT_SITE_MAX_SLOT );
		ui->cboMaxSlot->setCurrentIndex( 0 );
		nValue = DEFAULT_SITE_MAX_SLOT;
	}
	else
	{
		if( nValue == 16 ) ui->cboMaxSlot->setCurrentIndex( 0 );
		else               ui->cboMaxSlot->setCurrentIndex( 1 );
	}

	//CPU PER SLOT
	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_CPU_PER_SLOT);
	if( !UTIL::INI::readSettings( m_sSiteIni, section_key, nValue ) )
	{
		UTIL::INI::writeSettings( m_sSiteIni, section_key, DEFAULT_SITE_CPU_PER_SLOT );
		ui->cboCpuPerSlot->setCurrentIndex( 0 );
		nValue = 1;
	}
	if( nValue == 1 ) ui->cboCpuPerSlot->setCurrentIndex( 0 );
	else              ui->cboCpuPerSlot->setCurrentIndex( 1 );
	nCpuPerSlot = nValue;

	//PGM PER SLOT
	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_PGM_PER_SLOT);
	if( !UTIL::INI::readSettings( m_sSiteIni, section_key, nValue ) )
	{
		UTIL::INI::writeSettings( m_sSiteIni, section_key, DEFAULT_SITE_PGM_PER_SLOT );
		ui->cboPgmPerSlot->setCurrentIndex( 1 );
		nValue = 2;
	}
	if( nValue == 1 ) ui->cboPgmPerSlot->setCurrentIndex( 0 );
	else              ui->cboPgmPerSlot->setCurrentIndex( 1 );

	nMaxSlot = ( ui->cboMaxSlot->currentIndex() == 0 )?16:32;
	QTableWidgetItem* initItem = nullptr;
	for( int i=0;i<nMaxSlot;i++ )
	{
		//MASK
		bool bSlotMask = false;
		section_key = tr("%1%2/%3").arg(SECTION_SITE_SLOT).arg(i+1).arg(KEY_SITE_SLOT_MASK);
		if( !UTIL::INI::readSettings( m_sSiteIni, section_key, nValue ) )
		{
			UTIL::INI::writeSettings( m_sSiteIni, section_key, 0x1 );
			nValue = 0x1;
		}
		if( nValue > 0x0 )
		{
			m_pCheckMask[i]->setChecked( true );
			m_pCheckMask[i]->setText( STR_DISABLE );
			bSlotMask = true;
		}
		else
		{
			m_pCheckMask[i]->setChecked( false );
			m_pCheckMask[i]->setText( STR_ENABLE );
		}
		//CPU1
		section_key = tr("%1%2/%3").arg(SECTION_SITE_SLOT).arg(i+1).arg(KEY_SITE_CPU1_IP);
		if( !UTIL::INI::readSettings( m_sSiteIni, section_key, sValue ) )
		{
			QString defaultIp = tr("%1.%2").arg(KEY_SITE_CPU_IP_START).arg( i+1 );
			UTIL::INI::writeSettings( m_sSiteIni, section_key, defaultIp );
			sValue = defaultIp;
		}
		initItem = ui->tblSlotIpMask->item( i, SITE_HEAD_CPU1_IP );
		initItem->setText( sValue );

		//CPU2
		section_key = tr("%1%2/%3").arg(SECTION_SITE_SLOT).arg(i+1).arg(KEY_SITE_CPU2_IP);
		if( !UTIL::INI::readSettings( m_sSiteIni, section_key, sValue ) )
		{
			QString defaultIp = tr("%1.%2").arg(KEY_SITE_CPU_IP_START).arg( i+1+nMaxSlot );
			UTIL::INI::writeSettings( m_sSiteIni, section_key, defaultIp );
			sValue = defaultIp;
		}
		initItem = ui->tblSlotIpMask->item( i, SITE_HEAD_CPU2_IP );
		initItem->setText( sValue );

		//STATUS
		initItem = ui->tblSlotIpMask->item( i, SITE_HEAD_STATUS );
		initItem->setText(bSlotMask?STR_DISABLE_STAT:STR_ENABLE_STAT);

		//UPDATE
		initItem = ui->tblSlotIpMask->item( i, SITE_HEAD_UPDATE );
		initItem->setText("");

		if( bSlotMask )
		{
			ui->tblSlotIpMask->cellWidget( i, SITE_HEAD_MASK )->setStyleSheet(tr("QWidget{ background-color:%1;}").arg(DISABLE_ROW_QSS_COLOR) );
			ui->tblSlotIpMask->item( i, SITE_HEAD_SLOT    )->setBackground( DISABLE_ROW_QCOLOR );
			ui->tblSlotIpMask->item( i, SITE_HEAD_CPU1_IP )->setBackground( DISABLE_ROW_QCOLOR );
			if( nCpuPerSlot == 1 )
				ui->tblSlotIpMask->item( i, SITE_HEAD_CPU2_IP )->setBackground( DISABLE_CPU_QCOLOR );
			else
				ui->tblSlotIpMask->item( i, SITE_HEAD_CPU2_IP )->setBackground( DISABLE_ROW_QCOLOR );
			ui->tblSlotIpMask->item( i, SITE_HEAD_STATUS  )->setBackground( DISABLE_ROW_QCOLOR );
			ui->tblSlotIpMask->item( i, SITE_HEAD_UPDATE  )->setBackground( DISABLE_ROW_QCOLOR );
		}
		else
		{
			ui->tblSlotIpMask->cellWidget( i, SITE_HEAD_MASK )->setStyleSheet(tr("QWidget{ background-color:%1;}").arg(ENABLE_ROW_QSS_COLOR) );
			ui->tblSlotIpMask->item( i, SITE_HEAD_SLOT    )->setBackground( ENABLE_ROW_QCOLOR );
			ui->tblSlotIpMask->item( i, SITE_HEAD_CPU1_IP )->setBackground( ENABLE_ROW_QCOLOR );
			if( nCpuPerSlot == 1 )
				ui->tblSlotIpMask->item( i, SITE_HEAD_CPU2_IP )->setBackground( DISABLE_CPU_QCOLOR );
			else
				ui->tblSlotIpMask->item( i, SITE_HEAD_CPU2_IP )->setBackground( ENABLE_ROW_QCOLOR );
			ui->tblSlotIpMask->item( i, SITE_HEAD_STATUS  )->setBackground( ENABLE_ROW_QCOLOR );
			ui->tblSlotIpMask->item( i, SITE_HEAD_UPDATE  )->setBackground( ENABLE_ROW_QCOLOR );
		}
	}

	return true;
}

void CConfig::SlotSlotMaskChecked()
{
	int nMaxSlot = ( ui->cboMaxSlot->currentIndex() == 0 )?16:32;
	QCheckBox* sender = qobject_cast<QCheckBox*>(QObject::sender());
	for( int slot=0;slot<nMaxSlot;slot++ )
	{
		if( sender == m_pCheckMask[slot] )
		{
			if( m_pCheckMask[slot]->isChecked() )
			{
				m_pCheckMask[slot]->setText( STR_DISABLE );
				ui->tblSlotIpMask->item( slot, SITE_HEAD_UPDATE )->setText("SLOT MASK");
			}
			else
			{
				m_pCheckMask[slot]->setText( STR_ENABLE );
				ui->tblSlotIpMask->item( slot, SITE_HEAD_UPDATE )->setText("SLOT USE");
			}
		}
	}
}

void CConfig::SlotSlotCpuPerSlotChanged(int index)
{
	if( index == 0 )
		ui->cboPgmPerSlot->setCurrentIndex( 1 );
	else
		ui->cboPgmPerSlot->setCurrentIndex( 0 );
}


void CConfig::SlotSlotBtnReloadClicked()
{
	LoadSiteIni();
}

void CConfig::SlotSlotBtnSaveClicked()
{
	QString siteini ="";
	QString section_key = "";
	QString sValue = 0x0;

	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_MAX_SLOT);
	int MaxSlot = (ui->cboMaxSlot->currentIndex() == 0 )?16:32;
	UTIL::INI::writeSettings( m_sSiteIni, section_key, MaxSlot );

	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_CPU_PER_SLOT);
	int CpuPerSlot = (ui->cboCpuPerSlot->currentIndex() == 0 )?1:2;
	UTIL::INI::writeSettings( m_sSiteIni, section_key, CpuPerSlot );

	section_key = tr("%1/%2").arg(SECTION_SITE_COMMON).arg(KEY_SITE_PGM_PER_SLOT);
	int PgmPerCpu = (ui->cboPgmPerSlot->currentIndex() == 0 )?1:2;
	UTIL::INI::writeSettings( m_sSiteIni, section_key, PgmPerCpu );

	for( int i=0;i<MaxSlot;i++ )
	{
		//MASK
		section_key = tr("%1%2/%3").arg(SECTION_SITE_SLOT).arg(i+1).arg(KEY_SITE_SLOT_MASK);
		int nMask = m_pCheckMask[i]->isChecked()?0x1:0x0;
		UTIL::INI::writeSettings( m_sSiteIni, section_key, nMask );

		//CPU1_IP
		section_key = tr("%1%2/%3").arg(SECTION_SITE_SLOT).arg(i+1).arg(KEY_SITE_CPU1_IP);
		QString ip1 = ui->tblSlotIpMask->item(i,SITE_HEAD_CPU1_IP)->text().trimmed();
		if( !UTIL::VALIDATION::IsValidIpV4( ip1.toStdString() ) )
		{
			QMessageBox::critical( this, tr("SLOT%1 CPU1_IP").arg(i+1), QObject::tr("%1 is not valid ipv4").arg(ip1), QMessageBox::Ok );
			ui->tblSlotIpMask->item(i,SITE_HEAD_CPU1_IP)->setBackground( ERROR_ROW_QCOLOR );
			return;
		}
		UTIL::INI::writeSettings( m_sSiteIni, section_key, ip1 );

		//CPU2_IP
		section_key = tr("%1%2/%3").arg(SECTION_SITE_SLOT).arg(i+1).arg(KEY_SITE_CPU2_IP);
		QString ip2 = ui->tblSlotIpMask->item(i,SITE_HEAD_CPU2_IP)->text().trimmed();
		if( CpuPerSlot == 2 )
		{
			if( !UTIL::VALIDATION::IsValidIpV4( ip2.toStdString() ) )
			{
				QMessageBox::critical( this, tr("SLOT%1 CPU1_IP").arg(i+1), QObject::tr("%1 is not valid ipv4").arg(ip1), QMessageBox::Ok );
				ui->tblSlotIpMask->item(i,SITE_HEAD_CPU1_IP)->setBackground( ERROR_ROW_QCOLOR );
				return;
			}
		}
		UTIL::INI::writeSettings( m_sSiteIni, section_key, ip2 );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CConfig::SaveSiteIni
/// \return
///
bool CConfig::SaveSiteIni()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CConfig::LoadSystemIni
/// \return
///
bool CConfig::LoadSystemIni()
{
    QString section_key = "";
    QString s_value = "";
    int n_value = 0;
    bool b_value = true;

    // SMPS COMM

    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_PORT_NAME).arg("A");
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, s_value ) && ui->comboBoxPortNameCH_A->findText(s_value) != -1 )
        ui->comboBoxPortNameCH_A->setCurrentIndex(ui->comboBoxPortNameCH_A->findText(s_value));
//    else
//        ui->comboBoxPortNameCH_A->setCurrentIndex(0);

    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_BAUD_RATE).arg("A");
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, s_value ) && ui->comboBoxBaudRateCH_A->findText(s_value) != -1 )
        ui->comboBoxBaudRateCH_A->setCurrentIndex(ui->comboBoxBaudRateCH_A->findText(s_value));
//    else
//        ui->comboBoxBaudRateCH_A->setCurrentIndex(0);

    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_PORT_NAME).arg("B");
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, s_value ) && ui->comboBoxPortNameCH_B->findText(s_value) != -1 )
        ui->comboBoxPortNameCH_B->setCurrentIndex(ui->comboBoxPortNameCH_B->findText(s_value));
//    else
//        ui->comboBoxPortNameCH_B->setCurrentIndex(0);

    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_BAUD_RATE).arg("B");
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, s_value ) && ui->comboBoxBaudRateCH_B->findText(s_value) != -1 )
        ui->comboBoxBaudRateCH_B->setCurrentIndex(ui->comboBoxBaudRateCH_B->findText(s_value));
//    else
//        ui->comboBoxBaudRateCH_B->setCurrentIndex(0);

    // DEV PGM
    section_key = tr("%1/%2").arg(SECTION_DEV_PGM).arg(KEY_FILE_EXTENSION);
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, s_value ) )
        ui->lineEditFileExtension->setText(s_value);
    else
        ui->lineEditFileExtension->setText(".dev");

    // OPEN TIME
    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_OPEN_TIME_ENABLE);
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, b_value ) )
        ui->systemOpenTime->setChecked(b_value);
    else
        ui->systemOpenTime->setChecked(true);

    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_FIRST);
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, n_value ) )
        ui->spinBoxOpenTimeFirst->setValue(n_value);
    else
        ui->spinBoxOpenTimeFirst->setValue(0);

    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_SECOND);
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, n_value ) )
        ui->spinBoxOpenTimeSecond->setValue(n_value);
    else
        ui->spinBoxOpenTimeSecond->setValue(0);

    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_FAST_OPEN_TIME);
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, n_value ) )
        ui->spinBoxFastOpenTime->setValue(n_value);
    else
        ui->spinBoxFastOpenTime->setValue(0);

    // FAIL RATE SETUP
    int bin_count = 9;

    QCheckBox* bin_check_arr[bin_count];
    bin_check_arr[0] = ui->checkBoxBin1;
    bin_check_arr[1] = ui->checkBoxBin2;
    bin_check_arr[2] = ui->checkBoxBin3;
    bin_check_arr[3] = ui->checkBoxBin4;
    bin_check_arr[4] = ui->checkBoxBin5;
    bin_check_arr[5] = ui->checkBoxBin6;
    bin_check_arr[6] = ui->checkBoxBin7;
    bin_check_arr[7] = ui->checkBoxBin8;
    bin_check_arr[8] = ui->checkBoxBin9;

    QSpinBox* bin_value_arr[bin_count];
    bin_value_arr[0] = ui->spinBoxBin1;
    bin_value_arr[1] = ui->spinBoxBin2;
    bin_value_arr[2] = ui->spinBoxBin3;
    bin_value_arr[3] = ui->spinBoxBin4;
    bin_value_arr[4] = ui->spinBoxBin5;
    bin_value_arr[5] = ui->spinBoxBin6;
    bin_value_arr[6] = ui->spinBoxBin7;
    bin_value_arr[7] = ui->spinBoxBin8;
    bin_value_arr[8] = ui->spinBoxBin9;

    section_key = tr("%1/%2").arg(SECTION_FAIL_RATE).arg(KEY_FAIL_RATE_ENABLE);
    if ( UTIL::INI::readSettings( m_sSystemIni, section_key, b_value ) )
        ui->systemFailRate->setChecked(b_value);
    else
        ui->systemFailRate->setChecked(true);

    for (int i = 0; i < bin_count; i += 1)
    {
        section_key = tr("%1/BIN_%2_%3").arg(SECTION_FAIL_RATE).arg(i + 1).arg(KEY_BIN_ENABLE);
        if ( UTIL::INI::readSettings( m_sSystemIni, section_key, b_value ) )
            bin_check_arr[i]->setChecked(b_value);
        else
            bin_check_arr[i]->setChecked(false);

        section_key = tr("%1/BIN_%2_%3").arg(SECTION_FAIL_RATE).arg(i + 1).arg(KEY_BIN_FAIL_RATE);
        if ( UTIL::INI::readSettings( m_sSystemIni, section_key, n_value ) )
            bin_value_arr[i]->setValue(n_value);
        else
            bin_value_arr[i]->setValue(0);
    }
	//2023.08.04 - KWMOON
	section_key = tr("%1/%2").arg(SECTION_SLOT_OPTION).arg(KEY_EMPTY_BIB_TEST);
	if ( UTIL::INI::readSettings( m_sSystemIni, section_key, n_value ) )
		ui->cboNoBibTest->setCurrentIndex( n_value );
	else
	{
		UTIL::INI::writeSettings( m_sSystemIni, section_key, 0x0 );
		ui->cboNoBibTest->setCurrentIndex(0);
	}

	//2024.06.21 - yseom
	section_key = tr("%1/%2").arg(SECTION_BIB_SIZE).arg(KEY_BIB_ROWS);
	if ( UTIL::INI::readSettings( m_sSystemIni, section_key, n_value ) )
	{
		ui->spinBoxRows->setValue(n_value);
	}
	else
	{
		ui->spinBoxRows->setValue(0);
	}

	section_key = tr("%1/%2").arg(SECTION_BIB_SIZE).arg(KEY_BIB_COLS);
	if ( UTIL::INI::readSettings( m_sSystemIni, section_key, n_value ) )
	{
		ui->spinBoxColumns->setValue(n_value);
	}
	else
	{
		ui->spinBoxColumns->setValue(0);
	}

	return true;
}

void CConfig::SystemBtnSaveClicked()
{
    QString section_key="";

    // SMP COMM SETUP
    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_PORT_NAME).arg("A");
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->comboBoxPortNameCH_A->currentText() );
    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_BAUD_RATE).arg("A");
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->comboBoxBaudRateCH_A->currentText() );
    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_PORT_NAME).arg("B");
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->comboBoxPortNameCH_B->currentText() );
    section_key = tr("%1/%2_CH%3").arg(SECTION_SMPS_COMM).arg(KEY_SMPS_BAUD_RATE).arg("B");
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->comboBoxBaudRateCH_B->currentText() );

    // DEV PGM SETUP
    section_key = tr("%1/%2").arg(SECTION_DEV_PGM).arg(KEY_FILE_EXTENSION);
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->lineEditFileExtension->text() );

    // OPEN TIME SETUP
    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_OPEN_TIME_ENABLE);
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->systemOpenTime->isChecked() );
    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_FIRST);
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->spinBoxOpenTimeFirst->text() );
    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_SECOND);
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->spinBoxOpenTimeSecond->text() );
    section_key = tr("%1/%2").arg(SECTION_OPEN_TIME).arg(KEY_FAST_OPEN_TIME);
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->spinBoxFastOpenTime->text() );

    // FAIL RATE SETUP
    int bin_count = 9;

    QCheckBox* bin_check_arr[bin_count];
    bin_check_arr[0] = ui->checkBoxBin1;
    bin_check_arr[1] = ui->checkBoxBin2;
    bin_check_arr[2] = ui->checkBoxBin3;
    bin_check_arr[3] = ui->checkBoxBin4;
    bin_check_arr[4] = ui->checkBoxBin5;
    bin_check_arr[5] = ui->checkBoxBin6;
    bin_check_arr[6] = ui->checkBoxBin7;
    bin_check_arr[7] = ui->checkBoxBin8;
    bin_check_arr[8] = ui->checkBoxBin9;

    QSpinBox* bin_value_arr[bin_count];
    bin_value_arr[0] = ui->spinBoxBin1;
    bin_value_arr[1] = ui->spinBoxBin2;
    bin_value_arr[2] = ui->spinBoxBin3;
    bin_value_arr[3] = ui->spinBoxBin4;
    bin_value_arr[4] = ui->spinBoxBin5;
    bin_value_arr[5] = ui->spinBoxBin6;
    bin_value_arr[6] = ui->spinBoxBin7;
    bin_value_arr[7] = ui->spinBoxBin8;
    bin_value_arr[8] = ui->spinBoxBin9;

    section_key = tr("%1/%2").arg(SECTION_FAIL_RATE).arg(KEY_FAIL_RATE_ENABLE);
    UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->systemOpenTime->isChecked());

    for (int i = 0; i < bin_count; i += 1)
    {
        section_key = tr("%1/BIN_%2_%3").arg(SECTION_FAIL_RATE).arg(i + 1).arg(KEY_BIN_ENABLE);
        UTIL::INI::writeSettings( m_sSystemIni, section_key, bin_check_arr[i]->isChecked());
        section_key = tr("%1/BIN_%2_%3").arg(SECTION_FAIL_RATE).arg(i + 1).arg(KEY_BIN_FAIL_RATE);
        UTIL::INI::writeSettings( m_sSystemIni, section_key, bin_value_arr[i]->text() );
    }
	//2023.08.04 - KWMOON
	int nIndex = ui->cboNoBibTest->currentIndex();
	section_key = tr("%1/%2").arg(SECTION_SLOT_OPTION).arg(KEY_EMPTY_BIB_TEST);
	UTIL::INI::writeSettings( m_sSystemIni, section_key, nIndex );

	//2024.06.21 - yseom
	section_key = tr("%1/%2").arg(SECTION_BIB_SIZE).arg(KEY_BIB_ROWS);
	UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->spinBoxRows->value() );
	section_key = tr("%1/%2").arg(SECTION_BIB_SIZE).arg(KEY_BIB_COLS);
	UTIL::INI::writeSettings( m_sSystemIni, section_key, ui->spinBoxColumns->value() );
}

void CConfig::SystemBtnReloadClicked()
{
    LoadSystemIni();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CConfig::LoadFtpIni
/// \return
///
bool CConfig::LoadFtpIni()
{
	QString siteini ="";
	QString section_key = "";
	QString sValue = 0x0;

	//FTP IP
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_IP);
	if( !UTIL::INI::readSettings( m_sFtpIni, section_key, sValue ) )
	{
		sValue = DEFAULT_EWS_FTP_IP;
		UTIL::INI::writeSettings( m_sFtpIni, section_key, sValue );
	}
	ui->leHostFtpIp->setText( sValue );

	//FTP PORT
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PORT);
	if( !UTIL::INI::readSettings( m_sFtpIni, section_key, sValue ) )
	{
		sValue = DEFAULT_EWS_FTP_PORT;
		UTIL::INI::writeSettings( m_sFtpIni, section_key, sValue );
	}
	ui->leHostFtpPort->setText( sValue );

	//FTP USER
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_USER);
	if( !UTIL::INI::readSettings( m_sFtpIni, section_key, sValue ) )
	{
		sValue = DEFAULT_EWS_FTP_USER;
		UTIL::INI::writeSettings( m_sFtpIni, section_key, sValue );
	}
	ui->leHostFtpUser->setText( sValue );

	//FTP PASSWD
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PASSWD);
	if( !UTIL::INI::readSettings( m_sFtpIni, section_key, sValue ) )
	{
		sValue = DEFAULT_EWS_FTP_PASSWD;
		UTIL::INI::writeSettings( m_sFtpIni, section_key, sValue );
	}
	ui->leHostFtpPasswd->setText( sValue );

	return true;
}

void CConfig::SlotFtpCboHostListChanged(int index)
{
	if( index == 0 ) return;
	ui->leHostFtpIp->setText( ui->cboHostAvailIp->currentText().trimmed() );
}


void CConfig::SlotFtpBtnReloadClicked()
{
	LoadFtpIni();
}

void CConfig::SlotFtpBtnSaveClicked()
{
	QString siteini ="";
	QString section_key = "";

	QString ip     = ui->leHostFtpIp->text();
	QString port   = ui->leHostFtpPort->text();
	QString user   = ui->leHostFtpUser->text();
	QString passwd = ui->leHostFtpPasswd->text();

	//FTP IP
	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_IP);
	UTIL::INI::writeSettings( m_sFtpIni, section_key, ip );

	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PORT);
	UTIL::INI::writeSettings( m_sFtpIni, section_key, port );

	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_USER);
	UTIL::INI::writeSettings( m_sFtpIni, section_key, user );

	section_key = tr("%1/%2").arg(SECTION_EWS_FTP).arg(KEY_EWS_FTP_PASSWD);
	UTIL::INI::writeSettings( m_sFtpIni, section_key, passwd );
}

bool CConfig::LoadChamberIni() {
    QString section_key = "";
    QString s_value = "";
    int n_value = 0;
    double d_value = 0.0;
    bool b_value = true;

	//CHAMBER USE
	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_USE);
	if ( UTIL::INI::readSettings( m_sChamberIni, section_key, n_value ) )
		ui->chkChamberUse->setChecked( n_value?true:false );
	else
		ui->chkChamberUse->setChecked( DEFAULT_CHMB_USE );

    // TCP SETUP
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_IP);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, s_value ) && UTIL::VALIDATION::IsValidIpV4(s_value.toStdString()))
        ui->lePlcIp->setText(s_value);
    else
        ui->lePlcIp->setText(DEFAULT_CHMB_IP);

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_PORT);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, s_value ) )
        ui->lePlcPort->setText(s_value);
    else
        ui->lePlcPort->setText(QString::number(DEFAULT_CHMB_PORT));

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_WATCHDOG_ENABLE);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, b_value ) )
        ui->cbWatchDogEnable->setChecked(b_value);
    else
        ui->cbWatchDogEnable->setChecked(false);

    // CHAMBER SETUP
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_WATCHDOG_PERIOD);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, n_value ) )
        ui->spinWatchDogPeriod->setValue(n_value);
    else
        ui->spinWatchDogPeriod->setValue(DEFAULT_CHMB_WATCHDOG_PERIOD);

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_AMB_SV);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, d_value ) )
        ui->spinAMBSV->setValue(d_value);
    else
        ui->spinAMBSV->setValue(DEFAULT_CHMB_AMB_SV);

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_LOW);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, d_value ) )
        ui->spinTempLimitLow->setValue(d_value);
    else
        ui->spinTempLimitLow->setValue(DEFAULT_CHMB_TEMP_LIMIT_LOW);

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_HIGH);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, d_value ) )
        ui->spinTempLimitHigh->setValue(d_value);
    else
        ui->spinTempLimitHigh->setValue(DEFAULT_CHMB_TEMP_LIMIT_HIGH);

    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_SOAK_TIME);
    if ( UTIL::INI::readSettings( m_sChamberIni, section_key, n_value ) )
	{
        ui->spinSoakTime->setValue(n_value);
	}
    else
	{
        ui->spinSoakTime->setValue(DEFAULT_CHMB_SOAK_TIME);
	}

	//2023.07.09 - KWMOON
	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_KEEP_TIME);
	if ( UTIL::INI::readSettings( m_sChamberIni, section_key, n_value ) )
	{
		ui->spinKeepTime->setValue(n_value);
	}
	else
	{
		ui->spinKeepTime->setValue(DEFAULT_CHMB_KEEP_TIME);
	}
	return true;
}

void CConfig::ChamberBtnSaveClicked()
{
    QString section_key="";

	//CHAMBER USE
	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_USE);
	UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->chkChamberUse->isChecked()?0x1:0x0 );

    // TCP SETUP
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_IP);
    if ( UTIL::VALIDATION::IsValidIpV4(ui->lePlcIp->text().toStdString()) )
        UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->lePlcIp->text() );
    else
        QMessageBox::critical( this, tr("PLC TCP/IP IP ADDR"), QObject::tr("%1 is not valid ipv4").arg(ui->lePlcIp->text()), QMessageBox::Ok );
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_PORT);
    UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->lePlcPort->text() );
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_WATCHDOG_ENABLE);
    UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->cbWatchDogEnable->isChecked() );

    // CHAMBER SETUP
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_WATCHDOG_PERIOD);
    UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->spinWatchDogPeriod->text() );
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_AMB_SV);
    UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->spinAMBSV->text() );
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_LOW);
    UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->spinTempLimitLow->text() );
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_TEMP_LIMIT_HIGH);
    UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->spinTempLimitHigh->text() );
    section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_SOAK_TIME);
    UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->spinSoakTime->text() );
	section_key = tr("%1/%2").arg(SECTION_CHMB_COMMON).arg(KEY_CHMB_KEEP_TIME);
	UTIL::INI::writeSettings( m_sChamberIni, section_key, ui->spinKeepTime->text() );
}

void CConfig::ChamberBtnReloadClicked()
{
    LoadChamberIni();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CConfig::LoadServerIni
/// \return
///
bool CConfig::LoadServerIni()
{
	QString section_key = "";
	QString s_value = "";


	//SERVER SECTION
	//FLS SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leFlsServer->setText(s_value);
	}
	else {
		ui->leFlsServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leFlsUser->setText(s_value);
	}
	else {
		ui->leFlsUser->setText(DEFAULT_FLS_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leFlsPassword->setText(s_value);
	}
	else {
		ui->leFlsPassword->setText(DEFAULT_FLS_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leFlsDirectory->setText(s_value);
	}
	else {
		ui->leFlsDirectory->setText(DEFAULT_FLS_DIR);
	}

	//TDB SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbServer->setText(s_value);
	}
	else {
		ui->leTdbServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbUser->setText(s_value);
	}
	else {
		ui->leTdbUser->setText(DEFAULT_TDB_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbPassword->setText(s_value);
	}
	else {
		ui->leTdbPassword->setText(DEFAULT_TDB_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbDirectory->setText(s_value);
	}
	else {
		ui->leTdbDirectory->setText(DEFAULT_TDB_DIR);
	}

	//TDBCUM SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbcumServer->setText(s_value);
	}
	else {
		ui->leTdbcumServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbcumUser->setText(s_value);
	}
	else {
		ui->leTdbcumUser->setText(DEFAULT_TDBCUM_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbcumPassword->setText(s_value);
	}
	else {
		ui->leTdbcumPassword->setText(DEFAULT_TDBCUM_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leTdbcumDirectory->setText(s_value);
	}
	else {
		ui->leTdbcumDirectory->setText(DEFAULT_TDBCUM_DIR);
	}

	//ITEMCUM SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leItemcumServer->setText(s_value);
	}
	else {
		ui->leItemcumServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leItemcumUser->setText(s_value);
	}
	else {
		ui->leItemcumUser->setText(DEFAULT_ITEMCUM_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leItemcumPassword->setText(s_value);
	}
	else {
		ui->leItemcumPassword->setText(DEFAULT_ITEMCUM_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leItemcumDirectory->setText(s_value);
	}
	else {
		ui->leItemcumDirectory->setText(DEFAULT_ITEMCUM_DIR);
	}

	//MRD SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leMrdServer->setText(s_value);
	}
	else {
		ui->leMrdServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leMrdUser->setText(s_value);
	}
	else {
		ui->leMrdUser->setText(DEFAULT_MRD_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leMrdPassword->setText(s_value);
	}
	else {
		ui->leMrdPassword->setText(DEFAULT_MRD_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leMrdDirectory->setText(s_value);
	}
	else {
		ui->leMrdDirectory->setText(DEFAULT_MRD_DIR);
	}

	//EQPR SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leEqprServer->setText(s_value);
	}
	else {
		ui->leEqprServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leEqprUser->setText(s_value);
	}
	else {
		ui->leEqprUser->setText(DEFAULT_EQPR_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leEqprPassword->setText(s_value);
	}
	else {
		ui->leEqprPassword->setText(DEFAULT_EQPR_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leEqprDirectory->setText(s_value);
	}
	else {
		ui->leEqprDirectory->setText(DEFAULT_EQPR_DIR);
	}

	//CHDR SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leChdrServer->setText(s_value);
	}
	else {
		ui->leChdrServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leChdrUser->setText(s_value);
	}
	else {
		ui->leChdrUser->setText(DEFAULT_CHDR_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leChdrPassword->setText(s_value);
	}
	else {
		ui->leChdrPassword->setText(DEFAULT_CHDR_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leChdrDirectory->setText(s_value);
	}
	else {
		ui->leChdrDirectory->setText(DEFAULT_CHDR_DIR);
	}

	//DATA SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_SERVER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leDataServer->setText(s_value);
	}
	else {
		ui->leDataServer->setText(DEFAULT_SERVER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_USER);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leDataUser->setText(s_value);
	}
	else {
		ui->leDataUser->setText(DEFAULT_DATA_USER);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_PASSWD);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leDataPassword->setText(s_value);
	}
	else {
		ui->leDataPassword->setText(DEFAULT_DATA_PASSWD);
	}

	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_DIR);
	if(UTIL::INI::readSettings(m_sServerIni, section_key, s_value)) {
		ui->leDataDirectory->setText(s_value);
	}
	else {
		ui->leDataDirectory->setText(DEFAULT_DATA_DIR);
	}

	return true;
}

void CConfig::SlotServerBtnReloadClicked() {
	LoadServerIni();
}

bool CConfig::SaveServerIni() {
	QString section_key="";

	//SERVER SECTION
	//FLS SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leFlsServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leFlsUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leFlsPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_FLS_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leFlsDirectory->text());

	//TDB SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDB_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbDirectory->text());

	//TDBCUM SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbcumServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbcumUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbcumPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_TDBCUM_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leTdbcumDirectory->text());

	//ITEMCUM SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leItemcumServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leItemcumUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leItemcumPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_ITEMCUM_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leItemcumDirectory->text());

	//MRD SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leMrdServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leMrdUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leMrdPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_MRD_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leMrdDirectory->text());

	//EQPR SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leEqprServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leEqprUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leEqprPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_EQPR_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leEqprDirectory->text());

	//CHDR SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leChdrServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leChdrUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leChdrPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_CHDR_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leChdrDirectory->text());

	//DATA SERVER
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_SERVER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leDataServer->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_USER);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leDataUser->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_PASSWD);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leDataPassword->text());
	section_key = tr("%1/%2").arg(SECTION_SERVER).arg(KEY_DATA_DIR);
	UTIL::INI::writeSettings(m_sServerIni, section_key, ui->leDataDirectory->text());


	return true;
}

void CConfig::SlotServerBtnSaveClicked() {
	SaveServerIni();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CConfig::LoadTmsIni
/// \return
///
bool CConfig::LoadTmsIni()
{
	return true;
}

/////////
////POPUP
/////////

void CConfig::showPopup(const QString& message) {
    qDebug() << message;
    Popup* popup = new Popup();
    popup->show();
}

