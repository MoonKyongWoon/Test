#include <QGraphicsDropShadowEffect>
#include "versioninfo.hpp"
#include "ui_versioninfo.h"

CVersionInfo::CVersionInfo(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CVersionInfo)
{
	ui->setupUi(this);
	DisplayVersion();
}

CVersionInfo::~CVersionInfo()
{
	delete ui;
}

QString CVersionInfo::GetHostname()
{
	QString sHost  = UTIL::TCPIP::GetHostName().c_str();
	return sHost;
}

QString CVersionInfo::GetVersion()
{
	QString sver = QString("%1").arg(ATMAIN_VER " [" ATMAIN_DATE "]") ;
	return sver;
}

void CVersionInfo::DisplayVersion()
{
	QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
	effect->setColor(Qt::white);
	effect->setBlurRadius(0);
	effect->setXOffset(1);
	effect->setYOffset(0);

	QFont font = ui->lblHostName->font();
	font.setPointSize( 11 );
	ui->lblHostName->setGraphicsEffect(effect);
	ui->lblHostName->setFont( font );
	ui->lblSwVersion->setFont( font );

	ui->lblHostName->setText( GetHostname() );
	ui->lblSwVersion->setText( GetVersion() );
}

void CVersionInfo::DisplayInterruptMode()
{
	if( m_bInterrupt )
	{
		ui->btnInterrupt->setText("MANUAL");
		ui->btnInterrupt->setStyleSheet("QPushButton{background-color: #4077c9; color : #ffffff;}");
	}
	else
	{
		ui->btnInterrupt->setText("AUTO");
		ui->btnInterrupt->setStyleSheet("QPushButton{background-color: #d24726; color : #ffffff;}");
	}
}

//from mainui at startup
void CVersionInfo::SetUiAutoMode(bool automode)
{
	this->m_bUiAutoMode = automode;
	this->m_bInterrupt  = !automode;
	if( automode )
		ui->btnInterrupt->setChecked( true );
	else
	{
		ui->btnInterrupt->setChecked( false );
		ui->btnInterrupt->setEnabled( false );
	}

	DisplayInterruptMode();
	connect( ui->btnInterrupt, &QPushButton::toggled, this, &CVersionInfo::SlotBtnInterruptChanged );
}

void CVersionInfo::SlotBtnInterruptChanged(bool checked)
{
	if( checked )
	{
		this->m_bInterrupt  = false;
		emit SigInterruptMode( false );
	}
	else
	{
		this->m_bInterrupt  = true;
		emit SigInterruptMode( true );
	}
	DisplayInterruptMode();
}
