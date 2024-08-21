#include "slotconnview.hpp"
#include "ui_slotconnview.h"

CSlotConnView::CSlotConnView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSlotConnView)
{
	ui->setupUi(this);
	m_nCols = MAX_SLOT;
	m_nRows = SLOT_CONN_MAX_ROW;
	InitUi();
	InitStatus();
}

CSlotConnView::~CSlotConnView()
{
	delete ui;
}

void CSlotConnView::InitUi()
{
	ui->tbSlotConnView->setRowCount( m_nRows );
	ui->tbSlotConnView->setColumnCount( m_nCols );
	ui->tbSlotConnView->clearContents();
	ui->tbSlotConnView->verticalHeader()->setFixedWidth( SLOTCONN_VHEAD_WIDTH );
	ui->tbSlotConnView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->tbSlotConnView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tbSlotConnView->setSelectionBehavior(QAbstractItemView::SelectItems);
	ui->tbSlotConnView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tbSlotConnView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tbSlotConnView->setAutoScroll(false);
	ui->tbSlotConnView->setSortingEnabled(false);
	ui->tbSlotConnView->horizontalHeader()->setVisible(true);
	ui->tbSlotConnView->verticalHeader()->setVisible(true);
	//ui->tbSlotConnView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	//ui->tbSlotConnView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->tbSlotConnView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);	//Fixed
	ui->tbSlotConnView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);		//Fixed
	ui->tbSlotConnView->horizontalHeader()->setStretchLastSection(true);
	ui->tbSlotConnView->verticalHeader()->setStretchLastSection(true);
}

void CSlotConnView::InitStatus()
{
	QSize IconSize (18, 18);
	for( int r=0;r<m_nRows;r++ )
	{
		for( int c=0;c<m_nCols;c++ )
		{
			QWidget* pWidget = new QWidget();
			pWidget->setStyleSheet("QWidget{background-color: #FFFFFF;}");
			QLabel* pLabel  = new QLabel;
			QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
			pLabel->setMaximumSize(IconSize);
			pLabel->setScaledContents(true);
			pLabel->setPixmap( PIXMAP_OFFLINE );
			pLabel->setToolTip("OFFLINE");
			pLayout->addWidget(pLabel);
			pLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			pLayout->setContentsMargins(0,0,0,0);
			pWidget->setLayout(pLayout);
			ui->tbSlotConnView->setCellWidget( r, c, pWidget);
		}
	}
	for( int c=0;c<m_nCols;c++ )
	{
		SetRunnerStatus( c, CPU00, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET, EN_SLOT_OFFLINE );
		SetRunnerStatus( c, CPU00, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET, EN_SLOT_OFFLINE );
		SetRunnerStatus( c, CPU00, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET , EN_SLOT_OFFLINE );
		SetRunnerStatus( c, CPU01, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET, EN_SLOT_OFFLINE );
		SetRunnerStatus( c, CPU01, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET, EN_SLOT_OFFLINE );
		SetRunnerStatus( c, CPU01, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET , EN_SLOT_OFFLINE );
	}
}

void CSlotConnView::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent( event );
	ResizeView();
}

void CSlotConnView::ResizeView()
{
	int nHeadWidth   = ui->tbSlotConnView->verticalHeader()->width();
	int nHeadHeight  = ui->tbSlotConnView->horizontalHeader()->height();
	int nWidth       = ui->tbSlotConnView->width();
	int nHeight      = ui->tbSlotConnView->height();
	int nScrolWidth  = 1;
	int nScrolHeight = 1;
	if( ui->tbSlotConnView->verticalScrollBar()->isVisible() )
	{
		if( ui->tbSlotConnView->verticalScrollBar()->isVisible() )
			nScrolWidth = ui->tbSlotConnView->verticalScrollBar()->width();
	}
	if( ui->tbSlotConnView->horizontalHeader()->isVisible() )
	{
		if( ui->tbSlotConnView->horizontalScrollBar()->isVisible() )
			nScrolHeight = ui->tbSlotConnView->horizontalScrollBar()->height();
	}

	int nViewWidth   = static_cast<int>( nWidth  - nHeadWidth - nScrolWidth  );
	int nViewHeight  = static_cast<int>( nHeight - nScrolHeight - nHeadHeight );
	int nStepHeight  = nViewHeight/ui->tbSlotConnView->rowCount();
	int nEachWidth   = nViewWidth / m_nCols;

	for( int c=0;c<m_nCols;c++ )
		ui->tbSlotConnView->setColumnWidth( c, nEachWidth );
	for (int row = 0; row<m_nRows; row++)
		ui->tbSlotConnView->setRowHeight(row, nStepHeight);
}

void CSlotConnView::SetLoaderStatus( int slot, EN_SLOT_STAT stat )
{
	int col = slot;
	int row = ROW_SLOT_LOADER;
	QLabel* pWidget = ui->tbSlotConnView->cellWidget( row, col )->findChild<QLabel*>();
	if( pWidget )
	{
		switch ( stat )
		{
			case EN_SLOT_STAT::EN_SLOT_MASK:
			{
				pWidget->setPixmap( PIXMAP_MASK );
				pWidget->setToolTip("SLOT MASK");
			}
			break;

			case EN_SLOT_STAT::EN_SLOT_OFFLINE:
			{
				pWidget->setPixmap( PIXMAP_OFFLINE );
				pWidget->setToolTip("LOADER OFFLINE");
			}
			break;

			case EN_SLOT_STAT::EN_SLOT_ONLINE:
			{
				pWidget->setPixmap( PIXMAP_ONLINE );
				pWidget->setToolTip("LOADER ONLINE");
			}
			break;
		}
	}
}

void CSlotConnView::SetRunnerStatus( int slot, int runner, PROTOCOL::EN_PGM_SOCKET_TYPE type, EN_SLOT_STAT stat )
{
	int col = slot;
	int row = 0x0;
	QString sRunner = "";
	if( runner == 0x0 )
	{
		if( type == PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET )
		{
			row = ROW_RUN1_CTRL;
			sRunner = "CPU#1 CTRL";
		}
		else if( type == PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET )
		{
			row = ROW_RUN1_DATA;
			sRunner = "CPU#1 DATA";
		}
		else
		{
			row = ROW_RUN1_LOG;
			sRunner = "CPU#1 LOG";
		}
	}
	else
	{
		if( type == PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET )
		{
			row = ROW_RUN2_CTRL;
			sRunner = "CPU#2 CTRL";
		}
		else if( type == PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET )
		{
			row = ROW_RUN2_DATA;
			sRunner = "CPU#2 DATA";
		}
		else
		{
			row = ROW_RUN2_LOG;
			sRunner = "CPU#2 LOG";
		}
	}

	QLabel* pWidget = ui->tbSlotConnView->cellWidget( row, col )->findChild<QLabel*>();
	if( pWidget )
	{
		switch ( stat )
		{
			case EN_SLOT_STAT::EN_SLOT_MASK:
			{
				pWidget->setPixmap( PIXMAP_MASK );
				pWidget->setToolTip( tr("%1 MASK").arg(sRunner) );
			}
			break;

			case EN_SLOT_STAT::EN_SLOT_OFFLINE:
			{
				pWidget->setPixmap( PIXMAP_OFFLINE );
				pWidget->setToolTip( tr("%1 OFFLINE").arg(sRunner) );
			}
			break;

			case EN_SLOT_STAT::EN_SLOT_ONLINE:
			{
				pWidget->setPixmap( PIXMAP_ONLINE );
				pWidget->setToolTip( tr("%1 ONLINE").arg(sRunner) );
			}
			break;
		}
	}
}

void CSlotConnView::SlotLoaderStatusChanged(int slot, EN_SLOT_STAT stat )
{
	SetLoaderStatus( slot, stat );
	if( stat == EN_SLOT_MASK )
	{
		SetRunnerStatus( slot, CPU00, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET, EN_SLOT_MASK );
		SetRunnerStatus( slot, CPU00, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET, EN_SLOT_MASK );
		SetRunnerStatus( slot, CPU00, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET , EN_SLOT_MASK );
		SetRunnerStatus( slot, CPU01, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_CTRL_SOCKET, EN_SLOT_MASK );
		SetRunnerStatus( slot, CPU01, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_DATA_SOCKET, EN_SLOT_MASK );
		SetRunnerStatus( slot, CPU01, PROTOCOL::EN_PGM_SOCKET_TYPE::EN_LOG_SOCKET , EN_SLOT_MASK );
	}
}

void CSlotConnView::SlotPgmStatusChanged( int slot, int runner, PROTOCOL::EN_PGM_SOCKET_TYPE type, EN_SLOT_STAT stat )
{
	SetRunnerStatus( slot, runner, type, stat );
}
