#include "statusbar.hpp"
#include "ui_statusbar.h"

CStatusBar::CStatusBar(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CStatusBar)
{
	InitUi();
}

CStatusBar::~CStatusBar()
{
	delete ui;
}

void CStatusBar::InitUi()
{
	ui->setupUi(this);

	m_pixmapON.load( ":/icon/green_marble.ico" );
	m_pixmapOFF.load( ":/icon/red_marble.ico" );

	m_pLed[CHMB_LED_IPC] = ui->statChmbComm;
	m_pLed[CHMB_LED_PLC] = ui->statChmbPlc;

	SetStatus( CHMB_LED_IPC  , false );
	SetStatus( CHMB_LED_PLC  , false );
}

void CStatusBar::SetStatus( CHMB_LED desc, bool conn )
{
	switch ( desc )
	{
		case CHMB_LED_IPC:
			if( conn )
			{
				m_pLed[CHMB_LED_IPC]->setPixmap( m_pixmapON  );
				m_pLed[CHMB_LED_IPC]->setToolTip( tr("%1 connectd!").arg(ATCHMBCOMM_NAME) );
			}
			else
			{
				m_pLed[CHMB_LED_IPC]->setPixmap( m_pixmapOFF );
				m_pLed[CHMB_LED_IPC]->setToolTip( tr("%1 disconnectd!").arg(ATCHMBCOMM_NAME) );
			}
			break;
		case CHMB_LED_PLC:
			if( conn )
			{
				m_pLed[CHMB_LED_PLC]->setPixmap( m_pixmapON  );
				m_pLed[CHMB_LED_PLC]->setToolTip( tr("%1 to %2 connectd!").arg(ATCHMBCOMM_NAME).arg("PLC") );
			}
			else
			{
				m_pLed[CHMB_LED_PLC]->setPixmap( m_pixmapOFF );
				m_pLed[CHMB_LED_PLC]->setToolTip( tr("%1 to %2 disconnectd!").arg(ATCHMBCOMM_NAME).arg("PLC") );
			}
			break;
		default:
			break;
	}
}

void CStatusBar::SlotChmbConnStatus( bool ipc, bool plc)
{
	SetStatus( CHMB_LED_IPC  , ipc );
	SetStatus( CHMB_LED_PLC  , plc );
}
