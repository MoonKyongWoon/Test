/*H**********************************************************************
* FILENAME : loader.hpp
*
* DESCRIPTION :
*       CPU Daemon Program for Device Test/Communication with ATMGR
*
* CHANGES :
* DATE          VER       AUTHOR    DETAIL
* 2023/01/03    v1.0.0    kwmoon    first write
*
*H*/
#ifndef CLOADER_HPP
#define CLOADER_HPP

#include <QObject>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QThread>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QProcess>
#include <iostream>
#include <stdlib.h>
#include "bitchart.hpp"
#include "commstruct.hpp"
#include "devmgr.hpp"
#include "downloader.hpp"
#include "localserver.hpp"
#include "logger.hpp"
#include "io.hpp"
#include "sysini.hpp"
#include "protocol.hpp"
#include "tcpclient.hpp"
#include "tcpserver.hpp"

enum EN_BIB_SEL
{
	LBIB_SEL0    = (0x1<<0),
	LBIB_SEL1    = (0x1<<1),
	LBIB_SEL2    = (0x1<<2),
	LBIB_SEL3    = (0x1<<3),
};

#define BIB_MAX_SEL		( 4)
#define LOOPBACK		"127.0.0.1"

class CLoader : public QObject
{
    Q_OBJECT
public:
    CLoader();
    ~CLoader();
    void CleanUp();
public:
	void showHelp();
    int parseArguments(int, char*[]);

public slots:
	void SlotMgrAcceptError(QAbstractSocket::SocketError);
	void SlotMgrIncommingConnection( qintptr );
	void SlotMgrStatusChanged(bool);
	void SlotMgrReceivePacket(stTcpElement);
	void SlotMgrLogMessage(LOG_TYPE, QString);
public slots:
	void SlotToolAcceptError(QAbstractSocket::SocketError);
	void SlotToolIncommingConnection( qintptr );
	void SlotToolStatusChanged(bool);
	void SlotToolReceivePacket(stTcpElement);
	void SlotToolLogMessage(LOG_TYPE, QString);
public slots:
	void SlotDownloadFtpResult(bool, QString);
	void SlotDownloadProgress(int);
public slots:
	void SlotDevMgrLogMessage( LOG_TYPE, QString );
	void SlotDevMgrRunAck( bool, QString );
	void SlotDevMgrReRunAck( bool, QString );
	void SlotDevMgrUnloadAck( bool, QString );
	void SlotDevMgrPgmFinished( uint );
signals:
	void SigDownloadProgram();
	void SigSaveLog(LOG_TYPE, QString );
signals:
	void SigMgrSendPacket( char*, uint );
	void SigDevMgrSlotCpuInfo( int, int, int );
	void SigDevMgrProgramInfo( QString, QString );
	void SigDevMgrRunDevPgm(bool);
	void SigDevMgrTerminateDevPgm(bool);
private:
    void Run();
    void InitInstance();
    void InitDirectory();
	void SetDownloadStatusProgress(int);
    bool MakeDirectory(QString);
    bool LoadMyIp();
	bool RunTcpMgrServer();
	bool RunTcpToolServer();
	bool RunLocalServer();
    bool RunDownloadThread();
	bool RunDevMgrThread();
    bool RunLogger();
	bool ChmodSetX( QString );
	bool MakeFtpIni();
private:
	bool LoadFtpIni();						//not use
    bool PrepareDownload();
    bool DeleteDownloadDirectoryFiles();
    bool DownloadProgram();
    bool ReloadProgram();
	bool StandbyExecuteProgram();
private:
	bool AckVersion();
	bool SetSlotCpuInfo( stTcpElement& );
	bool SetEwsFtpInfo ( stTcpElement& );
	bool SetPgmLoadInfo ( stTcpElement& );
	bool AckSlotCpuInfo(bool);
	bool AckEwsFtpInfo(bool);
	bool AckReadBibId(PROTOCOL::TCP_COMMAND);
	bool AckPgmLoadStatus(bool, QString);
	bool AckPgmLoad(bool,bool);
	bool AckPgmUnload(bool,QString);
	bool AckPgmFinished();
	bool IsValidBibIdParity(uint);

private:
    QString m_sMyIp;
    QString m_sCurrentOsPath;
	QString m_sCurrentOsConfig;
	QString m_sCurrentRunPath;		//atsystem
	QString m_sCurrentWorkPath;		//atsystem/run
    QString m_sDownloadPath;
	QString m_sProgramPath;
	QString m_sLogPath;
	QString m_sConfigPath;			//2023.07.25 - KWMOON
	QString m_sCfgPath;				//2023.07.25 - KWMOON
	int m_bDebug;
private:
	CDevMgr*      m_pDevMgr;
	QThread*      m_pDevMgrTh;
	CDownloader*  m_pDownloader;
	QThread*      m_pDownloaderTh;
	CTcpServer* m_pTcpMgrSvr;
	CTcpClient* m_pTcpMgrClient;
	CTcpServer* m_pTcpToolSvr;
	CTcpClient* m_pTcpToolClient;
	CLocalServer* m_pIpcSvr;
    CLogger* m_pLogger;
    CPcie*   m_pcieTpg0;
    CPcie*   m_pcieTpg1;
private:
    stPgmDownload m_stPgmLoad;
	stPgmLoadStatus m_stPgmLoadStatus;
	stPgmLoadStatus m_stPgmUnloadStatus;
	stSlotCpu m_SlotCpu;
	stFtpInfo m_EwsFtpInfo;
	uint m_nBibId;
};

#endif //LOADER_HPP
