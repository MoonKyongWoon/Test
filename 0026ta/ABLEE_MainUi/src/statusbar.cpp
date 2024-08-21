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
	m_pixmapOFF.load( ":/icon/icon/red_marble.ico"   );

	m_pLed[LED_MAIN_TO_MGR] = ui->ledMgr;
	m_pLed[LED_MAIN_TO_SLOT] = ui->ledMgrToSlot;
	m_pLed[LED_MAIN_TO_TMS] = ui->ledMgrToTms;
	m_pLed[LED_TMS_TO_EAP] = ui->ledMgrToTmsSvr;

	SetStatus( LED_MAIN_TO_MGR  , false );
	SetStatus( LED_MAIN_TO_SLOT , false );
	SetStatus( LED_MAIN_TO_TMS  , false );
	SetStatus( LED_TMS_TO_EAP   , false );
}

void CStatusBar::SetStatus( LED_DESC desc, bool conn )
{
	switch ( desc )
	{
		case LED_MAIN_TO_MGR:
			if( conn )
			{
				m_pLed[LED_MAIN_TO_MGR]->setPixmap( m_pixmapON  );
				m_pLed[LED_MAIN_TO_MGR]->setToolTip( tr("%1 to %2 connected!").arg(ATMAIN_NAME).arg(ATMGR_NAME) );
			}
			else
			{
				m_pLed[LED_MAIN_TO_MGR]->setPixmap( m_pixmapOFF );
				m_pLed[LED_MAIN_TO_MGR]->setToolTip( tr("%1 to %2 disconnected!").arg(ATMAIN_NAME).arg(ATMGR_NAME) );
			}
			break;
		case LED_MAIN_TO_SLOT:
			if( conn )
			{
				m_pLed[LED_MAIN_TO_SLOT]->setPixmap( m_pixmapON  );
				m_pLed[LED_MAIN_TO_SLOT]->setToolTip( tr("%1 to SLOT connected!").arg(ATMAIN_NAME) );
			}
			else
			{
				m_pLed[LED_MAIN_TO_SLOT]->setPixmap( m_pixmapOFF );
				m_pLed[LED_MAIN_TO_SLOT]->setToolTip( tr("%1 to SLOT disconnected!").arg(ATMAIN_NAME) );
			}
			break;
		case LED_MAIN_TO_TMS:
			if( conn )
			{
				m_pLed[LED_MAIN_TO_TMS]->setPixmap( m_pixmapON  );
				m_pLed[LED_MAIN_TO_TMS]->setToolTip( tr("%1 to %2 connected!").arg(ATMAIN_NAME).arg(ATTMSUI_NAME) );
			}
			else
			{
				m_pLed[LED_MAIN_TO_TMS]->setPixmap( m_pixmapOFF );
				m_pLed[LED_MAIN_TO_TMS]->setToolTip( tr("%1 to %2 disconnected!").arg(ATMAIN_NAME).arg(ATTMSUI_NAME) );
			}
			break;
		case LED_TMS_TO_EAP:
			if( conn )
			{
				m_pLed[LED_TMS_TO_EAP]->setPixmap( m_pixmapON  );
				m_pLed[LED_TMS_TO_EAP]->setToolTip( tr("%1 to EAP connected!").arg(ATTMSUI_NAME) );
			}
			else
			{
				m_pLed[LED_TMS_TO_EAP]->setPixmap( m_pixmapOFF );
				m_pLed[LED_TMS_TO_EAP]->setToolTip( tr("%1 to EAP disconnectd!").arg(ATTMSUI_NAME) );
			}
			break;
		default:
			break;
	}
}

void CStatusBar::SlotStatusChanged( LED_DESC desc, bool conn )
{
	SetStatus( desc, conn );
}
