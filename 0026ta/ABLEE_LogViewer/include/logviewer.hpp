#ifndef LOGVIEWER_HPP
#define LOGVIEWER_HPP

#include <QCloseEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include "sysdef.hpp"
#include "sysver.hpp"
#include "slotviewer.hpp"

enum EN_STATUS_LABEL
{
	STATUS_BAR_CPU0          ,
	STATUS_BAR_CPU1          ,
	MAX_STATUS_LABEL         ,
};

#define PASS_BG_QCOLOR				QColor(255,255,255)
#define FAIL_BG_QCOLOR				QColor(255,  0,  0)

#define PASS_STBAR_QCOLOR			"rgb(243,243,243)"
#define FAIL_STBAR_QCOLOR			"rgb(255, 43, 43)"

namespace Ui {
	class CLogViewer;
}

class CLogViewer : public QMainWindow
{
	Q_OBJECT

public:
	explicit CLogViewer(QWidget *parent = nullptr);
	~CLogViewer();
protected:
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
private:
	void InitUi();
	void InitValriables();
	void CreateStatusBar();
	void SetStatusBarMessage( EN_STATUS_LABEL kind, QString msg, bool bError );
private:
	Ui::CLogViewer *ui;
	QLabel* m_pStatusBarLabel[MAX_STATUS_LABEL];
};

#endif // LOGVIEWER_HPP
