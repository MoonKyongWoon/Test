#ifndef JUMPSTEPDLG_HPP
#define JUMPSTEPDLG_HPP

#include <QAction>
#include <QDebug>
#include <QDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "common.hpp"
#include "commstruct.hpp"
#include "filemaplib.hpp"
#include "syslog.hpp"

namespace Ui {
	class CJumpStepDlg;
}

enum EN_TREE_HEADER
{
	HEADER_STEP_NAME   = 0x0,
	HEADER_STEP_NO          ,
	HEADER_STEP_NAME_ORG    ,
	HEADER_COLUMN_COUNT     ,
};

class CJumpStepDlg : public QDialog
{
	Q_OBJECT

public:
	explicit CJumpStepDlg(QWidget *parent = nullptr);
	~CJumpStepDlg();
public:
	void SetTestDataMgrPtr(CFileMap*);
public slots:
	void SlotCustomContextMenuRequested( QPoint );
public slots:
	void SlotTestItemListup(uint);
	void SlotItemSelectionChanged();
	void SlotItemChanged(QTreeWidgetItem *item, int column);
	void SlotCheckAllStep();
	void SlotUncheckAllStep();
	void SlotCheckSelectedStep();
	void SlotUncheckSelectedStep();
public slots:
	void SlotBtnJumpClicked();
	void SlotBtnCancelClicked();
	void SlotChkChamberClicked(bool bCheck);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private:
	void InitUi();
	void ClearItemTree();
	void DisplayItemTree( QString );
	void CheckAllStep();
	void UncheckAllStep();
	void InitJumpItems();
	bool MakeJumpItems();
	int  GetJumpItemCount();
	void CheckUncheckStepsBySpace();
signals:
	void SigJumpLogMessage ( LOG_TYPE, QString );
	void SigJumpTest( stTestItem );
	void SigJumpChamberUse(bool);
private:
	Ui::CJumpStepDlg *ui;
	stTestItem m_stTestItem;
	CFileMap* m_pTestDataMgr;
	QStringList m_listItems;
	stTestItem m_stJumpItems;
};

#endif // JUMPSTEPDLG_HPP
