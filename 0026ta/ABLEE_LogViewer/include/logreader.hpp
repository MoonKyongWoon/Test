#ifndef CLOGREADER_HPP
#define CLOGREADER_HPP

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "sysdef.hpp"
#include "syslog.hpp"
#include "util.hpp"

typedef struct _stFileInfo
{
	QString ymd;
	QString filepath;
	QString filename;
	QString filefullname;
	qint64 filesize;
	qint64 lastoffset;
	qint64 readsize;
}stFileInfo;

#define MBYTE				*1024*1024
#define ONCE_READ_SIZE		(3*(1024*1024))

class CLogReader : public QObject
{
	Q_OBJECT
public:
	explicit CLogReader( uint, uint, QObject *parent = nullptr);
	~CLogReader();
public slots:
	void SlotStandBy();
	void SlotReadStart();
	void SlotReadStop();
signals:
	void SigLogFileName(QString);
	void SigUpdateStatus(qint64,qint64);
	void SigLogData(QString);
	void SigFileStatus(bool);
private:
	void LogFileSet();
	bool IsDateChanged();
private:
	uint m_nSlotNo;
	uint m_nCpuNo;

private:
	uint m_nViewLine;
	uint m_nUpdateLine;
	stFileInfo m_fileinfo;
	bool m_bBusy;
};

#endif // CLOGREADER_HPP
