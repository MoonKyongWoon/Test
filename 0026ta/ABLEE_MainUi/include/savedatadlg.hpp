#ifndef SAVEDATADLG_HPP
#define SAVEDATADLG_HPP

#include <QMovie>
#include <QDialog>
#include <QProcess>
#include <QKeyEvent>
#include "syslog.hpp"
#include "sysver.hpp"
#include "util.hpp"

namespace Ui {
	class CSaveDataDlg;
}

#define PASS_TEXT_QCOLOR		QColor(  0,  0,  0)
#define FAIL_TEXT_QCOLOR		QColor(255, 77, 77)
#define FAULT_TEXT_QCOLOR		QColor(255,  0,  0)	//2023.03.18

class CSaveDataDlg : public QDialog
{
	Q_OBJECT

public:
	explicit CSaveDataDlg(QWidget *parent = nullptr);
	~CSaveDataDlg();
public:
	void Test();
protected:
	void keyPressEvent(QKeyEvent *e);
public slots:
	void SlotMakeReport();
	void SlotResultSendSavedMap(int exit, QProcess::ExitStatus stat, QString cmd);
public slots:
	void SlotSaveDataFinished(int exit, QProcess::ExitStatus stat);
	void SlotSaveDataStdOutput();
	void SlotBtnSaveClicked();
	void SlotBtnHideClicked();
signals:
	void SigSysLogMessage ( LOG_TYPE, QString );
	void SigMakeReportDone(bool);
private:
	void Init();
	void AppendLog(LOG_TYPE t, QString s);
	void RunMovie();
	void StopMovie();
	void ClearLog();
	bool MakeData();
	bool RunSaveData();
	bool SendSavedMap();

private:
	Ui::CSaveDataDlg *ui;
	QMovie* m_pMovie;
	QProcess* m_pSaveProcess;
	bool m_bAutoScroll;
};

#endif // SAVEDATADLG_HPP
