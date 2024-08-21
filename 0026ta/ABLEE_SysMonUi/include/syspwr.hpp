#ifndef SYSPWR_HPP
#define SYSPWR_HPP

#include <QAction>
#include <QComboBox>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QLCDNumber>
#include <QPlainTextEdit>
#include <QPoint>
#include <QTimer>
#include <QThread>
#include <QWidget>
#include "syspwrth.hpp"
#include "util.hpp"

namespace Ui {
	class CSysPwr;
}

#define MAX_LOG_LINE_NUM		(32768)
#define PASS_QCOLOR				QColor(  0,  0,  0)
#define FAIL_QCOLOR				QColor(255,  0,  0)
#define PIXMAP_PASS				QPixmap("://icon/green_marble.ico")
#define PIXMAP_FAIL				QPixmap("://icon/red_marble.ico")
#define PIXMAP_MASK				QPixmap("://icon/black_marble.ico")

class CSysPwr : public QWidget
{
	Q_OBJECT

public:
	explicit CSysPwr(QWidget *parent = nullptr);
	~CSysPwr();
private:
	void InitVariables();
	void LoadIni();
	void InitUi();
	void InitUiComm();
	void InitUiLineVA();
	void InitUiModules();
	void InitUiModuleStat();
	void InitUiLog();
	void InitSigSlot();
	void InitTimer();
	void MakePMBusCommandMap();
	void AppendLog( SIDE_AB, LOG_TYPE, QString );
private:
	void CreateThread();
signals:
	void SigManualTest(stUiRequest);
    void SigOpenDevice();
    void SigTextTest(SIDE_AB, QString);
    void SigCloseDeviceA();
    void SigCloseDeviceB();
    void SigStartMon(SIDE_AB);
public slots:
    void SlotOpenDeviceResult( SIDE_AB, bool );
public slots:
	void SlotCustomContextMenuRequested( QPoint );
	void SlotScrollStatusChanged();
	void SlotCboAddressChanged(int);
	void SlotCboCommandChanged(int);
	void SlotBtnManualTestClicked();
    void SlotTimeout485Con();

public slots:
	void SlotLogMessage( SIDE_AB, LOG_TYPE, QString );
    void SlotLineMeasData( SIDE_AB, stLineMeas );
    void SlotModuleMeasData( SIDE_AB, stModulelMeas );

private:
	Ui::CSysPwr *ui;
	QLCDNumber* m_pLcdLine[MAX_SIDE][MAX_LINE][MAX_LINE_ITEM];
	QLCDNumber* m_pLcdModl[MAX_SIDE][MAX_MODULE][MAX_MODULE_ITEM-1];
	QLabel*     m_pStatModl[MAX_SIDE][MAX_MODULE];
	bool m_bAutoScroll;
private:
	std::map<QString, stPmubusCommand> m_mapPbCmdIsoComm;
	std::map<QString, stPmubusCommand> m_mapPbCmdModule;

	stLineMeas    m_LineMeas[MAX_SIDE][MAX_LINE];
	stModulelMeas m_ModlMeas[MAX_SIDE][MAX_MODULE];
	QString m_stty[MAX_SIDE];
private:
	CSysPwrTh* m_pPwrTh [MAX_SIDE];
	QThread*   m_pThread[MAX_SIDE];
    QTimer* m_tmr485con;
    QMutex  m_mutex;
    bool    m_bOpen[MAX_SIDE];
};

#endif // SYSPWR_HPP
