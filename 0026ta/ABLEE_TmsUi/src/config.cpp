#include "config.hpp"
#include "ui_config.h"

CConfig::CConfig(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CConfig)
{
	ui->setupUi(this);
	LoadOsEnv();
	InitVars();
	InitUi();
}

CConfig::~CConfig()
{
	delete ui;
}

void CConfig::showEvent(QShowEvent *event)
{
	QDialog::showEvent( event );
	LoadTmsIni();
}

void CConfig::LoadOsEnv()
{
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		m_sOsDir = QString(env_os);
	}
	else
	{
		m_sOsDir = DIR_ATOS_PATH;
	}
	m_sTmsIni = tr("%1/%2/%3").arg(m_sOsDir).arg(CONFIG_DIR).arg(TMS_INI);
}

void CConfig::InitVars()
{
	for(int t = 0; t < KEY_TIMEOUT_MAX; t++)
		m_pSpinBox[t] = nullptr;

	m_dTimeoutDefault[0] = DEFAULT_TMS_T3_TIME;
	m_dTimeoutDefault[1] = DEFAULT_TMS_T5_TIME;
	m_dTimeoutDefault[2] = DEFAULT_TMS_T6_TIME;
	m_dTimeoutDefault[3] = DEFAULT_TMS_T7_TIME;
	m_dTimeoutDefault[4] = DEFAULT_TMS_T8_TIME;
}

void CConfig::InitUi()
{
	ui->leEquipId->setText("");
	ui->leDeviceId->setText("");
	ui->leTcpPort->setText("");

	m_pSpinBox[0] = ui->spinT3;
	m_pSpinBox[1] = ui->spinT5;
	m_pSpinBox[2] = ui->spinT6;
	m_pSpinBox[3] = ui->spinT7;
	m_pSpinBox[4] = ui->spinT8;

	//ui->spinConnTime->setValue( DEFAULT_TMS_CONN_TIME );
	//ui->spinConnTime->setValue( DEFAULT_TMS_CEID_TIME );

	connect( ui->btnSave   , &QPushButton::clicked , this, &CConfig::SlotSaveIni        );
	connect( ui->btnRestart, &QPushButton::clicked , this, &CConfig::SlotRestartService );
}

bool CConfig::LoadTmsIni()
{
	bool bOk = true;
	int nValue = 0;
	double dValue = 0.0;
	QString sValue = 0;
	QString section_key = "";

	//EQUIPMENT ID
	section_key = tr("%1/%2").arg(SECTION_SYSTEM).arg(KEY_TMS_EQUIPID);
	if( !UTIL::INI::readSettings( m_sTmsIni, section_key, sValue ) )
	{
		UTIL::INI::writeSettings( m_sTmsIni, section_key, DEFAULT_TMS_EQUIPID );
		sValue = DEFAULT_TMS_EQUIPID;
	}
	ui->leEquipId->setText( tr("%1").arg(sValue));

	//TCP PORT
	section_key = tr("%1/%2").arg(SECTION_COMM).arg(KEY_TMS_PORT);
	if( !UTIL::INI::readSettings( m_sTmsIni, section_key, nValue ) )
	{
		UTIL::INI::writeSettings( m_sTmsIni, section_key, DEFAULT_TMS_PORT );
		nValue = DEFAULT_TMS_PORT;
	}
	ui->leTcpPort->setText( tr("%1").arg(nValue));

	//DEVICE ID
	section_key = tr("%1/%2").arg(SECTION_COMM).arg(KEY_TMS_DEVID);
	if( !UTIL::INI::readSettings( m_sTmsIni, section_key, nValue ) )
	{
		UTIL::INI::writeSettings( m_sTmsIni, section_key, DEFAULT_TMS_DEVID );
		nValue = DEFAULT_TMS_DEVID;
	}
	ui->leDeviceId->setText( tr("%1").arg(nValue));

	//T3, T5 ~ T8

	section_key = tr("%1/%2").arg(SECTION_COMM).arg("KEY_T3_TIME");
	if(!UTIL::INI::readSettings(m_sTmsIni, section_key, dValue))
	{
		UTIL::INI::writeSettings(m_sTmsIni, section_key, m_dTimeoutDefault[0]);
		dValue = m_dTimeoutDefault[0];
	}
	if(m_pSpinBox[0] != nullptr)
	{
		m_pSpinBox[0]->setValue(dValue);
	}

	for(int t = 1; t < KEY_TIMEOUT_MAX; t++)
	{
		QString sKey = tr("KEY_T%1_TIME").arg(t + 4);
		section_key = tr("%1/%2").arg(SECTION_COMM).arg(sKey);
		if( !UTIL::INI::readSettings(m_sTmsIni, section_key, dValue))
		{
			UTIL::INI::writeSettings(m_sTmsIni, section_key, m_dTimeoutDefault[t]);
			dValue = m_dTimeoutDefault[t];
		}
		if(m_pSpinBox[t] != nullptr)
		{
			m_pSpinBox[t]->setValue(dValue);
		}
	}

	return bOk;
}

bool CConfig::SaveTmsIni()
{
	bool bOk = true;
	int nValue = 0;
	QString sValue = "";
	QString section_key = "";

	//EQP ID
	sValue = ui->leEquipId->text().trimmed();
	if( sValue.isEmpty() )
	{
		QMessageBox::critical( this, tr("ERROR"), tr("EQUIPMENT ID is empty"), QMessageBox::Ok );
		return false;
	}
	section_key = tr("%1/%2").arg(SECTION_SYSTEM).arg(KEY_TMS_EQUIPID);
	bOk &= UTIL::INI::writeSettings( m_sTmsIni, section_key, sValue );

	//DEVICE ID
	sValue = ui->leDeviceId->text().trimmed();
	if( sValue.isEmpty() )
	{
		QMessageBox::critical( this, tr("ERROR"), tr("DEVICE ID is empty"), QMessageBox::Ok );
		return false;
	}
	section_key = tr("%1/%2").arg(SECTION_COMM).arg(KEY_TMS_DEVID);
	bOk &= UTIL::INI::writeSettings(  m_sTmsIni, section_key, sValue );

	//PORT
	sValue = ui->leTcpPort->text().trimmed();
	if( sValue.isEmpty() )
	{
		QMessageBox::critical( this, tr("ERROR"), tr("TCP PORT is empty"), QMessageBox::Ok );
		return false;
	}
	section_key = tr("%1/%2").arg(SECTION_COMM).arg(KEY_TMS_PORT);
	bOk &= UTIL::INI::writeSettings(  m_sTmsIni, section_key, sValue );

	//T3, T5 ~ T8
	section_key = tr("%1/%2").arg(SECTION_COMM).arg("KEY_T3_TIME");
	double dValue = m_pSpinBox[0]->value();
	bOk &= UTIL::INI::writeSettings( m_sTmsIni, section_key, dValue );

	for(int t = 1; t < KEY_TIMEOUT_MAX; t++)
	{
		QString sKey = tr("KEY_T%1_TIME").arg(t + 4);
		section_key = tr("%1/%2").arg(SECTION_COMM).arg(sKey);
		double dValue = m_pSpinBox[t]->value();
		bOk &= UTIL::INI::writeSettings( m_sTmsIni, section_key, dValue );
	}
	return bOk;
}

void CConfig::SlotSaveIni()
{
	if( SaveTmsIni() )
		emit SigConfigSaved();
	LoadTmsIni();
}

void CConfig::SlotRestartService()
{
	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, qApp->applicationName(), "Do you want to restart tcp service?",
		QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		emit SigConfigRestartService();
	}
}
