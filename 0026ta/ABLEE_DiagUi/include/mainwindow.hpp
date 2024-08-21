#ifndef CALDIAG_HPP
#define CALDIAG_HPP

#include <QApplication>
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCloseEvent>
#include <QDebug>
#include <QDockWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QStatusBar>
#include <QSettings>
#include <QSpinBox>
#include <QStyleFactory>
#include <QToolBar>
#include <QTabWidget>
#include <QThread>
#include <QVBoxLayout>

#include "mainview.hpp"
#include "slotbox.hpp"
#include "common.hpp"
#include "sysini.hpp"
#include "sysver.hpp"

class CDiagUi : public QMainWindow
{
    Q_OBJECT
public:
	explicit CDiagUi( QString name, QWidget *parent = nullptr );
	~CDiagUi();
private:
    void InitInstance();
    void InitVariable();
    void InitUi();
    void InitTitles();
    void CreateToolbar();
    void CreateDockWindows();
private:
    void SetStatusBarText(QString);
public:
    void ResizeWidgets();
protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
private:
    QString m_sApplicationName;
    QStringList m_listLogMode;
    QMenu *m_pMenu;
    QAction *m_pActLoad;
    QAction *m_pActReload;
    QAction *m_pActTest;
    QAction *m_pActStop;
    QAction *m_pActSave;
private:
    CSlotBox       *m_pSlotBox;
    CMainView      *m_pMainView;
public slots:
    void SlotActLoad();
    void SlotActReload();
    void SlotActTest();
    void SlotActStop();
};

#endif // CALDIAG_HPP
