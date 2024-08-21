#include "setup.hpp"
#include "ui_setup.h"

CSetup::CSetup(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CSetup)
{
	InitUi();
	m_bAutoMode = false;
}

CSetup::~CSetup()
{
	delete ui;
}

void CSetup::InitUi()
{
	ui->setupUi(this);
	InitUiValue();

	connect( ui->btnOk    , &QPushButton::clicked, this, &CSetup::SlotBtnOkClicked     );
	connect( ui->btnCancel, &QPushButton::clicked, this, &CSetup::SlotBtnCancelClicked );
}

void CSetup::InitUiValue()
{
	memset( &m_stSetupInfo, 0x0, sizeof(stSetupInfo) );

	ui->leLotNo->setText("");
	ui->lePartNo->setText("");
}

void CSetup::SetFocus()
{
	ui->lePartNo->setFocus();
}

void CSetup::SetUiValue(QString lot, QString part)
{
	ui->leLotNo->setText(lot);
	ui->lePartNo->setText(part);
}

void CSetup::SetAutoMode(bool mode)
{
	m_bAutoMode = mode;
	InitUiValue();
	if( mode == true )
	{
//		ui->btnOk->setEnabled( false );
//		ui->btnCancel->setEnabled( false );
		ui->fraPartLot->setVisible( false );
		ui->btnOk->setEnabled( true );
		ui->btnCancel->setEnabled( true );
		this->setMinimumHeight( 100 );
		this->setMaximumHeight( 100 );
	}
	else
	{
		ui->btnOk->setEnabled( true );
		ui->btnCancel->setEnabled( true );
		this->setMinimumHeight( 160 );
		this->setMaximumHeight( 160 );
	}
}
void CSetup::Setup()
{

	QString lotno = ui->leLotNo->text().trimmed();
	QString partno = ui->lePartNo->text().trimmed();
	snprintf( m_stSetupInfo.lot_number , sizeof(m_stSetupInfo.lot_number) , "%s", lotno.toStdString().c_str()  );
	snprintf( m_stSetupInfo.part_number, sizeof(m_stSetupInfo.part_number), "%s", partno.toStdString().c_str() );

	emit SigSetupDlgResult( m_bAutoMode, m_stSetupInfo );
	done( Accepted );
}


void CSetup::SlotBtnOkClicked()
{
	if( !m_bAutoMode )
	{
		if( ui->lePartNo->text().trimmed() == "" )
		{
			QMessageBox::critical( this, "PART NUMBER", tr("please input PART NUMBER"), QMessageBox::Ok );
			ui->lePartNo->setFocus();
			return;
		}
		if( ui->leLotNo->text().trimmed() == "" )
		{
			QMessageBox::critical( this, "LOT NUMBER", tr("please input LOT NUMBER"), QMessageBox::Ok );
			ui->leLotNo->setFocus();
			return;
		}
	}
	Setup();
}

void CSetup::SlotBtnCancelClicked()
{
	done( Rejected );
}
