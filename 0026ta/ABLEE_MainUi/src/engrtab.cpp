#include "engrtab.hpp"
#include "ui_engrtab.h"

CEngrTab::CEngrTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CEngrTab)
{
	InitVariables();
	InitUi();
	InitSigSlot();
}

CEngrTab::~CEngrTab()
{
	for( int i=0;i<MAX_SLOT;i++ )
	{
		for(int r=0;r<MAX_SYS_CPU;r++ )
		{
			if( m_pLoggerSlot[i][r] )
			{
				delete m_pLoggerSlot[i][r];
				m_pLoggerSlot[i][r] = nullptr;
			}
		}
	}
	if( m_pInitFail != nullptr )
	{
		delete[] m_pInitFail;
		m_pInitFail = nullptr;
	}
	if( m_pStepFail != nullptr )
	{
		delete[] m_pStepFail;
		m_pStepFail = nullptr;
	}
	if( m_pSortBin != nullptr )
	{
		delete[] m_pSortBin;
		m_pSortBin = nullptr;
	}
	if( m_pFailCnt != nullptr )
	{
		delete[] m_pFailCnt;
		m_pFailCnt = nullptr;
	}
	if( m_pstChipId != nullptr )
	{
		delete[] m_pstChipId;
		m_pstChipId = nullptr;
	}
	if( m_pCumBlockFail != nullptr )
	{
		delete[] m_pCumBlockFail;
		m_pCumBlockFail = nullptr;
	}
	if( m_pFailChip != nullptr )
	{
		delete[] m_pFailChip;
		m_pFailChip = nullptr;
	}
	if( m_pCumFailChip != nullptr )
	{
		delete[] m_pCumFailChip;
		m_pCumFailChip = nullptr;
	}

	delete ui;
}

void CEngrTab::InitVariables()
{
	m_nCurrentSlot = 0;
	m_bHasItemList = false;
	m_pTestDataMgr = nullptr;
	m_pStackLog[CPU00] = nullptr;
	m_pStackLog[CPU01] = nullptr;
	m_nBibRows = DEFAULT_BIB_ROWS;
	m_nBibCols = DEFAULT_BIB_COLS;
	for( uint slot=0;slot<MAX_SLOT;slot++ )
	{
		//m_pBibView[slot] = new CBibView;
		//m_pBibView[slot]->SetSlotNo( slot );

		for(int r=0;r<MAX_SYS_CPU;r++ )
		{
			m_bAutoScrollSlot[slot][r] = true;
			int type = PROTOCOL::_ATSLOT01 + slot;
			m_pLoggerSlot[slot][r] = new CLogger( static_cast<PROTOCOL::SOURCE_TYPE>(type), r );
			connect( this, &CEngrTab::SigSaveSlotLog, m_pLoggerSlot[slot][r], &CLogger::SlotLogSlotMessage );
		}
	}

	memset( &m_stUiTestInfo , 0x0, sizeof(stUiTestInfo) );
	memset( &m_stSlotLive   , 0x0, sizeof(stSlotLive)   );
	memset( &m_stPgmLive    , 0x0, sizeof(stPgmLive)    );
	memset( m_stSlotTestInfo, 0x0, sizeof(stSlotTestInfo)*MAX_SLOT );
	memset( &m_stDevPgmVars , 0x0, sizeof(stDevPgmVariables) );

	m_ResultType = BTN_NORMAL;
	for( int i=0;i<BTN_RESULT_COUNT;i++ ) m_arrayBtn[i] = nullptr;

	m_pInitFail = nullptr;
	m_pStepFail = nullptr;
	m_pstChipId = nullptr;
	m_pSortBin  = nullptr;
	m_pFailCnt  = nullptr;
	m_pCumBlockFail = nullptr;
	m_pCumFailChip = nullptr;
	m_pFailChip = nullptr;
	m_vecFailBlk.clear();
}

void CEngrTab::InitUi()
{
	ui->setupUi(this);
	m_arrayBtn[BTN_NORMAL  ] = ui->btnNormal;
	m_arrayBtn[BTN_MAKER   ] = ui->btnMaker;
	m_arrayBtn[BTN_DEV_TYPE] = ui->btnDevType;
	m_arrayBtn[BTN_FAIL_CNT] = ui->btnFailCnt;
	m_arrayBtn[BTN_ADD_FAIL] = ui->btnAddFail;
	m_arrayBtn[BTN_BIN     ] = ui->btnBin;

	InitSlotLogTextView();
	InitBibMapView();
	InitBtnResult();

	ui->cboStep->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui->tabSlotData->setTabText(0, tr("SLOT#%1").arg(0+1));
	ui->teDutData->setLineWrapMode(QPlainTextEdit::NoWrap);

}

void CEngrTab::SetTestDataMgrPtr(CFileMap* ptr)
{
	m_pTestDataMgr = ptr;
	//DisplaySlotData( m_nCurrentSlot );
}

void CEngrTab::InitSigSlot()
{
	connect( ui->tblBibMap       , &QTableWidget::cellClicked        , this, &CEngrTab::SlotCellOfBibMapClicked       );
	connect(ui->cboStep, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CEngrTab::SlotItemSelectChanged  );
	connect( ui->btnNormal       , &QPushButton::clicked             , this, &CEngrTab::SlotBtnResultKindChange       );
	connect( ui->btnMaker        , &QPushButton::clicked             , this, &CEngrTab::SlotBtnResultKindChange       );
	connect( ui->btnDevType      , &QPushButton::clicked             , this, &CEngrTab::SlotBtnResultKindChange       );
	connect( ui->btnFailCnt      , &QPushButton::clicked             , this, &CEngrTab::SlotBtnResultKindChange       );
	connect( ui->btnAddFail      , &QPushButton::clicked             , this, &CEngrTab::SlotBtnResultKindChange       );
	connect( ui->btnBin          , &QPushButton::clicked             , this, &CEngrTab::SlotBtnResultKindChange       );
}

void CEngrTab::InitSlotLogTextView()
{
	QFont font;
	font.setPointSize(8);
	font.setFamily("DejaVu Sans Mono");

	ui->tabS1_1->clear();
	ui->tabS1_2->clear();
	for( int cpu=0;cpu<MAX_SYS_CPU;cpu++ )
	{
		m_pStackLog[cpu] = new QStackedWidget;
		for( int s=0;s<MAX_SLOT;s++ )
		{
			m_pSlotTextEdit[s][cpu] = new QPlainTextEdit;
			m_pSlotTextEdit[s][cpu]->setFont(font);
			m_pSlotTextEdit[s][cpu]->setReadOnly(true);
			m_pSlotTextEdit[s][cpu]->setContextMenuPolicy( Qt::CustomContextMenu );
			m_pSlotTextEdit[s][cpu]->setMaximumBlockCount( MAX_LOG_LINE_NUM );
			m_pSlotTextEdit[s][cpu]->setLineWrapMode( QPlainTextEdit::LineWrapMode::NoWrap );
			m_pSlotTextEdit[s][cpu]->appendPlainText( tr("%1-%2").arg(s).arg(cpu));
			connect( m_pSlotTextEdit[s][cpu], &QPlainTextEdit::customContextMenuRequested, this, &CEngrTab::SlotCustomContextMenuRequested );
			m_pStackLog[cpu]->addWidget( m_pSlotTextEdit[s][cpu] );
		}
		if( cpu == 0 )	ui->tabS1_1->addTab( m_pStackLog[cpu], tr("CPU%1").arg(cpu+1) );
		else            ui->tabS1_2->addTab( m_pStackLog[cpu], tr("CPU%1").arg(cpu+1) );
	}
/*
	return;

	for( int s=0;s<MAX_SLOT;s++ )
	{
		QWidget* tWdg = new QWidget;
		QHBoxLayout* pHlayout = new QHBoxLayout;

		for( int r=0;r<2;r++ )
		{
			QTabWidget* tab = new QTabWidget();
			tab->setObjectName( tr("tabS%1_%2").arg(s).arg(r) );
			QWidget* rWdg = new QWidget;
			QVBoxLayout* vLayout = new QVBoxLayout;
			tab->setTabPosition( QTabWidget::North );

			m_pSlotTextEdit[s][r] = new QPlainTextEdit;
			m_pSlotTextEdit[s][r]->setFont(font);
			m_pSlotTextEdit[s][r]->setReadOnly(true);
			//m_pSlotTextEdit[s][r]->setLineWrapMode( QPlainTextEdit::LineWrapMode::WidgetWidth );
			m_pSlotTextEdit[s][r]->setContextMenuPolicy( Qt::CustomContextMenu );
			m_pSlotTextEdit[s][r]->setMaximumBlockCount( MAX_LOG_LINE_NUM );
			m_pSlotTextEdit[s][r]->setLineWrapMode( QPlainTextEdit::LineWrapMode::NoWrap );
			connect( m_pSlotTextEdit[s][r], &QPlainTextEdit::customContextMenuRequested, this, &CEngrTab::SlotCustomContextMenuRequested );

			vLayout->addWidget( m_pSlotTextEdit[s][r] );
            vLayout->setContentsMargins( 2, 2, 2, 2 );
            vLayout->setSpacing( 2 );
			rWdg->setLayout( vLayout );
			tab->addTab( rWdg, QIcon(":/icon/icon/ic_memory.ico"), tr("RUN#%1").arg(r+1));
			tab->setTabToolTip( static_cast<int>(s), tr("SLOT#%1 PGM#%2").arg(s+1).arg(r+1));
			pHlayout->addWidget( tab );
		}
		pHlayout->setContentsMargins( 2, 3, 2, 2 );
		tWdg->setLayout(pHlayout);
		//ui->tabLog->addTab( tWdg, QIcon(":/icon/icon/slotlog.ico"), tr("S#%1").arg(s+1) );
		//ui->tabLog->setTabToolTip( static_cast<int>(s), tr("SLOT#%1 LOG").arg(s+1));
	}
*/
}

void CEngrTab::SetUiTestInfo(stUiTestInfo& uitestinfo )
{
	memcpy( &m_stUiTestInfo, &uitestinfo, sizeof(stUiTestInfo) );
}

void CEngrTab::SetSlotTestInfo(stSlotTestInfo* pSlottestinfo )
{
	memcpy( m_stSlotTestInfo, pSlottestinfo, sizeof(stSlotTestInfo)*MAX_SLOT );
}

void CEngrTab::SetSlotLive(stSlotLive& slotlive)
{
	memcpy( &m_stSlotLive, &slotlive, sizeof(stSlotLive) );
}

void CEngrTab::SetPgmLive(stPgmLive& pgmlive)
{
	memcpy( &m_stPgmLive, &pgmlive, sizeof(stPgmLive) );
}

void CEngrTab::SlotCustomContextMenuRequested( QPoint point )
{
	// Start with the standard menu.
	QPlainTextEdit* pSender = static_cast<QPlainTextEdit*>(QObject::sender());
	QMenu * pMenu = pSender->createStandardContextMenu();
	QAction * pAction;
	bool bSender = false;
	for( int s=0;s<MAX_SLOT;s++ )
	{
		for(int r=0;r<MAX_SYS_CPU;r++)
		{
			if( pSender == m_pSlotTextEdit[s][r] )
			{
				bSender = true;
				pAction = new QAction( tr("Auto Scroll On"), pSender );
				pAction->setCheckable( true );
				if( m_bAutoScrollSlot[s][r] ) pAction->setChecked(true);
				else                          pAction->setChecked(false);
				connect( pAction, &QAction::triggered, this, &CEngrTab::SlotScrollStatusChanged );
				pMenu->addAction( pAction );
				break;
			}
		}
	}

	if( !bSender )
	{
		delete pMenu;
		return;
	}

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

void CEngrTab::SlotScrollStatusChanged()
{
	QAction* pSender = static_cast<QAction*>(QObject::sender());
	bool bMatch = false;
	for( int s=0;s<MAX_SLOT;s++ )
	{
		for(int r=0;r<MAX_SYS_CPU;r++)
		{
			if( pSender->parent() == m_pSlotTextEdit[s][r] )
			{
				m_bAutoScrollSlot[s][r] = !m_bAutoScrollSlot[s][r];
				//qDebug() << tr("SLOT%1-%2 LOG").arg(s).arg(r);
				bMatch = true;
			}
			if( bMatch ) break;
		}
		if( bMatch ) break;
	}
}


void CEngrTab::InitBibMapView()
{
	ui->titleInit->setStyleSheet( tr("background-color: %1; color: #FFFFFF;").arg(INIT_TEST_RGB) );
	ui->titlePass->setStyleSheet( tr("background-color: %1; color: #FFFFFF;").arg(PASS_TEST_RGB) );
	ui->titleFail->setStyleSheet( tr("background-color: %1; color: #FFFFFF;").arg(FAIL_TEST_RGB) );
	ui->titleMask->setStyleSheet( tr("background-color: %1; color: #FFFFFF;").arg(MASK_TEST_RGB) );

	ui->tblBibMap->clear();
	ui->tblBibMap->setRowCount( m_nBibRows );
	ui->tblBibMap->setColumnCount( m_nBibCols );
	ui->tblBibMap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->tblBibMap->setSelectionMode(QAbstractItemView::NoSelection);
//	ui->tblBibMap->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tblBibMap->setSelectionBehavior(QAbstractItemView::SelectItems);
	ui->tblBibMap->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tblBibMap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tblBibMap->setAutoScroll(false);
	ui->tblBibMap->setSortingEnabled(false);
	ui->tblBibMap->horizontalHeader()->setVisible(true);
	ui->tblBibMap->verticalHeader()->setVisible(true);
	ui->tblBibMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);	//Fixed
	ui->tblBibMap->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);		//Fixed
	//ui->tblBibMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);	//2023.07.31 - KWMOON(ORG)
	//ui->tblBibMap->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);		//2023.07.31 - KWMOON(ORG)
	ui->tblBibMap->horizontalHeader()->setStretchLastSection(true);
	ui->tblBibMap->verticalHeader()->setStretchLastSection(true);
	//ui->tblBibMap->setStyleSheet("QTableWidget::item:selected { background-color: rgba(255, 0, 0, 10); border: 1px solid #408080;} QTableWidget {selection-background-color:transparent;}");
	CreateBibDut();
	//ResizeBibMapView();
}

void CEngrTab::ReInitBibMapView(int r,int c)
{
	m_nBibRows = r;
	m_nBibCols = c;
	InitBibMapView();
}

void CEngrTab::CreateBibDut()
{
	for( int row=0;row<m_nBibRows;row++ )
	{
		QTableWidgetItem* vItem = new QTableWidgetItem;
		vItem->setText( tr("").asprintf("R%02d",row+1) );
		vItem->setTextAlignment( Qt::AlignVCenter | Qt::AlignHCenter );
		ui->tblBibMap->setVerticalHeaderItem(row, vItem);
		for (int col = 0; col<m_nBibCols; col++)
		{
			if( row == 0 )
				ui->tblBibMap->setHorizontalHeaderItem(col, new QTableWidgetItem(tr("").asprintf("C%02d",col+1)));
			QTableWidgetItem* item = new QTableWidgetItem;
			item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
			item->setFlags( item->flags() & ~Qt::ItemIsEditable );
			item->setBackground( NODEVICE_TEST_QCOLOR );
			item->setText( "" );
			ui->tblBibMap->setItem(row, col, item );
		}
	}
	//ui->tblBibMap->horizontalHeader()->setMinimumWidth ( 25 );
	//ui->tblBibMap->verticalHeader()->setMinimumWidth( 25 );
}

void CEngrTab::InitSlotBibDut()
{
	for( int row=0;row<m_nBibRows;row++ )
	{
		for (int col = 0; col<m_nBibCols; col++)
		{
			QTableWidgetItem* dutItem = ui->tblBibMap->item( row,col );
			if( dutItem )
			{
				dutItem->setBackground( NODEVICE_TEST_QCOLOR );
				dutItem->setText( "" );
			}
		}
	}
	//ui->leSlotDutCnt->setText ( "" );
	ui->leSlotPassCnt->setText( "" );
	ui->leSlotFailCnt->setText( "" );
	ui->leSlotYield->setText  ( "" );

}

void CEngrTab::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED( event )
	ResizeView();
	QWidget::resizeEvent( event );
}

void CEngrTab::ResizeView()
{
	//ResizeSlotView();
	//ResizeBibMapView();
}

void CEngrTab::ResizeBibMapView()
{
	int nHeadWidth  = 30;
	int nHeadHeight = 25;
	int nWidth      = ui->tblBibMap->width();
	int nHeight     = ui->tblBibMap->height();
	int nScrolWidth = ui->tblBibMap->verticalScrollBar()->isVisible()?ui->tblBibMap->verticalScrollBar()->width():0;
	int nScrolHeight= ui->tblBibMap->horizontalScrollBar()->isVisible()?ui->tblBibMap->horizontalScrollBar()->height():0;
	int nViewWidth  = nWidth  - nHeadWidth  - nScrolWidth;
	int nViewHeight = nHeight - nHeadHeight - nScrolHeight;

	int nModHeight  = nViewHeight%m_nBibRows;
	int nModWidth   = nViewWidth%m_nBibCols;
	int nRowHeight  = static_cast<int>( nViewHeight / m_nBibRows ) ;
	int nColWidth   = static_cast<int>( nViewWidth  / m_nBibCols );

	ui->tblBibMap->horizontalHeader()->setFixedHeight( nHeadHeight + nModHeight );
	ui->tblBibMap->verticalHeader()->setFixedWidth( nHeadWidth + nModWidth );
	for (int row = 0; row<m_nBibRows; row++)
	{
		ui->tblBibMap->setRowHeight(row, nRowHeight);
	}

	for (int col = 0; col<m_nBibCols; col++)
	{
		ui->tblBibMap->setColumnWidth(col, nColWidth);
	}
}

void CEngrTab::DisplaySlotData(uint slot )
{
	m_nCurrentSlot = slot;
	DisplaySlotInfo( slot );
	ShowSlotLogView( slot );
}

void CEngrTab::DisplaySlotInfo(uint slot)
{
	QString bibid  = "";
	QString partno = "";
	QString lotno  = "";

	SlotViewPgmVariables( slot );
	DisplayTestItemList( slot );

	int bibx = m_pTestDataMgr->GET_BIBX( slot, CPU00 );
	int biby = m_pTestDataMgr->GET_BIBY( slot, CPU00 );
	int dut_cnt = bibx * biby;

	m_pTestDataMgr->GET_DEVICE( slot, CPU00, m_stSlotTestInfo[slot].szTpName );
	ui->showBibId->setText( bibid );
	ui->leSlotTpName->setText( m_stSlotTestInfo[slot].szTpName );
	ui->leSlotDutCnt->setText( tr("%1").arg( dut_cnt) );
	ui->tabSlotData->setTabText(0, tr("SLOT#%1").arg(slot+1));

	DisplayBibmapTestData( slot );
}

void CEngrTab::ShowSlotLogView(uint slot)
{
	int nSlot = static_cast<int>(slot);
	if( m_pStackLog[CPU00] != nullptr )
	{
		if( m_pStackLog[CPU00]->count() > nSlot )
			m_pStackLog[CPU00]->setCurrentIndex( slot );
	}
	if( m_pStackLog[CPU01] != nullptr )
	{
		if( m_pStackLog[CPU01]->count() > nSlot )
			m_pStackLog[CPU01]->setCurrentIndex( slot );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CEngrTab::SlotFromSlotViewCurrentSlot
/// \param slot
/// \details from slotview current slot change or same
void CEngrTab::SlotFromSlotViewCurrentSlot(uint slot)
{
	DisplaySlotData( slot );
}


bool CEngrTab::DisplayDutFailBlock( uint slot, uint dut )
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CEngrTab::SlotFromMainUiLoadExistData
/// \details load ui
void CEngrTab::SlotFromMainUiLoadExistData()
{
	uint slot = m_nCurrentSlot;
	DisplaySlotData( slot );
}

/*
bool CEngrTab::DisplaySlotViewPassTotal(uint slot)
{
	if( m_pTestDataMgr == nullptr ) return false;
	uint nMaxDut = m_pTestDataMgr->GET_BIBX(slot,CPU00) * m_pTestDataMgr->GET_BIBY(slot,CPU00);
	uint nMaxDutPerCpu = nMaxDut / 2;
	if( nMaxDut < 1 ) return false;

	stInitTestHead initHead[MAX_SYS_CPU];
	char* pInitFail = new char[ nMaxDut ];
	if( !m_pTestDataMgr->GET_INITTEST_DATA( slot, CPU00, initHead[CPU00], pInitFail, sizeof(char)*nMaxDutPerCpu) )
	{
		delete[] pInitFail;
		return false;
	}
	if( !m_pTestDataMgr->GET_INITTEST_DATA( slot, CPU01, initHead[CPU01], pInitFail+nMaxDutPerCpu, sizeof(char)*nMaxDutPerCpu) )
	{
		delete[] pInitFail;
		return false;
	}
	uint nSlotTotal = nMaxDut;
	for( uint dut=0;dut<nMaxDut;dut++ )
	{
		if( pInitFail[dut] ) nSlotTotal--;
	}
	delete[] pInitFail;

	////////////////////////////////////////////////////////////////////////////////////////////////
	/// Step Fail Summay needs...
	////////////////////////////////////////////////////////////////////////////////////////////////

	QString sPT = tr("%1/%2").arg(nSlotTotal).arg(nMaxDut);
	m_pSlotView->item( slot, EN_SI_PT       )->setText( sPT );
	return true;
}
*/

bool CEngrTab::MakeCumFailBlock(const char* pFailBlock, uint maxstep )
{
	uint nMaxDut = m_stDevPgmVars.nBibX * m_stDevPgmVars.nBibY;
	uint nBlockSize = nMaxDut * m_stDevPgmVars.nBlockPerDutBl;

	if( pFailBlock == nullptr ) return false;
	if( m_pCumBlockFail != nullptr )
	{
		delete[] m_pCumBlockFail;
		m_pCumBlockFail = nullptr;
	}
	m_pCumBlockFail = new (std::nothrow)char[ nBlockSize ];
	memset( m_pCumBlockFail, 0x0, sizeof(char)*nBlockSize );
	for(  uint step=0;step<maxstep;step++ )
	{
		for( uint dut=0;dut<nMaxDut;dut++ )
		{
			uint nDutOffset  = dut*m_stDevPgmVars.nBlockPerDutBl;
			uint nStepOffset = nBlockSize*step;
			for( uint blk=0;blk<m_stDevPgmVars.nBlockPerDutBl;blk++ )
			{
				m_pCumBlockFail[nDutOffset+blk] |= pFailBlock[nStepOffset+nDutOffset+blk];
			}
		}
	}
	return true;
}

bool CEngrTab::LoadDutFailBlock( uint slot, uint row, uint col, uint dut )
{
	uint nMaxDutPerCpu  = m_stDevPgmVars.nMaxDutPerCpu;
	uint nBlockPerDutBl = m_stDevPgmVars.nBlockPerDutBl;
	uint nMaxStep0  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU00 );
	uint nMaxStep1  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU01 );
	uint nCePerDut  = m_stDevPgmVars.nCe;
	uint nChipPerCe = m_stDevPgmVars.nChip;
	uint nChipPerDut= nCePerDut*nChipPerCe;
	uint nMaxStep   = nMaxStep0;
	uint nMaxDut    = nMaxDutPerCpu * 2;
	uint nStepBlockSize = nMaxDutPerCpu * nBlockPerDutBl;
	uint nMaxStepBlockSize = 0;

	if( nMaxStep0 > nMaxStep1 ) nMaxStep = nMaxStep1;
	if( nMaxStep0 > nMaxStep1 ) nMaxStep = nMaxStep1;
	if( nMaxStep < 1 ) return false;

	nMaxStepBlockSize = nMaxDut * nBlockPerDutBl * nMaxStep;
	char* pFailBlock = new (std::nothrow)char[ nMaxStepBlockSize ];
	if( !m_pTestDataMgr->GET_STEPFAIL_BLOCK_DATA(slot, CPU00, pFailBlock, nMaxStep, nStepBlockSize ) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}
	if( !m_pTestDataMgr->GET_STEPFAIL_BLOCK_DATA(slot, CPU01, pFailBlock, nMaxStep, nStepBlockSize ) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}
	if( !MakeCumFailBlock( pFailBlock, nMaxStep ) ) return false;

/////////////////////////////////////////////////////////////////////////////////
	int* pStepUseMode = new int[ nMaxStep ];

	int *pStepUseModeCpu[MAX_SYS_CPU] = { nullptr,nullptr };
	pStepUseModeCpu[CPU00] = new int[ nMaxStep ];
	pStepUseModeCpu[CPU01] = new int[ nMaxStep ];

	for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
	{
		if( !m_pTestDataMgr->GET_STEPUSE_MODE(slot, iCpu, pStepUseModeCpu[iCpu], nMaxStep, sizeof(int) ) )
		{
			emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
			return false;
		}
	}

	for(uint iStep=0; iStep<nMaxStep; ++iStep)
	{
		pStepUseMode[iStep] = pStepUseModeCpu[CPU00][iStep] | pStepUseModeCpu[CPU01][iStep];
	}

	for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
	{
		if ( pStepUseModeCpu[iCpu] != nullptr )
		{
			delete [] pStepUseModeCpu[iCpu];
			pStepUseModeCpu[iCpu] = nullptr;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////

	QString sDutData = "";
	QString sDutTemp = "";
	QString sBibId = QString(m_pTestDataMgr->GET_BIBID(slot));
	for( uint nCe=0;nCe<nCePerDut;nCe++ )
	{
		for( uint nChip=0;nChip<nChipPerCe;nChip++ )
		{
			sDutTemp = QString("").asprintf( "Slot%02d.(%s)R%02d, C%02d, CE%02d, CHIP%02d",
					 slot+1, sBibId.toStdString().c_str(), row+1, col+1, nCe+1, nChip+1 );
			sDutData += sDutTemp;

			sDutTemp = QString("").asprintf( "     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL",
						m_pstChipId[dut*nChipPerDut + nCe*nChipPerCe + nChip].szchipId,
						m_pstChipId[dut*nChipPerDut + nCe*nChipPerCe + nChip].uWaferId,
						m_pstChipId[dut*nChipPerDut + nCe*nChipPerCe + nChip].uXPos   ,
						m_pstChipId[dut*nChipPerDut + nCe*nChipPerCe + nChip].uYPos  );
			sDutData += sDutTemp + CRLF;

			sDutTemp = QString("").asprintf( "Cum(%s/%s)                     ",
											(IsChipFail(m_pCumFailChip, dut)?"FAIL":"PASS"), (IsChipFail(m_pCumFailChip, dut, nCe, nChip)?"FAIL":"PASS") );
			sDutData += sDutTemp;
			std::vector<uint> vBlkAddr;
			GetFailBlockAddress( vBlkAddr, m_pCumBlockFail, dut, nCe, nChip );
			sDutTemp = QString("").asprintf( "%dEA", (int)vBlkAddr.size() );
			sDutData += sDutTemp;
			int nFailBlk = (int)vBlkAddr.size();
			if ( nFailBlk > 254 ) nFailBlk = 254;
			for(int i=0; i<nFailBlk; ++i)
			{
				sDutTemp = QString("").asprintf( " %x ", vBlkAddr[i] );
				sDutData += sDutTemp;
			}
			sDutData += CRLF;

			std::vector<std::string> stepnames = m_pTestDataMgr->GET_STEPNAMES( slot, CPU00 );
			std::vector<uint> vCumBlkAddr;
			vCumBlkAddr.clear();
			if( stepnames.size() < nMaxStep ) nMaxStep = stepnames.size();
			for(uint iStep=0; iStep<nMaxStep; ++iStep)
			{
				QString sStep = "";
				sStep = QString("").asprintf( "%02d.%s%s", iStep+1, stepnames[iStep].c_str(), "(N)" );
				sDutTemp = QString("").asprintf( "%-35s", sStep.toStdString().c_str() );
				sDutData += sDutTemp;

				vBlkAddr.clear();
				GetFailBlockAddress(vBlkAddr, pFailBlock, dut, nCe, nChip, iStep);

				if ( vBlkAddr.size()<1 && IsChipFail(m_pFailChip, dut, nCe, nChip, iStep) )
				{
					if ( (pStepUseMode[iStep])&0x2) sDutTemp = QString("").asprintf( "%dEA"      , 0 );
					else                            sDutTemp = QString("").asprintf( "%dEA FAIL ", 1 );
					sDutData += sDutTemp + CRLF;
				}
				else
				{
					sDutTemp = QString("").asprintf( "%dEA", (int)vBlkAddr.size() );
					sDutData += sDutTemp;
					nFailBlk = (int)vBlkAddr.size();
					if ( nFailBlk > 253 ) nFailBlk = 253;
					for(int i=0; i<nFailBlk; ++i)
					{
						sDutTemp = QString("").asprintf( " %x%c", vBlkAddr[i], !IsIn(vCumBlkAddr, vBlkAddr[i])?'!':' ' );
						sDutData += sDutTemp;
					}
					if ( vBlkAddr.size() > 253 ) sDutData += " ...";
					sDutData += CRLF;
				}
				mergeFailBlockInfo(vCumBlkAddr, vBlkAddr);
			}
		}
	}
	AppendDutDataTextEdit( _NORMAL_LOG, sDutData );

	if( pFailBlock != nullptr )
	{
		delete[] pFailBlock;
		pFailBlock = nullptr;
	}

	if( pStepUseMode != nullptr )
	{
		delete[] pStepUseMode;
		pStepUseMode = nullptr;
	}

	return true;
}

void CEngrTab::SlotCellOfBibMapClicked(int r, int c )
{
	int dut = 0x0;
	int col_count = ui->tblBibMap->columnCount();
	int row_count = ui->tblBibMap->rowCount();
	uint slot = m_nCurrentSlot;

	uint nMaxStep0  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU00 );
	uint nMaxStep1  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU01 );
	uint nMaxStep   = nMaxStep0;
	if( nMaxStep0 > nMaxStep1 ) nMaxStep = nMaxStep1;

	dut = (c*row_count)+r;
	ui->leDutXPos->setText( tr("%1").arg(r+1) );
	ui->leDutYPos->setText( tr("%1").arg(c+1) );
	ui->leDutDutNo->setText( tr("%1").arg(dut+1) );

	ClearDutDataTextEdit();
	AppendDutDataTextEdit( _NORMAL_LOG,
						   tr("Slot#%1 R%2 C%3").arg(slot+1, 2, 10, QChar('0')).arg(r+1, 2, 10, QChar('0')).arg(c+1, 2, 10, QChar('0')) );

	if( !LoadDevPgmVar( slot, CPU00) )
	{
		AppendDutDataTextEdit( _ERROR_LOG,
							   tr("Slot#%1 Pgm variable load error!!").arg(slot+1, 2, 10, QChar('0')) );
		return;
	}
	if( m_pInitFail == nullptr ) return;
	if( m_pInitFail[dut] ) return;

	if( !LoadChipInfo( slot ) )
	{
		AppendDutDataTextEdit( _ERROR_LOG,
							   tr("Slot#%1 R%2 C%3 chip data load error!").arg(slot+1, 2, 10, QChar('0')).arg(r+1, 2, 10, QChar('0')).arg(c+1, 2, 10, QChar('0')) );
		return;
	}

	if( !LoadFailChip( slot ) )
	{
		AppendDutDataTextEdit( _ERROR_LOG,
							   tr("Slot#%1 fail chip load error!").arg(slot+1, 2, 10, QChar('0')) );
		return;
	}

	if( !MakeCumFailChip( m_pFailChip, nMaxStep ) )
	{
		AppendDutDataTextEdit( _ERROR_LOG,
							   tr("Slot#%1 make cumfail chip error!").arg(slot+1, 2, 10, QChar('0')) );
		return;
	}

	if( !LoadDutFailBlock( slot, r, c, dut ) )
	{
		AppendDutDataTextEdit( _ERROR_LOG,
							   tr("Slot#%1 block fail load error!").arg(slot+1, 2, 10, QChar('0')) );
		return;
	}
	//DisplayDutFailBlock( slot, dut );
}

void CEngrTab::SlotCellOfBibMapChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	return;
	int dut = 0x0;
	int col_count = ui->tblBibMap->columnCount();
	int row_count = ui->tblBibMap->rowCount();
	dut = (currentColumn*row_count)+currentRow+1;
	ui->leDutXPos->setText( tr("%1").arg(currentRow+1) );
	ui->leDutYPos->setText( tr("%1").arg(currentColumn+1) );
	ui->leDutDutNo->setText( tr("%1").arg(dut) );
}


void CEngrTab::SlotPgmLogMessage( uint slot, uint runner, LOG_TYPE type, QString s )
{
	QStringList list = s.split("\n");
	for(QString str : list )
	{
		if( str.trimmed() == "" ) continue;

		AppendPgmTextEdit( slot, runner, type, str );				
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CEngrTab::SlotPgmLoad
/// \details from main ui
void CEngrTab::SlotPgmLoad()
{
	for( int s=0;s<MAX_SLOT;s++ )
	{
		for( int r=0;r<MAX_SYS_CPU;r++)
		{
			m_pSlotTextEdit[s][r]->clear();
		}
	}
	ui->cboStep->clear();
	ReInitBibMapView( m_nBibRows, m_nBibCols );
	ClearDutDataTextEdit();
}

void CEngrTab::SlotPgmLoadDone()
{
	for( int s=0;s<MAX_SLOT;s++ )
	{
		for( int r=0;r<MAX_SYS_CPU;r++)
		{
			m_pSlotTextEdit[s][r]->clear();
		}
	}
	//ReInitBibMapView( m_nBibRows, m_nBibCols );
	//ClearDevVarTextEdit();
	//ClearDutDataTextEdit();
}

void CEngrTab::ClearDutDataTextEdit()
{
	QPlainTextEdit* pEditor = ui->teDutData;
	if( pEditor != nullptr )
		pEditor->clear();
}

void CEngrTab::AppendDutDataTextEdit( LOG_TYPE t, QString s)
{
	QPlainTextEdit* pEditor = ui->teDutData;
	if( pEditor == nullptr ) return;

	if( pEditor->document()->blockCount()> MAX_LOG_LINE_NUM )
		pEditor->clear();
	QTextCharFormat tf;
	tf = pEditor->currentCharFormat();
	tf.setForeground( FAIL_TEXT_QCOLOR );

	QString sLog = s;
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
	QTextCursor c = pEditor->textCursor();
	c.movePosition(QTextCursor::Start);
	pEditor->setTextCursor(c);
}

void CEngrTab::AppendPgmTextEdit ( uint slot, uint runner, LOG_TYPE t, QString msg )
{
	QPlainTextEdit* pEditor = m_pSlotTextEdit[slot][runner];

	if( pEditor->document()->blockCount()> MAX_LOG_LINE_NUM )
		pEditor->clear();
	QTextCharFormat tf;
	tf = pEditor->currentCharFormat();
	tf.setForeground( FAIL_TEXT_QCOLOR );

	QString sLog = UTIL::DATE::yyMMDDHHmmss(true) + msg;
	if( t == _NORMAL_LOG )
	{
		tf.setForeground( PASS_TEXT_QCOLOR );
		 pEditor->setCurrentCharFormat( tf );
	}
	else if( t == _ERROR_LOG )
	{
		tf.setForeground( FAIL_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	else
	{
		tf.setForeground( FAULT_TEXT_QCOLOR );
		pEditor->setCurrentCharFormat( tf );
	}
	pEditor->appendPlainText( sLog );

	if( m_bAutoScrollSlot[slot][runner] )
	{
		QTextCursor c = pEditor->textCursor();
		c.movePosition(QTextCursor::End);
		pEditor->setTextCursor(c);
	}
	emit SigSaveSlotLog( slot, runner, t, sLog );
}

void CEngrTab::SlotSlotSetupResult( int slot, bool setup_ok, QString bibid, int total, int pass, QString part, QString lot )	//Setup Result
{
	/*
	int offset  = slot;
	QString sPT = tr("%1/%2").arg(pass).arg(total);

	m_pSlotView->item( offset, EN_SI_BIBID    )->setText( bibid );
	m_pSlotView->item( offset, EN_SI_PT       )->setText( sPT );
	m_pSlotView->item( offset, EN_SI_PARTNAME )->setText( part );
	m_pSlotView->item( offset, EN_SI_LOTNAME  )->setText( lot );
	if( !setup_ok )
	{
		m_pSlotView->item( offset, EN_SI_BIBID    )->setBackground( FAIL_BG_QCOLOR );
		m_pSlotView->item( offset, EN_SI_PT       )->setBackground( FAIL_BG_QCOLOR );
		m_pSlotView->item( offset, EN_SI_PARTNAME )->setBackground( FAIL_BG_QCOLOR );
		m_pSlotView->item( offset, EN_SI_LOTNAME  )->setBackground( FAIL_BG_QCOLOR );
		QProgressBar* pItem = static_cast<QProgressBar*>( m_pSlotView->cellWidget( offset, EN_SI_PROGRESS) );
		if( pItem )
		{
			pItem->setStyleSheet( PASS_PRGBAR_QSS );
			pItem->setToolTip("Setup error!");
			pItem->setValue( 0 );
		}
	}
	else
	{
		m_pSlotView->item( offset, EN_SI_BIBID    )->setBackground( PASS_BG_QCOLOR );
		m_pSlotView->item( offset, EN_SI_PT       )->setBackground( PASS_BG_QCOLOR );
		m_pSlotView->item( offset, EN_SI_PARTNAME )->setBackground( PASS_BG_QCOLOR );
		m_pSlotView->item( offset, EN_SI_LOTNAME  )->setBackground( PASS_BG_QCOLOR );
		QProgressBar* pItem = static_cast<QProgressBar*>( m_pSlotView->cellWidget( offset, EN_SI_PROGRESS) );
		if( pItem )
		{
			pItem->setStyleSheet( PASS_PRGBAR_QSS );
			pItem->setToolTip("Setup OK");
			pItem->setValue( 0 );
		}
	}

	//test
	if( !m_pTestDataMgr->LOT_SET( lot.toStdString().c_str() ) )
	{
		emit SigSaveSysLog( _ERROR_LOG, m_pTestDataMgr->GET_ERRMSG() );
		emit SigAppendSystemLog( _ERROR_LOG, m_pTestDataMgr->GET_ERRMSG() );
	}
	else
	{
		emit SigSaveSysLog( _NORMAL_LOG, tr("LOT_SET : %1").arg(lot) );
		emit SigAppendSystemLog( _NORMAL_LOG, tr("LOT_SET : %1").arg(lot) );
	}
	if( ui->tblSlotView1->currentRow() == slot )
		DisplaySlotInfo( slot );
		*/
}

void CEngrTab::SlotInitTestDone( uint update_slot )
{
	//if( m_nCurrentSlot != last_slot ) return;
	if( m_pTestDataMgr == nullptr ) return;

	uint slot = m_nCurrentSlot;
	if( slot>(MAX_SLOT-1) ) return;

	//DisplaySlotViewPassTotal( update_slot );
	if( m_pTestDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
	{
		uint bibx = m_pTestDataMgr->GET_BIBX( slot, CPU00 );
		uint biby = m_pTestDataMgr->GET_BIBY( slot, CPU00 );
		if( update_slot == slot )
		{
			ReInitBibMapView( bibx, biby );
			DisplayBibmapTestData( slot );
		}
		//ui->tblSlotView1->setCurrentCell(last_slot, EN_SI_BIBID );
		//DisplayBibmapTestData( slot );
	}
	else
	{
		//emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT#%1 has no DevPgm Variables").arg(update_slot) );
	}
	m_bHasItemList = false;
}

void CEngrTab::SlotTestItemListup(uint slot)
{
	if( m_pTestDataMgr == nullptr ) return;
	if( m_nCurrentSlot != slot ) return;

	if( !m_bHasItemList )
	{
		DisplayTestItemList( slot );
		m_bHasItemList = true;
	}
}

//TEST ITEM ComboBox Changed
void CEngrTab::SlotItemSelectChanged(int idx)
{
	if( idx < 0 ) return;
	DisplayBibmapTestData( m_nCurrentSlot );
}

void CEngrTab::SlotViewPgmVariables( uint last_slot )
{
	if( m_pTestDataMgr == nullptr ) return;
	if( last_slot>(MAX_SLOT-1) ) return;

	ClearDutDataTextEdit();
}

void CEngrTab::DisplayTestItemList(uint last_slot)
{
	ui->cboStep->clear();
	if( !m_pTestDataMgr->IS_EXIST_TESTITEM(last_slot) )
	{
		//emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT#%1 has no TestItem Variables").arg(last_slot+1) );
		//emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return;
	}
	stTestItem testitem;
	if( !m_pTestDataMgr->GET_TESTITEM( last_slot, CPU00, testitem) )
	{
		AppendPgmTextEdit( last_slot, CPU00, _ERROR_LOG, tr("SLOT#%1 %2").arg(last_slot+1).arg(m_pTestDataMgr->GET_ERRMSG()) );
		return;
	}
	QStringList itemlist;
	itemlist.clear();
	stInitTestHead initHead;
	if( !m_pTestDataMgr->GET_INITTEST_HEAD( last_slot, CPU00, initHead) )
	{
		AppendPgmTextEdit( last_slot, CPU00, _ERROR_LOG, tr("SLOT#%1 %2").arg(last_slot+1).arg(m_pTestDataMgr->GET_ERRMSG()) );
		//emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT#%1 %2").arg(last_slot+1).arg(m_pTestDataMgr->GET_ERRMSG()) );
		return;
	}
	std::list<stStepOutInfo> stepout0;
	std::list<stStepOutInfo> stepout1;
	m_pTestDataMgr->GET_STEPOUT_LIST( last_slot, CPU00, &stepout0 );
	m_pTestDataMgr->GET_STEPOUT_LIST( last_slot, CPU01, &stepout1 );

	//Init Item Append
	int nStepNo = 0;
	QString sItem = QString("%1 %2.%3").arg("> ").arg(++nStepNo,3,10,QChar('0')).arg(initHead.szStepName);
	itemlist.append( sItem );
	for( uint i=0;i<testitem.nItemCount;i++ )
	{
		bool bDone0 = false, bDone1 = false;
		for( stStepOutInfo outinfo : stepout0 )
		{
			if( outinfo.nStepNo == i )
			{
				bDone0 = true;
				break;
			}
		}
		for( stStepOutInfo outinfo : stepout1 )
		{
			if( outinfo.nStepNo == i )
			{
				bDone1 = true;
				break;
			}
		}
		sItem = QString("%1 %2.%3").arg( (bDone0&&bDone1)?"> ":"# ").arg(++nStepNo,3,10,QChar('0')).arg(testitem.ItemStep[i].szItemName);
		itemlist.append( sItem );
	}
	sItem = QString("%1.%2").arg(0,3,10,QChar('0')).arg("All steps");
	itemlist.append( sItem );

	ui->cboStep->addItems( itemlist );
	//ui->cboStep->setCurrentIndex( last_item );
}

bool CEngrTab::GetDutToXy(int dut, int& r, int& c)
{
	bool bFound = false;
	for( int y=0;y<m_nBibRows;y++ )
	{
		for( int x=0;x<m_nBibCols;x++ )
		{
			if( DUTMAP_16x12_ui[y][x] == dut )
			{
				r = y;
				c = x;
				bFound = true;
				break;
			}
		}
	}
	return bFound;
}

bool CEngrTab::LoadDevPgmVar( uint slot, uint cpu )
{
	return m_pTestDataMgr->GET_DEVPGM_VAR( slot, cpu, &m_stDevPgmVars );
}

bool CEngrTab::LoadInitFail(uint slot)
{
	int nMaxDutPerCpu = m_stDevPgmVars.nMaxDutPerCpu;
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	int max_dut = bibx * biby;

	if( nMaxDutPerCpu == -1 ) return false;
	if( max_dut == -1 ) return false;

	//Init Fail
	if( m_pInitFail != nullptr )
	{
		delete[] m_pInitFail;
		m_pInitFail = nullptr;
	}
	m_pInitFail = new char[max_dut];
	memset( m_pInitFail, 0x0, max_dut );

	for( int cpu=0;cpu<2;cpu++ )
	{
		if( !m_pTestDataMgr->GET_INITTEST_FAIL( slot, cpu, m_pInitFail+(cpu*nMaxDutPerCpu), nMaxDutPerCpu ) )
		{
			emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT%1-%2 INITDATA GET ERROR:%3").arg(slot+1).arg(cpu+1).arg(m_pTestDataMgr->GET_ERRMSG()) );
			continue;
		}
		uint dut = 0x0;
		int row  = 0x0;
		int col  = 0x0;
		char* pInitPtr  = m_pInitFail+(cpu*nMaxDutPerCpu);
		for( int idx=0;idx<nMaxDutPerCpu;idx++ )
		{
			dut = (cpu*nMaxDutPerCpu)+idx;
			if( !GetDutToXy( dut, row, col ) ) continue;
			QTableWidgetItem* dutItem = ui->tblBibMap->item( row,col );
			if( dutItem )
			{
				if( pInitPtr )
				{
					if( (*pInitPtr) == 0x1 ) dutItem->setBackground( INIT_TEST_QCOLOR );
					else                     dutItem->setBackground( PASS_TEST_QCOLOR );
				}
				else
				{
					emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT%1-%2 INITDATA PTR ERROR").arg(slot+1).arg(cpu+1) );
				}
				pInitPtr++;
			}
		}
	}
	return true;
}

bool CEngrTab::LoadChipInfo(uint slot)
{
	int nMaxDutPerCpu = m_stDevPgmVars.nMaxDutPerCpu;
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	int chipperdut = m_stDevPgmVars.nChipPerDut;
	int max_dut = bibx * biby;

	uint nChipDataSize  = max_dut * chipperdut;
	uint nDutPerCpuSize = nMaxDutPerCpu * chipperdut;
	uint nChipIdSize    = sizeof(stChipId);
	if( m_pstChipId != nullptr )
	{
		delete[] m_pstChipId;
		m_pstChipId = nullptr;
	}
	m_pstChipId = new (std::nothrow)stChipId[ nChipDataSize ];
	if( !m_pTestDataMgr->GET_CHIPID_DATA(slot, CPU00, (char*)m_pstChipId, nDutPerCpuSize, nChipIdSize ) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}
	if( !m_pTestDataMgr->GET_CHIPID_DATA(slot, CPU01, (char*)(m_pstChipId+nDutPerCpuSize), nDutPerCpuSize, nChipIdSize ) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}
	return true;
}

bool CEngrTab::MakeCumFailChip(const char* pFailChip, uint maxstep )
{
	uint nMaxDut = m_stDevPgmVars.nBibX * m_stDevPgmVars.nBibY;
	uint nChipDataSize = nMaxDut * m_stDevPgmVars.nChipPerDutBl;

	if( pFailChip == nullptr ) return false;
	if( m_pCumFailChip != nullptr )
	{
		delete[] m_pCumFailChip;
		m_pCumFailChip = nullptr;
	}
	m_pCumFailChip = new (std::nothrow)char[ nChipDataSize ];
	memset( m_pCumFailChip, 0x0, sizeof(char)*nChipDataSize );
	for(  uint step=0;step<maxstep;step++ )
	{
		uint nStepOffset = nChipDataSize*step;
		for( uint dut=0;dut<nMaxDut;dut++ )
		{
			uint nDutOffset  = dut*m_stDevPgmVars.nChipPerDutBl;
			for( uint blk=0;blk<m_stDevPgmVars.nChipPerDutBl;blk++ )
			{
				//if( pFailChip[nStepOffset+nDutOffset+blk] )
				//	AppendDutDataTextEdit( _NORMAL_LOG, tr("step:%1 dut:%2 blk:%3 data:0x%4").arg(step).arg(dut).arg(blk).arg(pFailChip[nStepOffset+nDutOffset+blk], 2, 16, QChar('0')) );
				m_pCumFailChip[nDutOffset+blk] |= pFailChip[nStepOffset+nDutOffset+blk];
			}
		}
	}
	return true;
}

bool CEngrTab:: LoadFailChip(uint slot)
{
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	int max_dut = bibx * biby;
	uint nMaxStep0  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU00 );
	uint nMaxStep1  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU01 );
	uint nMaxStep   = nMaxStep0;
	if( nMaxStep0 > nMaxStep1 ) nMaxStep = nMaxStep1;
	uint nStepChipDataSize = max_dut * m_stDevPgmVars.nChipPerDutBl * nMaxStep;
	uint nCpuChipDataSize  = (max_dut/2) * m_stDevPgmVars.nChipPerDutBl;

	if( nMaxStep < 1 ) return false;
	if( max_dut  < 1 ) return false;

	if( m_pFailChip != nullptr )
	{
		delete[] m_pFailChip;
		m_pFailChip = nullptr;
	}
	m_pFailChip = new (std::nothrow)char[ nStepChipDataSize ];
	if( !m_pTestDataMgr->GET_STEPFAIL_CHIP_DATA( slot, CPU00, m_pFailChip, nMaxStep, nCpuChipDataSize) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}
	if( !m_pTestDataMgr->GET_STEPFAIL_CHIP_DATA( slot, CPU01, m_pFailChip, nMaxStep, nCpuChipDataSize) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}

	return true;
}

bool CEngrTab::LoadStepFail(uint slot, uint item, uint item_count)
{
	uint nMaxDutPerCpu = m_stDevPgmVars.nMaxDutPerCpu;
	uint nMaxDut       = nMaxDutPerCpu * 2;
	uint nStep0   = m_pTestDataMgr->GET_MAX_STEP( slot, CPU00 );
	uint nStep1   = m_pTestDataMgr->GET_MAX_STEP( slot, CPU01 );
	uint nStep    = nStep0;
	bool bAllStep = false;

	if( item_count < 1 ) return false;
	if( (item == 0) || (item == (item_count-1) ) ) bAllStep = true;
	if( nStep0 > nStep1 ) nStep = nStep1;

	if( nStep < 1 ) return false;
	if( !bAllStep )
	{
		if( item > nStep ) return false;
		item -= 1;
	}

	if( m_pStepFail )
	{
		delete[] m_pStepFail;
		m_pStepFail = nullptr;
	}
	m_pStepFail = new char[nMaxDut*nStep];
	char* pStepFail = new char[nMaxDut*nStep];
	memset( m_pStepFail, 0x0, nMaxDut );
	memset( pStepFail  , 0x0, nMaxDut*nStep );

	m_pTestDataMgr->GET_STEPFAIL_DATA( slot, CPU00, pStepFail, nStep, nMaxDutPerCpu );
	m_pTestDataMgr->GET_STEPFAIL_DATA( slot, CPU01, pStepFail, nStep, nMaxDutPerCpu );
	if( bAllStep )
	{
		uint cnt  = 0;
		uint pass = 0;
		uint fail = 0;
		for( uint d=0;d<nMaxDut;d++ )
		{
			for( uint i=0;i<nStep;i++ )
			{
				m_pStepFail[d] |= pStepFail[d+(i*nMaxDut)];
			}
			//YIELD, PASS, FAIL
			if( m_pInitFail != nullptr )
			{
				if( m_pInitFail[d] ) continue;
			}
			if( m_pStepFail[d] ) fail += 1;
			else                 pass += 1;
			cnt++;
		}
		double dYield = static_cast<double>(pass) / static_cast<double>(cnt) * 100.0;
		ui->leSlotPassCnt->setText( tr("%1").arg(pass) );
		ui->leSlotFailCnt->setText( tr("%1").arg(fail) );
		ui->leSlotYield->setText  ( QString("").asprintf("%5.1f", dYield) );
	}
	else
	{
		for( uint d=0;d<nMaxDut;d++ )
		{
			m_pStepFail[d] |= pStepFail[d+(item*nMaxDut)];
		}
	}
	return true;
}

bool CEngrTab::DisplayBibSortBin(uint slot)
{
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	int max_dut = bibx * biby;
	int row  = 0x0;
	int col  = 0x0;

	char* pInitPtr  = m_pInitFail;
	for( int dut=0;dut<max_dut;dut++ )
	{
		if( !GetDutToXy( dut, row, col ) ) continue;
		QTableWidgetItem* dutItem = ui->tblBibMap->item( row,col );
		if( dutItem )
		{
			if( pInitPtr )
			{
				if( (*pInitPtr) == 0x1 ) dutItem->setBackground( INIT_TEST_QCOLOR );
				else
				{
					dutItem->setText(tr("%1").arg(m_pSortBin[dut]));
					dutItem->setToolTip( tr("%1").arg(m_pSortBin[dut]) );
					if( m_pSortBin[dut] > 5 )
					{
						dutItem->setBackground( FAIL_TEST_QCOLOR );
					}
					else
					{
						dutItem->setBackground( PASS_TEST_QCOLOR );
					}
				}
			}
			else
			{
				emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT%1 Step fail display error!").arg(slot+1) );
			}
			pInitPtr++;
		}
	}
	return true;
}

bool CEngrTab::DisplayBibStepFail( uint slot)
{
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	int max_dut = bibx * biby;
	int row  = 0x0;
	int col  = 0x0;

	char* pInitPtr  = m_pInitFail;
	for( int dut=0;dut<max_dut;dut++ )
	{
		if( !GetDutToXy( dut, row, col ) ) continue;
		QTableWidgetItem* dutItem = ui->tblBibMap->item( row,col );
		if( dutItem )
		{
			dutItem->setText(tr(""));
			dutItem->setToolTip(tr(""));
			if( pInitPtr )
			{
				if( (*pInitPtr) == 0x1 ) dutItem->setBackground( INIT_TEST_QCOLOR );
				else
				{
					if( m_pStepFail[dut] == 0x1 )
					{
						dutItem->setBackground( FAIL_TEST_QCOLOR );
					}
					else
					{
						dutItem->setBackground( PASS_TEST_QCOLOR );
					}
				}
			}
			else
			{
				emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT%1 Step fail display error!").arg(slot+1) );
			}
			pInitPtr++;
		}
	}
	return true;
}

bool CEngrTab::IsFailBlockIn(uint block)
{
	for(int i=0; i<(int)m_vecFailBlk.size(); ++i)
	{
		if ( m_vecFailBlk[i]==block ) return true;
	}
	m_vecFailBlk.push_back( block );
	return false;
}

bool CEngrTab::IsChipFail(char* pFailChip, uint dut, uint step)
{
	if ( pFailChip==nullptr ) return false;
	uint maxdut = m_stDevPgmVars.nBibX * m_stDevPgmVars.nBibY;
	ssize_t nFailChipSizePerStep = maxdut * m_stDevPgmVars.nChipPerDutBl;

	bool bFail = false;
	for(uint iChip=0; iChip<m_stDevPgmVars.nCe*m_stDevPgmVars.nChip; iChip+=8)
	{
		int chip_index = iChip;
		int m = nFailChipSizePerStep*step + dut * m_stDevPgmVars.nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
		if ( pFailChip[m] )
		{
			bFail = true;
			break;
		}
	}
	return bFail;
}

bool CEngrTab::IsChipFail( char* pFailChip, uint dut, uint ce, uint chip, uint step )
{
	if ( pFailChip==nullptr ) return false;
	uint maxdut = m_stDevPgmVars.nBibX * m_stDevPgmVars.nBibY;
	ssize_t nFailChipSizePerStep = maxdut * m_stDevPgmVars.nChipPerDutBl;	//* sizeof(char)

	int chip_index = ce * m_stDevPgmVars.nChip + chip;
	int m = nFailChipSizePerStep*step + dut * m_stDevPgmVars.nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;														//chip_index % 8;

	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
}

void CEngrTab::mergeFailBlockInfo(std::vector<uint> &vCumBlkAddr, std::vector<uint> &vBlkAddr)
{
	std::vector<uint> vNewBlkAddr;
	for(int i=0; i<(int)vBlkAddr.size(); ++i)
	{
		if ( ! IsIn(vCumBlkAddr, vBlkAddr[i]) )
			vNewBlkAddr.push_back( vBlkAddr[i] );
	}

	for(int i=0; i<(int)vNewBlkAddr.size(); ++i)
		vCumBlkAddr.push_back( vNewBlkAddr[i] );
}

bool CEngrTab::IsIn(std::vector<uint> &vCumBlkAddr, uint iBlkAddr)
{
	for(int i=0; i<(int)vCumBlkAddr.size(); ++i)
	{
		if ( vCumBlkAddr[i]==iBlkAddr ) return true;
	}

	return false;
}

bool CEngrTab::GetFailBlockAddress( std::vector<uint>& vBlkAddr, const char* pFailBlock, uint dut, uint ce, uint chip, uint step )
{
	uint slot = m_nCurrentSlot;
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	uint max_dut = bibx * biby;
	uint nBlockPerDutBl= m_stDevPgmVars.nBlockPerDutBl;						//m_pTestDataMgr->GET_BLOCKPERDUTBL( slot, CPU00 );
	uint nBlockPerCe   = m_stDevPgmVars.nBlockPerCe;						// m_pTestDataMgr->GET_BLOCKPERCE( slot, CPU00 );
	uint nBlockPerchip = m_stDevPgmVars.nBlockPerCe/m_stDevPgmVars.nChip;	//m_stDevPgmVars.nBlock m_pTestDataMgr->GET_BLOCKPERCHIP( slot, CPU00 );

	ssize_t nFailBlockSizePerStep = max_dut * nBlockPerDutBl;
	uint iOffset = nFailBlockSizePerStep*step + dut*nBlockPerDutBl;
	uint minaddr = ce * nBlockPerCe + chip * nBlockPerchip;
	uint maxaddr = ce * nBlockPerCe + chip * nBlockPerchip + nBlockPerchip - 1;

	for(uint iBlock=minaddr; iBlock<=maxaddr; ++iBlock)
	{
		int m = iOffset + (iBlock>>3);
		if ( ! pFailBlock[m] )
		{
			iBlock += 7 - (iBlock%8);
			continue;
		}

		int n = iBlock&0x7;
		bool bFail = static_cast<bool>( (pFailBlock[m] >> n) & 1);
		if ( bFail )
		{
			vBlkAddr.push_back( iBlock );
		}
	}
	return true;
}

uint CEngrTab::GetStepFailDutBlockCount( uint slot, uint maxdut, uint dut, uint step, const char* pFail )
{
	(void)slot;
	uint nStepFailCount = 0;
	uint nBlockPerDutBl = m_stDevPgmVars.nBlockPerDutBl;	//m_pTestDataMgr->GET_BLOCKPERDUTBL( slot, CPU00);
	uint nDutBlockSize  = m_stDevPgmVars.nBlockPerDut;		//m_pTestDataMgr->GET_BLOCKPERDUT( slot, CPU00 );
	uint nFailBlockSizePerStep = maxdut * nBlockPerDutBl;
	uint nStepOffset    = (step*nFailBlockSizePerStep) + (dut*nBlockPerDutBl);
	for( uint blk=0;blk<nDutBlockSize;blk++ )
	{
		int m = nStepOffset + (blk>>3);
		int n = blk & 0x7;
		//block 8ea no fail
		if( !pFail[m] )
		{
			blk += 7 - (blk%8);
			continue;
		}
		bool bFail = static_cast<bool>( (pFail[m] >> n) & 1);
		if( bFail )
		{
			if( !IsFailBlockIn(blk) )
				nStepFailCount++;
		}
	}
	return nStepFailCount;
}

//FAIL CNT - BIB VIEW
bool CEngrTab::LoadFailCnt(uint slot, uint item, uint item_count)
{
	uint nMaxDutPerCpu  = m_stDevPgmVars.nMaxDutPerCpu;
	uint nBlockPerDutBl = m_stDevPgmVars.nBlockPerDutBl;
	uint nMaxStep0  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU00 );
	uint nMaxStep1  = m_pTestDataMgr->GET_MAX_STEP( slot, CPU01 );
	uint nMaxStep   = nMaxStep0;
	uint nMaxDut    = nMaxDutPerCpu * 2;
	uint nStepBlockSize = nMaxDutPerCpu * nBlockPerDutBl;
	uint nMaxStepBlockSize = 0;
	bool bAllStep  = false;

	if( nMaxStep0 > nMaxStep1 ) nMaxStep = nMaxStep1;
	if( item_count < 1 ) return false;
	if( (item == 0) || (item == (item_count-1) ) ) bAllStep = true;
	if( nMaxStep0 > nMaxStep1 ) nMaxStep = nMaxStep1;

	if( nMaxStep < 1 ) return false;
	if( !bAllStep )
	{
		if( item > nMaxStep ) return false;
		item -= 1;	//TN
	}
	if( m_pFailCnt )
	{
		delete[] m_pFailCnt;
		m_pFailCnt = nullptr;
	}

	nMaxStepBlockSize = nMaxDut * nBlockPerDutBl * nMaxStep;
	m_pFailCnt = new uint[nMaxDut];
	char* pFailBlock = new (std::nothrow)char[ nMaxStepBlockSize ];
	if( !m_pTestDataMgr->GET_STEPFAIL_BLOCK_DATA(slot, CPU00, pFailBlock, nMaxStep, nStepBlockSize ) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}
	if( !m_pTestDataMgr->GET_STEPFAIL_BLOCK_DATA(slot, CPU01, pFailBlock, nMaxStep, nStepBlockSize ) )
	{
		emit SigAppendSystemLog( _ERROR_LOG, tr("%1").arg(m_pTestDataMgr->GET_ERRMSG()) );
		return false;
	}
	uint startStep = 0;
	uint endStep   = nMaxStep-1;
	if( !bAllStep )
	{
		startStep = item;
		endStep   = item;
	}
	memset( m_pFailCnt, 0x0, sizeof(uint)*nMaxDut );
	for( uint dut=0;dut<nMaxDut;dut++ )
	{
		m_vecFailBlk.clear();
		for(uint iStep=startStep; iStep<=endStep; ++iStep)
		{
			m_pFailCnt[dut] += GetStepFailDutBlockCount( slot, nMaxDut, dut, iStep, pFailBlock );
		}
	}
	if( pFailBlock != nullptr )
	{
		delete[] pFailBlock;
		pFailBlock = nullptr;
	}
	m_vecFailBlk.clear();
	return true;
}

bool CEngrTab::DisplayBibFailCnt( uint slot)
{
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	//int limit = m_pTestDataMgr->GET_BLOCKLIMITPERCE( slot, CPU00 );
	int max_dut = bibx * biby;
	int row  = 0x0;
	int col  = 0x0;

	char* pInitPtr  = m_pInitFail;
	for( int dut=0;dut<max_dut;dut++ )
	{
		if( !GetDutToXy( dut, row, col ) ) continue;
		QTableWidgetItem* dutItem = ui->tblBibMap->item( row,col );
		if( dutItem )
		{
			dutItem->setText( "" );
			if( pInitPtr )
			{
				if( (*pInitPtr) == 0x1 ) dutItem->setBackground( INIT_TEST_QCOLOR );
				else
				{
					dutItem->setText( tr("%1").arg(m_pFailCnt[dut]) );
					dutItem->setToolTip( tr("%1").arg(m_pFailCnt[dut]) );
					if( m_pStepFail[dut] == 0x1 )
					{
						dutItem->setBackground( FAIL_TEST_QCOLOR );
					}
					else
					{
						dutItem->setBackground( PASS_TEST_QCOLOR );
					}
				}
			}
			else
			{
				emit SigAppendSystemLog( _ERROR_LOG, tr("SLOT%1 Step fail display error!").arg(slot+1) );
			}
			pInitPtr++;
		}
	}
	return true;
}

bool CEngrTab::LoadBin(uint slot)
{
	int nMaxDutPerCpu = m_stDevPgmVars.nMaxDutPerCpu;	// m_pTestDataMgr->GET_DUTPERCPU(slot,CPU00);
	int bibx = m_stDevPgmVars.nBibX;
	int biby = m_stDevPgmVars.nBibY;
	int max_dut = bibx * biby;
	bool bSortRead = false;
	if( m_pSortBin )
		delete[] m_pSortBin;
	m_pSortBin = new uint[max_dut];
	for( int i=0;i<max_dut;i++ ) m_pSortBin[i] = 1;

	bSortRead = m_pTestDataMgr->GET_SORTBIN_DATA(slot, CPU00, m_pSortBin                , sizeof(int)*nMaxDutPerCpu );
	bSortRead = m_pTestDataMgr->GET_SORTBIN_DATA(slot, CPU01, m_pSortBin+nMaxDutPerCpu  , sizeof(int)*nMaxDutPerCpu );
	return true;
}

#define ALLSTEP		(0)
void CEngrTab::DisplayBibmapTestData(uint slot)
{
	InitSlotBibDut();
	if( m_pTestDataMgr == nullptr ) return;
	if( !LoadDevPgmVar( slot, CPU00) ) return;
	if( !LoadInitFail( slot ) )	return;

	uint cboItem = ui->cboStep->currentIndex();
	uint cboSize = ui->cboStep->count();

	if( !LoadStepFail( slot, ALLSTEP, cboSize ) ) return;

	switch( m_ResultType )
	{
		case BTN_NORMAL:
			if( !LoadStepFail( slot, cboItem, cboSize ) ) return;
			DisplayBibStepFail( slot );
			break;
		case BTN_MAKER:
			break;
		case BTN_DEV_TYPE:
			break;
		case BTN_FAIL_CNT:
			if( !LoadFailCnt ( slot, cboItem, cboSize ) ) return;
			DisplayBibFailCnt( slot );
			break;
		case BTN_ADD_FAIL:
			break;
		case BTN_BIN:
			LoadBin( slot );
			DisplayBibSortBin( slot);
			break;
		default:
			break;
	}
}

void CEngrTab::DisplayPgmVariables(uint slot)
{
	/*
	char szText[96]={0x0,};
	memset( szText, 0x0, sizeof(szText) );

	if( !m_pTestDataMgr->IS_EXIST_DEVPGM_VAR( slot ) )
	{
		AppendDevVarTextEdit( _NORMAL_LOG, tr("[SLOT%1-CPU%2 PGM VARIABLES]").arg(slot+1).arg(CPU00+1) );
		AppendDevVarTextEdit( _NORMAL_LOG, tr("No Data") );
		return;
	}

	AppendDevVarTextEdit( _NORMAL_LOG, tr("[SLOT%1-CPU%2 PGM VARIABLES]").arg(slot+1).arg(CPU00+1) );
	if( m_pTestDataMgr->GET_VERSION(slot,CPU00, szText ) )
		AppendDevVarTextEdit( _NORMAL_LOG, tr("VERSION : %1").arg(szText) );
	else
		AppendDevVarTextEdit( _NORMAL_LOG, tr("VERSION : %1").arg("UNDEF") );

	if( m_pTestDataMgr->GET_DEVICE(slot,CPU00, szText ) )
		AppendDevVarTextEdit( _NORMAL_LOG, tr("DEVICE : %1").arg(szText) );
	else
		AppendDevVarTextEdit( _NORMAL_LOG, tr("DEVICE : %1").arg("UNDEF") );

	AppendDevVarTextEdit( _NORMAL_LOG, tr("MAX DUT PER CPU : %1").arg(m_pTestDataMgr->GET_MAXDUTPERCPU(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BIB X           : %1").arg(m_pTestDataMgr->GET_BIBX(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BIB Y           : %1").arg(m_pTestDataMgr->GET_BIBY(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("CE COUNT        : %1").arg(m_pTestDataMgr->GET_CE_COUNT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("CHIP COUNT      : %1").arg(m_pTestDataMgr->GET_CHIP_COUNT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BLOCK PER CE    : %1").arg(m_pTestDataMgr->GET_BLOCKPERCE(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BLOCK PER DUT   : %1").arg(m_pTestDataMgr->GET_BLOCKPERDUT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("CHIP PER DUT    : %1").arg(m_pTestDataMgr->GET_CHIPPERDUT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("DUT PER FC      : %1").arg(m_pTestDataMgr->GET_DUTPERFC(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BLOCK SHIFT     : %1").arg(m_pTestDataMgr->GET_BLOCKSHIFT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("PAGE SHIFT      : %1").arg(m_pTestDataMgr->GET_PAGESHIFT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("COLUMN SIZE     : %1").arg(m_pTestDataMgr->GET_COLSIZE(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BLOCK LIMIT     : %1").arg(m_pTestDataMgr->GET_BLOCKLIMIT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BLOCK LIMIT CE  : %1").arg(m_pTestDataMgr->GET_BLOCKLIMITPERCE(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("ADDR LIMIT      : %1").arg(m_pTestDataMgr->GET_ADDRLIMIT(slot,CPU00)) );
	//AppendDevVarTextEdit( _NORMAL_LOG, tr("BLOCK LIMIT     : %1").arg(m_pTestDataMgr->GET_BLOCKLIMIT(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("FAIL MODE       : %1").arg(m_pTestDataMgr->GET_FAILMODE(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("SCAN INVISIBLE  : %1").arg(m_pTestDataMgr->GET_SCANINVISIBLE(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("BIB SCAN        : %1").arg(m_pTestDataMgr->GET_BIBSCAN(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("IO SIZE         : %1").arg(m_pTestDataMgr->GET_IOSIZE(slot,CPU00)) );
	AppendDevVarTextEdit( _NORMAL_LOG, tr("CPUX2           : %1").arg(m_pTestDataMgr->GET_CPU2X(slot,CPU00)) );
	*/
}

void CEngrTab::SlotTestStartFlag()
{
	m_bTest = true;
	//for( uint slot=0;slot<MAX_SLOT;slot++ )
	//	InitSlotProgress( slot );
}

void CEngrTab::SlotTestEndFlag()
{
	m_bTest = false;
}

void CEngrTab::SlotStepIn(uint slot)
{
	(void)slot;
}

void CEngrTab::SlotStepOut(uint slot)
{
    //2023.08.10 - KWMOON(TEMP COMMENT - UI HANG TEST)
    //if( m_nCurrentSlot == slot )
    //	DisplaySlotInfo( slot );
	//UpdateSlotProgress( slot );
}

void CEngrTab::SlotTestPgmVariableFlag(uint slot)
{
	if( m_nCurrentSlot == slot )
		DisplaySlotInfo( slot );
}

void CEngrTab::InitBtnResult()
{
	for( int i=0;i<BTN_RESULT_COUNT;i++ )
	{
		if( m_arrayBtn[i] == nullptr ) continue;
		if( m_ResultType == static_cast<EN_BTN_RESULT>(i) )
		{
			m_arrayBtn[i]->setStyleSheet( SELECT_BTN_QSS );
		}
		else
		{
			if( m_arrayBtn[i]->isEnabled() )
				m_arrayBtn[i]->setStyleSheet( UNSELECT_BTN_QSS );
			else
				m_arrayBtn[i]->setStyleSheet( DISABLE_BTN_QSS );
		}
	}
}

void CEngrTab::SlotBtnResultKindChange()
{
	QPushButton* sender = static_cast<QPushButton*>( QObject::sender() );
	for( int i=0;i<BTN_RESULT_COUNT;i++ )
	{
		if( sender == m_arrayBtn[i] )
		{
			m_ResultType = static_cast<EN_BTN_RESULT>(i);
			break;
		}
	}
	InitBtnResult();
	DisplayBibmapTestData( m_nCurrentSlot );
}
