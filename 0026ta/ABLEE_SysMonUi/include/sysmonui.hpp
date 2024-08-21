#ifndef SYSMONUI_HPP
#define SYSMONUI_HPP

#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QWidget>
#include <QSystemTrayIcon>
#include "syspfm.hpp"
#include "syspwr.hpp"
#include "sysver.hpp"

namespace Ui {
	class CSysMonUi;
}

#define PROGRAM_NAME	"ATMONUI"

class CSysMonUi : public QWidget
{
	Q_OBJECT

public:
	explicit CSysMonUi(QWidget *parent = nullptr);
	~CSysMonUi();
private:
	void RegisterMetaType();
	void InitUi();
	void InitTab();
	void CreateActions();		// for trayicon
	void CreateTrayIcon();		// for trayicon
	void SetTrayIcon( bool );	// for trayicon
protected:
	void closeEvent(QCloseEvent *event);
//	void showEvent(QShowEvent *event);
private:
	Ui::CSysMonUi *ui;
	CSysPwr* m_pSysPwrUi;
	CSysPfm* m_pSysPfmUi;

	bool m_bShow;
	QSystemTrayIcon *trayIcon;	// for trayicon
	QAction *restoreAction;		// for trayicon
	QAction *quitAction;		// for trayicon
	QMenu *trayIconMenu;		// for trayicon

public slots:
	void QuitProgram();
};

#endif // SYSMONUI_HPP
