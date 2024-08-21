#ifndef CTESTMGR_HPP
#define CTESTMGR_HPP

#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <QCoreApplication>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QFileInfo>

#include "commstruct.hpp"
#include "logger.hpp"
#include "devctrlmgr.hpp"
#include "devdatamgr.hpp"
#include "devlogmgr.hpp"
#include "chmbmgr.hpp"
#include "loadermgr.hpp"
#include "localclient.hpp"
#include "localserver.hpp"
#include "systype.hpp"
#include "filemaplib.hpp"

class CTestMgr : public QObject
{
	Q_OBJECT
public:
    CTestMgr();
    ~CTestMgr();

public:
	int run(int argc, char* argv[] );
	void Terminate(int code);
	void DeleteInstance();
public slots:
	void SlotIpcConnected(quintptr);
	void SlotIpcStatusChanged(bool);
	void SlotIpcRecvPacket(stIpcElement);
	void SlotIpcLogMessage(LOG_TYPE, QString);
public slots:
	void SlotDevCtrlLogMessage( LOG_TYPE, QString );
	void SlotDevCtrlStatusChanged( uint, uint, bool );
	void SlotDevCtrlRecvPacket( stTcpElement );
public slots:
	void SlotDevDataLogMessage( LOG_TYPE, QString );
	void SlotDevDataStatusChanged( uint, uint, bool );
	void SlotDevDataRecvPacket( stTcpElement );
public slots:
	void SlotDevLogLogMessage( LOG_TYPE, QString );
	void SlotDevLogStatusChanged( uint, uint, bool );
	void SlotDevLogRecvPacket( stTcpElement );
public slots:
	void SlotAckLoaderConnSlots( stSlotLive );
	void SlotLoaderRecvPacket( int,int, stTcpElement );
	void SlotLoaderLogMessage(LOG_TYPE, QString);
public slots:
	void SlotChmbMgrLogMessage( LOG_TYPE, QString );
	void SlotRecvChmbMgrAck( stTcpElement );
	void SlotRecvChmbConnStatus( bool );
	void SlotChmbTempMonSet( stGetTemp );
	void SlotChmbTempMonEnd( stGetTemp );
signals:
	void SigMakeSockLoaderMgr();
	void SigRunLoaderMgr();
	void SigStopLoaderMgr();
	void SigRunDevCtrlMgr();
	void SigRunDevDataMgr();
	void SigRunDevLogMgr();				//2023.08.13 - kwmoon
	void SigRunChmbMgr();
	void SigChamberUse(bool);
	void SigLiveSlotInfo(stSlotLive);	//2023.05.15 - kwmoon
	void SigLivePgmList(stPgmLive);		//2023.05.23 - kwmoon
signals:
	void SigIpcSendPacket( char*, uint);
	void SigSendPacketToLoader( stTcpElement );
	void SigSaveLog( LOG_TYPE t, QString s );
	void SigSendPacketToDevCtrlMgr( stTcpElement );
	void SigSendPacketToDevDataMgr( stTcpElement );
signals:
    void SigSendChamberCmd(char*,uint); //chmbmgr
    void SigSendTestCmd(char*,uint);    //chmbmbr
private:
	void RegisterMetaType();
	void StandByDirectory();
	bool LoadIni();
	void CreateInstance();
	bool RunLocalServer();		//Main Ui
	bool RunTcpLoaderManager();	//Slot
	bool RunPgmCtrlManager();	//Test Pgm
	bool RunPgmDataManager();	//Test Pgm
	bool RunPgmLogManager();	//Test Pgm - 2023.08.13
	bool RunChamberManager();	//2023.05.13 - kwmoon(pgm chmb ctrl)
private:
	bool MakeDir(QString);
	bool MakeTxtFile(QString f,QString s="");
	bool MakeBinFile(QString f, uint, char* p = nullptr );
private:
	void ReqToLoader(stIpcElement&);
	void SendToDevCtrlMgr(stIpcElement&);
	void SendToDevDataMgr(stIpcElement&);
private:
	bool AckSlotInfo(stIpcElement&);
	bool AckSlotLive(stIpcElement&);
	bool AckDevPgmLive(stIpcElement&);
	bool AckVersion (stIpcElement&);
	bool DoEngrSetup(stIpcElement&);		//2023.06.17 - kwmoon
	bool DoAutoSetup(stIpcElement&);		//2023.06.17 - kwmoon
	//bool DoTestDataInit(stIpcElement&);	//2023.06.17 - kwmoon(not use)
	bool DoUiChamberUse(stIpcElement&);
	bool DoInitTestDataInit(uint bits);
	bool DoStepTestDataInit(uint bits);		//2023.04.25 - kwmoon
	//bool AckTestDataInit (bool bInit);	//2023.06.17 - kwmoon(not use)
	bool AckUiChamberUse (bool bError, bool bConn);
	bool AckEngrSetup(bool);
	bool AckAutoSetup(bool);
private:
    void DeletePfmFiles();
    void createInitialPfmFiles();

private:
	CLogger* m_pLogger;
	CLocalServer* m_pIpcSvr;
	CLoaderMgr* m_pLoaderMgr;
	CDevCtrlMgr* m_pDevCtrlMgr;
	CDevDataMgr* m_pDevDataMgr;
	CDevLogMgr* m_pDevLogMgr;		//2023.08.13 - KWMOON
	QThread* m_pDevCtrlMgrTh;
	QThread* m_pLoaderMgrTh;
	QThread* m_pDevDataMgrTh;
	QThread* m_pDevLogMgrTh;		//2023.08.13 - KWMOON
	CChmbMgr* m_pChmbMgr;			//2023.05.13 - kwmoon
	QThread* m_pChmbMgrTh;			//2023.05.13 - kwmoon
	CFileMap* m_pTestDataMgr;
private:
	bool m_bDebugPrint;
	bool m_bUiCon;
	bool m_bIniValid;
	QString m_sCurrentDir;
	QString m_sDataDir;
	QString m_sDataModeDir;
	QVector<CLocalClient*> m_vecIpcs;
	stSlotInfo m_stSlotInfo;
	stSlotLive m_stSlotLive;
	stPgmLive  m_stPgmLive;
	stSetupInfo m_stSetupInfo;
	stSlotTestInfo m_stTestInfo;	//2023.08.13 - KWMOON
	uint m_uTestSlots;
};

#endif // TESTMGR_HPP
