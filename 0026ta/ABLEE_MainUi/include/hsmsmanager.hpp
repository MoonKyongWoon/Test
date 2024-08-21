#ifndef CHSMSMANAGER_HPP
#define CHSMSMANAGER_HPP

#include <QObject>
#include <QDebug>
#include <QMap>
#include <QByteArray>
#include <QFile>
#include <QDataStream>
#include <QSharedMemory>
#include <QTimer>
#include "commstruct.hpp"
#include "secsgem.hpp"
#include "util.hpp"
#include <sys/types.h>
#include <time.h>
#include <filemaplib.hpp>
#include <QQueue>
#include <QThread>
#include "hsmsmessagequeue.hpp"

#define SSH_ID						"ablee"
#define SSH_PASSWD					"ablee"

#define READY_TO_LOAD_TIME			( 5 * 60 * 1000 )		// 5 * 60 sec
#define RACK_IN_OUT_CHK_TIME		( 3 * 1000 )			// 4 sec
#define DOOR_CLOSE_OPEN_CHK_TIME	( 4 * 1000 )			// 5 sec
#define DOOR_RACK_LIMIT_SEC			( 100 )					//100 sec
#define CHECK_TIME_5MS				( 5 * 1000 )
#define CHECK_TIME_10MS				( 10 * 1000 )

enum _ZONE_NO
{
	_ZONE1,
	_ZONE2,
	_ZONE_MAX,
};

enum _TIMER_ID
{
	_READY_TO_LOAD,
	_LOAD_CMPLETE,
	_RACK_IN,
	_RACK_OUT,
	_DOOR_CLOSE,
	_DOOR_OPEN,
	_AMB,
	_AMB_SOAK,
	_DRY,
	_DRY_SOAK,
	_COUNT,
};

enum _UNLOAD_TIMER
{
	_PRE_UNLOAD,
	_UNLOAD_DOOR_OPEN,
	_UNLOAD_RACK_OUT,
	_READY_TO_UNLOAD,
	_UNLOAD_COUNT,
};

enum _PRE_UNLOAD_CHECK
{
	_AMB_DONE,
	_REPORT_DONE,
	_CHECK_COUNT,
};

enum class _SETUP_PROCESS {
	_INITIAL_STATUS = 0b00000000,
	_INIT_INFO		= 0b10000000,
	_ONLINE_MODE	= 0b01000000,
	_SYNC_TIME		= 0b00100000,
	_SET_CEID		= 0b00010000,
	_SET_RPTID		= 0b00001000,
	_SET_VID		= 0b00000100,
	_CHMB_INFO		= 0b00000010,
	_STATUS_INFO	= 0b00000001,
	_FINISH			= 0b11111111,
};

enum class _STATUS_ID {
	_IDLE_IN   	 = 0,
	_IDLE_OUT		,
	_RUN			,
	_DOWN			,
	_INIT			,
};

enum class _EVENT_ID {
	_IDLE_IN	 = 0,
	_LOT_IN_INFO	,
	_LOT_IN			,
	_LOTIN			,
	_READY			,
	_SETUP			,
	_SETUP_END		,
	_PRETEST		,
	_PRETEST_END	,
	_START			,
	_MCPRN			,
	_MCEND			,
	_IDLE_OUT		,
	_LOT_OUT_INFO	,
	_LOT_OUT		,
	_JAM			,
	_BUSMS			,
	_BUSME			,
	_INIT			,
};

enum class _CHMB_MODE {
	_SET		 = 0,
	_END			,
};

//2024.08.19 - KWMOON
enum class _TOWERLAMP_COLOR
{
	_RED ,
	_YELLOW,
	_GREEN,
};

class CTestInfo : public QObject
{
	Q_OBJECT
private:
	uint				valid_slot_count;
	ushort				valid_slot;
	QString				board_id[MAX_SLOT];
	QVector<QString>	lot_list;
	QVector<QString>	pgm_list;
	QString				slot_pgm[MAX_SLOT];
	QString				mpg_code;
	CSecsMsg			*reply_msg;
	int					current_slot;
	CFileMap			*file_map;
	int					init_retest_count;
	double				init_yield;
	uint				setup_count;
	uint				init_count;
	stSetupTimePair		setup_time;

public:
	int					recursive;
public:
	explicit CTestInfo(CFileMap* file_map, QObject *parent = nullptr);
//	CTestInfo(uint valid_slot_count, short valid_slot, QString board_id[16], QVector<QString> lot_list);
	~CTestInfo();
	void		setReplyMsg(CSecsMsg* secs_msg);
	void		setValidSlotCount(uint valid_slot_count);
	void		setValidSlot(int index, bool is_valid);
    void        setValidSlot(ushort valid_slot);
	void		setBoardId(int index, QString lot_no);
	void		setLotList(QVector<QString> lot_list);
	void		setLotList(QStringList lot_list);
	void		setPgmList(QVector<QString> pgm_list);
	void		setPgmList(QStringList pgm_list);
	void		setSlotPgm(uint slot, QString pgm);
	void		setMpgCode(QString mpg_code);
	void		setCurrentSlot(int num);
	void		setInitRetestCount(int count);
	void		setInitYield(double yield);
	void		setSetupStartTime();
	void		setSetupEndTime();

	CSecsMsg*	getReplyMsg();
	uint		getValidSlotCount();
	ushort		getValidSlot();
	QString		getBoardId(int index);
	int			getLotListCount();
	QString		getLot(int index);
	int			getPgmListCount();
	QString		getPgm(int index);
	int			getCurrentSlot();
	int			getInitRetestCount();
	double		getInitYield();
	uint		getSetupCount();
	uint		getInitCount();

	void		initCurrentSlot();
	void		initSlotPgm();
	void		initInitRetestCount();
	void		initInitYield();
	void		initSetupCount();
	void		initInitCount();
	void		clearTestInfoData();

	void		clearLotList();
	void		clearPgmList();
	void		addLot(QString lot);
	void		addPgm(QString pgm);
	void		increaseSetupCount();
	void		increaseInitCount();
	void		decreaseInitRetestCount();
	bool		compareSlotPgm(uint slot, QString pgm);	
	bool		compareMpgCode(QString mpg_code);
	bool		isCorrectPgm(QString lot, QString pgm);
	void		saveSetupStartTime();
	void		saveSetupEndTime();
	void		readSetupTime();
};

class CHsmsManager : public QObject
{
	Q_OBJECT

private:
	QTimer						*chmb_check_timer;
	QString						m_data_dir_path;
	uint						system_byte;
	unsigned char				device_id[2];
	CSecsGem					*secs_gem;
	QSharedMemory				*m_pShmHsms;
	bool                        m_bOnline_mode;
	bool						m_bConnect_tms_eap;
	stChamberStatus				m_stChamberStatus;
	int							request_slot_num;
	_STATUS_ID					status_id;
	_EVENT_ID					event_id;
	uint						sent_ceid;
	uint						next_ceid;
	_CHMB_MODE					chmb_mode;
	char						auto_setup_status;
	CTestInfo					*test_info;
	CFileMap					*file_map;
	ushort						slot_with_bib;
	bool						b_rack_ignore;
	CHsmsMessageQueue			*message_queue;
	QThread						*message_queue_thread;
	bool						m_bLoadCompleted[_ZONE_NO::_ZONE_MAX];	//2024.07.23 - KWMOON
	bool						m_bUiAutoMode;							//2024.07.23 - KWMOON
	QTimer						*m_pTimer[_TIMER_ID::_COUNT];			//2024.07.23 - KWMOON
	QTimer						*m_pUnloadTimer[_UNLOAD_TIMER::_UNLOAD_COUNT];
	QDateTime					m_dtStart[_TIMER_ID::_COUNT];			//2024.07.23 - KWMOON
	stSlotLive					m_stSlotLive;
	QQueue<int>					repaet_slot_list;
	bool						unload_check[_PRE_UNLOAD_CHECK::_CHECK_COUNT];
	bool						m_bInterruptMode;
public:
	explicit CHsmsManager(QObject *parent = nullptr);
	~CHsmsManager();

public:
	void			SET_UIAUTOMODE(bool);	//2024.07.23 - KWMOON

//temp
public:
    void            SetStatusId(_STATUS_ID);
    void            SetEventId(_EVENT_ID);
    void            SetVidValue(QString vid, QString value);
	void            GetVidValue(QString vid, QString& rvalue);
	void			SetInterruptMode(bool);
private:
	void			initVariables();
	void			initTimer();
	void			initPath();
	void			initVidList();
	void			initSharedMemory();
	void			increaseDataIdInSharedMemory();

	void			deleteTimer();

	_RCMD			handleRcmd(CSecsMsg* secs_msg);
	_RCMD			convertToStructFromRcmd(CSecsMsg *secs_msg);
	_RCMD			saveRcmdIniData(CSecsMsg *secs_msg);
	bool			writeBinaryFile(const QString& file, const QByteArray* data);
	bool			readBinaryFile(const QString& file, QByteArray* data);
	bool			setTime(QString date);
	_RCMD			getRcmdFromString(atstring sRcmd);
	_RCMD			getRcmdPairedWithCeid(uint ceid);

	void			setStatusId(_STATUS_ID status_id);
	void			setEventId(_EVENT_ID event_id, bool b_force = false);
	void			setSlotWithBIBIni();
	void			setChmbMode(_CHMB_MODE chmb_mode);
	void			setAutoSetupStatus(char status, bool set);
    bool			setVidValue(QString vid, QString value);
	bool			setVidValue(QString vid, uint value);
	bool			getVidValue(QString vid, QString& rvalue);
	bool			checkBibType(int slot_bits);
//	bool			addMessageQueue(CSecsMsg* secs_msg, bool non_paired_message = false, bool only_stream_2 = false);
//	bool			setMessageQueue(CSecsMsg* secs_msg, int msg_transaction_index);

	QString			convertFromStatusIdToQString(_STATUS_ID status_id);
	QString			convertFromEventIdToQString(_EVENT_ID event_id);
    QString         makeCommentFromEventId(_EVENT_ID event_id);
	uint			getPairedCEIDfromRCMD(_RCMD rcmd);

	bool            setChamberRackIn();		 //2024.07.22 - KWMOON
	bool            setChamberRackOut();	 //2024.07.22 - KWMOON
	bool            setChamberDoorClose();	 //2024.07.22 - KWMOON
	bool            setChamberDoorOpen();	 //2024.07.22 - KWMOON
	bool            setChamberAmbient();	 //2024.07.22 - KWMOON	
	void			setAmbAfterTest();

	bool			setChamberTowerLampRemote();	//2024.08.19 - KWMOON
	bool			setChamberTowerLampLocal();		//2024.08.19 - KWMOON
	bool			setChamberTowerLampOn(_TOWERLAMP_COLOR);		//2024.08.19 - KWMOON
	bool			setChamberTowerLampBlink(_TOWERLAMP_COLOR);		//2024.08.19 - KWMOON
	bool			setChamberTowerLampOff();		//2024.08.19 - KWMOON

	bool			getInitTestResult(int slot_no);

	void			runAtSaveMap(QString option, uint count = 0);

public slots:
    void            slotChamberDoorClose();
	void			slotSetAmbAfterTest();

private:
	void			S1F3(CSecsMsg* secs_msg);
	void			S1F13(CSecsMsg* secs_msg);
	void			S1F17(CSecsMsg* secs_msg);
	void			S2F31(CSecsMsg* secs_msg);
	void			S2F33(CSecsMsg* secs_msg);
	void			S2F35(CSecsMsg* secs_msg);
	void			S2F37(CSecsMsg* secs_msg);
	void			S2F41(CSecsMsg* secs_msg);
	void			S6F12(CSecsMsg* secs_msg);
	void			S2F42(bool ack);

private:
	void			sendNextCeidMessage();
	bool			sendCEID5300byLot();
	bool			sendCEID4500byLot();
	bool			sendCEID4600byLot();
	bool			sendCEID6600byLot();
	bool			sendCEID5400byLot();
	bool			sendCEID8300byLot();
	bool			sendCEID5500byLot();
	bool			sendCEID4400byLot();
	bool			sendCEID3400byBib();
	bool			sendCEID3410byBib();
	bool			sendCEID4200bySlot();

	bool			makeCEIDMessage(uint ceid);
	bool			CEID5110();
	bool			CEID7000();
	bool			CEID8001();
	bool			CEID7007();
	bool			CEID7010();
	bool			CEID5120();
	bool			CEID7100();
	bool			CEID3300();
	bool			CEID3400();
	bool			CEID7340();
	bool			CEID4400();
	bool			CEID3410();
	bool			CEID7300();
	bool			CEID6100();
	bool			CEID6200();
	bool			CEID5300();
	bool			CEID4500();
    //TODO
    //apply recursive for slot
	bool			CEID3500();
	bool			CEID3600();
	bool			CEID4600();
	bool			CEID6600();
	bool			CEID5400();
	bool			CEID8300();
	bool			CEID5500();
	bool			CEID7320();
	bool			CEID7360();
	bool			CEID7400();
	bool			CEID4200();
	bool			CEID5150();
	bool			CEID5130();
	bool			CEID5140();
	bool			CEID8100();
private:
	void            timeoutReadyToLoad();
	void            timeoutRackIn();
	void            timeoutRackOut();
	void            timeoutDoorClose();
	void            timeoutDoorOpen();
	void			timeoutAmb();
	void			timeoutAmbSoak();
	void			timeoutDry();
	void			timeoutDrySoak();
	void			timeoutPreUnload();
	void			timeoutPreUnloadDoorOpen();
	void			timeoutPreUnloadRackOut();
	void			timeoutReadyToUnload();

public:
	void			handleHsmsMessage(stHsmsElement *element);

public slots:
	void			slotRecvPacketFromTms(stIpcElement element);
	void			slotSyncSlotTimeResult(unsigned short result, CSecsMsg* secs_msg);
	void			slotFromChmbMgrResponse(PROTOCOL::PLC_COMMAND, bool);					//2024.06.10 - KWMOON
	void			slotFromChmbMgrStatus(stChamberStatus);									//2024.06.10 - KWMOON
	void			slotRecvSlotLiveStatus(stSlotLive);
	void			slotDownloadPgmResult(bool, QString);									//2024.06.12 - yseom
	void			slotRcmdReply(bool);
	void			slotAutoInitTestStart();
	void			slotAutoInitTestDone();
	void            slotAutoTestStart();
	void            slotAutoTestStartDone();
    void            slotSetRackIgnore(bool rack_ignore);
    void            slotReadyToLoad();
    void            slotLoadComplete();
	void            slotTimerTimeout();		//2024.07.22 - KWMOON
	void            slotBibIdReadDone();	//2024.07.23 - KWMOON
	void			slotSendMessage(CSecsMsg* secs_msg);
	void			slotQueueMonitoring();
	void			slotSendNextCeidMessage();
	void			slotAutoManualSetupDone(bool done, int slotbits);
	void			slotMakeReportDone(bool done);
	void			slotFinishAtSaveMap(int exit, QProcess::ExitStatus stat, QString cmd);
	void            slotChamberTempMonSet( stGetTemp );
	void            slotChamberTempMonEnd( stGetTemp );
signals:
	void			sigEapConnectStatus(bool bConnect);
	void			sigSendToTmsUi(char* data, uint size);
	void			sigSendRcmdStructure(_RCMD rcmd);
	void			sigSendRcmd(_RCMD rcmd);
	void			sigSyncSlotTime(CSecsMsg* secs_msg);
	void			sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND, int );
	void            sigBoardMaintInfo(int);							//2024.07.23 - KWMOON
	void			sigPgmDownloadInAutoMode(QString lot_info);

	void			sigAddNewMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid = 0);
	void			sigAddMessageBlock(CSecsMsg* secs_msg, bool is_send, uint ceid = 0);

	void			sigAppenErrorPopup(QString msg);

	void			sigStartInitTest();
    void            sigStartTest();
	void			sigAutoDiag();
	void			sigBiCloseLot();
	void			sigBiInitialEnd();
	void			sigBiInitialStart();
	void			sigBiOpenLot();
	void			sigBiRecloseLot();
	void			sigBiTestTime();
//	void			sigBoardMaintInfo(int slot_num);
//	void			sigBoardMaintInfo();
	void			sigBoardStatusInfo();
	void			sigBoardType(ushort valid_slot);
	void			sigCheckInBoard();
	void			sigCheckOutBoard();
	void			sigEqEffInfo();
	void			sigHoldCheck();
	void			sigInterLockLimit();
	void			sigReadyToLoad();
	void			sigLotCall();
	void			sigLotSet();
	void			sigLotSetInfo();
	void			sigManualMaskMap();
	void			sigPmCheck();
	void			sigRackSetInfo();
	void			sigRecheckInBoard();
	void			sigAutoBypass();
	void			sigReplyAutoBypassEqpState();
	void			sigRequestBoardMaskSet();
	void			sigRequestRackSet();
	void			sigRMVOU();
	void			sigSlotMaskInfo1();
	void			sigSlotMaskInfo2();
	void			sigStartBiCmd();
	void			sigTGET(QString lot_info);
	void			sigTMVIN();
	void			sigTMVOU();
	void			sigVMVOU();
	void			sigReadyToUnload();
	void			sigError();
	void			sigSysLogMessage ( LOG_TYPE, QString );		//2024.07.23 - KWMOON
	void            sigBibIdRead(uint);							//2024.07.23 - KWMOON
	void			sigQueueMonitoring();
	void			sigSetWorking(bool);
	void			sigUpdateAutoInitResult(uint, uint, uint);
	void			sigInitSlotData();
	void			sigToSlotViewUpdateSetup();
};

#endif // CHSMSMANAGER_HPP
