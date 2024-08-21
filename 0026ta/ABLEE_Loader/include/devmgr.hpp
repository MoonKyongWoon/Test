/*H**********************************************************************
* FILENAME : devmgr.hpp
*
* DESCRIPTION :
*       Device Program Manager ( Extract / Execute / Terminate / Status )
*
* CHANGES :
* DATE          VER       AUTHOR    DETAIL
* 2023/01/03    v1.0.0    kwmoon    first write
*
*H*/

#ifndef CDEVMGR_HPP
#define CDEVMGR_HPP

#include <QObject>
#include <QProcess>
#include <QMap>
#include <sys/stat.h>
#include <sys/types.h>
#include "commstruct.hpp"
#include "sysdef.hpp"
#include "syslog.hpp"
#include "protocol.hpp"
#include "util.hpp"

enum DEV_EXPECT_ACK
{
	EXPECT_RUN ,
	EXPECT_QUIT,
	EXPECT_RUNTIME_ERROR,
	EXPECT_NONE,
};

class CDevMgr : public QObject
{
	Q_OBJECT
public:
	explicit CDevMgr(QObject *parent = nullptr);
	~CDevMgr();
public:
	void SetRunDir(QString);
private:
    QProcess::ProcessState Execute(uint);	//cpu
	bool Terminate(uint);
	bool Extract( QString );
    bool PushProcess(uint,QProcess*);
	bool PopProcess(uint);
    uint GetProcessCpu(QProcess*);
	void InitAckSum(DEV_EXPECT_ACK);
signals:
    void SigPgmRunAck(bool,QString);
    void SigPgmRerunAck(bool,QString);
    void SigPgmFinished( uint);
    void SigPgmUnloadAck(bool,QString);
	void SigLogMessage( LOG_TYPE, QString );
public slots:
	void SlotSlotCpuInfo(int,int,int);
    void SlotRunDevice(bool bRerun=false);
	void SlotTerminateDevice(bool);
	void SlotProgramInfo( QString, QString );
public slots:
	void SlotDevPgmFinished(int, QProcess::ExitStatus);
	void SlotDevPgmStdOutput();
    //void SlotDevPgmErrorOccurred(QProcess::ProcessError);
private:
	QProcess* m_pExtract;
    QMap<uint, QProcess*> m_mapDev;
    //QMap<uint, qint64> m_mapPid;
private:
	QString m_sDevice;
	QString m_sRunDir;
	QString m_sDownloadDir;
private:
	DEV_EXPECT_ACK m_DevExpectAck;
	uint m_nCpuAck;
	uint m_nCpuSum;
	uint m_nMySlot;
	bool m_bTwoDeviceRun;
};

#endif // CDEVMGR_HPP
