#include "failrate.hpp"
#include "ui_failrate.h"

CFailRate::CFailRate(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CFailRate)
{
	InitVars();
	InitUi();
}

CFailRate::~CFailRate()
{
	delete ui;
}


void CFailRate::InitVars()
{
	for( int b=0;b<FAIL_RATE_MAX_BIN;b++ )
	{
		m_pChkBin[b] = nullptr;
		m_pSpinBin[b] = nullptr;
	}
}

void CFailRate::InitUi()
{
	ui->setupUi(this);
	m_pChkBin[0] = ui->chkBin1;
	m_pChkBin[1] = ui->chkBin2;
	m_pChkBin[2] = ui->chkBin3;
	m_pChkBin[3] = ui->chkBin4;
	m_pChkBin[4] = ui->chkBin5;
	m_pChkBin[5] = ui->chkBin6;
	m_pChkBin[6] = ui->chkBin7;
	m_pChkBin[7] = ui->chkBin8;
	m_pChkBin[8] = ui->chkBin9;

	m_pSpinBin[0] = ui->sbBin1;
	m_pSpinBin[1] = ui->sbBin2;
	m_pSpinBin[2] = ui->sbBin3;
	m_pSpinBin[3] = ui->sbBin4;
	m_pSpinBin[4] = ui->sbBin5;
	m_pSpinBin[5] = ui->sbBin6;
	m_pSpinBin[6] = ui->sbBin7;
	m_pSpinBin[7] = ui->sbBin8;
	m_pSpinBin[8] = ui->sbBin9;

	connect( ui->chkFailRate , &QCheckBox::toggled    , this, &CFailRate::SlotChkFailRateToggled      );
	for( int b=0;b<FAIL_RATE_MAX_BIN;b++ )
	{
		connect( m_pChkBin[b] , &QCheckBox::toggled    , this, &CFailRate::SlotChkBinsToggled      );
		connect( m_pSpinBin[b], static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CFailRate::SlotSpinBinValueChanged );
		m_pChkBin[b]->setChecked( true );
		m_pChkBin[b]->setChecked( false );
	}
	ui->chkFailRate->setChecked( true  );
	ui->chkFailRate->setChecked( false );
	connect( ui->btnSave  , &QPushButton::clicked , this, &CFailRate::SlotBtnSaveClicked );
	connect( ui->btnLoad  , &QPushButton::clicked , this, &CFailRate::SlotBtnLoadClicked );
	connect( ui->btnClose , &QPushButton::clicked , this, &CFailRate::SlotBtnExitClicked );
}

void CFailRate::SlotBtnSaveClicked()
{

}

void CFailRate::SlotBtnLoadClicked()
{

}

void CFailRate::SlotBtnExitClicked()
{
	close();
}

void CFailRate::SlotChkFailRateToggled(bool toggle)
{
	for( int b=0;b<FAIL_RATE_MAX_BIN;b++ )
	{
		m_pChkBin[b]->setEnabled( toggle );
		if( m_pChkBin[b]->isChecked() && toggle )
			m_pSpinBin[b]->setEnabled( true );
		else
			m_pSpinBin[b]->setEnabled( false );
	}
}

void CFailRate::SlotChkBinsToggled(bool toggle)
{
	QCheckBox* pbin = static_cast<QCheckBox*>(QObject::sender());
	if( pbin )
	{
		for( int b=0;b<FAIL_RATE_MAX_BIN;b++ )
		{
			if( m_pChkBin[b] == pbin )
			{
				m_pSpinBin[b]->setEnabled( toggle );
				break;
			}
		}
	}
}

void CFailRate::SlotSpinBinValueChanged(int val)
{
	(void)val;
}
