#include "slotviewer.hpp"
#include "ui_slotviewer.h"

CSlotViewer::CSlotViewer(uint s, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSlotViewer),
	m_nSlotNo(s)
{
	InitUi();
	InitCpuLog();
}

CSlotViewer::~CSlotViewer()
{
	delete ui;
}

void CSlotViewer::InitUi()
{
	ui->setupUi(this);
}

void CSlotViewer::InitCpuLog()
{
	ui->widgetCpu0->SetSlotCpuNo( m_nSlotNo, CPU00 );
	ui->widgetCpu1->SetSlotCpuNo( m_nSlotNo, CPU01 );
	connect( this, &CSlotViewer::SigCpuViewerInit, ui->widgetCpu0, &CCpuViewer::SlotCpuViewerInit );
	connect( this, &CSlotViewer::SigCpuViewerInit, ui->widgetCpu1, &CCpuViewer::SlotCpuViewerInit );

	emit SigCpuViewerInit();
}
