#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <QAction>
#include <QMenu>
#include <QPlainTextEdit>
#include <QWidget>
#include "slotview.hpp"
#include "systemview.hpp"
#include "sysdef.hpp"

namespace Ui {
class CMainView;
}

class CMainView : public QWidget
{
    Q_OBJECT

public:
    explicit CMainView(QWidget *parent = nullptr);
    ~CMainView();
private:
    void InitUi();
	void InitSysLogTab();
	void InitSlotTab();
	void InitVariables();
protected:
    void resizeEvent(QResizeEvent *event);
private:
	Ui::CMainView *ui;
	CSystemView* m_pSysView;
	CSlotView* m_pSlotView[MAX_SLOT];
	bool m_bAutoScrollSys;

};

#endif // MAINVIEW_HPP
