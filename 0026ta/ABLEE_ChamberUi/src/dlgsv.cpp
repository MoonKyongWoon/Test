#include "dlgsv.hpp"
#include "ui_dlgsv.h"

CDlgSv::CDlgSv(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CDlgSv)
{
	ui->setupUi(this);
	connect( ui->btnSet, &QPushButton::clicked, this, &CDlgSv::SlotBtnSetClicked );
}

CDlgSv::~CDlgSv()
{
	delete ui;
}

void CDlgSv::setText(QString text)
{
	ui->btnSet->setText( text );
}

void CDlgSv::setTempLimit( double l, double h)
{
	m_dLimitLow = l;
	m_dLimitHigh = h;
}

void CDlgSv::setMode(EN_SV_MODE mode)
{
	m_SvMode = mode;
	if( mode == SV_TEMP )
		ui->btnSet->setText( "TEMP SV" );
	else
		ui->btnSet->setText( "AMB SV" );
}

void CDlgSv::SlotBtnSetClicked()
{
	double dSet = ui->spinSV->value();
	if( (dSet<m_dLimitLow) || (dSet>m_dLimitHigh) )
	{
		QMessageBox::critical( this, this->windowTitle(), tr("SV(%1) range error, use %2 ~ %3").arg(dSet,5).arg(m_dLimitLow,5).arg(m_dLimitHigh,5) );
		return;
	}
	emit SigSvSet( m_SvMode, dSet );
	accept();
}
