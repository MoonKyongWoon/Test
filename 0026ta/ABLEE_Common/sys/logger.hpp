#ifndef CLOGGER_HPP
#define CLOGGER_HPP

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>

#include "syslog.hpp"
#include "protocol.hpp"
#include "util.hpp"

class CLogger : public QObject
{
	Q_OBJECT
public:
	CLogger( PROTOCOL::SOURCE_TYPE );
	CLogger( PROTOCOL::SOURCE_TYPE, uint r );
	~CLogger();
private:
	void Init();
	void SaveLog( LOG_TYPE, QString );
    void SaveSlotFile( QString );
	void InitLogDir();
public:
	void SetDebugPrint(bool);
	void SetLogDir(QString);
	void Append ( LOG_TYPE, const char*, ... );		//NORM, ERR, FAULT
public slots:
	void SlotLogMessage( LOG_TYPE, QString );
	void SlotLogSlotMessage( uint, uint, LOG_TYPE, QString );
private:
	PROTOCOL::SOURCE_TYPE m_AppType;
	uint m_SubType;
	QString  m_LogRoot;
	QString  m_LogSystem;
	QString  m_LogChamber;
	QString  m_LogSite;
	QString  m_LogDir;
private:
	QString  m_AppName;
	QMutex   m_mutex;
	bool     m_bDebugPrint;
};

#endif
