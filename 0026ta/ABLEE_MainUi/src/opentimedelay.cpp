#include "opentimedelay.hpp"
#include "ui_opentimedelay.h"

COpenTimeDelay::COpenTimeDelay(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::COpenTimeDelay)
{
	InitVars();
	InitUi();
	LoadIni();
}

COpenTimeDelay::~COpenTimeDelay()
{
	delete ui;
}

void COpenTimeDelay::InitVars()
{

}

void COpenTimeDelay::InitUi()
{
	ui->setupUi(this);
	connect( ui->chkEnable, &QCheckBox::toggled   , this, &COpenTimeDelay::SlotChkEnableToggled );
	connect( ui->btnSave  , &QPushButton::clicked , this, &COpenTimeDelay::SlotBtnSaveClicked   );
	connect( ui->btnClose , &QPushButton::clicked , this, &COpenTimeDelay::SlotBtnExitClicked   );

	ui->chkEnable->setChecked( true );
	ui->chkEnable->setChecked( false );
}

void COpenTimeDelay::LoadIni()
{

}

bool COpenTimeDelay::SaveIni()
{
	return true;
}

void COpenTimeDelay::SlotBtnSaveClicked()
{
	bool bSaved = SaveIni();
	if( bSaved )
		QMessageBox::information( this, "SAVE", "Save success!" );
	else
		QMessageBox::critical( this, "SAVE", "Save error!" );
}

void COpenTimeDelay::SlotBtnExitClicked()
{
	close();
}

void COpenTimeDelay::SlotChkEnableToggled(bool toggle)
{
	ui->sbFirst->setEnabled( toggle );
	ui->sbSecond->setEnabled( toggle );
	ui->sbFastOpenTime->setEnabled( toggle );
}

