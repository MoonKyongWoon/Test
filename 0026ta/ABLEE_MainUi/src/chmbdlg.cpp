#include "chmbdlg.hpp"
#include "ui_chmbdlg.h"

CChmbDlg::CChmbDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CChmbDlg)
{
	ui->setupUi(this);
	InitVars();
	InitUi();
}

CChmbDlg::~CChmbDlg()
{
	if( m_pControllerTh != nullptr )
	{
		m_pControllerTh->quit();
		m_pControllerTh->wait();
		delete m_pControllerTh;
	}
	delete ui;
}

void CChmbDlg::InitVars()
{
	m_sCheckSeq[EN_CHK_CONN]      = "CONN CHECK";
	m_sCheckSeq[EN_RESET_ALARM]   = "ALARM RESET";
	m_sCheckSeq[EN_CHK_ALARM]     = "ALARM CHECK";
	m_sCheckSeq[EN_DO_RACKIN]     = "RACKIN";
	m_sCheckSeq[EN_CHK_RACKIN]    = "RACKIN CHECK";
	m_sCheckSeq[EN_DO_DOOR_CLOSE] = "DOOR CLOSE";
	m_sCheckSeq[EN_CHECK_DOOR]    = "DOOR CLOSE CHECK";

	m_pController = new CChmbDlgController;
	m_pControllerTh = new QThread;
	m_pController->moveToThread( m_pControllerTh );
	connect( m_pControllerTh , &QThread::finished                        , m_pController, &QObject::deleteLater            );
	connect( m_pController   , &CChmbDlgController::SigControllerSeqDone , this         , &CChmbDlg::SlotControllerSeqDone );
	connect( m_pController   , &CChmbDlgController::SigControllerSeqError, this         , &CChmbDlg::SlotControllerSeqError);
	connect( this            , &CChmbDlg::SigChmbCtrlInit                , m_pController, &CChmbDlgController::SlotInit    );
	connect( this            , &CChmbDlg::SlotCheckStart                 , m_pController, &CChmbDlgController::SlotInit    );
	m_pControllerTh->start();
}

void CChmbDlg::InitUi()
{
	ui->tblCheck->clear();
	ui->tblCheck->setRowCount( EN_CHKECK_SEQ::EN_MAX_CHECK_SEQ );
	ui->tblCheck->setColumnCount( EN_MAX_CHECK_COL );
	ui->tblCheck->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->tblCheck->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tblCheck->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tblCheck->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->tblCheck->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->tblCheck->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tblCheck->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tblCheck->horizontalHeader()->setStretchLastSection(true);
	ui->tblCheck->verticalHeader()->setStretchLastSection(true);
	ui->tblCheck->setSortingEnabled(false);
	ui->tblCheck->horizontalHeader()->setVisible(true);
	ui->tblCheck->verticalHeader()->setVisible(true);

	//Init Table Item
	for( int r=0;r<ui->tblCheck->rowCount();r++ )
	{
		QString sHeader = tr("%1").arg(r+1);
		QTableWidgetItem* item = new QTableWidgetItem( sHeader );
		item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
		ui->tblCheck->setVerticalHeaderItem( r, item );

		for( int c=0;c<ui->tblCheck->columnCount();c++ )
		{
			QString sHeader = "";
			if( c == EN_COL_ITEM ) sHeader = "CHECK ITEM";
			else                   sHeader = "STATUS";
			QTableWidgetItem* item = new QTableWidgetItem( sHeader );
			item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
			ui->tblCheck->setHorizontalHeaderItem( c, item );
		}
		for( int c=0;c<ui->tblCheck->columnCount();c++ )
		{
			QString sHeader = tr("%1").arg("");
			QTableWidgetItem* item = new QTableWidgetItem;
			QFont font = item->font();
			font.setPointSize( 9 );
			item->setTextAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
			item->setFlags( item->flags() & ~Qt::ItemIsEditable );
			item->setFont( font );
			item->setText( sHeader );
			item->setBackground( PASS_BG_QCOLOR );
			ui->tblCheck->setItem(r, c, item );
		}
	}
	//Title label
	for( int r=0;r<ui->tblCheck->rowCount();r++ )
	{
		QTableWidgetItem* item = ui->tblCheck->item(r,0);
		if( item != nullptr )
		{
			item->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
			item->setText( m_sCheckSeq[r] );
		}
	}

	connect( ui->btnSimm , &QPushButton::clicked, this, &CChmbDlg::SlotBtnSimmClicked  );
	connect( ui->btnClose, &QPushButton::clicked, this, &CChmbDlg::SlotBtnCloseClicked );
}

void CChmbDlg::InitRequest()
{
	m_RequestSeq = EN_CHK_CONN;
	memset( m_ResponseSeq, 0x0, sizeof(m_ResponseSeq) );

	for( int r=0;r<ui->tblCheck->rowCount();r++ )
	{
		QTableWidgetItem* item = ui->tblCheck->item(r, EN_COL_STATUS );
		if( item != nullptr )
		{
			item->setText( "" );
			item->setBackground( PASS_BG_QCOLOR );
		}
	}
}

void CChmbDlg::SlotBtnSimmClicked()
{
	CheckStart();
}

void CChmbDlg::SlotBtnCloseClicked()
{
	close();
}

void CChmbDlg::SlotInitChmbDlg()
{
	emit SigChmbCtrlInit();
}

void CChmbDlg::SlotCheckStart()
{
	CheckStart();
}

void CChmbDlg::CheckStart()
{
	emit SigChmbInitCheckReq();
}

void CChmbDlg::SlotControllerSeqDone(EN_CHKECK_SEQ seq, bool ok)
{

}

void CChmbDlg::SlotControllerSeqError(EN_CHKECK_SEQ seq, QString msg)
{

}

void CChmbDlg::CheckDone( EN_CHKECK_SEQ seq, bool ok )
{
	m_ResponseSeq[seq] = ok;
	UpdateSequence( seq, ok );
}

void CChmbDlg::UpdateSequence( EN_CHKECK_SEQ seq, bool ok )
{
	QTableWidgetItem* item = ui->tblCheck->item(seq, EN_COL_STATUS );
	if( item != nullptr )
	{
		item->setText( tr("%1").arg(ok?"SUCCESS!":"FAIL!") );
		if( ok )
			item->setBackground( PASS_BG_QCOLOR );
		else
			item->setBackground( FAIL_BG_QCOLOR );
	}
}
