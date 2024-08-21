#ifndef MAINUI_H
#define MAINUI_H

#include <QApplication>
#include <QAction>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QSharedMemory>
#include <QStatusBar>
#include <QSettings>
#include <QStyleFactory>
#include <QToolBar>
#include <QToolButton>
#include <QTimer>
#include <QTabWidget>
#include <QThread>
#include <QVBoxLayout>
#include <QCheckBox>
#include <unistd.h>
#include "common.hpp"
#include "logger.hpp"
#include "logview.hpp"
#include "localsocket.hpp"
#include "tcpserver.hpp"
#include "tcpclient.hpp"
#include "sysver.hpp"
#include "slotview.hpp"
#include "slotconnview.hpp"
#include "statusbar.hpp"
#include "hsmsmanager.hpp"
#include "masstab.hpp"
#include "engrtab.hpp"
#include "errorpopup.hpp"
#include "progressdlg.hpp"
#include "setup.hpp"
#include "failrate.hpp"
#include "jumpstepdlg.hpp"
#include "opentimedelay.hpp"
#include "savedatadlg.hpp"
#include "versioninfo.hpp"
#include "chambermgr.hpp"
#include "downloader.hpp"

namespace Ui {
class CMainWindow;
}


enum MAIN_TAB
{
	RUN_TAB,
	ENGR_TAB,
};

enum EN_STATUS_LABEL
{
	STATUS_BAR_STATUS        ,
	STATUS_BAR_STEP          ,
	STATUS_BAR_SYS_NAME      ,
	STATUS_BAR_MGR_VER       ,
	MAX_STATUS_LABEL         ,
};

enum EN_TIMER_ID
{
	EN_TIMER_MGR_CON   = 0x0,
	EN_TIMER_CHMB_CON       ,
	EN_TIMER_PSMON_CON      ,
};

enum EN_TIMER_VALUE
{
	TIMER_MGR_VALUE    = 1500,
	TIMER_SLOT_VALUE   = 1500,
	TIMER_PGM_VALUE    = 1000,
	TIMER_TMS_VALUE	   = 1500,
};

enum EN_UI_STATUS
{
	UI_INIT          ,
	UI_SETUP_DONE    ,
    UI_LOAD_READY    ,
	UI_LOAD_DONE     ,
	UI_INIT_TESTING  ,
	UI_INIT_TEST_DONE,
	UI_TESTING       ,
	UI_TEST_DONE     ,
	UI_TEST_JUMP     ,
	UI_KNOWN         ,
};

enum EN_ACTION_DONE
{
	ACTION_SETUP,
	ACTION_LOAD,
	ACTION_UNLOAD,
	ACTION_INITTEST,
	ACTION_INITTEST_DONE,
	ACTION_TEST,
	ACTION_TEST_DONE,
	ACTION_STOP,
	ACTION_STOP_DONE,
	ACTION_BIBID_READ,		//2024.06.13 - KWMOON
	ACTION_MAX,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
	void SetAutoMode(bool);
private:
	void SetWindowTitle(QString title, bool bVersion=true);
	void RegisterMetaType();
	void InitDirectory();
    void InitVariable();
    void InitUi();
    void InitInstance();
	void ConnectRcmdSigSlot();
    void CreateToolbar();
    void CreateStatusBar();
    void CreateMassEngrTab();
	void CreateDockWidget();
	void InitTimer();
	void KillTimer();
	void SetToolBarStatus(EN_UI_STATUS);
	void SetTestStatus(EN_UI_STATUS);
	EN_UI_STATUS GetTestStatus();
	void SetStatusBarMessage( EN_STATUS_LABEL, QString, bool );
	uint GetAliveSlotBits();
    uint GetBiCmdSlots();
	void UpdateProgressDlg( uint, int, QString, bool, bool );
	void InitSlotTestInfo(uint);
private:
	bool InitSharedMemory();
	void SetSharedMemory(bool,bool);
	bool RemoveTempDatFile();
private:
	void ClearSlotActionDoneAck(EN_ACTION_DONE);
	void SetSlotActionReq(EN_ACTION_DONE a, uint s, uint c);
    void SetSlotActionAck(EN_ACTION_DONE a, uint s, uint c, bool err=false, bool killed=false);
    bool IsActionDone( EN_ACTION_DONE, bool& );
	bool IsActionSlot( EN_ACTION_DONE, uint );
	bool IsLiveSlotBibCon();
	QString GetActionString(EN_ACTION_DONE);
	uint GetActionSlotBits( EN_ACTION_DONE );
private:
	bool LoadSysChmbIni();
private:
	void ReqMgrVersion();
	//void ReqMgrTestDataInit();	//2024.06.17 - KWMOON(not use)
	void ReqUiChamberUse();
	void ReqMgrEngrSetup();		//2024.06.17 - KWMOON
	void ReqMgrAutoSetup();		//2024.06.17 - KWMOON
	void ReqInfoSlots();
	void ReqLiveSlots();
	void ReqLivePgms(bool);
	void ReqGetLoaderVersion();
	void ReqLoaderBibId();
	void ReqLoaderBibIdAutoMode();
	void ReqLoadPgm( QString );
	void ReqLoadPgmAutoMode( QString, QString );
	void ReqReLoadPgm();
	void ReqUnloadPgm();
	void ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND );
	void ReqDevPgmCtrl( PROTOCOL::TCP_COMMAND, uint, uint, char* p = nullptr );
	bool DoProgressDlgSlotInit( QString );
	void ShowProgressDlg();
	void DoManualSetup();
	bool DoCommonStup();
	void AckInfoSlots(stIpcElement);
	void AckLiveSlots(stIpcElement);
	void AckLivePgms(stIpcElement);
	//void AckSetupBibId(stIpcElement);			//2024.06.17 - KWMOON(not use)
	void AckPgmLoadStatus(stIpcElement);
	void AckPgmLoadDone(stIpcElement,bool);
	void AckPgmUnloadDone(stIpcElement);
	void AckPgmInitTest(stIpcElement);
	void AckPgmTest(stIpcElement);
	void AckPgmItemTest(stIpcElement);
	void AckPgmStopTest(stIpcElement);
	//void AckMgrTestDataInit(stIpcElement);	//2024.06.17 - KWMOON(not use)
	void AckPgmInitTestDone(stIpcElement);
	void AckPgmStepIn(stIpcElement);
	void AckPgmStepOut(stIpcElement);
	void AckPgmCycleIn(stIpcElement);
	void AckPgmCycleOut(stIpcElement);
	void AckPgmTestDone(stIpcElement);
	void RecvPgmFinished(stIpcElement);
	void RecvLogPrint(stIpcElement);
	void RecvPgmRuntimeError(stIpcElement);
	void RecvPgmVariables(stIpcElement);
	void AckMgrUiChamberUse(stIpcElement);
	void AckReadBibId(stIpcElement);	//2024.06.17 - KWMOON
	void AckEngrSetup(stIpcElement);	//2024.06.17 - KWMOON
	void AckAutoSetup(stIpcElement);	//2024.06.17 - KWMOON
	void AckAutoInitTestStart(uint);
	void AckAutoInitTestDone(uint);

	void AckAutoLoadDone();

	void AckChmbTempMonSet(stIpcElement);	//2024.08.14 - KWMOON
	void AckChmbTempMonEnd(stIpcElement);	//2024.08.14 - KWMOON
protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);
signals:
	void SigSlotLoaderStatusChanged( int, EN_SLOT_STAT );
	void SigSlotPgmStatusChanged( int, int, PROTOCOL::EN_PGM_SOCKET_TYPE, EN_SLOT_STAT );
	void SigSlotSetupResult( int, bool, QString, int, int, QString, QString );	//Setup Result
	void SigSlotSetupUpdateBibId( int, QString );
	void SigSlotSetupUpdatePassTotal( int, int, int );
	void SigSlotSetupUpdatePartName( int, QString );
	void SigSlotSetupUpdateLotName( int, QString );
signals:
	void SigProgressDlgTitle(QString);
	void SigProgressDlgInit(  );
	void SigProgressDlgValue( int, int);
	void SigProgressDlgText ( int, QString, bool error);
	void SigProgressDlgHide();
signals:
	void SigMgrTmsStatusChanged( LED_DESC , bool );
signals:
	void SigIpcMgrConnect();
	void SigIpcTmsConnect();
	void SigIpcChmbConnect();
	void SigIpcPsMonConnect();
signals:
	void SigPgmLoad();
	void SigPgmLoadDone();
	void SigSysLogMessage ( LOG_TYPE, QString );
	void SigChmbLogMessage( LOG_TYPE, QString );
	void SigPgmLogMessage( uint,uint, LOG_TYPE, QString );
	void SigIpcMgrSendPacket( char*, uint size );
	void SigIpcTmsSendPacket( char*, uint size );
	void SigStartInitTest();
	void SigInitTestDone(uint);
	void SigTestItemListup(uint);
	void SigAutoInitTestStart();
	void SigAutoInitTestDone();
	void SigAutoStartTest();
	void SigAutoStartTestDone();
	void sigInformSlotLiveToHsmsMgr(stSlotLive);
	//void SigViewPgmVariables(uint);
	void SigChmbTempMonSet( stGetTemp );
	void SigChmbTempMonEnd( stGetTemp );
	void SigEngrInitTestDone();	//2024.08.19 - KWMOON
signals:
	void SigLoadExistData();	//2024.06.20 - KWMOON
	void SigSetupView();
	void SigTestStartFlag();
	void SigTestPgmVariableFlag(uint);
	void SigTestEndFlag();
	void SigStepIn(uint);
	void SigStepOut(uint);
signals:
	void SigToSlotViewUpdateBibId(uint);
	void SigToSlotViewUpdateSetup();
//	void SigToSlotViewUpdatePartNo();
//	void SigToSlotViewUpdateMlbCode();
//	void SigToSlotViewUpdateLotNo();
//	void SigToSlotViewUpdatePassAndTotal();
	void SigToSlotViewSetup();
	void SigInitSlotData();
signals:
    void SigAppendErrorPopup( QString );    //2023.03.22 - kwmoon
	void SigStartChmbPlot();
	void SigStopChmbPlot();
	void SigResetChmbPlot();
signals:
	void SigMakeReport();
signals:
	void SigChmbMrStart(bool);
	void SigSyncSlotTimeResult(unsigned short result, CSecsMsg* secs_msg);
	void SigDownloadPgm();
	void SigDownloadPgmResult(bool, QString);
//	void sigBoardMaintInfo(int);		//2024.07.02 - yseom for test
//	void sigTGET(QString);				//2024.07.02 - yseom for test
    void SigSetRackIgnore(bool rack_ignore);
    void SigReadyToLoad();
    void SigLoadComplete();
    void sigChmbCtrolToChmbMgr(PROTOCOL::PLC_COMMAND cmd, int data);        //2024.07.19 - yseom
	void sigQueueMonitoring();
	void SigMakeReportDone(bool);
	void sigTestAmb();
public slots:
	void SlotActManualSetup();
	void SlotActLoadPgm();
	void SlotActReloadPgm();
	void SlotActUnloadPgm();
	void SlotActInitTest();
	void SlotActTestStart();
	void SlotActTestStop();
	void SlotActJump();
	void SlotActFailRate();		//2023.06.14 - kwmoon
	void SlotActOTDelay();
	void SlotActAlarm();
	void SlotActJudge();
	void SlotActMaintCall();
	void SlotActSaveData();
	void SlotActLogView();
	void SlotActChamberCheck();		//2023.09.13 - kwmoon
//	void SlotActTemp();				//2024.07.01 - yseom
    void SlotActReadyToLoad();
    void SlotActLoadComplete();
    void SlotActSetRackIgnore(bool);
	void SlotDownloadProgress(int);					//2024.06.11 - yseom
	void SlotDownloadFtpError(QString, QString);	//2024.06.13 - yseom
	void SlotDownloadFtpResult(bool, QString, QString);		//2024.06.26 - yseom
	void SlotPgmDownloadAutoMode(QString);
	void SlotRecvEapConnectStatus(bool);
	void SlotRunManualTabChanged(int);
    void slotTempTest();
    void slotTempInitTest();
	void SlotActLotSet();		//2024.08.14 - KWMOON
public slots:
	void SlotAutoBibIdReq();
	void SlotAutoSetupReq();
	void SlotSetupDlgResult( bool, stSetupInfo );
	void SlotSetStatusBarText(EN_STATUS_LABEL,QString);
	void SlotFromSlotViewCurrentSlot(uint);
public slots:
	void SlotIpcMgrIpcLogMessage (LOG_TYPE, QString);
	void SlotIpcMgrIpcError ( QLocalSocket::LocalSocketError, QString );
	void SlotIpcMgrConnectionStatus(bool);
	void SlotIpcMgrRecvPacket( stIpcElement );
	void SlotIpcTmsIpcLogMessage (LOG_TYPE, QString);
	void SlotIpcTmsIpcError ( QLocalSocket::LocalSocketError, QString );
	void SlotIpcTmsConnectionStatus(bool);
	void SlotTimeoutMgrCon();
	void SlotTimeoutSlotCon();
	void SlotTimeoutChmbCon();
	void SlotTimeoutPsMonCon();
	void SlotTimeoutPgmCon();
	void SlotTimeoutTmsCon();
	void SlotJumpTest( stTestItem );
	void SlotJumpChamberUse(bool);
public slots:
	void SlotMakeReportDone(bool);
public slots:
	void SlotSyncSlotTime(CSecsMsg* secs_msg);
signals:
	void SigRcmdReply(bool);
	void SigToHsmsMgrBibIdReadDone();
	void SigToHsmsMgrAutoManualSetupDone(bool,int);
public slots:
	void SlotFromHsmsMgrAutoDiag();
	void SlotFromHsmsMgrBiCloseLot();
	void SlotFromHsmsMgrBiInitialEnd();
	void SlotFromHsmsMgrBiInitialStart();
	void SlotFromHsmsMgrBiOpenLot();
	void SlotFromHsmsMgrBiRecloseLot();
	void SlotFromHsmsMgrBiTestTime();
	void SlotFromHsmsMgrBoardMaintInfo(int slot_num);
	void SlotFromHsmsMgrBoardStatusInfo();
	void SlotFromHsmsMgrBoardType(ushort valid_slot);
	void SlotFromHsmsMgrCheckInBoard();
	void SlotFromHsmsMgrCheckOutBoard();
	void SlotFromHsmsMgrEqEffInfo();
	void SlotFromHsmsMgrHoldCheck();
	void SlotFromHsmsMgrInterLockLimit();
	void SlotFromHsmsMgrReadyToLoad();
	void SlotFromHsmsMgrLotCall();
	void SlotFromHsmsMgrLotSet();
	void SlotFromHsmsMgrLotSetInfo();
	void SlotFromHsmsMgrManualMaskMap();
	void SlotFromHsmsMgrPmCheck();
	void SlotFromHsmsMgrRackSetInfo();
	void SlotFromHsmsMgrRecheckInBoard();
	void SlotFromHsmsMgrAutoBypass();
	void SlotFromHsmsMgrReplyAutoBypassEqpState();
	void SlotFromHsmsMgrRequestBoardMaskSet();
	void SlotFromHsmsMgrRequestRackSet();
	void SlotFromHsmsMgrRMVOU();
	void SlotFromHsmsMgrSlotMaskInfo1();
	void SlotFromHsmsMgrSlotMaskInfo2();
	void SlotFromHsmsMgrStartBiCmd();
	void SlotFromHsmsMgrTGET(QString lot_info);
	void SlotFromHsmsMgrTMVIN();
	void SlotFromHsmsMgrTMVOU();
	void SlotFromHsmsMgrVMVOU();
	void SlotFromHsmsMgrReadyToUnload();
	void SlotFromHsmsMgrError();
	void SlotFromHsmsMgrBibIdRead(ushort slot_num);
//signals:
//    void sigSetChamberDoorClose();
private:
	Ui::CMainWindow *ui;
    QString m_sApplicationName;
	EN_UI_STATUS m_UiStatus;
	EN_UI_STATUS m_TestStatus;
	CFileMap* m_pTestDataMgr;
	stTestItem m_stJumpItems;		//2023.08.31 - KWMOON
private:
	QMenu *m_pLoadMenu;
	QToolButton* m_pLoadBtn;
    QAction *m_pActSetup;
    QAction *m_pActLoad;
    QAction *m_pActReload;
	QAction *m_pActUnload;
	QAction *m_pActInitTest;
    QAction *m_pActTest;
    QAction *m_pActStop;
	QAction *m_pActJump;
	QAction *m_pActFailRate;
	QAction *m_pActOtDelay;
	QAction *m_pActJudge;
    QAction *m_pActMCall;
    QAction *m_pActAlarm;
	QAction *m_pActSaveData;
	QAction *m_pActLogView;		//2023.08.14 - kwmoon
	QAction *m_pActInitChmb;	//2023.09.13 - kwmoon for test
	QAction *m_pActTemp;		//2024.07.01 - yseom for test
    QAction *m_pActReadyToLoad;
    QAction *m_pActLoadComplete;
	QAction *m_pActLotSet;		//2023.08.14 - kwmoon
private:
	CStatusBar *m_pStatusBar;
	QLabel* m_pStatusBarLabel[MAX_STATUS_LABEL];
    CMassTab* m_pMassView;
    CEngrTab* m_pEngrView;
	CSetup*   m_pSetupDlg;
	CProgressDlg* m_pPrgDlg;
	CFailRate* m_pFailRate;
	CJumpStepDlg* m_pJumpDlg;
	COpenTimeDelay* m_pOtDelay;
	CErrorPopup* m_pErrPopup;		//2023.03.22 - kwmoon
	CVersionInfo* m_pVerInfo;		//2023.06.07 - kwmoon
	CSaveDataDlg* m_pSaveDataDlg;	//2023.07.23 - KWMOON
	//CChmbDlg* m_pChmbDlg;			//2023.09.13 - KWMOON --> //2024.05.31 - comment
	CHsmsManager* m_pHsmsManager;
	CChamberMgr* m_pChmbMgr;		//2024.05.31 - KWMOON
	QThread* m_pChmbMgrTh;			//2024.06.03 - KWMOON
	CDownloader* m_pPgmDownloader;	//2024.06.11 - yseom
	QThread* m_pPgmDownloaderTh;	//2024.06.11 - yseom
	QThread* m_pHsmsMgrTh;			//2024.06.03 - KWMOON
	CLogView* m_pLogView;			//2024.06.14 - KWMOON
	CSlotConnView* m_pSlotConnView;	//2024.06.14 - KWMOON
	CSlotView* m_pSlotView;
private:
	CLocalSocket* m_pMgrIpc;
	CLocalSocket* m_pTmsIpc;		//2024.05.02 - hsbae
	QSharedMemory* m_pShmChmb;
	QTimer* m_timerSlotCon;
	QTimer* m_timerMgrCon;
	QTimer* m_timerChmbCon;
	QTimer* m_timerPsMonCon;
	QTimer* m_timerPgmCon;
	QTimer* m_timerTmsCon;
	QVector<CTcpClient*> m_vecTcps;
private:
	bool m_bAutoMode;
	bool m_bInitialized;
	bool m_bMgrConn;
	bool m_bTmsConn;
	bool m_bEapConn;
	bool m_bQuitAtTestMgr;
	QString m_sLoadFile;
	QString m_sOsDir;
	QString m_sCurrentDir;
	QString m_sDataRoot;
	QString m_sDataDir;
	QString m_sDownloadDir;
	QString m_sTestDataDir;
	QString m_sProgramDir;
	QString m_sCurrentLoadDir;
	QString m_sTempDatDir;
	QString m_sPfmDatDir;
	QString m_sCfgDir;
	stSlotInfo m_stSlotInfo;
	stSlotLive m_stSlotLive;
	stPgmLive  m_stPgmLive;
	uint m_nCurrentSlot;
	QString m_sCurrentLotId;			//2024.07.02 - yseom
	uint m_uCorrespondingSlotBits;		//2024.07.02 - yseom
	uint m_uValidSlotBit;				//2024.07.24 - yseom
	bool m_bTestStart[MAX_SLOT][MAX_SYS_CPU];
	bool m_bTestDone[MAX_SLOT][MAX_SYS_CPU];
private:
	stSetupInfo m_stSetupInfo;
	stSlotTestInfo m_stSlotTestInfo[MAX_SLOT];
	stUiSlotAck m_stSlotActionDone[ACTION_MAX][MAX_SLOT];
	stUiInitInfo m_uiInitInfo;
	stUiTestInfo m_uiTestInfo;
	bool m_bInterruptMode;		//2024.08.16 - KMOON
};

#endif // MAINUI_H
