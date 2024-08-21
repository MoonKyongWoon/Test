#ifndef CHAMBERUI_HPP
#define CHAMBERUI_HPP

#include <QAction>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QShortcut>
#include <QSystemTrayIcon>
#include <QToolBar>
#include "chamberview.hpp"
#include "statusbar.hpp"

#define CONFIG_INI_KEY				"at6200"

namespace Ui {
	class CChamberUi;
}

class CChamberUi : public QMainWindow
{
	Q_OBJECT

public:
	explicit CChamberUi(QWidget *parent = nullptr);
	~CChamberUi();
private:
	void InitUi();
	void InitVariable();
	void InitInstance();
	void InitStatusBar();
	void SetChmbControls(bool);
	void RegisterMetaType();

private:
	void CreateActions();
	void CreateTrayIcon();
	void SetTrayIcon( bool );
protected:
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);
signals:
	void SigStartChmbIpc();
public slots:
	void QuitProgram();
private:
	bool m_bShow;
	Ui::CChamberUi *ui;
	CChamberView* m_pChmbView;
	CStatusBar* m_pStatusBar;
private:
	QSystemTrayIcon *trayIcon;	//2023.05.22 - kwmoon
	QAction *restoreAction;		//2023.05.22 - kwmoon
	QAction *quitAction;		//2023.05.22 - kwmoon
	QMenu *trayIconMenu;		//2023.05.22 - kwmoon
};

#endif // CHAMBERUI_HPP
