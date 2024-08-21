#ifndef LOGVIEW_HPP
#define LOGVIEW_HPP

#include <QAction>
#include <QScrollBar>
#include <QMenu>
#include <QPlainTextEdit>
#include <QWidget>
#include "common.hpp"
#include "commstruct.hpp"
#include "syslog.hpp"
#include "logger.hpp"

namespace Ui {
	class CLogView;
}

class CLogView : public QWidget
{
	Q_OBJECT

public:
	explicit CLogView(QWidget *parent = nullptr);
	~CLogView();
signals:
	void SigSaveSysLog( LOG_TYPE, QString );
public slots:
	void SlotCustomContextMenuRequested( QPoint );
	void SlotScrollStatusChanged();
	void SlotAppendSysLog( LOG_TYPE, QString );
private:
	void InitUi();
	void InitSysLogView();
	void AppendSysLog( LOG_TYPE, QString );
private:
	Ui::CLogView *ui;
	CLogger*  m_pSysLogger;
	bool m_bAutoScrollSysLog;
};

#endif // LOGVIEW_HPP
