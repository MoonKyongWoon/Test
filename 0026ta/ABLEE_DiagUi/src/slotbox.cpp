#include "slotbox.hpp"
#include "ui_slotbox.h"

CSlotBox::CSlotBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CSlotBox)
{
    ui->setupUi(this);
    InitVariables();
    InitUi();
    InitSigSlots();
}

CSlotBox::~CSlotBox()
{
    delete ui;
}

void CSlotBox::InitVariables()
{
	m_pchkAllSlot= ui->chkSlotAll;

	m_pchkSlot[0 ] = ui->chkSlot1;
	m_pchkSlot[1 ] = ui->chkSlot2;
	m_pchkSlot[2 ] = ui->chkSlot3;
	m_pchkSlot[3 ] = ui->chkSlot4;
	m_pchkSlot[4 ] = ui->chkSlot5;
	m_pchkSlot[5 ] = ui->chkSlot6;
	m_pchkSlot[6 ] = ui->chkSlot7;
	m_pchkSlot[7 ] = ui->chkSlot8;
	m_pchkSlot[8 ] = ui->chkSlot9;
	m_pchkSlot[9 ] = ui->chkSlot10;
	m_pchkSlot[10] = ui->chkSlot11;
	m_pchkSlot[11] = ui->chkSlot12;
	m_pchkSlot[12] = ui->chkSlot13;
	m_pchkSlot[13] = ui->chkSlot14;
	m_pchkSlot[14] = ui->chkSlot15;
	m_pchkSlot[15] = ui->chkSlot16;


	m_pLblStat[0 ] = ui->lblStatus;
	m_pLblStat[1 ] = ui->lblStatus_2;
	m_pLblStat[2 ] = ui->lblStatus_3;
	m_pLblStat[3 ] = ui->lblStatus_4;
	m_pLblStat[4 ] = ui->lblStatus_5;
	m_pLblStat[5 ] = ui->lblStatus_6;
	m_pLblStat[6 ] = ui->lblStatus_7;
	m_pLblStat[7 ] = ui->lblStatus_8;
	m_pLblStat[8 ] = ui->lblStatus_9;
	m_pLblStat[9 ] = ui->lblStatus_10;
	m_pLblStat[10] = ui->lblStatus_11;
	m_pLblStat[11] = ui->lblStatus_12;
	m_pLblStat[12] = ui->lblStatus_13;
	m_pLblStat[13] = ui->lblStatus_14;
	m_pLblStat[14] = ui->lblStatus_15;
	m_pLblStat[15] = ui->lblStatus_16;
}

void CSlotBox::InitUi()
{
    InitPixmap();
    InitUiConnStatus();
    InitUiCheckStatus();
}

void CSlotBox::InitPixmap()
{
	QSize sizeIcon(24, 24);
    m_pixmapR = QPixmap(":/icon/icon/red_marble.ico").scaled( sizeIcon);
    m_pixmapG = QPixmap(":/icon/icon/green_marble.ico").scaled( sizeIcon);
}

void CSlotBox::InitUiConnStatus()
{
    for( int s=0;s<MAX_SLOT;s++ )
    {
		m_pLblStat[s]->setStyleSheet("background-color: transparent;margin:1px;");
		m_pLblStat[s]->setPixmap( m_pixmapR );
    }
}

void CSlotBox::InitUiCheckStatus()
{
    for( int s=0;s<MAX_SLOT;s++ )
	{
		m_pchkSlot[s]->setChecked( false );
	}
	m_pchkAllSlot->setChecked( false );
}

void CSlotBox::InitSigSlots()
{
	connect( m_pchkAllSlot, &QCheckBox::stateChanged, this, &CSlotBox::SlotAllSlotSelected );
}

void CSlotBox::SlotAllSlotSelected(int state)
{
    bool bChecked = (state == Qt::Checked )?true:false;
    for( int nSlot=0;nSlot<MAX_SLOT;nSlot++ )
    {
		m_pchkSlot[nSlot]->setChecked( bChecked );
    }
}
