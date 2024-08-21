#include "passdlg.hpp"
#include "ui_passdlg.h"

CPassDlg::CPassDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CPassDlg)
{
	ui->setupUi(this);
	Init();
}

CPassDlg::~CPassDlg()
{
	delete ui;
}

void CPassDlg::Init()
{
	m_mode = PASSWD_DC_ON;
	connect( ui->pushButton, &QPushButton::clicked, this, &CPassDlg::SlotCheckCklicked );
}

void CPassDlg::SetMode(PASSWD_MODE mode)
{
	m_mode = mode;
	ui->lePasswd->setText("");
}

void CPassDlg::SlotCheckCklicked()
{
	if( ui->lePasswd->text().trimmed() == "" )
	{
		QMessageBox::critical( this, "Password", tr("Input the password"), QMessageBox::Ok );
		ui->lePasswd->setFocus();
		return;
	}
	QString sPassword = ui->lePasswd->text().trimmed();
	if( sPassword == PASSWORD )
	{
		emit SigAccepted(m_mode);
		close();
	}
	else
	{
		QMessageBox::critical( this, "Pasword", tr("Passwords do not match!"), QMessageBox::Ok );
		ui->lePasswd->setFocus();
	}
}
