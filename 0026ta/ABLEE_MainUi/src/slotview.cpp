#include "slotview.hpp"
#include "ui_slotview.h"

CSlotView::CSlotView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSlotView)
{
	InitVariables();
	InitUi();
}

CSlotView::~CSlotView()
{
	delete ui;
}

void CSlotView::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED( event )
	ResizeSlotView();
	QWidget::resizeEvent( event);
}

void CSlotView::InitVariables()
{
	m_pTestDataMgr = nullptr;
	memset( &m_stSlotInfo, 0x0, sizeof(stSlotInfo) );
	memset( &m_stSlotLive, 0x0, sizeof(stSlotLive) );
}

void CSlotView::InitUi()
{
	ui->setupUi(this);
	InitSlotView();
}

void CSlotView::ResizeSlotView()
{
	int nHeadWidth   = ui->tblSlotView->verticalHeader()->width();
	int nHeadHeight  = ui->tblSlotView->horizontalHeader()->height();
	int nWidth       = ui->tblSlotView->width();
	int nHeight      = ui->tblSlotView->height();
	int nScrolWidth  = 2;
	int nScrolHeight = 2;
	if( ui->tblSlotView->verticalScrollBar()->isVisible() )
	{
		if( ui->tblSlotView->verticalScrollBar()->isVisible() )
			nScrolWidth = ui->tblSlotView->verticalScrollBar()->width();
	}
	if( ui->tblSlotView->horizontalHeader()->isVisible() )
	{
		if( ui->tblSlotView->horizontalScrollBar()->isVisible() )
			nScrolHeight = ui->tblSlotView->horizontalScrollBar()->height();
	}

	int nViewWidth   = static_cast<int>( nWidth  - nHeadWidth - nScrolWidth  );
	int nViewHeight  = static_cast<int>( nHeight - nScrolHeight - nHeadHeight );
	//int nStepHeight  = nViewHeight/ui->tblSlotView->rowCount();

	ui->tblSlotView->setColumnWidth( EN_SI_BIBID     , nViewWidth * 0.15 );
	ui->tblSlotView->setColumnWidth( EN_SI_PT        , nViewWidth * 0.15 );
	ui->tblSlotView->setColumnWidth( EN_SI_PARTNAME  , nViewWidth * 0.25 );
	ui->tblSlotView->setColumnWidth( EN_SI_LOTNAME   , nViewWidth * 0.25 );
	ui->tblSlotView->setColumnWidth( EN_SI_MLB       , nViewWidth * 0.10 );
	ui->tblSlotView->setColumnWidth( EN_SI_PROGRESS  , nViewWidth * 0.10 );
	//for (int row = 0; row<ui->tblSlotView->rowCount(); row++)
	//{
	//	ui->tblSlotView->setRowHeight(row, nStepHeight);
	//}
}

void CSlotView::InitSlotView()
{
	ui->tblSlotView->setRowCount( SLOT_VIEW_MAX_ROWS*2 );
	ui->tblSlotView->clearContents();
	ui->tblSlotView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->tblSlotView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tblSlotView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tblSlotView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tblSlotView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tblSlotView->setAutoScroll(false);
	ui->tblSlotView->setSortingEnabled(false);
	ui->tblSlotView->horizontalHeader()->setVisible(true);
	ui->tblSlotView->horizontalHeader()->setFixedHeight( DEFAULT_HHEAD_HEIGHT );
	ui->tblSlotView->horizontalHeader()->setStretchLastSection(true);
	ui->tblSlotView->verticalHeader()->setVisible(true);
	ui->tblSlotView->verticalHeader()->setFixedWidth( DEFAULT_VHEAD_WIDTH );
	ui->tblSlotView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tblSlotView->setCurrentCell( 0, 0 );
	//m_pSlotView[s]->verticalHeader()->setStretchLastSection(true);
	connect( ui->tblSlotView  , &QTableWidget::cellClicked, this, &CSlotView::SlotCellOfSlotClicked   );
	InitSlotData();
}

void CSlotView::InitSlotData()
{
	//QSize IconSize (20, 20);
	for( int r=0;r<ui->tblSlotView->rowCount();r++ )
	{
		QString sHeader = tr("#%1").arg(r+1);
		QTableWidgetItem* item = new QTableWidgetItem( sHeader );
		item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
		ui->tblSlotView->setVerticalHeaderItem( r, item );
		for( int c=0;c<ui->tblSlotView->columnCount();c++ )
		{
			if( c != EN_SI_PROGRESS )
			{
				QTableWidgetItem* item = new QTableWidgetItem;
				QFont font = item->font();
				font.setPointSize( 7 );
				item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
				item->setFlags( item->flags() & ~Qt::ItemIsEditable );
				item->setText( tr("") );
				item->setBackground( PASS_BG_QCOLOR );
				item->setFont( font );
				ui->tblSlotView->setItem(r, c, item );
			}
			else
			{
				QProgressBar* item = new QProgressBar;
				QFont font = item->font();
				font.setPointSize( 7 );
				item->setFont( font );
				item->setMinimum( 0 );
				item->setMaximum( 100 );
				item->setValue( 0 );
				item->setStyleSheet( PASS_PRGBAR_QSS );
				ui->tblSlotView->setCellWidget( r, c, item );
			}
		}
	}
}

void CSlotView::SetTestDataMgrPtr(CFileMap* ptr)
{
	m_pTestDataMgr = ptr;
}

void CSlotView::SetSlotInfo(stSlotInfo& rSlotInfo)
{
	memcpy( &m_stSlotInfo, &rSlotInfo, sizeof(stSlotInfo) );
}

void CSlotView::SetSlotLive(stSlotLive& rSlotLive)
{
	memcpy( &m_stSlotLive, &rSlotLive, sizeof(stSlotLive) );
}

void CSlotView::SlotCellOfSlotClicked( int row, int col )
{
	uint slot = static_cast<uint>(row);
	//DisplayBoardMaintInfo();
	emit SigToMainUiCurrentSlot( slot );
}

void CSlotView::InitSlotProgress(uint slot)
{
	QProgressBar* pPrg =  static_cast<QProgressBar*>( ui->tblSlotView->cellWidget( slot, EN_SI_PROGRESS) );
	if( pPrg )
	{
		pPrg->setValue( 0.0 );
	}
}

void CSlotView::UpdateSlotProgress( uint slot )
{
	if( m_pTestDataMgr == nullptr ) return;

	stTestItem testitem;
	if( !m_pTestDataMgr->GET_TESTITEM( slot, CPU00, testitem) )
	{
		//AppendSysTextEdit( _ERROR_LOG, tr("SLOT#%1 TestItem read error").arg(slot+1) );
		return;
	}
	uint total  = testitem.nItemCount;
	uint tested = 0;
	std::list<stStepOutInfo> stepout0;
	std::list<stStepOutInfo> stepout1;
	m_pTestDataMgr->GET_STEPOUT_LIST( slot, CPU00, &stepout0 );
	m_pTestDataMgr->GET_STEPOUT_LIST( slot, CPU01, &stepout1 );
	if( stepout0.size() > stepout1.size() )
		tested = stepout1.size();
	else
		tested = stepout0.size();
	int prgs = static_cast<int>( (static_cast<double>(tested)/total)*100.0 );
	QProgressBar* pPrg =  static_cast<QProgressBar*>( ui->tblSlotView->cellWidget( slot, EN_SI_PROGRESS) );
	if( pPrg )
	{
		pPrg->setValue( prgs );
	}
}

void CSlotView::DisplaySlotInfo(uint slot)
{
	QString bibid  = "";
	QString partno = "";
	QString lotno  = "";

	if( ui->tblSlotView->item(slot,EN_SI_BIBID) )
		bibid = tr("%1").arg(ui->tblSlotView->item(slot,EN_SI_BIBID)->text().trimmed());
	else
		bibid = tr("");

	if( ui->tblSlotView->item(slot,EN_SI_PARTNAME) )
		partno = tr("%1").arg(ui->tblSlotView->item(slot,EN_SI_PARTNAME)->text().trimmed());
	else
		partno = tr("");

	if( ui->tblSlotView->item(slot,EN_SI_LOTNAME) )
		lotno = tr("%1").arg(ui->tblSlotView->item(slot,EN_SI_LOTNAME)->text().trimmed());
	else
		lotno = tr("");

}

bool CSlotView::DisplaySlotViewPassTotal(uint slot)
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
	/// Step Fail Summary needs...
	////////////////////////////////////////////////////////////////////////////////////////////////

	QString sPT = tr("%1/%2").arg(nSlotTotal).arg(nMaxDut);
	ui->tblSlotView->item( slot, EN_SI_PT       )->setText( sPT );
	return true;
}

void CSlotView::SlotFromMainUiUpdateBibId( uint slot)
{
	DisplayBibId( slot );
}

void CSlotView::SlotFromMainUiUpdateSetup()
{
	DisplayBoardMaintInfo();
}

void CSlotView::SlotFromUiUpdateAutoInitResult(uint slot, uint pass_qty,uint total_qty)
{
	ui->tblSlotView->item( slot, EN_SI_PT )->setText( tr("%1/%2").arg(pass_qty).arg(total_qty) );
}

void CSlotView::SlotInitSlotData()
{
	InitSlotData();
}

//void CSlotView::SlotFromMainUiUpdatePartNo( uint slot )
//{
//	UpdatePartNo(slot);
//}

//void CSlotView::SlotFromMainUiUpdatePartNo()
//{
//	char* env_data = getenv(ATDATA_DIR_ENV);
//	QString data_path("");
//	if( env_data != NULL )
//		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
//	else
//		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

//	QString ini_file(QObject::tr("%1/%2/%3").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(EVENT_INI));
//	int slot_with_bib = 0x0;
//	QString hex_value;
//	UTIL::INI::readSettings(ini_file, KEY_SLOT_WITH_BIB, hex_value);

//	bool bOk = false;
//	slot_with_bib = hex_value.toInt(&bOk, 16);

//	for (int i = 0; i < MAX_SLOT; i++)
//	{
//		if ( slot_with_bib & (0b1 << i) )
//		{
//			UpdatePartNo(i);
//		}
//	}
//}

//void CSlotView::UpdatePartNo(uint slot)
//{
//	char* env_data = getenv(ATDATA_DIR_ENV);
//	QString data_path("");
//	if( env_data != NULL )
//		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
//	else
//		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

//	QString sRcmdMainInfoFile = tr("%1/%2/%3/SLOT%4%5").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(BOARD_MAINT_DIR).arg(slot).arg(EXT_RCMD);
//	char * pReadMainInfo = new char[sizeof(stRcmdBoardMaintInfo)];

//	if ( !m_pTestDataMgr->READ_FILE(sRcmdMainInfoFile.toStdString().c_str(), pReadMainInfo, sizeof(stRcmdBoardMaintInfo)) )
//	{
//		ui->tblSlotView->item( slot, EN_SI_PARTNAME )->setBackground( FAIL_BG_QCOLOR );
//		return;
//	}
//	stRcmdBoardMaintInfo rcmdBoardMaintInfo;
//	memcpy( &rcmdBoardMaintInfo, pReadMainInfo, sizeof(stRcmdBoardMaintInfo));
//	delete[] pReadMainInfo;

//	ui->tblSlotView->item( slot, EN_SI_PARTNAME )->setBackground( PASS_BG_QCOLOR );
//	ui->tblSlotView->item( slot, EN_SI_PARTNAME )->setText( rcmdBoardMaintInfo.PART_NO );
//}

//void CSlotView::SlotFromMainUiUpdateMlbCode()
//{
//	char* env_data = getenv(ATDATA_DIR_ENV);
//	QString data_path("");
//	if( env_data != NULL )
//		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
//	else
//		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

//	QString ini_file(QObject::tr("%1/%2/%3").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(EVENT_INI));
//	int slot_with_bib = 0x0;
//	QString hex_value;
//	UTIL::INI::readSettings(ini_file, KEY_SLOT_WITH_BIB, hex_value);

//	bool bOk = false;
//	slot_with_bib = hex_value.toInt(&bOk, 16);

//	for (int i = 0; i < MAX_SLOT; i++)
//	{
//		if ( slot_with_bib & (0b1 << i) )
//		{
//			UpdateMlbCode(i);
//		}
//	}
//}

//void CSlotView::UpdateMlbCode(uint slot)
//{
//	char* env_data = getenv(ATDATA_DIR_ENV);
//	QString data_path("");
//	if( env_data != NULL )
//		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
//	else
//		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

//	QString sRcmdMainInfoFile = tr("%1/%2/%3/SLOT%4%5").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(BOARD_MAINT_DIR).arg(slot).arg(EXT_RCMD);
//	char * pReadMainInfo = new char[sizeof(stRcmdBoardMaintInfo)];

//	if ( !m_pTestDataMgr->READ_FILE(sRcmdMainInfoFile.toStdString().c_str(), pReadMainInfo, sizeof(stRcmdBoardMaintInfo)) )
//	{
//		return;
//	}
//	stRcmdBoardMaintInfo rcmdBoardMaintInfo;
//	memcpy( &rcmdBoardMaintInfo, pReadMainInfo, sizeof(stRcmdBoardMaintInfo));
//	delete[] pReadMainInfo;

//	ui->tblSlotView->item( slot, EN_SI_MLB )->setBackground( PASS_BG_QCOLOR );
//	ui->tblSlotView->item( slot, EN_SI_MLB )->setText( QString(rcmdBoardMaintInfo.MLB_CODE) );
//}

//void CSlotView::SlotFromMainUiUpdateLotNo()
//{
//	char* env_data = getenv(ATDATA_DIR_ENV);
//	QString data_path("");
//	if( env_data != NULL )
//		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
//	else
//		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

//	QString ini_file(QObject::tr("%1/%2/%3").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(EVENT_INI));
//	int slot_with_bib = 0x0;
//	QString hex_value;
//	UTIL::INI::readSettings(ini_file, KEY_SLOT_WITH_BIB, hex_value);

//	bool bOk = false;
//	slot_with_bib = hex_value.toInt(&bOk, 16);

//	for (int i = 0; i < MAX_SLOT; i++)
//	{
//		if ( slot_with_bib & (0b1 << i) )
//		{
//			UpdateLotNo(i);
//		}
//	}
//}

//void CSlotView::UpdateLotNo(uint slot)
//{
//	char* env_data = getenv(ATDATA_DIR_ENV);
//	QString data_path("");
//	if( env_data != NULL )
//		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
//	else
//		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

//	QString sRcmdMainInfoFile = tr("%1/%2/%3/SLOT%4%5").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(BOARD_MAINT_DIR).arg(slot).arg(EXT_RCMD);
//	char * pReadMainInfo = new char[sizeof(stRcmdBoardMaintInfo)];

//	if ( !m_pTestDataMgr->READ_FILE(sRcmdMainInfoFile.toStdString().c_str(), pReadMainInfo, sizeof(stRcmdBoardMaintInfo)) )
//	{
//		ui->tblSlotView->item( slot, EN_SI_LOTNAME )->setBackground( FAIL_BG_QCOLOR );
//		return;
//	}
//	stRcmdBoardMaintInfo rcmdBoardMaintInfo;
//	memcpy( &rcmdBoardMaintInfo, pReadMainInfo, sizeof(stRcmdBoardMaintInfo));
//	delete[] pReadMainInfo;

//	ui->tblSlotView->item( slot, EN_SI_LOTNAME )->setBackground( PASS_BG_QCOLOR );
//	ui->tblSlotView->item( slot, EN_SI_LOTNAME )->setText( QString(rcmdBoardMaintInfo.LOT_NO) );
//}

//void CSlotView::SlotFromMainUiUpdatePassAndTotal()
//{
//	char* env_data = getenv(ATDATA_DIR_ENV);
//	QString data_path("");
//	if( env_data != NULL )
//		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
//	else
//		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

//	QString ini_file(QObject::tr("%1/%2/%3").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(EVENT_INI));
//	int slot_with_bib = 0x0;
//	QString hex_value;
//	UTIL::INI::readSettings(ini_file, KEY_SLOT_WITH_BIB, hex_value);

//	bool bOk = false;
//	slot_with_bib = hex_value.toInt(&bOk, 16);

//	for (int i = 0; i < MAX_SLOT; i++)
//	{
//		if ( slot_with_bib & (0b1 << i) )
//		{
//			UpdatePassAndTotal(i);
//		}
//	}
//}

//void CSlotView::UpdatePassAndTotal(uint slot)
//{
//	ui->tblSlotView->item( slot, EN_SI_PT )->setBackground( PASS_BG_QCOLOR );
//	ui->tblSlotView->item( slot, EN_SI_PT )->setText( QString("0/0") );
//}

void CSlotView::SlotFromMainUiSetupView()
{
	//int offset  = slot;
	if( m_pTestDataMgr == nullptr )
	{
		emit SigToMainUiSetupResult( false );
		return;
	}
	else
	{
		stRcmdStartBiCmd stBicmd;
		QString sStartBicmdfile = tr("%1/%2%3").arg(m_pTestDataMgr->DATA_ROOTDIR()).arg(RCMD_START_BI_CMD).arg(EXT_RCMD);
		if( m_pTestDataMgr->READ_FILE( sStartBicmdfile.toStdString().c_str(), (char*)&stBicmd, sizeof(stRcmdStartBiCmd) ) )
		{
			//stBicmd.
		}
	}

	/*
	if( !bSetup )
	{
		ui->tblSlotView->item( offset, EN_SI_BIBID    )->setBackground( FAIL_BG_QCOLOR );
		ui->tblSlotView->item( offset, EN_SI_PT       )->setBackground( FAIL_BG_QCOLOR );
		ui->tblSlotView->item( offset, EN_SI_PARTNAME )->setBackground( FAIL_BG_QCOLOR );
		ui->tblSlotView->item( offset, EN_SI_LOTNAME  )->setBackground( FAIL_BG_QCOLOR );
		QProgressBar* pItem = static_cast<QProgressBar*>( ui->tblSlotView->cellWidget( offset, EN_SI_PROGRESS) );
		if( pItem )
		{
			pItem->setStyleSheet( PASS_PRGBAR_QSS );
			pItem->setToolTip("Setup error!");
			pItem->setValue( 0 );
		}
	}
	else
	{
		ui->tblSlotView->item( offset, EN_SI_BIBID    )->setBackground( PASS_BG_QCOLOR );
		ui->tblSlotView->item( offset, EN_SI_PT       )->setBackground( PASS_BG_QCOLOR );
		ui->tblSlotView->item( offset, EN_SI_PARTNAME )->setBackground( PASS_BG_QCOLOR );
		ui->tblSlotView->item( offset, EN_SI_LOTNAME  )->setBackground( PASS_BG_QCOLOR );
		QProgressBar* pItem = static_cast<QProgressBar*>( ui->tblSlotView->cellWidget( offset, EN_SI_PROGRESS) );
		if( pItem )
		{
			pItem->setStyleSheet( PASS_PRGBAR_QSS );
			pItem->setToolTip("Setup OK");
			pItem->setValue( 0 );
		}
	}
	*/
}

void CSlotView::SlotFromMainUiTestStartFlag()
{
	m_bTestRun = true;
	for( uint slot=0;slot<MAX_SLOT;slot++ )
		InitSlotProgress( slot );
}

void CSlotView::SlotFromMainUiTestEndFlag()
{
	m_bTestRun = false;
}

void CSlotView::SlotFromMainUiStepOut(uint slot)
{
	UpdateSlotProgress( slot );
}

void CSlotView::SlotFromMainUiLoadExistData()
{
	if( m_pTestDataMgr == nullptr ) return;
	DisplayBoardMaintInfo();
}

//2024.08.19 - KWMOON Add.
void CSlotView::DisplayBoardMaintInfo()
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	QString data_path("");
	if( env_data != NULL )
		data_path = tr("%1/%2").arg(env_data).arg(DIR_ATDATA_NAME);
	else
		data_path = tr("%1/%2").arg(DIR_ATDATA_PATH).arg(DIR_ATDATA_NAME);

	QString uimode_ini(tr("%1/%2").arg(data_path).arg(UIMODE_INI));
	QString section_key = tr("%1/%2").arg(SECTION_UIMODE).arg(KEY_UIMODE);
	QString sValue("");
	UTIL::INI::readSettings(uimode_ini, section_key, sValue);

	QString ini_file(QObject::tr("%1/%2/%3")
					 .arg(data_path)
					 .arg(sValue.compare("auto") == 0 ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH)
					 .arg(EVENT_INI));

	int slot_with_bib = 0x0;
	QString hex_value;
	bool bSlotWidthbib = UTIL::INI::readSettings(ini_file, KEY_SLOT_WITH_BIB, hex_value);

	bool bOk = false;
	slot_with_bib = hex_value.toInt(&bOk, 16);

	//2024.08.19 - KWMOON
	if( slot_with_bib == 0x0 )
	{
		if( !bSlotWidthbib )
		{
			emit SigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("%1[%2] not exist!").arg(ini_file).arg(KEY_SLOT_WITH_BIB) );
		}
		else
		{
			emit SigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("%1 is zero!").arg(KEY_SLOT_WITH_BIB) );
		}
	}
	else
	{
		for (int i = 0; i < MAX_SLOT; i++)
		{
			if ( slot_with_bib & (0b1 << i) )
			{
				DisplayBibId( i );
				QString sRcmdMainInfoFile = tr("%1/%2/%3/SLOT%4%5").arg(data_path).arg(m_pTestDataMgr->GET_UIAUTOMODE()? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH).arg(BOARD_MAINT_DIR).arg(i).arg(EXT_RCMD);
	//			QString sRcmdMainInfoFile = tr("%1/%2/%3/SLOT%4%5").arg(data_path).arg(DIR_AUTO_DATA_PATH).arg(BOARD_MAINT_DIR).arg(i).arg(EXT_RCMD);
				char * pReadMainInfo = new char[sizeof(stRcmdBoardMaintInfo)];

				if ( !m_pTestDataMgr->READ_FILE(sRcmdMainInfoFile.toStdString().c_str(), pReadMainInfo, sizeof(stRcmdBoardMaintInfo)) )
				{
					ui->tblSlotView->item( i, EN_SI_PARTNAME )->setBackground( FAIL_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_MLB )->setBackground( FAIL_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_LOTNAME )->setBackground( FAIL_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_PT )->setBackground( FAIL_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_PARTNAME )->setText( QString("ERROR") );
					emit SigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("%1 not exist!").arg(BOARD_MAINT_DIR) );
				}
				else
				{
					stRcmdBoardMaintInfo rcmdBoardMaintInfo;
					memcpy( (char*)&rcmdBoardMaintInfo, pReadMainInfo, sizeof(stRcmdBoardMaintInfo));

					ui->tblSlotView->item( i, EN_SI_PARTNAME )->setBackground( PASS_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_PARTNAME )->setText( rcmdBoardMaintInfo.PART_NO );
					ui->tblSlotView->item( i, EN_SI_MLB )->setBackground( PASS_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_MLB )->setText( QString(rcmdBoardMaintInfo.MLB_CODE) );
					ui->tblSlotView->item( i, EN_SI_LOTNAME )->setBackground( PASS_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_LOTNAME )->setText( QString(rcmdBoardMaintInfo.LOT_NO) );
					ui->tblSlotView->item( i, EN_SI_PT )->setBackground( PASS_BG_QCOLOR );
					ui->tblSlotView->item( i, EN_SI_PT )->setText( QString("0/0") );
					UpdateSlotProgress( i );
					DisplaySlotViewPassTotal( i );
				}
				delete[] pReadMainInfo;
			}
		}
	}
}

//2024.08.19 - KWMOON
void CSlotView::DisplayBibId(uint slot)
{
	stBibId bibid;
	memset( &bibid, 0x0, sizeof(stBibId) );

	ui->tblSlotView->item( slot, EN_SI_BIBID )->setBackground( PASS_BG_QCOLOR );
	ui->tblSlotView->item( slot, EN_SI_MLB  )->setBackground( PASS_BG_QCOLOR );
	if( m_pTestDataMgr->GET_BIBID( slot, bibid ) )
	{
		if( bibid.bParityError )
		{
			ui->tblSlotView->item( slot, EN_SI_BIBID )->setBackground( FAIL_BG_QCOLOR );
			ui->tblSlotView->item( slot, EN_SI_BIBID )->setText( bibid.szBibId );
		}
		else if( !bibid.bBibConn )
		{
//			ui->tblSlotView->item( slot, EN_SI_BIBID )->setBackground( FAIL_BG_QCOLOR );
			ui->tblSlotView->item( slot, EN_SI_BIBID )->setText( bibid.szBibId  );
		}
		else
		{
			ui->tblSlotView->item( slot, EN_SI_BIBID )->setText( bibid.szBibId  );
		}
	}
	else
	{
		ui->tblSlotView->item( slot, EN_SI_BIBID )->setBackground( FAIL_BG_QCOLOR );
		ui->tblSlotView->item( slot, EN_SI_BIBID )->setText( bibid.szBibId );
		emit SigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("BIBID read error!") );
	}
}

//2024.08.19 - KWMOON
void CSlotView::SlotFromMainUiInitTestDone()
{
	DisplayBoardMaintInfo();
}
