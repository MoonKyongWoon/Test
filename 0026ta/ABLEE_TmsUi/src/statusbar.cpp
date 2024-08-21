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

	m_pixmapON.load( ":/icon/icon/green_marble.ico" );
	m_pixmapOFF.load( ":/icon/icon/red_marble.ico"  );

	m_pLed[_LED_EAP_TCP]    = ui->led1;
	m_pLed[_LED_EAP_SELECT] = ui->led2;
	m_pLed[_LED_MAINUI]     = ui->led3;

	SetStatus( _LED_EAP_TCP    , false );
	SetStatus( _LED_EAP_SELECT , false );
	SetStatus( _LED_MAINUI     , false );
}

void CStatusBar::SetStatus( _LED_DESC desc, bool conn )
{
	switch ( desc )
	{
		case _LED_EAP_TCP:
			if( conn )
			{
				m_pLed[_LED_EAP_TCP]->setPixmap( m_pixmapON  );
				m_pLed[_LED_EAP_TCP]->setToolTip( tr("%1 connectd!").arg("EAP TCP") );
			}
			else
			{
				m_pLed[_LED_EAP_TCP]->setPixmap( m_pixmapOFF );
				m_pLed[_LED_EAP_TCP]->setToolTip( tr("%1 disconnectd!").arg("EAP TCP") );
			}
			break;
		case _LED_EAP_SELECT:
			if( conn )
			{
				m_pLed[_LED_EAP_SELECT]->setPixmap( m_pixmapON  );
				m_pLed[_LED_EAP_SELECT]->setToolTip( tr("%1!").arg("EAP SELECTED") );
			}
			else
			{
				m_pLed[_LED_EAP_SELECT]->setPixmap( m_pixmapOFF );
				m_pLed[_LED_EAP_SELECT]->setToolTip( tr("%1!").arg("EAP NOT SELECTED") );
			}
			break;
		case _LED_MAINUI:
			if( conn )
			{
				m_pLed[_LED_MAINUI]->setPixmap( m_pixmapON  );
				m_pLed[_LED_MAINUI]->setToolTip( tr("%1 connectd!").arg("MAIN UI") );
			}
			else
			{
				m_pLed[_LED_MAINUI]->setPixmap( m_pixmapOFF );
				m_pLed[_LED_MAINUI]->setToolTip( tr("%1 disconnectd!").arg("MAIN UI") );
			}
			break;
		default:
			break;
	}
}

void CStatusBar::SlotStatusChanged( _LED_DESC desc, bool conn )
{
	SetStatus( desc, conn );
}
