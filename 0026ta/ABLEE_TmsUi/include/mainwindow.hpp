#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QAction>
#include <QCloseEvent>
#include <QLabel>
#include <QFrame>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QSettings>
#include <QStyleFactory>
#include <QToolBar>
#include <QToolButton>
#include <QTimer>
#include <QVBoxLayout>
#include "statusbar.hpp"
#include "config.hpp"
#include "tmsui.hpp"
#include "sysini.hpp"
#include "sysver.hpp"
#include "util.hpp"

enum _LOG_TAB
{
	_LOG_SYSTEM  ,
	_LOG_SML      ,
};

enum _STATUS_LABEL
{
	_STATUS_EAP_TCP     ,
	_STATUS_EAP_SELECT  ,
	_STATUS_MAINUI_IPC  ,
	_STATUS_CHMB_COMM   ,
	_STATUS_MAX_LABEL   ,
};

namespace Ui {
	class CMainWindow;
}

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit CMainWindow(QWidget *parent = nullptr);
	~CMainWindow();
private:
	void RegisterMetaType();
	void InitVariables();
	void InitUi();	
	void CreateToolbar();
	void CreateStatusBar();
	void SetStatusMessage( QString );
protected:
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);
signals:
	void SigStartService();
	void SigRestartService();
	void SigStatusChanged( _LED_DESC, bool );

signals:
	void SigSysLog( LOG_TYPE, QString );

public slots:
	void SlotIpcStatusChanged(bool);
	void SlotTcpStatusChanged(bool);
	void SlotHsmsStatusChanged(bool);
	void SlotActConfig();
	void SlotActReSend();
	void SlotConfigSaved();
	void SlotConfigRestartService();

private:
	QAction *m_pActConfig;
	QAction *m_pActResend;
	bool m_bInitialized;

private:
	Ui::CMainWindow *ui;
	CTmsUi* m_pTmsUi;
	CConfig* m_pConfigDlg;
	CStatusBar* m_pStatusBar;
};

#endif // MAINWINDOW_HPP
