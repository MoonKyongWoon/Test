#ifndef CPUVIEWER_HPP
#define CPUVIEWER_HPP

#include <QAction>
#include <QThread>
#include <QTimer>
#include <QMenu>
#include <QWidget>
#include "logreader.hpp"
#include "sysdef.hpp"
#include "syslog.hpp"
#include "util.hpp"

namespace Ui {
	class CCpuViewer;
}

#define LOG_REFRESH_PERIOD			(3)

class CCpuViewer : public QWidget
{
	Q_OBJECT

public:
	explicit CCpuViewer(QWidget *parent = nullptr);
	~CCpuViewer();
public:
	void SetSlotCpuNo(uint,uint);
public slots:
	void SlotReadLog(QString);
	void SlotViewLineChanged(int);
	void SlotReadStatus(qint64,qint64);
	void SlotFileStatus(bool);
	void SlotLogFile(QString);
signals:
	void SigInitLogReader();
	void SigReadLog();
	void SigStopLog();
public:
	void SlotCpuViewerInit();
	void SlotTimeoutLogRefresh();
	void SlotViewStopToggled(bool);
	void SlotCustomContextMenuRequested( QPoint );
	void SlotScrollStatusChanged(bool);
	void SlotBtnReloadClicked();
private:
	void InitUi();
	void InitVariables();
	void InitLogThread();
	void InitTimer();
	void SetLogMaxViewLine(int);
	void SetLogViewEnable(bool);
private:
	Ui::CCpuViewer *ui;
	uint m_nSlotNo;
	uint m_nCpuNo;
	CLogReader* m_pLogReader;
	QThread* m_pLogTh;
	QTimer m_tmrLogView;
private:
	QString m_sLogPath;
	QString m_sLogFile;
	uint m_nViewLine;
	bool m_bStart;
	bool m_bAutoScroll;
	QPixmap m_pixmapOK;
	QPixmap m_pixmapNG;

};

#endif // CPUVIEWER_HPP

