#include "mainview.hpp"
#include "ui_mainview.h"

CMainView::CMainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMainView)
{
	InitVariables();
	InitUi();
}

CMainView::~CMainView()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////
/// \brief CMainView::InitVariables
///
void CMainView::InitVariables()
{
	m_bAutoScrollSys = true;
}

///////////////////////////////////////////////////////////////////////
/// \brief CMainView::InitUi
/// \details Maybe can add widgets later, so use the layout
void CMainView::InitUi()
{
	ui->setupUi(this);
	ui->tabSlot->clear();
	InitSysLogTab();
	InitSlotTab();
}

void CMainView::InitSysLogTab()
{
	m_pSysView = new CSystemView;
	ui->tabSlot->addTab( m_pSysView, QIcon(":/icon/icon/file_info.ico"), tr("SYSTEM") );
}

void CMainView::InitSlotTab()
{
	for( int t=0;t<MAX_SLOT;t++ )
	{
		m_pSlotView[t] = new CSlotView;
		ui->tabSlot->addTab( m_pSlotView[t], QIcon(":/icon/icon/chip_device.ico"), tr("SLOT%1").arg(t+1) );
	}
}

void CMainView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED( event )
}

