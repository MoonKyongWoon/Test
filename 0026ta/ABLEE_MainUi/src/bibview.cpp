#include "bibview.hpp"
#include "ui_bibview.h"

CBibView::CBibView( uint slot, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CBibView),
	m_nSlotNo(slot)
{
	InitVars();
	InitUi();
}

CBibView::~CBibView()
{
	delete ui;
}


QTableWidget* CBibView::GetBibMap()
{
	return ui->tblBibMap;
}

QComboBox* CBibView::GetStepCombo()
{
	return ui->cboStep;
}

void CBibView::InitVars()
{
	m_nRow = DEFAULT_BIB_ROWS;
	m_nCol = DEFAULT_BIB_COLS;

	char* env_os   = getenv(ATOS_DIR_ENV);
	QString os_dir = "";
	if( env_os != NULL )
	{
		os_dir = QString(env_os);
	}
	else
	{
		os_dir = QString(DIR_ATOS_PATH);
	}

	QString sysini = tr("%1/%2/%3").arg(os_dir).arg(CONFIG_DIR).arg(SYSTEM_INI);
	QString section_key = tr("%1/%2").arg(SECTION_BIB_SIZE).arg(KEY_BIB_ROWS);
	if( !UTIL::INI::readSettings( sysini, section_key, m_nRow ) )
	{
		m_nRow = DEFAULT_BIB_ROWS;
	}

	section_key = tr("%1/%2").arg(SECTION_BIB_SIZE).arg(KEY_BIB_COLS);
	if( !UTIL::INI::readSettings( sysini, section_key, m_nCol ) )
	{
		m_nCol = DEFAULT_BIB_ROWS;
	}

	m_ResultType = BTN_NORMAL;
	for( int i=0;i<BTN_RESULT_COUNT;i++ ) m_arrayBtn[i] = nullptr;
}

void CBibView::InitBtnResult()
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

void CBibView::InitUi()
{
	ui->setupUi(this);

	m_arrayBtn[BTN_NORMAL  ] = ui->btnNormal;
	m_arrayBtn[BTN_MAKER   ] = ui->btnMaker;
	m_arrayBtn[BTN_DEV_TYPE] = ui->btnDevType;
	m_arrayBtn[BTN_FAIL_CNT] = ui->btnFailCnt;
	m_arrayBtn[BTN_ADD_FAIL] = ui->btnAddFail;
	m_arrayBtn[BTN_BIN     ] = ui->btnBin;

	ClearStepCombo();
	ClearBibMap();
	ui->tblBibMap->setRowCount( m_nRow );
	ui->tblBibMap->setColumnCount( m_nCol );
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

	InitBtnResult();
}

void CBibView::InitSignalSlots()
{
	connect( ui->tblBibMap       , &QTableWidget::cellClicked                , this, &CBibView::SlotCellOfBibMapClicked );
	connect( ui->cboStep, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CBibView::SlotItemSelectChanged   );
	connect( ui->btnNormal       , &QPushButton::clicked                     , this, &CBibView::SlotBtnResultKindChange );
	connect( ui->btnMaker        , &QPushButton::clicked                     , this, &CBibView::SlotBtnResultKindChange );
	connect( ui->btnDevType      , &QPushButton::clicked                     , this, &CBibView::SlotBtnResultKindChange );
	connect( ui->btnFailCnt      , &QPushButton::clicked                     , this, &CBibView::SlotBtnResultKindChange );
	connect( ui->btnAddFail      , &QPushButton::clicked                     , this, &CBibView::SlotBtnResultKindChange );
	connect( ui->btnBin          , &QPushButton::clicked                     , this, &CBibView::SlotBtnResultKindChange );
}

void CBibView::ClearStepCombo()
{
	ui->cboStep->clear();
}

void CBibView::ClearBibMap()
{
	ui->tblBibMap->clear();
}

void CBibView::SetSlotNo(uint s)
{
	m_nSlotNo = s;
}

void CBibView::SetBibRowCol(uint r, uint c)
{
	m_nRow = r;
	m_nCol = c;
	DrawBibMap();
}

void CBibView::DrawBibMap()
{
	ClearBibMap();
	ui->tblBibMap->setRowCount( m_nRow );
	ui->tblBibMap->setColumnCount( m_nCol );
}
