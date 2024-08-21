#include "masstab.hpp"
#include "ui_masstab.h"

CMassTab::CMassTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMassTab)
{
	InitVariables();
    InitUi();
	InitSigSlot();
	attachSharedMemory();
}

CMassTab::~CMassTab()
{
	StopTimer();
    delete ui;
}

void CMassTab::InitVariables()
{
	m_pTmrTest  = new QTimer(this);
	m_pTmrPwr   = new QTimer(this);
    m_pTmrPfm   = new QTimer(this);

	m_nCurrentSlot = 0;
	m_pTestDataMgr = nullptr;
	memset( &m_stUiTestInfo , 0x0, sizeof(stUiTestInfo) );
	memset( &m_stSlotLive   , 0x0, sizeof(stSlotLive)   );
	memset( &m_stPgmLive    , 0x0, sizeof(stPgmLive)    );
	memset( m_stSlotTestInfo, 0x0, sizeof(stSlotTestInfo)*MAX_SLOT );

}

void CMassTab::InitPlotView() {
}

void CMassTab::InitUi()
{
    ui->setupUi(this);
    InitLotInfoView();
    InitPpsView();
	InitPlotView();
/*
	//TEST INFO AND PS
	{
		QSettings settings( CONFIG_INI_KEY, ui->splitterInfonPs->objectName() );
		QByteArray saved_geometry = settings.value("geometry").toByteArray();
		if ( !saved_geometry.isEmpty() )
			ui->splitterInfonPs->restoreGeometry( saved_geometry );
		QByteArray saved_state = settings.value("windowState").toByteArray();
		if ( !saved_state.isEmpty() )
			ui->splitterInfonPs->restoreState(saved_state);
	}
	//TESTINFO+PS AND CHAMBER
	{
		QSettings settings( CONFIG_INI_KEY, ui->splitterPsnChmb->objectName() );
		QByteArray saved_geometry = settings.value("geometry").toByteArray();
		if ( !saved_geometry.isEmpty() )
			ui->splitterPsnChmb->restoreGeometry( saved_geometry );
		QByteArray saved_state = settings.value("windowState").toByteArray();
		if ( !saved_state.isEmpty() )
			ui->splitterPsnChmb->restoreState(saved_state);
	}
	ui->tbl->setCurrentCell(0, EN_SI_BIBID );
*/
}

void CMassTab::StartTestTimer()
{
	m_pTmrTest->start( MASS_TIMER_TEST );
}

void CMassTab::StartPwrTimer()
{
	m_pTmrPwr->start( MASS_TIMER_PWR );
}

void CMassTab::StartPfmTimer()
{
    if (!m_pTmrPfm->isActive()) {
        m_pTmrPfm->start( MASS_TIMER_PFM );
    }
}

void CMassTab::StopTimer()
{
	if( m_pTmrTest->isActive() ) m_pTmrTest->stop();
	if( m_pTmrPwr->isActive()  ) m_pTmrPwr->stop();
    if( m_pTmrPfm->isActive()  ) m_pTmrPfm->stop();
}

void CMassTab::SetUiTestInfo(stUiTestInfo& uitestinfo )
{
	memcpy( &m_stUiTestInfo, &uitestinfo, sizeof(stUiTestInfo) );
/*
	if( m_stUiTestInfo.ui_chamberuse )
	{
		ui->chkChmbUse->setChecked( true );
	}
	else
	{
		ui->chkChmbUse->setChecked( false );
	}
*/
}

void CMassTab::SetSlotTestInfo(stSlotTestInfo* pSlottestinfo)
{
	memcpy( m_stSlotTestInfo, pSlottestinfo, sizeof(stSlotTestInfo)*MAX_SLOT );
}

void CMassTab::SetSlotLive(stSlotLive& slotlive)
{
	memcpy( &m_stSlotLive, &slotlive, sizeof(stSlotLive) );
}

void CMassTab::SetPgmLive(stPgmLive& pgmlive)
{
	memcpy( &m_stPgmLive, &pgmlive, sizeof(stPgmLive) );
}

void CMassTab::InitSigSlot()
{
	//connect( m_pSlotView[0]       , &QTableWidget::cellClicked, this, &CMassTab::SlotCellOfSlotClicked   );
	//connect( m_pSlotView[1]       , &QTableWidget::cellClicked, this, &CMassTab::SlotCellOfSlotClicked   );
	//connect( ui->splitterInfonPs  , &QSplitter::splitterMoved , this, &CMassTab::SlotSplitterMoved       );
	//connect( ui->splitterPsnChmb  , &QSplitter::splitterMoved , this, &CMassTab::SlotSplitterMoved       );

	connect( m_pTmrTest           , &QTimer::timeout          , this, &CMassTab::SlotTimeoutTest         );
	connect( m_pTmrPwr            , &QTimer::timeout          , this, &CMassTab::SlotTimeoutTest         );
    connect( m_pTmrPfm            , &QTimer::timeout          , this, &CMassTab::slotSetPfm		         ); // hsbae

	//connect( this->ui->pushButton_plot, &QPushButton::clicked, this->ui->widget_plot_chamber, &CPlotChamber::slotStartPlot);
	//2023.06.20 - kwmoon(chamber graph)
	connect( this                 , &CMassTab::SigStartChmbGraph, this->ui->widget_plot_chamber, &CPlotChamber::slotStartPlot );
	connect( this                 , &CMassTab::SigStopChmbGraph , this->ui->widget_plot_chamber, &CPlotChamber::slotStopPlot  );
	connect( this                 , &CMassTab::SigResetChmbGraph, this->ui->widget_plot_chamber, &CPlotChamber::slotResetPlot );
}

void CMassTab::SlotFromMainUiLoadExistData()
{

}

void CMassTab::SlotFromSlotViewCurrentSlot(uint slot)
{
	m_nCurrentSlot = slot;
	DisplayTestTime( slot );
	DisplaySlotInfo( slot );
}

void CMassTab::InitLotInfoView()
{
    ui->tblLotInfo->clearContents();
    ui->tblLotInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tblLotInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblLotInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tblLotInfo->setFrameShape(QFrame::NoFrame);	//NoFrame, Box, Panel, StyledPanel
    //ui->tblLotInfo->setFrameShadow(QFrame::Plain);	//Plain, Raised, Sunken
    ui->tblLotInfo->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblLotInfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tblLotInfo->setAutoScroll(false);
    ui->tblLotInfo->setSortingEnabled(false);
    ui->tblLotInfo->horizontalHeader()->setVisible(true);
    ui->tblLotInfo->verticalHeader()->setVisible(true);
    //ui->tblLotInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tblLotInfo->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tblLotInfo->horizontalHeader()->setStretchLastSection(true);
    //ui->tblLotInfo->verticalHeader()->setStretchLastSection(true);
}

void CMassTab::InitPpsView()
{
	m_pPsView[0] = ui->tblPpsC1;
	m_pPsView[0]->clearContents();
	m_pPsView[0]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_pPsView[0]->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pPsView[0]->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pPsView[0]->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pPsView[0]->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pPsView[0]->setAutoScroll(false);
	m_pPsView[0]->setSortingEnabled(false);
	m_pPsView[0]->horizontalHeader()->setVisible(true);
	m_pPsView[0]->verticalHeader()->setVisible(false);
	m_pPsView[0]->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	m_pPsView[0]->horizontalHeader()->setStretchLastSection(true);

	m_pPsView[1] = ui->tblPpsC2;
	m_pPsView[1]->clearContents();
	m_pPsView[1]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_pPsView[1]->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pPsView[1]->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pPsView[1]->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pPsView[1]->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pPsView[1]->setAutoScroll(false);
	m_pPsView[1]->setSortingEnabled(false);
	m_pPsView[1]->horizontalHeader()->setVisible(true);
	m_pPsView[1]->verticalHeader()->setVisible(false);
	m_pPsView[1]->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	m_pPsView[1]->horizontalHeader()->setStretchLastSection(true);

	for (int row = 0; row < 16; row += 1) {
		m_pPsView[0]->insertRow(row);
		m_pPsView[1]->insertRow(row);
		m_pPsView[0]->setItem(row, 1, new QTableWidgetItem(QString::number(row % 8 + 1)));
		m_pPsView[1]->setItem(row, 1, new QTableWidgetItem(QString::number(row % 8 + 1)));
		m_pPsView[0]->item(row, 1)->setTextAlignment(Qt::AlignCenter);
		m_pPsView[1]->item(row, 1)->setTextAlignment(Qt::AlignCenter);
	}

	m_pPsView[0]->setSpan(0, 0, 8, 1);
	m_pPsView[0]->setSpan(8, 0, 8, 1);
	m_pPsView[0]->setItem(0, 0, new QTableWidgetItem("PPS"));
	m_pPsView[0]->setItem(8, 0, new QTableWidgetItem("S1PS"));
	m_pPsView[0]->item(0, 0)->setTextAlignment(Qt::AlignCenter);
	m_pPsView[0]->item(8, 0)->setTextAlignment(Qt::AlignCenter);


	m_pPsView[1]->setSpan(0, 0, 8, 1);
	m_pPsView[1]->setSpan(8, 0, 8, 1);
	m_pPsView[1]->setItem(0, 0, new QTableWidgetItem("PPS"));
	m_pPsView[1]->setItem(8, 0, new QTableWidgetItem("S1PS"));
	m_pPsView[1]->item(0, 0)->setTextAlignment(Qt::AlignCenter);
	m_pPsView[1]->item(8, 0)->setTextAlignment(Qt::AlignCenter);

	setInitValuePpsView();
}

/*
void CMassTab::InitPlotView()
{
	//ui->plotChamber->setTitle("Temp Monitoring");
	//ui->plotChamber->setAxisTitle( QwtPlot::yLeft  , "Temperature" );
	ui->plotChamber->setAxisTitle( QwtPlot::xBottom, "Sample Test" );
	ui->plotChamber->setAxisScale( QwtPlot::yLeft  , -30, 150, 10 );
	ui->plotChamber->setAxisScale( QwtPlot::xBottom, 0, 24, 1 );
	m_pPlotCurve = new QwtPlotCurve;
	for( int i=0;i< 24;i++ )
	{
		int iValue = QRandomGenerator::global()->bounded(-30, 150);
		m_PlotData.m_x.push_back(i);
		m_PlotData.m_y.push_back( iValue*1.0 );
		//qDebug() << i << " : " << iValue ;
	}

	QwtPointArrayData * const data = new QwtPointArrayData( &m_PlotData.m_x[0], &m_PlotData.m_y[0], m_PlotData.m_x.size() );
	m_pPlotCurve->setStyle( QwtPlotCurve::CurveStyle::Lines );
	m_pPlotCurve->attach( ui->plotChamber );
	m_pPlotCurve->setAxes( 0, 0 );
	m_pPlotCurve->setData( data );
	ui->plotChamber->replot();
}
*/

void CMassTab::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent( event );
	ResizeView();
}

void CMassTab::ResizeView()
{
	ResizeLotInfoView();
	ResizePpsView();
}

void CMassTab::ResizeLotInfoView()
{
	int nWidth      = ui->tblLotInfo->width();
	int nHeadWidth  = ui->tblLotInfo->verticalHeader()->width();
	int nScrolWidth = ui->tblLotInfo->verticalScrollBar()->isVisible()?ui->tblLotInfo->verticalScrollBar()->width():0;
	int nViewWidth = static_cast<int>( nWidth - nHeadWidth - nScrolWidth );
	ui->tblLotInfo->setColumnWidth( EN_PART , nViewWidth * 0.25 );
	ui->tblLotInfo->setColumnWidth( EN_LOT  , nViewWidth * 0.25 );
	ui->tblLotInfo->setColumnWidth( EN_INS  , nViewWidth * 0.1 );
	ui->tblLotInfo->setColumnWidth( EN_MNT  , nViewWidth * 0.1 );
	ui->tblLotInfo->setColumnWidth( EN_PASS , nViewWidth * 0.1 );
	ui->tblLotInfo->setColumnWidth( EN_FAIL , nViewWidth * 0.1 );
	ui->tblLotInfo->setColumnWidth( EN_YIELD, nViewWidth * 0.1 );
}

void CMassTab::ResizePpsView()
{
	int current_tab = this->ui->tabPPS->currentIndex();
	int nWidth      = m_pPsView[current_tab]->width();
	int nHeadWidth  = m_pPsView[current_tab]->verticalHeader()->width();
	int nScrolWidth = m_pPsView[current_tab]->verticalScrollBar()->isVisible()?m_pPsView[current_tab]->verticalScrollBar()->width():0;
	int nViewWidth = static_cast<int>( nWidth - nHeadWidth - nScrolWidth );

	m_pPsView[0]->setColumnWidth( EN_GR , nViewWidth * 0.1 );
	m_pPsView[0]->setColumnWidth( EN_CH , nViewWidth * 0.1 );
	m_pPsView[0]->setColumnWidth( EN_SV , nViewWidth * 0.2 );
	m_pPsView[0]->setColumnWidth( EN_RV , nViewWidth * 0.2 );
	m_pPsView[0]->setColumnWidth( EN_RC , nViewWidth * 0.2 );
	m_pPsView[0]->setColumnWidth( EN_STA, nViewWidth * 0.2 );

	m_pPsView[1]->setColumnWidth( EN_GR , nViewWidth * 0.1 );
	m_pPsView[1]->setColumnWidth( EN_CH , nViewWidth * 0.1 );
	m_pPsView[1]->setColumnWidth( EN_SV , nViewWidth * 0.2 );
	m_pPsView[1]->setColumnWidth( EN_RV , nViewWidth * 0.2 );
	m_pPsView[1]->setColumnWidth( EN_RC , nViewWidth * 0.2 );
	m_pPsView[1]->setColumnWidth( EN_STA, nViewWidth * 0.2 );
}

void CMassTab::SlotSlotSetupResult( int slot, bool setup_ok, QString bibid, int total, int pass, QString part, QString lot )	//Setup Result
{
	int seq     = slot / (MAX_SLOT/2);
	int offset  = slot % (MAX_SLOT/2);
	QString sPT = tr("%1/%2").arg(pass).arg(total);
/*
	m_pSlotView[seq]->item( offset, EN_SI_BIBID    )->setText( bibid );
	m_pSlotView[seq]->item( offset, EN_SI_PT       )->setText( sPT );
	m_pSlotView[seq]->item( offset, EN_SI_PARTNAME )->setText( part );
	m_pSlotView[seq]->item( offset, EN_SI_LOTNAME  )->setText( lot );
	if( !setup_ok )
	{
		m_pSlotView[seq]->item( offset, EN_SI_BIBID    )->setBackground( FAIL_BG_QCOLOR );
		m_pSlotView[seq]->item( offset, EN_SI_PT       )->setBackground( FAIL_BG_QCOLOR );
		m_pSlotView[seq]->item( offset, EN_SI_PARTNAME )->setBackground( FAIL_BG_QCOLOR );
		m_pSlotView[seq]->item( offset, EN_SI_LOTNAME  )->setBackground( FAIL_BG_QCOLOR );
		QProgressBar* pItem = static_cast<QProgressBar*>( m_pSlotView[seq]->cellWidget( offset, EN_SI_PROGRESS) );
		if( pItem )
		{
			pItem->setStyleSheet( PASS_PRGBAR_QSS );
			pItem->setToolTip("Setup error!");
		}
	}
	else
	{
		m_pSlotView[seq]->item( offset, EN_SI_BIBID    )->setBackground( PASS_BG_QCOLOR );
		m_pSlotView[seq]->item( offset, EN_SI_PT       )->setBackground( PASS_BG_QCOLOR );
		m_pSlotView[seq]->item( offset, EN_SI_PARTNAME )->setBackground( PASS_BG_QCOLOR );
		m_pSlotView[seq]->item( offset, EN_SI_LOTNAME  )->setBackground( PASS_BG_QCOLOR );
		QProgressBar* pItem = static_cast<QProgressBar*>( m_pSlotView[seq]->cellWidget( offset, EN_SI_PROGRESS) );
		if( pItem )
		{
			pItem->setStyleSheet( PASS_PRGBAR_QSS );
			pItem->setToolTip("Setup OK");
		}
	}
	*/
	ui->leLotNo->setText( lot );
    ui->lePartNo->setText( part );  //2023.07.07 - kwmoon(bug fix lot -> part )
	ui->leStartTime->setText( UTIL::DATE::yyyyMMDDHHmmss() );
	m_dtTestStart = QDateTime::currentDateTime();
	//if( ui->tblSlotView1->current)
}

void CMassTab::SetTestDataMgrPtr(CFileMap* ptr)
{
	m_pTestDataMgr = ptr;
}

void CMassTab::SlotInitTestStartFlag() {
    StartPfmTimer();
}

void CMassTab::SlotTestStartFlag()
{
	m_dtTestStart = QDateTime::currentDateTime();
	ui->leStartTime->setText( UTIL::DATE::yyyyMMDDHHmmss() );

	//Test Time Timer
	StartTestTimer();
	StartPwrTimer();
    StartPfmTimer();
	//Start Time / Total Time / Bi Time / Left Time
	//Chamber Graph
	m_bTest = true;
}

void CMassTab::SlotTestEndFlag()
{
	m_dtTestEnd = QDateTime::currentDateTime();
	ui->leEndTime->setText( UTIL::DATE::yyyyMMDDHHmmss() );
	StopTimer();
	m_bTest = false;
}

void CMassTab::SlotTestPgmVariableFlag(uint slot)
{
	(void)slot;
}


void CMassTab::SlotStepOut(uint slot)
{

}

void CMassTab::DisplayTestTime(uint slot)
{
	ui->leTotalTime->setText("");
	ui->leBibTime->setText("");
	ui->leRemainTime->setText("");

	//uint slot = static_cast<uint>(m_nCurrentSlot);

	if( m_stSlotLive.slot_status[slot][CPU00] != EN_SLOT_ONLINE   ) return;
	if( m_stPgmLive.pgm_status[slot][PROTOCOL::EN_PGM_RUN0][PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET] != EN_SLOT_ONLINE ) return;

	//TOTAL TIME
	int totalime = m_pTestDataMgr->GET_BITIME( slot, CPU00 );
	int min  = totalime / 60;
	int sec  = totalime % 60;
	int hour = min / 60;
	min = min % 60;
	ui->leTotalTime->setText( tr("%1:%2:%3").arg(hour,2,10,QChar('0')).arg(min,2,10,QChar('0')).arg(sec,2,10,QChar('0')) ) ;

	//BI TIME
	QDateTime current = m_dtUpdateTime[slot];
	qint64 elapsed_sec = m_dtTestStart.secsTo( current );
	int bitime  = static_cast<int>(elapsed_sec);
	int bi_min  = bitime / 60;
	int bi_sec  = bitime % 60;
	int bi_hour = bi_min / 60;
	bi_min = bi_min % 60;
	ui->leBibTime->setText( tr("%1:%2:%3").arg(bi_hour,2,10,QChar('0')).arg(bi_min,2,10,QChar('0')).arg(bi_sec,2,10,QChar('0')) ) ;

	//LEFT TIME
	int diff_sec = (totalime - bitime );
	int left_min  = diff_sec / 60;
	int left_sec  = diff_sec % 60;
	int left_hour = left_min / 60;
	left_min = left_min % 60;
	ui->leRemainTime->setText( tr("%1:%2:%3").arg(left_hour,2,10,QChar('0')).arg(left_min,2,10,QChar('0')).arg(left_sec,2,10,QChar('0')) ) ;
}

void CMassTab::DisplaySlotInfo(uint slot)
{
    char* env_data = getenv(ATDATA_DIR_ENV);
    QString data_path = "";

    if( env_data != NULL )
    {
        data_path = tr("%1").arg(env_data);
    }
    else
    {
        data_path = tr("%1").arg(DIR_ATDATA_PATH);
    }

    //FIX
	//Bin -> FileMap
	QString sErrMsg = "";
    QString sRcmdBoardMaintInfoFile = tr("%1/%2/%3/%4/SLOT%5%6").arg(data_path).arg(DIR_ATDATA_NAME).arg(m_pTestDataMgr->GET_UIAUTOMODE()? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH).arg(BOARD_MAINT_DIR).arg(slot).arg(EXT_RCMD);
	char* pReadBoardMaintInfo = new char[sizeof(stRcmdBoardMaintInfo)];

	if ( !m_pTestDataMgr->READ_FILE(sRcmdBoardMaintInfoFile.toStdString().c_str(), pReadBoardMaintInfo, sizeof(stRcmdBoardMaintInfo)) )
	{
		sErrMsg = tr("FILE NOT FOUND: %1").arg(sRcmdBoardMaintInfoFile);
		qDebug() << tr("[ERROR] %1").arg(sErrMsg);

		return;
	}

	stRcmdBoardMaintInfo rcmdBoardMainInfo;
    memcpy( (char*)&rcmdBoardMainInfo, pReadBoardMaintInfo, sizeof(stRcmdBoardMaintInfo));
	delete[] pReadBoardMaintInfo;

	//PART NO
	QString part_no = rcmdBoardMainInfo.PART_NO;
	ui->lePartNo->setText(part_no);
	//LOT NO
	QString lot_no = rcmdBoardMainInfo.LOT_NO;
	ui->leLotNo->setText(lot_no);

    if ( m_pTestDataMgr->GET_DEVICE( slot, CPU00, m_stSlotTestInfo[slot].szTpName) )
    {
        ui->lePgmName->setText(m_stSlotTestInfo[slot].szTpName);
    }
}

void CMassTab::SlotTimeoutTest()
{
	for( uint i=0;i<MAX_SLOT;i++ )
		m_dtUpdateTime[i] = QDateTime::currentDateTime();
	DisplayTestTime( m_nCurrentSlot );
}

void CMassTab::SlotTimeoutPwr()
{

}

void CMassTab::SlotStartChmbPlot()
{
	emit SigSysLogMessage( _NORMAL_LOG, "[INFO]CHAMBER PLOT START");
	emit SigStartChmbGraph();
}

void CMassTab::SlotStopChmbPlot()
{
	emit SigSysLogMessage( _NORMAL_LOG, "[INFO]CHAMBER PLOT STOP");
	emit SigStopChmbGraph();
}

void CMassTab::SlotResetChmbPlot()
{
	emit SigSysLogMessage( _NORMAL_LOG, "[INFO]CHAMBER PLOT RESET");
	emit SigResetChmbGraph();
}

void CMassTab::SlotRecvRcmd(_RCMD rcmd)
{
	QString ini_file("/data/testdata/auto/rcmd.ini");
	switch(rcmd)
	{
		case _RCMD::AUTO_DIAG:
			{
				QString section("AUTO_DIAG");
				QString key[4] = {"RETURN_FLAG", "ERROR_DESCRIPTION", "AUTO_DIAG_DESC", "AUTO_DIAG_FLAG"};
				atstring sValue("");
				for (int i = 0; i < 4; i++)
				{
					UTIL::INI::readSettings(ini_file, tr("%1/%2").arg(section).arg(key[i]), sValue);
//					qDebug() << key[i] << " : " << QString(sValue);
				}
			}
			break;
		case _RCMD::EQ_EFF_INFO:
			{
				QString section("EQ_EFF_INFO");
				QString key[5] = {"RETURN_FLAG", "ERROR_DESCRIPTION", "JAM_GRADE", "JAM_CONTENT", "SINGO_EVENT"};
				atstring sValue("");
				for (int i = 0; i < 5; i++)
				{
					UTIL::INI::readSettings(ini_file, tr("%1/%2").arg(section).arg(key[i]), sValue);
//					qDebug() << key[i] << " : " << QString(sValue);
				}
			}
			break;
		default:
//			qDebug() << "ERROR";
			break;
	}
}


// FROM THIS LINE
// hsbae add for pfm

void CMassTab::attachSharedMemory() {
	m_pShmPfm = new QSharedMemory(SHM_KEY::SHM_PFM_MON_KEY, this);
	if (m_pShmPfm->attach(QSharedMemory::ReadWrite))
		;
//		qDebug() << "ATTACH!";
	else {
		m_pShmPfm->create(sizeof(stShmMainPfm), QSharedMemory::ReadWrite);
//		qDebug() << "CREATE!";
	}
}

void CMassTab::slotSetPfm() {
	sm_slot_data* slot_data = &((stShmMainPfm*)m_pShmPfm->data())->slot_data[m_nCurrentSlot];

	PFM_STATUS_TYPE pfm_status_type;

	for (int i = 0; i < 2; i += 1) {
		for (int j = 0; j < 8; j += 1) {
			this->m_pPsView[i]->item(j, 2)->setText(QString("").asprintf("%6.2f", slot_data->cpu_data[i].pps_data[j].set_voltage) + "V");
			this->m_pPsView[i]->item(j, 3)->setText(QString("").asprintf("%6.2f", slot_data->cpu_data[i].pps_data[j].present_voltage) + "V");
			this->m_pPsView[i]->item(j, 4)->setText(QString("").asprintf("%6.2f", slot_data->cpu_data[i].pps_data[j].present_current) + "A");

			pfm_status_type = getPfmStatus(slot_data->cpu_data[i].pps_data[j].status_word, slot_data->cpu_data[i].pps_data[j].status_vout);
			this->m_pPsView[i]->item(j, 5)->setText(getQStringFromPfmStatusType(pfm_status_type));
            this->m_pPsView[i]->item(j, 5)->setForeground(QBrush(getGlobalColorFromPfmStatysType(pfm_status_type)));

			this->m_pPsView[i]->item(j + 8, 2)->setText(QString("").asprintf("%6.2f", slot_data->cpu_data[i].s1ps_data[j].set_voltage) + "V");
			this->m_pPsView[i]->item(j + 8, 3)->setText(QString("").asprintf("%6.2f", slot_data->cpu_data[i].s1ps_data[j].present_voltage) + "V");
			this->m_pPsView[i]->item(j + 8, 4)->setText(QString("").asprintf("%6.2f", slot_data->cpu_data[i].s1ps_data[j].present_current) + "A");

			pfm_status_type = getPfmStatus(slot_data->cpu_data[i].s1ps_data[j].status_word, slot_data->cpu_data[i].s1ps_data[j].status_vout);
			this->m_pPsView[i]->item(j + 8, 5)->setText(getQStringFromPfmStatusType(pfm_status_type));
			this->m_pPsView[i]->item(j + 8, 5)->setForeground(QBrush(getGlobalColorFromPfmStatysType(pfm_status_type)));
		}
    }
}

void CMassTab::setInitValuePpsView() {
	for (int row = 0; row < 16; row += 1) {
		for (int column = 2; column < 6; column += 1) {
			m_pPsView[0]->setItem(row, column, new QTableWidgetItem("-"));
			m_pPsView[1]->setItem(row, column, new QTableWidgetItem("-"));
			m_pPsView[0]->item(row, column)->setTextAlignment(Qt::AlignCenter);
			m_pPsView[1]->item(row, column)->setTextAlignment(Qt::AlignCenter);
		}
	}
}

bool	CMassTab::isStatusFault(int status_word, int status_vout) {
	if (status_word & ~STATUS_WORD_MASK || status_vout & ~STATUS_VOUT_MASK)
		return true;
	return false;
}

PFM_STATUS_TYPE	CMassTab::getPfmStatus(int status_word, int status_vout) {
	int masked_status_word = status_word & ~STATUS_WORD_MASK;	// 1111 1111 1100 0011
	int masked_status_vout = status_vout & ~STATUS_VOUT_MASK;	//			 0110 1011
	if (masked_status_word & 0x0020)
		return PFM_STATUS_TYPE::SW_VOUT_OV_FAULT;
	if (masked_status_word & 0x0010)
		return PFM_STATUS_TYPE::SW_IOUT_OC_FAULT;
	if (masked_status_word & 0x0008)
		return PFM_STATUS_TYPE::SW_VIN_UV_FAULT;
	if (masked_status_vout & 0x0080)
		return PFM_STATUS_TYPE::SV_VOUT_OV_FAULT;
	if (masked_status_vout & 0x0010)
		return PFM_STATUS_TYPE::SV_VOUT_UV_FAULT;
	return PFM_STATUS_TYPE::OK;
}

QString	CMassTab::getQStringFromPfmStatusType(PFM_STATUS_TYPE pfm_status_type) {
	switch (pfm_status_type) {
		case PFM_STATUS_TYPE::OK:
			return "OK";
			break;
		case PFM_STATUS_TYPE::SW_VOUT_OV_FAULT:
			return "OV";
			break;
		case PFM_STATUS_TYPE::SW_IOUT_OC_FAULT:
			return "OC";
			break;
		case PFM_STATUS_TYPE::SW_VIN_UV_FAULT:
			return "UV";
			break;
		case PFM_STATUS_TYPE::SV_VOUT_OV_FAULT:
			return "OV";
			break;
		case PFM_STATUS_TYPE::SV_VOUT_UV_FAULT:
			return "UV";
			break;
		default:
			return "undefined";
			break;
	}
}

Qt::GlobalColor	CMassTab::getGlobalColorFromPfmStatysType(PFM_STATUS_TYPE pfm_status_type) {
	if (pfm_status_type == PFM_STATUS_TYPE::OK)
		return Qt::black;
	return Qt::red;
}
