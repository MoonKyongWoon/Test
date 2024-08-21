#include "msgdlg.hpp"
#include "ui_msgdlg.h"

CMsgDlg::CMsgDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CMsgDlg)
{
	ui->setupUi(this);
	InitUi();
}

CMsgDlg::~CMsgDlg()
{
	delete ui;
}

void CMsgDlg::InitUi()
{
	Clear();
	connect( ui->btnSave, &QPushButton::clicked, this, &CMsgDlg::SlotSave );
	connect( ui->btnCancel, &QPushButton::clicked, this, &CMsgDlg::SlotCancel );
}

void CMsgDlg::Clear()
{
	ui->leSection->clear();
	ui->leDesc->clear();
	ui->teSML->clear();
}

void CMsgDlg::SetSectionList(QStringList sections)
{
	m_listSection = sections;
}

void CMsgDlg::SetSection(QString s)
{
	m_sSection = s;
	ui->leSection->setText( s );
}
void CMsgDlg::SetDescription(QString desc)
{
	m_sDescription = desc;
	ui->leDesc->setText( desc );
}

void CMsgDlg::SetSML(QString sml)
{
	m_sSML = sml;
	ui->teSML->clear();
	ui->teSML->appendPlainText( sml );
}

void CMsgDlg::SlotSave()
{
	QString section = ui->leSection->text().trimmed();
	QString description = ui->leDesc->text().trimmed();
	QString sml = ui->teSML->document()->toPlainText().trimmed();

	if( section.isEmpty() )
	{
		QMessageBox::critical( 0, "SAVE ERROR!", QObject::tr("SECTION is empty!"), QMessageBox::Ok );
		ui->leSection->setFocus();
		return;
	}

	if( sml.isEmpty() )
	{
		QMessageBox::critical( 0, "SAVE ERROR!", QObject::tr("SML is empty!"), QMessageBox::Ok );
		ui->teSML->setFocus();
		return;
	}

	for( QString item : m_listSection )
	{
		if( item == section )
		{
			QMessageBox::critical( 0, "SAVE ERROR!", QObject::tr("%1 is already exist!").arg(section), QMessageBox::Ok );
			return;
		}
	}
	emit SigSave( section, description, sml );
	accept();
}

void CMsgDlg::SlotCancel()
{
	close();
}
