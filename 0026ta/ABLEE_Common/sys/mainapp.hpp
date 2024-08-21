#ifndef CMAINAPP_HPP
#define CMAINAPP_HPP

#include "deviceapp.hpp"

class CMainApp : public QObject
{
    Q_OBJECT
public:
    CMainApp();
    ~CMainApp();
	void SetDeviceIf( const char*, CApi* );
	void SetTerminate();		//2023.04.03 - KWMOON
private:
    void RegisterMetaType();
    void RunManualMode();
    void RunAutoMode();
    void ShowHelp();
	bool LoadFtpIni();
	static void SignalHandler( int signum );
private:
	void SendPgmLoadStatus(std::string, bool);
	void SendSlotRunnerNo();
	void SendInitTestAck();
	void SendTestAck();
	void SendJumpTestAck();
	void SendStopAck();
	void SendKeepAlive();	//2023.08.10 - KWMOON
private:
    void CreateStartSocket();
    void CloseSocket();
    void PrintLog(LOG_TYPE, QString);
    void SaveLog(LOG_TYPE, QString);
signals:
	void SigOpenDevice(int,int);
	void SigPcieValidCheck(int,int);
	void SigConnectToHost(int,int,bool);
	void SigInitProgram();
	void SigManualRun();
	void SigInitTest( stUiInitInfo );
	void SigTest(stUiTestInfo);
	void SigJumpTest(stTestItem);
	void SigStopTest();
public:
	void SlotOpenDeviceResult(int,int,bool);
	void SlotPcieValidCheckResult(int,int,int,bool);
signals:
	void SigConnectToHostMgr(int);
	void SigSendPacket(int,char*,uint);
public slots:
	void SlotTcpStatusChanged(int,int,bool);
	void SlotTcpReceivePacket( int, int, stTcpElement);
	void SlotTcpLogMessage( int, int, LOG_TYPE, QString );

public slots:
	void SlotTimerConnectToHost();
	void SlotTimerKeepAlive();	//2023.08.10 - KWMOON
public:
	int  parseArguments( int argc, char* argv[] );

private:
    CDeviceInstance* m_pDevInst;
    QThread* m_pDevTh;
	CTcpSocket* m_pCtrlSock;
    QTimer m_tmrConn;
    QString m_sEwsIp;
	QTimer m_tmrKeepAlive;	//2023.08.10 - KWMOON
private:
	bool m_bUiAutoMode;
    bool m_bAutoRun;
	bool m_bToolPgm;
	bool m_bHwOpen;
	bool m_bValidVer;
	bool m_bEwsConn;
	int  m_nSlot;
	int  m_nRunner;
};

#endif // DEVICEHANDLER_HPP
