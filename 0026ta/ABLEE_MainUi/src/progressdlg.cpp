#include "progressdlg.hpp"
#include "ui_progressdlg.h"

CProgressDlg::CProgressDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CProgressDlg)
{
	InitUi();
}

CProgressDlg::~CProgressDlg()
{
	delete ui;
}

void CProgressDlg::InitUi()
{
	ui->setupUi(this);
	CreateProgressView();
	InitPrgTable();
	InitProgress();
	InitStatusText();

	connect( ui->btnClose , &QPushButton::clicked, this, &CProgressDlg::accept );
}

void CProgressDlg::resizeEvent(QResizeEvent *event)
{
	QDialog::resizeEvent( event );
	ResizeView();
}

void CProgressDlg::ResizeView()
{
	int twidth = ui->tblProgress->width();
	int hwidth = ui->tblProgress->verticalHeader()->width();
	int vwidth = twidth - hwidth;
	ui->tblProgress->setColumnWidth( COL_SLOT, vwidth * 0.15 );
	ui->tblProgress->setColumnWidth( COL_PROGRESSBAR, vwidth * 0.25 );
	ui->tblProgress->setColumnWidth( COL_TEXT, vwidth * 0.55 );
}

void CProgressDlg::CreateProgressView()
{
	ui->tblProgress->setRowCount( MAX_SLOT );
	ui->tblProgress->clearContents();
	ui->tblProgress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->tblProgress->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tblProgress->setSelectionMode(QAbstractItemView::NoSelection);
	ui->tblProgress->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->tblProgress->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tblProgress->setAutoScroll(false);
	ui->tblProgress->setSortingEnabled(false);
	ui->tblProgress->horizontalHeader()->setVisible(true);
	ui->tblProgress->verticalHeader()->setVisible(false);
	ui->tblProgress->verticalHeader()->setFixedWidth( 30 );
	ui->tblProgress->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui->tblProgress->horizontalHeader()->setStretchLastSection(true);

	InitPrgTable();
}

void CProgressDlg::InitPrgTable()
{
	for( int r=0;r<ui->tblProgress->rowCount();r++ )
	{
		QString sHeader = tr("%1").arg(r+1);
		QTableWidgetItem* hItem = new QTableWidgetItem( sHeader );
		hItem->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
		ui->tblProgress->setVerticalHeaderItem( r, hItem );
		for( int c=0;c<ui->tblProgress->columnCount();c++ )
		{
			if( c == COL_PROGRESSBAR )
			{
				QWidget* pWidget = new QWidget();
				pWidget->setStyleSheet("QWidget{background-color: #FFFFFF;}");
				m_prgBar[r]  = new QProgressBar;
				QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
				m_prgBar[r]->setValue(r);
				pLayout->addWidget( m_prgBar[r] );
				pLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				pLayout->setContentsMargins(1, 1, 1, 1);
				pWidget->setLayout(pLayout);
				ui->tblProgress->setCellWidget( r, c, pWidget);
			}
			else
			{
				QTableWidgetItem* colItem = new QTableWidgetItem;
				colItem->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
				colItem->setFlags( colItem->flags() & ~Qt::ItemIsEditable );
				if( c == COL_SLOT )
					colItem->setText( tr("SLOT #%1").arg(r+1,2,10,QChar('0')) );
				else
					colItem->setText( tr("") );
				colItem->setForeground( PASS_FOREGROUND );
				ui->tblProgress->setItem(r, c, colItem );
			}
		}
	}
}

void CProgressDlg::InitProgress()
{
	for( int r=0;r<ui->tblProgress->rowCount();r++ )
	{
		m_prgBar[r]->setValue( 0 );
	}
}

void CProgressDlg::InitStatusText()
{
	for( int r=0;r<ui->tblProgress->rowCount();r++ )
	{
		QTableWidgetItem* colItem = ui->tblProgress->item( r, COL_TEXT );
		if( colItem )
		{
			QFont newFont = colItem->font();
			newFont.setBold( false );
			colItem->setFont( newFont );
			colItem->setText("");
		}
	}
}

void CProgressDlg::SlotProgressDlgTitle(QString title)
{
	ui->lblTitle->setText( tr("%1").arg(title) );
}

void CProgressDlg::SlotProgressDlgInit()
{
	InitProgress();
	InitStatusText();
}

void CProgressDlg::SlotProgressDlgValue( int slot, int value )
{
	m_prgBar[slot]->setValue( value );
}

void CProgressDlg::SlotProgressDlgText ( int slot, QString msg, bool berror )
{
	QTableWidgetItem* colItem = ui->tblProgress->item( slot, COL_TEXT );
	if( colItem )
	{
		colItem->setText(msg);
		if( berror )
		{
			QFont newFont = colItem->font();
			newFont.setBold( true );
			colItem->setFont( newFont );
			colItem->setBackground( FAIL_BACKGROUND );
			//colItem->setForeground( FAIL_FOREGROUND );
		}
		else
		{
			QFont newFont = colItem->font();
			newFont.setBold( false );
			colItem->setFont( newFont );
			colItem->setBackground( PASS_BACKGROUND );
		}
	}
}

void CProgressDlg::SlotProgressDlgHide()
{
	accept();
}
