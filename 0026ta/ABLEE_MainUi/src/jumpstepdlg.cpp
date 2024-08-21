#include "jumpstepdlg.hpp"
#include "ui_jumpstepdlg.h"

CJumpStepDlg::CJumpStepDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CJumpStepDlg)
{
	m_pTestDataMgr = nullptr;
	InitUi();
}

CJumpStepDlg::~CJumpStepDlg()
{
	delete ui;
}

void CJumpStepDlg::InitUi()
{
	ui->setupUi(this);
	ui->treeItems->clear();
	ui->treeItems->setContextMenuPolicy( Qt::CustomContextMenu );
	ui->treeItems->setSelectionMode( QAbstractItemView::ExtendedSelection );
	ui->treeItems->header()->setStretchLastSection( true );
	//ui->treeItems->setColumnWidth(1,0);
	ui->treeItems->setColumnHidden( HEADER_STEP_NO      , true );
	ui->treeItems->setColumnHidden( HEADER_STEP_NAME_ORG, true );
	connect( ui->treeItems , &QTreeWidget::customContextMenuRequested, this, &CJumpStepDlg::SlotCustomContextMenuRequested );
	connect( ui->treeItems , &QTreeWidget::itemSelectionChanged      , this, &CJumpStepDlg::SlotItemSelectionChanged       );
	connect( ui->treeItems , &QTreeWidget::itemChanged               , this, &CJumpStepDlg::SlotItemChanged                );
	connect( ui->btnJump   , &QPushButton::clicked                   , this, &CJumpStepDlg::SlotBtnJumpClicked             );
	connect( ui->btnCancel , &QPushButton::clicked                   , this, &CJumpStepDlg::SlotBtnCancelClicked           );
	connect( ui->chkChamber, &QCheckBox::clicked                     , this, &CJumpStepDlg::SlotChkChamberClicked          );
	ui->treeItems->installEventFilter( this );
}

void CJumpStepDlg::SlotCustomContextMenuRequested( QPoint p )
{
	// Start with the standard menu.
	QTreeWidget* pSender = ui->treeItems;

	QAction* pActCheckAll        = new QAction( tr("CHECK   ALL"), this );
	QAction* pActUncheckAll      = new QAction( tr("UNCHECK ALL"), this );
	QAction* pActCheckSelected   = new QAction( tr("CHECK SELECTED STEP"), this );
	QAction* pActUncheckSelected = new QAction( tr("UNCHECK SELECTED STEP"), this );

	connect(pActCheckAll, SIGNAL(triggered()), this, SLOT(SlotCheckAllStep()));
	connect(pActUncheckAll, SIGNAL(triggered()), this, SLOT(SlotUncheckAllStep()));
	connect(pActCheckSelected, SIGNAL(triggered()), this, SLOT(SlotCheckSelectedStep()));
	connect(pActUncheckSelected, SIGNAL(triggered()), this, SLOT(SlotUncheckSelectedStep()));

	QMenu stepmenu(this);
	stepmenu.addAction( pActCheckAll );
	stepmenu.addAction( pActUncheckAll );
	stepmenu.addAction( pActCheckSelected );
	stepmenu.addAction( pActUncheckSelected );

	// Show the menu.
	QPoint q = pSender->mapToGlobal( p );
	stepmenu.exec( q );
}

//2023.08.25 - KWMOON
bool CJumpStepDlg::eventFilter(QObject *obj, QEvent *event)
{
	if( obj == ui->treeItems )
	{
		if( event->type() == QEvent::KeyPress )
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
			if( keyEvent->key() == Qt::Key_Space )
			{
				CheckUncheckStepsBySpace();
				return true;
			}
		}
		return false;
	}
	return QDialog::eventFilter(obj, event);
}

void CJumpStepDlg::SetTestDataMgrPtr(CFileMap* ptr)
{
	m_pTestDataMgr = ptr;
}

void CJumpStepDlg::SlotTestItemListup(uint slot)
{
	if( m_pTestDataMgr == nullptr ) return;
	if( (slot>(MAX_SLOT-1)) ) return;
	ui->spinLoopCount->setValue( 1 );
	if( !m_pTestDataMgr->IS_EXIST_TESTITEM(slot) )
	{
        //AppendSysTextEdit( _ERROR_LOG, tr("SLOT#%1 TestItem read error").arg(last_slot+1) );
		return;
	}
	QString sDevice = "";
	char szDevice[96] = {0x0,};
	if( !m_pTestDataMgr->GET_DEVICE(slot, CPU00, szDevice) )
		return;
	sDevice = QString(szDevice);

	stTestItem testitem;
	if( !m_pTestDataMgr->GET_TESTITEM( slot, CPU00, testitem) )
	{
		//AppendSysTextEdit( _ERROR_LOG, tr("SLOT#%1 TestItem read error").arg(last_slot+1) );
		return;
	}

	m_listItems.clear();
	for( uint i=0;i<testitem.nItemCount;i++ )
	{
		int nStepNo = i;
		QString sItem = QString("%1").arg(testitem.ItemStep[nStepNo].szItemName);
		m_listItems.append( sItem );
	}
	DisplayItemTree( sDevice );
}

void CJumpStepDlg::ClearItemTree()
{
	ui->treeItems->clear();
}

void CJumpStepDlg::DisplayItemTree( QString sDevice )
{
	ClearItemTree();
	ui->treeItems->setColumnCount( HEADER_COLUMN_COUNT );
	QTreeWidgetItem* top_item = new QTreeWidgetItem();
	top_item->setText( HEADER_STEP_NAME, sDevice );
	top_item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Unchecked );
	ui->treeItems->addTopLevelItem( top_item );
	for( int i=0;i<m_listItems.count();i++ )
	{
		QTreeWidgetItem* sub_item = new QTreeWidgetItem();
		QString sStepName = m_listItems.at(i).trimmed();
		sub_item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Unchecked );
		sub_item->setText(HEADER_STEP_NAME    , QString("%1.%2").arg(i+1,3,10,QChar('0')).arg(sStepName) );
		sub_item->setText(HEADER_STEP_NO      , tr("%1").arg(i) );
		sub_item->setText(HEADER_STEP_NAME_ORG, sStepName );
		top_item->addChild( sub_item );
	}
}

void CJumpStepDlg::SlotItemSelectionChanged()
{
	QList<QTreeWidgetItem*> selectItems = ui->treeItems->selectedItems();
	if( selectItems.count() == 0 ) return;

	bool bOk = true;
	for( QTreeWidgetItem* item : selectItems )
	{
		//item->columnCount()
		//qDebug() << item->text(HEADER_STEP_NAME) << " , " << item->text(1);
		int stepno = item->text(HEADER_STEP_NO).toInt(&bOk,10) + 1;
		ui->leStepNo->setText( tr("%1").arg(stepno) );
	}
}

void CJumpStepDlg::SlotItemChanged(QTreeWidgetItem *item, int column)
{
	if( !item->parent() )	//top level
	{
		if( item->checkState(HEADER_STEP_NAME) == Qt::CheckState::Checked )
			CheckAllStep();
		else				//child level
			UncheckAllStep();
	}
}

void CJumpStepDlg::SlotCheckAllStep()
{
	CheckAllStep();
}

void CJumpStepDlg::SlotUncheckAllStep()
{
	UncheckAllStep();
}

void CJumpStepDlg::CheckAllStep()
{
	uint top_count = ui->treeItems->topLevelItemCount();
	for( uint top=0;top<top_count;top++ )
	{
		QTreeWidgetItem* top_item = ui->treeItems->topLevelItem(top);
		if( top_item )
		{
			Qt::CheckState state = top_item->checkState(HEADER_STEP_NAME);
			if( state != Qt::CheckState::Checked )
				top_item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Checked );
			for( int child=0;child<top_item->childCount();child++ )
			{
				QTreeWidgetItem* child_item = top_item->child(child);
				Qt::CheckState state = child_item->checkState(HEADER_STEP_NAME);
				if( state != Qt::CheckState::Checked )
					child_item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Checked );
			}
		}
	}
}

void CJumpStepDlg::UncheckAllStep()
{
	uint top_count = ui->treeItems->topLevelItemCount();
	for( uint top=0;top<top_count;top++ )
	{
		QTreeWidgetItem* top_item = ui->treeItems->topLevelItem(top);
		if( top_item )
		{
			Qt::CheckState state = top_item->checkState(HEADER_STEP_NAME);
			if( state != Qt::CheckState::Unchecked )
				top_item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Unchecked );
			for( int child=0;child<top_item->childCount();child++ )
			{
				QTreeWidgetItem* child_item = top_item->child(child);
				Qt::CheckState state = child_item->checkState(HEADER_STEP_NAME);
				if( state != Qt::CheckState::Unchecked )
					child_item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Unchecked );

			}
		}
	}
}

void CJumpStepDlg::SlotCheckSelectedStep()
{
	QList<QTreeWidgetItem*> selectItems = ui->treeItems->selectedItems();
	if( selectItems.count() == 0 ) return;

	bool bOk = true;
	for( QTreeWidgetItem* item : selectItems )
	{
		Qt::CheckState state = item->checkState(HEADER_STEP_NAME);
		if( state != Qt::CheckState::Checked )
			item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Checked );
	}
}

void CJumpStepDlg::SlotUncheckSelectedStep()
{
	QList<QTreeWidgetItem*> selectItems = ui->treeItems->selectedItems();
	if( selectItems.count() == 0 ) return;

	bool bOk = true;
	for( QTreeWidgetItem* item : selectItems )
	{
		Qt::CheckState state = item->checkState(HEADER_STEP_NAME);
		if( state != Qt::CheckState::Unchecked )
			item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Unchecked );
	}
}

void CJumpStepDlg::CheckUncheckStepsBySpace()
{
	QList<QTreeWidgetItem*> selectItems = ui->treeItems->selectedItems();
	if( selectItems.count() < 1 ) return;

	for( QTreeWidgetItem* item : selectItems )
	{
		Qt::CheckState state = item->checkState(HEADER_STEP_NAME);
		if( state == Qt::CheckState::Unchecked )
			item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Checked );
		else
			item->setCheckState( HEADER_STEP_NAME, Qt::CheckState::Unchecked );
	}
}

void CJumpStepDlg::InitJumpItems()
{
	memset( &m_stJumpItems, 0x0, sizeof(stTestItem) );
}

bool CJumpStepDlg::MakeJumpItems()
{
	InitJumpItems();

	bool bSelect = false;
	uint top_count = ui->treeItems->topLevelItemCount();
	for( uint top=0;top<top_count;top++ )
	{
		QTreeWidgetItem* top_item = ui->treeItems->topLevelItem(top);
		if( top_item )
		{
			m_stJumpItems.bItemCategory = false;
			m_stJumpItems.nLoopCount    = ui->spinLoopCount->value();;
			for( int child=0;child<top_item->childCount();child++ )
			{
				bool bOk = true;
				bool bTest = false;
				QTreeWidgetItem* child_item = top_item->child(child);
				Qt::CheckState state = child_item->checkState(HEADER_STEP_NAME);
				if( state == Qt::CheckState::Checked )
				{
					bTest = true;
					bSelect = true;
				}
				int nStepNo       = child_item->text( HEADER_STEP_NO ).toInt(&bOk,10);
				QString sStepName = child_item->text( HEADER_STEP_NAME_ORG ).trimmed();
				if( !bOk ) continue;

				m_stJumpItems.ItemStep[child].bTestItem = bTest;
				m_stJumpItems.ItemStep[child].nStepNo   = nStepNo;
				snprintf( m_stJumpItems.ItemStep[child].szItemName, sizeof(m_stJumpItems.ItemStep[child].szItemName), "%s", sStepName.toStdString().c_str() );
				m_stJumpItems.nItemCount++;
			}
		}
	}
	if( !bSelect ) return false;
	return true;
}

int  CJumpStepDlg::GetJumpItemCount()
{
	return 0;
}

void CJumpStepDlg::SlotBtnJumpClicked()
{
	QMessageBox::StandardButton resBtn =
	QMessageBox::question( this, this->windowTitle(), tr("Do you want to jump test?"),
	QMessageBox::No | QMessageBox::Yes,
	QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes)
	{
		if( !MakeJumpItems() )
		{
			QMessageBox::critical( this, this->windowTitle(), tr("Jump Step is empty, please select jump step!") );
			return;
		}
		emit SigJumpLogMessage( _NORMAL_LOG, tr("START JUMP TEST [COUNT:%1]").arg(m_stJumpItems.nItemCount) );
		emit SigJumpTest( m_stJumpItems );
		accept();
	}
}

void CJumpStepDlg::SlotBtnCancelClicked()
{
	ui->spinLoopCount->setValue( 1 );
	UncheckAllStep();
	accept();
}

void CJumpStepDlg::SlotChkChamberClicked( bool bCheck)
{
	//emit SigJumpLogMessage( _NORMAL_LOG, tr("Jump Chamber Checkbox value : %1").arg(bCheck) );
	emit SigJumpChamberUse( bCheck );
}
