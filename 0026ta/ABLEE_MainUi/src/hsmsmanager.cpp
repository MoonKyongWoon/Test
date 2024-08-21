#include "hsmsmanager.hpp"

CTestInfo::CTestInfo(CFileMap *file_map, QObject *parent) : QObject(parent)
{
	this->valid_slot_count = 0;
	this->valid_slot = 0x0;
	for (auto id: this->board_id)
		id = "";
	this->lot_list.clear();
	this->recursive = 0;
	this->reply_msg = nullptr;
	this->current_slot = -1;
	this->pgm_list.clear();
	this->initSlotPgm();
	this->mpg_code = "";
	this->file_map = file_map;
	this->init_retest_count = 0;
	this->init_yield = 0.0;
	this->initSetupCount();
	this->initInitCount();
	this->setup_time.start_time = time(NULL);
	this->setup_time.end_time = time(NULL);
}

CTestInfo::~CTestInfo()
{
	if (this->reply_msg)
		delete this->reply_msg;
}

void		CTestInfo::setReplyMsg(CSecsMsg* secs_msg)
{
	this->reply_msg = secs_msg;
}

void		CTestInfo::setValidSlotCount(uint valid_slot_count)
{
	this->valid_slot_count = valid_slot_count;
}

void		CTestInfo::setValidSlot(int index, bool is_valid)
{
	ushort ref = 0b1 << index;
	if (is_valid)
		this->valid_slot = this->valid_slot | ref;
	else
		this->valid_slot = this->valid_slot & ~ref;
}

void		CTestInfo::setValidSlot(ushort valid_slot)
{
	this->valid_slot = valid_slot;
}

void		CTestInfo::setBoardId(int index, QString lot_no)
{
	if (index < 0)
		return ;
	if (index > 15)
		return ;
	this->board_id[index] = lot_no;
}

void		CTestInfo::setLotList(QVector<QString> lot_list)
{
	this->lot_list = lot_list;
}

void		CTestInfo::setLotList(QStringList lot_list)
{
	for (int i = 0; i < lot_list.length(); i++)
	{
		if ( !this->lot_list.contains(lot_list.at(i)) )
			this->lot_list.push_back(lot_list.at(i));
	}
}

void		CTestInfo::setPgmList(QVector<QString> pgm_list)
{
	this->pgm_list = pgm_list;
}

void		CTestInfo::setPgmList(QStringList pgm_list)
{
	for (int i = 0; i < pgm_list.length(); i++)
	{
		if ( !this->pgm_list.contains(pgm_list.at(i)) )
			this->pgm_list.push_back(pgm_list.at(i));
	}
}

void		CTestInfo::setCurrentSlot(int num)
{
	this->current_slot = num;
}

void		CTestInfo::setSlotPgm(uint slot, QString pgm)
{
	this->slot_pgm[slot] = pgm;
}

void		CTestInfo::setMpgCode(QString mpg_code)
{
	this->mpg_code = mpg_code;
}

void		CTestInfo::setInitRetestCount(int count)
{
	this->init_retest_count = count;
}

void		CTestInfo::setInitYield(double yield)
{
	this->init_yield = yield;
}

void		CTestInfo::setSetupStartTime()
{
	this->setup_time.start_time = time(NULL);
}

void		CTestInfo::setSetupEndTime()
{
	this->setup_time.end_time = time(NULL);
}

CSecsMsg*	CTestInfo::getReplyMsg()
{
	return this->reply_msg;
}

uint		CTestInfo::getValidSlotCount()
{
	return this->valid_slot_count;
}

ushort		CTestInfo::getValidSlot()
{
	return this->valid_slot;
}

QString		CTestInfo::getBoardId(int index)
{
	if (index < 0)
		return "";
	if (index > 15)
		return "";
	return this->board_id[index];
}

int			CTestInfo::getLotListCount()
{
	return this->lot_list.length();
}

QString		CTestInfo::getLot(int index)
{
	if (this->lot_list.length() - 1 < index)
		return "";
	return this->lot_list.at(index);
}

int			CTestInfo::getPgmListCount()
{
	return this->pgm_list.length();
}

QString		CTestInfo::getPgm(int index)
{
	if (this->pgm_list.length() - 1 < index)
		return "";
	return this->pgm_list.at(index);
}

int			CTestInfo::getCurrentSlot()
{
	return this->current_slot;
}

int			CTestInfo::getInitRetestCount()
{
	return this->init_retest_count;
}

double		CTestInfo::getInitYield()
{
	return this->init_yield;
}

uint			CTestInfo::getSetupCount()
{
	return this->setup_count;
}

uint			CTestInfo::getInitCount()
{
	return this->init_count;
}

void		CTestInfo::initCurrentSlot()
{
	this->current_slot = -1;
}

void		CTestInfo::initSlotPgm()
{
	for (int i = 0; i < MAX_SLOT; i += 1)
		this->slot_pgm[i] = "";
}

void		CTestInfo::initInitRetestCount()
{
	this->init_retest_count = 0;
}

void		CTestInfo::initInitYield()
{
	this->init_yield = 0.0;
}

void		CTestInfo::initSetupCount()
{
	this->setup_count = 1;
}

void		CTestInfo::initInitCount()
{
	this->init_count = 1;
}

void		CTestInfo::clearTestInfoData()
{
	this->setValidSlot(0x0);
	this->setValidSlotCount(0);
	this->initCurrentSlot();
	this->clearLotList();
	this->clearPgmList();
	this->initSlotPgm();
	this->initInitRetestCount();
	this->initInitYield();
	this->setMpgCode("");
}

void		CTestInfo::clearLotList()
{
	this->lot_list.clear();
}

void		CTestInfo::clearPgmList()
{
	this->pgm_list.clear();
}

void		CTestInfo::addLot(QString lot)
{
	if (!this->lot_list.contains(lot))
		this->lot_list.push_back(lot);
}

void		CTestInfo::addPgm(QString pgm)
{
	if (!this->pgm_list.contains(pgm))
		this->pgm_list.push_back(pgm);
}

void		CTestInfo::increaseSetupCount()
{
	this->setup_count += 1;
}

void		CTestInfo::increaseInitCount()
{
	this->init_count += 1;
}

void		CTestInfo::decreaseInitRetestCount()
{
	this->init_retest_count -= 1;
}

bool		CTestInfo::compareSlotPgm(uint slot, QString pgm)
{
	if (this->slot_pgm[slot].compare("") == 0)
	{
		this->slot_pgm[slot] = pgm;
		return true;
	}
	return this->slot_pgm[slot].compare(pgm) == 0 ? true : false;
}

bool		CTestInfo::compareMpgCode(QString mpg_code)
{
	if (this->mpg_code.compare("") == 0)
	{
		this->mpg_code = mpg_code;
		return true;
	}
	return this->mpg_code.left(9).compare(mpg_code.left(9)) == 0 ? true : false;
}

bool		CTestInfo::isCorrectPgm(QString lot, QString pgm)
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	QString data_path = "";

	if( env_data != NULL )
	{
		data_path = tr("%1").arg(env_data);
	}
	else
	{
		data_path = tr("%1").arg(DIR_ATDATA_PATH);
	}

	QString data_dir_path = tr("%1/%2/%3")
					  .arg(data_path)
					  .arg(DIR_ATDATA_NAME)
//					  .arg(this->file_map->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH);
					  .arg(DIR_AUTO_DATA_PATH);

	for (int slot = 0; slot < MAX_SLOT; slot += 1)
	{
		if (!(this->valid_slot & (0b1 << slot)))
			continue;
		QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
									 .arg(data_dir_path)
									 .arg(BOARD_MAINT_DIR)
									 .arg(slot)
									 .arg(EXT_RCMD);

		stRcmdBoardMaintInfo boardMaintInfo;
		this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));

		if (QString(boardMaintInfo.MLB_LOT_LIST).contains(lot))
		{
			if (!this->compareSlotPgm(slot, pgm))
				return false;
		}
	}
	return true;
}

void		CTestInfo::saveSetupStartTime()
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	QString data_path(env_data != NULL ? tr("%1").arg(env_data) : tr("%1").arg(DIR_ATDATA_PATH));
	QString setup_time_data_file = tr("%1/%2/%3/%4")
								   .arg(data_path)
								   .arg(DIR_ATDATA_NAME)
								   .arg(AUTO_DIR)
								   .arg(DAT_SETUP_TIME);

	QFile file(setup_time_data_file);
	if (!file.exists())
	{
		if (file.open(QIODevice::WriteOnly))
		{
			char* data = new char[sizeof(stSetupTimePair)];
			memcpy(data, &this->setup_time, sizeof(stSetupTimePair));
			uint write_size = file.write(data, sizeof(stSetupTimePair));
			delete[] data;
			file.close();
		}
	}
	else
	{
		if (file.open(QIODevice::ReadWrite))
		{
			char* read_data = new char[sizeof(stSetupTimePair)];
			uint read_size = file.read(read_data, sizeof(stSetupTimePair));
			stSetupTimePair read_setup_time;
			memcpy(&read_setup_time, read_data, sizeof(stSetupTimePair));
			delete[] read_data;
			this->setup_time.end_time = read_setup_time.end_time;

			file.seek(0);
			char* write_data = new char[sizeof(stSetupTimePair)];
			memcpy(write_data, &this->setup_time, sizeof(stSetupTimePair));
			uint write_size = file.write(write_data, sizeof(stSetupTimePair));
			delete[] write_data;
			file.close();
		}
	}
}

void		CTestInfo::saveSetupEndTime()
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	QString data_path(env_data != NULL ? tr("%1").arg(env_data) : tr("%1").arg(DIR_ATDATA_PATH));
	QString setup_time_data_file = tr("%1/%2/%3/%4")
								   .arg(data_path)
								   .arg(DIR_ATDATA_NAME)
								   .arg(AUTO_DIR)
								   .arg(DAT_SETUP_TIME);

	QFile file(setup_time_data_file);
	if (!file.exists())
	{
		if (file.open(QIODevice::WriteOnly))
		{		
			char* data = new char[sizeof(stSetupTimePair)];
			memcpy(data, &this->setup_time, sizeof(stSetupTimePair));
			uint write_size = file.write(data, sizeof(stSetupTimePair));
			delete[] data;
			file.close();
		}
	}
	else
	{
		if (file.open(QIODevice::ReadWrite))
		{
			char* read_data = new char[sizeof(stSetupTimePair)];
			uint read_size = file.read(read_data, sizeof(stSetupTimePair));
			stSetupTimePair read_setup_time;
			memcpy(&read_setup_time, read_data, sizeof(stSetupTimePair));
			delete[] read_data;
			this->setup_time.start_time = read_setup_time.start_time;

			file.seek(0);
			char* write_data = new char[sizeof(stSetupTimePair)];
			memcpy(write_data, &this->setup_time, sizeof(stSetupTimePair));
			uint write_size = file.write(write_data, sizeof(stSetupTimePair));
			delete[] write_data;
			file.close();
		}
	}
}

void		CTestInfo::readSetupTime()
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	QString data_path(env_data != NULL ? tr("%1").arg(env_data) : tr("%1").arg(DIR_ATDATA_PATH));
	QString setup_time_data_file = tr("%1/%2/%3/%4")
								   .arg(data_path)
								   .arg(DIR_ATDATA_NAME)
								   .arg(AUTO_DIR)
								   .arg(DAT_SETUP_TIME);

	QFile file(setup_time_data_file);
	if (!file.exists())
	{
		if (file.open(QIODevice::WriteOnly))
		{
			char* data = new char[sizeof(stSetupTimePair)];
			memcpy(data, &this->setup_time, sizeof(stSetupTimePair));
			file.write(data, sizeof(stSetupTimePair));
			delete[] data;
			file.close();
		}
	}
	else
	{
		if (file.open(QIODevice::ReadOnly))
		{
			stSetupTimePair read_setup_time;
			char* read_data = new char[sizeof(stSetupTimePair)];
			file.read(read_data, sizeof(stSetupTimePair));
			memcpy(&read_setup_time, read_data, sizeof(stSetupTimePair));
			delete[] read_data;
			file.close();
			qDebug() << "[READ][START] : " << ctime(&read_setup_time.start_time);
			qDebug() << "[READ][  END] : " << ctime(&read_setup_time.end_time);
		}
	}
}


CHsmsManager::CHsmsManager(QObject *parent) : QObject(parent)
{
	qRegisterMetaType<PROTOCOL::PLC_COMMAND>("PLC_COMMAND");
	initVariables();
	initPath();
	initSharedMemory();
	initTimer();

	char* env_data = getenv(ATDATA_DIR_ENV);
	QString data_path = "";
	if( env_data != NULL )
	{
		data_path = tr("%1").arg(env_data);
	}
	else
	{
		data_path = tr("%1").arg(DIR_ATDATA_PATH);
	}
	QString data_root = tr("%1/%2").arg(data_path).arg(DIR_ATDATA_NAME);
	this->file_map = new CFileMap(data_root.toStdString().c_str());
	this->test_info = new CTestInfo(this->file_map);
	initPath();
	initVidList();
	this->setVidValue(VID_SYSTEM_OP_ID, VID_DEFAULT_SYSTEM_OP_ID_VALUE);
}

void CHsmsManager::SET_UIAUTOMODE(bool mode)
{
	m_bUiAutoMode = mode;
	m_bInterruptMode = false;
}

CHsmsManager::~CHsmsManager()
{
	if (this->secs_gem) {
		delete this->secs_gem;
		this->secs_gem = nullptr;
	}
	if (this->message_queue_thread)
	{
//		this->message_queue_thread->quit();
//		this->message_queue_thread->wait();
		this->message_queue->deleteLater();
	}
//	if (this->message_queue)
//	{
//		delete this->message_queue;
//	}
	deleteTimer();
}

void	CHsmsManager::initVariables()
{
	this->system_byte = 0;
	this->device_id[0] = 0x0;
	this->device_id[1] = 0x0;
	this->secs_gem = new CSecsGem(this);
	this->m_bOnline_mode = false;
	this->m_bConnect_tms_eap = false;
	this->request_slot_num = 0;
	this->status_id = _STATUS_ID::_INIT;
	this->event_id = _EVENT_ID::_INIT;
	this->sent_ceid = 0;
	this->next_ceid = 0;
	this->auto_setup_status = 0b00000000;
	this->slot_with_bib = 0x0;
	this->m_data_dir_path = "";
	this->b_rack_ignore = false;
	this->repaet_slot_list.clear();
	memset(this->unload_check, false, sizeof(this->unload_check));
	memset(&this->m_stSlotLive, 0, sizeof(this->m_stSlotLive));

	//2024.07.22 - KWMOON
	for( int t=0;t<_TIMER_ID::_COUNT;t++ )
		this->m_pTimer[t] = nullptr;

	m_bLoadCompleted[_ZONE_NO::_ZONE1] = false;
	m_bLoadCompleted[_ZONE_NO::_ZONE2] = false;
	m_bUiAutoMode = false;
    this->b_rack_ignore = false;
	this->message_queue = new CHsmsMessageQueue(this->secs_gem);
	this->message_queue_thread = new QThread;
	this->message_queue->moveToThread(this->message_queue_thread);

	connect(this->message_queue_thread, &QThread::finished,							this->message_queue,	&QObject::deleteLater);
	connect(this,						&CHsmsManager::sigAddNewMessageBlock,		this->message_queue,	&CHsmsMessageQueue::slotAddNewMessageBlock);
	connect(this,						&CHsmsManager::sigAddMessageBlock,			this->message_queue,	&CHsmsMessageQueue::slotAddMessageBlock);
	connect(this->message_queue,		&CHsmsMessageQueue::sigSendMessage,			this,					&CHsmsManager::slotSendMessage);
	connect(this,						&CHsmsManager::sigQueueMonitoring,			this->message_queue,	&CHsmsMessageQueue::slotQueueMonitoring);
	connect(this->message_queue,		&CHsmsMessageQueue::sigSendNextCeidMessage,	this,					&CHsmsManager::slotSendNextCeidMessage);
	connect(this,						&CHsmsManager::sigSetWorking,				this->message_queue,	&CHsmsMessageQueue::slotSetWorking);
	this->message_queue_thread->start();

//	this->message_queue.clear();
	//TODO
	//load previous state for test_info
}

void	CHsmsManager::initTimer()
{
	for( int t=0;t<_TIMER_ID::_COUNT;t++ )
	{
		this->m_pTimer[t] = new QTimer(this);
		connect(this->m_pTimer[t],	&QTimer::timeout, this, &CHsmsManager::slotTimerTimeout );
	}
	for( int t=0;t<_UNLOAD_TIMER::_UNLOAD_COUNT;t++ )
	{
		this->m_pUnloadTimer[t] = new QTimer(this);
		connect(this->m_pUnloadTimer[t],	&QTimer::timeout, this, &CHsmsManager::slotTimerTimeout );
	}
}

void	CHsmsManager::initPath()
{
	char* env_data = getenv(ATDATA_DIR_ENV);
	QString data_path = "";

	if( env_data != NULL )
	{
		data_path = tr("%1").arg(env_data);
	}
	else
	{
		data_path = tr("%1").arg(DIR_ATDATA_PATH);
	}

	m_data_dir_path = tr("%1/%2/%3")
					  .arg(data_path)
					  .arg(DIR_ATDATA_NAME)
//					  .arg(this->file_map->GET_UIAUTOMODE() ? DIR_AUTO_DATA_PATH : DIR_ENGR_DATA_PATH);
					  .arg(DIR_AUTO_DATA_PATH);
}

void	CHsmsManager::initSharedMemory()
{
	this->m_pShmHsms = new QSharedMemory(SHM_KEY::SHM_HSMS_KEY, this);
	if (this->m_pShmHsms->isAttached())
		return;
	if (this->m_pShmHsms->attach())
		return;
	if (!this->m_pShmHsms->create(sizeof(_stShmHsms), QSharedMemory::ReadWrite))
	{
		// FIX
		// ERROR LOG
		qDebug() << "ERROR : Shared Memory attach error";
		return ;
	}
	m_pShmHsms->lock();
	stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
	memset(shm_data, 0, sizeof(stShmHsms));
	m_pShmHsms->unlock();
}

//2024.07.22 - KWMOON
void    CHsmsManager::slotTimerTimeout()
{
	QTimer* sender = static_cast<QTimer*>( QObject::sender() );
	if( sender == m_pTimer[_TIMER_ID::_READY_TO_LOAD] )
		timeoutReadyToLoad();
	else if( sender == m_pTimer[_TIMER_ID::_RACK_IN] )
		timeoutRackIn();
	else if( sender == m_pTimer[_TIMER_ID::_RACK_OUT] )
		timeoutRackOut();
	else if( sender == m_pTimer[_TIMER_ID::_DOOR_CLOSE] )
		timeoutDoorClose();
	else if( sender == m_pTimer[_TIMER_ID::_DOOR_OPEN] )
		timeoutDoorOpen();
	else if( sender == m_pTimer[_TIMER_ID::_AMB])
		timeoutAmb();
	else if( sender == m_pTimer[_TIMER_ID::_AMB_SOAK])
		timeoutAmbSoak();
	else if( sender == m_pTimer[_TIMER_ID::_DRY])
		timeoutDry();
	else if( sender == m_pTimer[_TIMER_ID::_DRY_SOAK])
		timeoutDrySoak();
	else if( sender == m_pUnloadTimer[_UNLOAD_TIMER::_PRE_UNLOAD])
		timeoutPreUnload();
	else if( sender == m_pUnloadTimer[_UNLOAD_TIMER::_UNLOAD_DOOR_OPEN])
		timeoutPreUnloadDoorOpen();
	else if( sender == m_pUnloadTimer[_UNLOAD_TIMER::_UNLOAD_RACK_OUT])
		timeoutPreUnloadRackOut();
	else if( sender == m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD])
		timeoutReadyToUnload();
	else
	{

	}
}

//2024.07.22 - KWMOON(READ TO LOAD)
void    CHsmsManager::timeoutReadyToLoad()
{
	qDebug() << "SEND READY TO LOAD";
	QString empty_zone = "";
	setVidValue(VID_MULU_PORT, "A");
	setVidValue(VID_EMPTY_DATE_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));

	if( !m_stChamberStatus.bBottomRackDetect && !m_stChamberStatus.bUpperRackDetect && m_stChamberStatus.bDoorOpen )
	{
		empty_zone = "1,2";
		setVidValue(VID_EMPTY_ZONE, empty_zone );
		makeCEIDMessage(5110);
	}
	else if( !m_stChamberStatus.bUpperRackDetect && m_stChamberStatus.bDoorOpen )
	{
		empty_zone = "1";
		setVidValue(VID_EMPTY_ZONE, empty_zone );
	}
	else if( !m_stChamberStatus.bBottomRackDetect && m_stChamberStatus.bDoorOpen )
	{
		empty_zone = "2";
		setVidValue(VID_EMPTY_ZONE, empty_zone );
	}
}

//2024.07.22 - KWMOON(RACK-IN)
void    CHsmsManager::timeoutRackIn()
{
	QDateTime now = QDateTime::currentDateTime();
	if( m_dtStart[_TIMER_ID::_RACK_IN].addSecs(DOOR_RACK_LIMIT_SEC) < now )
	{
		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("AUTO RACK-IN TIMEOUT %1 seconds").arg(DOOR_RACK_LIMIT_SEC));
		m_pTimer[_TIMER_ID::_RACK_IN]->stop();
		//error processing needed....
	}
	else
	{
		if( m_stChamberStatus.bRackInComplete && !m_stChamberStatus.bRackOutComplete)
		{
			m_pTimer[_TIMER_ID::_RACK_IN]->stop();
			if( this->status_id	== _STATUS_ID::_RUN )
			{
				emit sigInitSlotData();
				emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO RACK-IN COMPLETED"));
				emit sigBibIdRead( this->test_info->getValidSlot() );
				emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("BIB ID READ(0x%1) START").arg(this->test_info->getValidSlot(), 4, 16, QChar('0')) );
			}
		}
		else
		{
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO RACK-IN PROCESSING"));
		}
	}
}

//2024.07.22 - KWMOON(RACK-OUT)
void    CHsmsManager::timeoutRackOut()
{
	QDateTime now = QDateTime::currentDateTime();
	if( m_dtStart[_TIMER_ID::_RACK_OUT].addSecs(DOOR_RACK_LIMIT_SEC) < now )
	{
		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("AUTO RACK-OUT TIMEOUT %1 seconds").arg(DOOR_RACK_LIMIT_SEC));
		m_pTimer[_TIMER_ID::_RACK_OUT]->stop();
		//error processing needed....
	}
	else
	{
		if( m_stChamberStatus.bRackOutComplete && !m_stChamberStatus.bRackInComplete )
		{
			m_pTimer[_TIMER_ID::_RACK_OUT]->stop();
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO RACK-OUT COMPLETED"));
		}
		else
		{
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO RACK-OUT PROCESSING"));
		}
	}
}

//2024.07.22 - KWMOON(DOOR CLOSE)
void    CHsmsManager::timeoutDoorClose()
{
	QDateTime now = QDateTime::currentDateTime();
	if( m_dtStart[_TIMER_ID::_DOOR_CLOSE].addSecs(DOOR_RACK_LIMIT_SEC) < now )
	{
		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("AUTO DOOR-CLOSE TIMEOUT %1 seconds").arg(DOOR_RACK_LIMIT_SEC));
		m_pTimer[_TIMER_ID::_DOOR_CLOSE]->stop();
		//error processing needed....
	}
	else
	{
		if( m_stChamberStatus.bDoorClose && !m_stChamberStatus.bDoorOpen )
		{
			m_pTimer[_TIMER_ID::_DOOR_CLOSE]->stop();
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DOOR-CLOSE COMPLETED"));
            emit sigStartTest();
//            if (this->status_id == _STATUS_ID::_RUN && this->event_id == _EVENT_ID::_START)
//            {
//                emit sigStartTest();
//            }
		}
		else
		{
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DOOR-CLOSE PROCESSING"));
		}
	}
}

//2024.07.22 - KWMOON(DOOR OPEN)
void    CHsmsManager::timeoutDoorOpen()
{
	QDateTime now = QDateTime::currentDateTime();
	if( m_dtStart[_TIMER_ID::_DOOR_OPEN].addSecs(DOOR_RACK_LIMIT_SEC) < now )
	{
		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("AUTO DOOR-OPEN TIMEOUT %1 seconds").arg(DOOR_RACK_LIMIT_SEC));
		m_pTimer[_TIMER_ID::_DOOR_OPEN]->stop();
		//error processing needed....
	}
	else
	{
		if( m_stChamberStatus.bDoorOpen && !m_stChamberStatus.bDoorClose )
		{
			m_pTimer[_TIMER_ID::_DOOR_OPEN]->stop();
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DOOR-OPEN COMPLETED"));
		}
		else
		{
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DOOR-OPEN PROCESSING"));
		}
	}
}

void	CHsmsManager::timeoutAmb()
{
	QDateTime now = QDateTime::currentDateTime();
	int test_amb_time = DEFAULT_CHMB_AMB_WAIT_TIME * 60 * 1000;
	if (m_dtStart[_TIMER_ID::_AMB].addSecs(test_amb_time) < now)
	{
		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("AUTO AMB TIMEOUT %1 seconds").arg(test_amb_time));
		m_pTimer[_TIMER_ID::_AMB]->stop();
	}
	else
	{
		double temp_amb_sv = DEFAULT_CHMB_AMB_SV;
		double temp_margin = DEFAULT_CHMB_TEMP_MARGIN;
		if (m_stChamberStatus.dPV < temp_amb_sv + temp_margin && m_stChamberStatus.dPV > temp_amb_sv - temp_margin)
		{
			m_pTimer[_TIMER_ID::_AMB]->stop();
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO AMB WILL SOAK"));

			double amb_soak_time = DEFAULT_CHMB_AMB_SOAK_TIME * 60 * 1000;
			m_pTimer[_TIMER_ID::_AMB_SOAK]->start(amb_soak_time);
		}
		else
		{
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO AMB PROCESSING"));
		}
	}
}

void	CHsmsManager::timeoutAmbSoak()
{
	m_pTimer[_TIMER_ID::_AMB_SOAK]->stop();
	this->unload_check[_PRE_UNLOAD_CHECK::_AMB_DONE] = true;
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER, 0x1 ); // hsbae - 2024.08.06
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO AMB FINISH"));
}

void	CHsmsManager::timeoutDry()
{
	// for test
	if (!m_stChamberStatus.bRun){
		emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER, 0x1 ); // hsbae - 2024.08.06

		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("%1 : try RUN").arg(__func__));
	}
	if (m_stChamberStatus.dSV != DEFAULT_CHMB_DRY_SV)
	{
		double temp_dry_sv = DEFAULT_CHMB_DRY_SV;
		short sDrySv = static_cast<short>( temp_dry_sv * 10 );
		int nDrySv = static_cast<int>(sDrySv&0xFFFF);
		emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE, nDrySv );

		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("%1 : try SET SV %2").arg(__func__).arg(DEFAULT_CHMB_DRY_SV));
	}

	QDateTime now = QDateTime::currentDateTime();
	int test_dry_time = DEFAULT_CHMB_DRY_WAIT_TIME * 60 * 1000;
	if (m_dtStart[_TIMER_ID::_DRY].addSecs(test_dry_time) < now)
	{
		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("AUTO DRY TIMEOUT %1 seconds").arg(test_dry_time));
		m_pTimer[_TIMER_ID::_DRY]->stop();
	}
	else
	{
		double temp_dry_sv = DEFAULT_CHMB_DRY_SV;
		double temp_margin = DEFAULT_CHMB_TEMP_MARGIN;
		if (m_stChamberStatus.dPV < temp_dry_sv + temp_margin && m_stChamberStatus.dPV > temp_dry_sv - temp_margin)
		{
			m_pTimer[_TIMER_ID::_DRY]->stop();
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DRY WILL SOAK"));

			double dry_soak_time = DEFAULT_CHMB_DRY_SOAK_TIME * 60 * 1000;
			m_pTimer[_TIMER_ID::_DRY_SOAK]->start(dry_soak_time);
		}
		else
		{
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DRY PROCESSING"));
		}
	}
}

void	CHsmsManager::timeoutDrySoak()
{
	m_pTimer[_TIMER_ID::_DRY_SOAK]->stop();

	double temp_amb_sv = DEFAULT_CHMB_AMB_SV;
	short sAmbSv = static_cast<short>( temp_amb_sv * 10 );
	int nAmbSv = static_cast<int>(sAmbSv&0xFFFF);

	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE, nAmbSv );
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER, 0x1 ); // hsbae - 2024.08.06

	m_pTimer[_TIMER_ID::_AMB]->start(CHECK_TIME_10MS);
	m_dtStart[_TIMER_ID::_AMB] = QDateTime::currentDateTime();

	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DRY FINISH"));
}

void	CHsmsManager::timeoutPreUnload()
{
	for (int i = 0; i < _PRE_UNLOAD_CHECK::_CHECK_COUNT; i += 1 )
	{
		if (!this->unload_check[i])
			return;
	}
	m_pUnloadTimer[_UNLOAD_TIMER::_PRE_UNLOAD]->stop();
	this->setChamberDoorOpen();
	if ( CEID5150() )
		this->sent_ceid = 5150;
	this->m_pUnloadTimer[_UNLOAD_TIMER::_UNLOAD_DOOR_OPEN]->start(CHECK_TIME_5MS);
}

void	CHsmsManager::timeoutPreUnloadDoorOpen()
{
	if (m_stChamberStatus.bDoorOpen)
	{
		this->setChamberRackOut();
		this->m_pUnloadTimer[_UNLOAD_TIMER::_UNLOAD_DOOR_OPEN]->stop();
		this->m_pUnloadTimer[_UNLOAD_TIMER::_UNLOAD_RACK_OUT]->start(CHECK_TIME_5MS);
	}
}

void	CHsmsManager::timeoutPreUnloadRackOut()
{
	if (m_stChamberStatus.bRackOutComplete)
	{
		this->m_pUnloadTimer[_UNLOAD_TIMER::_UNLOAD_RACK_OUT]->stop();
		this->setEventId(_EVENT_ID::_IDLE_OUT);
	}
}

void    CHsmsManager::timeoutReadyToUnload()
{
	QString empty_zone = "";
	setVidValue(VID_MULU_PORT, "A");
	setVidValue(VID_EMPTY_DATE_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));

	if (m_stChamberStatus.bDoorOpen && m_stChamberStatus.bRackOutComplete)
	{
		if (!m_stChamberStatus.bUpperRackDetect && !m_stChamberStatus.bBottomRackDetect)
			return ;
		if (m_stChamberStatus.bUpperRackDetect && m_stChamberStatus.bBottomRackDetect)
			empty_zone = "1,2";
		else if (m_stChamberStatus.bUpperRackDetect)
			empty_zone = "1";
		else if (m_stChamberStatus.bBottomRackDetect)
			empty_zone = "2";
		setVidValue(VID_EMPTY_ZONE, empty_zone);
		qDebug() << "SEND READY TO UNLOAD";
		makeCEIDMessage(5130);
	}
}

bool    CHsmsManager::setChamberRackIn()		 //2024.07.22 - KWMOON
{
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, 0x0 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER  , 0x1 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER   , 0x1 );
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO RACK-IN START"));

	if( !m_pTimer[_TIMER_ID::_RACK_IN]->isActive() )
	{
		m_pTimer[_TIMER_ID::_RACK_IN]->start( RACK_IN_OUT_CHK_TIME );
		m_dtStart[_TIMER_ID::_RACK_IN] = QDateTime::currentDateTime();
	}
	return true;
}

bool    CHsmsManager::setChamberRackOut()	 //2024.07.22 - KWMOON
{
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, 0x0 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER  , 0x1 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER  , 0x1 );
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO RACK-OUT START"));

	if( !m_pTimer[_TIMER_ID::_RACK_OUT]->isActive() )
	{
		m_pTimer[_TIMER_ID::_RACK_OUT]->start( RACK_IN_OUT_CHK_TIME );
		m_dtStart[_TIMER_ID::_RACK_OUT] = QDateTime::currentDateTime();
	}
	return true;
}

void    CHsmsManager::slotChamberDoorClose()
{
    this->setChamberDoorClose();
}

void	CHsmsManager::slotSetAmbAfterTest()
{
	this->setAmbAfterTest();
}

bool    CHsmsManager::setChamberDoorClose()	 //2024.07.22 - KWMOON
{
    qDebug() << __func__;
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, 0x0 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER  , 0x1 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER, 0x1 );
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DOOR-CLOSE START"));

	if( !m_pTimer[_TIMER_ID::_DOOR_CLOSE]->isActive() )
	{
		m_pTimer[_TIMER_ID::_DOOR_CLOSE]->start(DOOR_CLOSE_OPEN_CHK_TIME );
		m_dtStart[_TIMER_ID::_DOOR_CLOSE] = QDateTime::currentDateTime();
	}
	return true;
}


bool    CHsmsManager::setChamberDoorOpen()	 //2024.07.22 - KWMOON
{
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER, 0x0 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER  , 0x1 );		//remote
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER , 0x1 );
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("AUTO DOOR-OPEN START"));

	if( !m_pTimer[_TIMER_ID::_DOOR_OPEN]->isActive() )
	{
		m_pTimer[_TIMER_ID::_DOOR_OPEN]->start(DOOR_CLOSE_OPEN_CHK_TIME );
		m_dtStart[_TIMER_ID::_DOOR_OPEN] = QDateTime::currentDateTime();
	}
	return true;
}

bool    CHsmsManager::setChamberAmbient()	 //2024.07.22 - KWMOON
{
	return true;
}

void	CHsmsManager::initVidList()
{
	char* env_os = getenv(ATOS_DIR_ENV);
	QString os_dir = "";

	if ( env_os != NULL )
	{
		os_dir = QString(env_os);
	}
	else
	{
		os_dir = DIR_ATOS_PATH;
	}

	QString sTmsIni = tr("%1/%2/%3").arg(os_dir).arg(CONFIG_DIR).arg(TMS_INI);

	int nValue = 0;
	QString sValue = "";
	QString section_key = "";

	section_key = tr("%1/%2").arg(SECTION_SYSTEM).arg(KEY_TMS_EQUIPID);
	if ( !UTIL::INI::readSettings( sTmsIni, section_key, sValue) )
	{
		sValue = DEFAULT_TMS_EQUIPID;
		qDebug() << "[HsmsMgr][ERROR] No EQ_ID in TmsIni";
	}
	setVidValue(VID_EQ_ID, sValue);
}

void	CHsmsManager::increaseDataIdInSharedMemory()
{
	stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());

	this->m_pShmHsms->lock();
	if (shm_data->data_id == 0xffffffff)
		shm_data->data_id = 0;
	else
		shm_data->data_id += 1;
	this->m_pShmHsms->unlock();
}

void	CHsmsManager::deleteTimer()
{
	for( int t=0;t<_TIMER_ID::_COUNT;t++ )
	{
		if (this->m_pTimer[t])
		{
			if (this->m_pTimer[t]->isActive())
			{
				this->m_pTimer[t]->stop();
				this->m_pTimer[t]->deleteLater();
			}
		}
	}
	for( int t=0;t<_UNLOAD_TIMER::_UNLOAD_COUNT;t++ )
	{
		if (this->m_pUnloadTimer[t])
		{
			if (this->m_pUnloadTimer[t]->isActive())
			{
				this->m_pUnloadTimer[t]->stop();
				this->m_pUnloadTimer[t]->deleteLater();
			}
		}
	}
}

bool	CHsmsManager::setTime(QString date)
{
	QString new_date = date.mid(0, 6) + " " + date.mid(6, 2) + ":" + date.mid(8, 2) + ":" + date.mid(10, 2);
	QString command = tr("echo '%1' | sudo -S date -s \"%2\"").arg(SSH_PASSWD).arg(new_date);
	int result = std::system(command.toUtf8().constData());

	if (result != 0) {
		return false;
	}
	return true;
}

QString convertFromUShortToBitsQString(ushort data)
{
	QString rtn = "";
	for (int i = 15; i >= 0; i -= 1)
		rtn += QString::number((data >> i) & 1);
	return rtn;
}

void	CHsmsManager::setSlotWithBIBIni()
{
	QString slot_with_bib(QString("0x%1").arg(this->test_info->getValidSlot(), 4, 16, QChar('0')).toUpper());
	QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(EVENT_INI));
	UTIL::INI::writeSettings(ini_file, KEY_SLOT_WITH_BIB, slot_with_bib);
}

void	CHsmsManager::setStatusId(_STATUS_ID status_id)
{
	QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(EVENT_INI));
	if ( UTIL::INI::writeSettings(ini_file, KEY_STATUS_ID, this->convertFromStatusIdToQString(status_id)) &&
		 this->setVidValue(VID_STATUS_ID, convertFromStatusIdToQString(status_id)) &&
		this->setVidValue(VID_STATUS_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss")) )
		this->status_id = status_id;
	if (makeCEIDMessage(7010))
		this->sent_ceid = 7010;
}

void	CHsmsManager::setEventId(_EVENT_ID event_id, bool b_force)
{
	if ( b_force || (this->event_id != event_id) )
	{
		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(EVENT_INI));
		if ( UTIL::INI::writeSettings(ini_file, KEY_EVENT_ID, this->convertFromEventIdToQString(event_id)) &&
			 this->setVidValue(VID_EVENT_ID, convertFromEventIdToQString(event_id)) &&
			 this->setVidValue(VID_COMMENT, makeCommentFromEventId(event_id)) )
//			 this->setVidValue(EVENT_ID, convertFromEventIdToQString(event_id)) )
			this->event_id = event_id;

		this->setVidValue(VID_ZONE1_FLAG, this->m_stChamberStatus.bUpperRackDetect ? "Y" : "N");
		this->setVidValue(VID_ZONE2_FLAG, this->m_stChamberStatus.bBottomRackDetect ? "Y" : "N");

		if (makeCEIDMessage(7000))
			this->sent_ceid = 7000;
	}
}

void    CHsmsManager::SetStatusId(_STATUS_ID status_id)
{
	this->setStatusId(status_id);
}

void    CHsmsManager::SetEventId(_EVENT_ID event_id)
{
	this->setEventId(event_id);
}

void     CHsmsManager::SetVidValue(QString vid, QString value)
{
	this->setVidValue(vid, value);
}

//2024.07.22 - KWMOON
void     CHsmsManager::GetVidValue(QString vid, QString& rvalue)
{
	this->getVidValue(vid, rvalue);
}

void	CHsmsManager::setChmbMode(_CHMB_MODE chmb_mode)
{
	if (this->chmb_mode != chmb_mode)
	{
		this->chmb_mode = chmb_mode;
		makeCEIDMessage(8100);
	}
}

void	CHsmsManager::setAutoSetupStatus(char status, bool set)
{
	if (set)
		this->auto_setup_status |= status;
	else
		this->auto_setup_status &= ~status;
}

bool	CHsmsManager::setVidValue(QString vid, QString value)
{
	QString ini_file(QObject::tr("%1/%2")
					 .arg(m_data_dir_path)
					 .arg(VID_VALUE_INI));

	return UTIL::INI::writeSettings(ini_file, vid, value);
}

bool	CHsmsManager::setVidValue(QString vid, uint value)
{
	QString ini_file(QObject::tr("%1/%2")
					 .arg(m_data_dir_path)
					 .arg(VID_VALUE_INI));

	return UTIL::INI::writeSettings(ini_file, vid, value);
}

//2024.07.22 - KWMOON
bool	CHsmsManager::getVidValue(QString vid, QString& rvalue)
{
	QString ini_file(QObject::tr("%1/%2")
					 .arg(m_data_dir_path)
					 .arg(VID_VALUE_INI));

	return UTIL::INI::readSettings(ini_file, vid, rvalue);
}

bool	CHsmsManager::checkBibType(int slot_bits)
{
	bool init_read_type(false);
	bool b_compare_type(true);
	QString type("");
	for (int slot = 0; slot < MAX_SLOT; slot += 1)
	{
		if (!(slot_bits & (0b1 << slot)))
			continue;
		if (!init_read_type)
		{
			type = QString(this->file_map->GET_BIBID(slot)).left(3);
			init_read_type = true;
		}
		else
		{
			if (type.compare(QString(this->file_map->GET_BIBID(slot)).left(3)) != 0)
			{
				b_compare_type = false;
				break;
			}
		}
	}
	this->setVidValue(VID_BIB_TYPE, b_compare_type ? type : "");
	return b_compare_type;
}

QString	CHsmsManager::convertFromStatusIdToQString(_STATUS_ID status_id)
{
	switch(status_id)
	{
		case _STATUS_ID::_DOWN:
			return "DOWN";
		case _STATUS_ID::_IDLE_IN:
			return "IDLE";
		case _STATUS_ID::_IDLE_OUT:
			return "IDLE";
		case _STATUS_ID::_INIT:
			return "INIT";
		case _STATUS_ID::_RUN:
			return "RUN";
		default:
			return "UNKNOWN";
	}
}

QString	CHsmsManager::convertFromEventIdToQString(_EVENT_ID event_id)
{
	switch(event_id)
	{
		case _EVENT_ID::_IDLE_IN:
			return "IDLE";
		case _EVENT_ID::_IDLE_OUT:
			return "IDLE";
		case _EVENT_ID::_LOT_IN_INFO:
			return "LOT IN INFO";
		case _EVENT_ID::_LOT_IN:
			return "LOT IN";
		case _EVENT_ID::_LOTIN:
			return "LOTIN";
		case _EVENT_ID::_READY:
			return "READY";
		case _EVENT_ID::_SETUP:
			return "SETUP";
		case _EVENT_ID::_SETUP_END:
			return "SETUP END";
		case _EVENT_ID::_PRETEST:
			return "PRETEST";
		case _EVENT_ID::_PRETEST_END:
			return "PRETEST END";
		case _EVENT_ID::_START:
			return "START";
		case _EVENT_ID::_MCPRN:
			return "MCPRN";
		case _EVENT_ID::_MCEND:
			return "MCEND";
		case _EVENT_ID::_LOT_OUT_INFO:
			return "LOT OUT INFO";
		case _EVENT_ID::_LOT_OUT:
			return "LOTOUT";
		case _EVENT_ID::_JAM:
			return "JAM";
		case _EVENT_ID::_BUSMS:
			return "BUSMS";
		case _EVENT_ID::_BUSME:
			return "BUSME";
		case _EVENT_ID::_INIT:
			return "INIT";
		default:
			return "UNKNOWN";
	}
}

QString CHsmsManager::makeCommentFromEventId(_EVENT_ID event_id)
{
    if (event_id == _EVENT_ID::_IDLE_IN)
		return "IDLE SEND";
	if (event_id == _EVENT_ID::_IDLE_OUT)
		return "IDLE SEND";
	if (event_id == _EVENT_ID::_READY)
		return "READY START";
	if (event_id == _EVENT_ID::_SETUP)
		return "SETUP START";
	if (event_id == _EVENT_ID::_SETUP_END)
		return "SETUP END";
	if (event_id == _EVENT_ID::_PRETEST)
		return "PRETEST START";
	if (event_id == _EVENT_ID::_PRETEST_END)
		return "PRETEST END";
	if (event_id == _EVENT_ID::_LOT_IN)
		return "LOT IN";
	if (event_id == _EVENT_ID::_LOTIN)
		return "";
	if (event_id == _EVENT_ID::_START)
		return "MANUAL START";
	if (event_id == _EVENT_ID::_MCPRN)
		return "MCPRN";
	if (event_id == _EVENT_ID::_MCEND)
		return "Y";
	if (event_id == _EVENT_ID::_LOT_OUT_INFO)
		return "";
	if (event_id == _EVENT_ID::_LOT_OUT)
		return "";
	return "ERROR";
}

uint	CHsmsManager::getPairedCEIDfromRCMD(_RCMD rcmd)
{
	if (rcmd == _RCMD::AUTO_DIAG)							return 7400;
	else if (rcmd == _RCMD::BI_CLOSE_LOT)					return 4600;
	else if (rcmd == _RCMD::BI_INITIAL_END)					return 6200;
	else if (rcmd == _RCMD::BI_INITIAL_START)				return 6100;
	else if (rcmd == _RCMD::BI_OPEN_LOT)					return 4500;
//	else if (rcmd == _RCMD::BI_RECLOSE_LOT)
	else if (rcmd == _RCMD::BI_TEST_TIME)					return 8300;
	else if (rcmd == _RCMD::BOARD_MAINT_INFO)				return 3400;
	else if (rcmd == _RCMD::BOARD_STATUS_INFO)				return 3410;
	else if (rcmd == _RCMD::BOARD_TYPE)						return 3300;
	else if (rcmd == _RCMD::CHECK_IN_BOARD)					return 3600;
	else if (rcmd == _RCMD::CHECK_OUT_BOARD)				return 3500;
	else if (rcmd == _RCMD::EQ_EFF_INFO)					return 7000;
	else if (rcmd == _RCMD::HOLD_CHECK)						return 7100;
	else if (rcmd == _RCMD::INTERLOCK_LIMIT)				return 7300;
	else if (rcmd == _RCMD::READY_TO_LOAD)					return 5110;
	else if (rcmd == _RCMD::READY_TO_UNLOAD)				return 5130;
	else if (rcmd == _RCMD::LOT_CALL)						return 5500;
//	else if (rcmd == _RCMD::LOT_SET)
//	else if (rcmd == _RCMD::LOT_SET_INFO)
//	else if (rcmd == _RCMD::MANUAL_MASK_MAP)
	else if (rcmd == _RCMD::PM_CHECK)						return 7320;
//	else if (rcmd == _RCMD::RACK_SET_INFO)
//	else if (rcmd == _RCMD::RECHECK_IN_BOARD)
	else if (rcmd == _RCMD::AUTO_BYPASS)					return 8001;
	else if (rcmd == _RCMD::REPLY_AUTO_BYPASS_EQP_STATE)	return 7007;
//	else if (rcmd == _RCMD::REQUEST_BOARD_MASK_SET)
//	else if (rcmd == _RCMD::REQUEST_RACK_SET)
//	else if (rcmd == _RCMD::RMVOU)
	else if (rcmd == _RCMD::SLOT_MASK_INFO)
	{
		if (this->event_id == _EVENT_ID::_SETUP)
			return 7340;
		else if (this->event_id == _EVENT_ID::_MCEND)
			return 7360;
		else
			return 0;
	}
	else if (rcmd == _RCMD::START_BI_CMD)					return 0;
	else if (rcmd == _RCMD::TGET)							return 4400;
	else if (rcmd == _RCMD::TMVIN)							return 5300;
	else if (rcmd == _RCMD::TMVOU)							return 5400;
	else if (rcmd == _RCMD::VMVOU)							return 6600;
	else													return 0;
}


// FIX
// ONLY TEMP DATA for AUTO_DIAG and EQ_EFF_INFO
// APPLY REAL DATA with CSecsGem Class

bool	CHsmsManager::getInitTestResult(int slot_no)
{
	QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
								 .arg(m_data_dir_path)
								 .arg(BOARD_MAINT_DIR)
								 .arg(slot_no)
								 .arg(EXT_RCMD);

	stRcmdBoardMaintInfo boardMaintInfo;
	this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));
	uint nMaxDut = file_map->GET_BIBX(slot_no, CPU00) * file_map->GET_BIBY(slot_no, CPU00);

	if ( nMaxDut < 1)
	{
		qDebug() << "[HsmsMgr][ERROR] MaxDut < 1";
		return false;
	}

	//Retrieve Data from File Map
	uint nMaxDutPerCpu = nMaxDut / MAX_SYS_CPU;

	stInitTestHead initTestHead[MAX_SYS_CPU];
	char* pInitFail = new char[nMaxDut];

	if ( !file_map->GET_INITTEST_DATA(slot_no, CPU00, initTestHead[CPU00], pInitFail, sizeof(char) * nMaxDutPerCpu) )
	{
		qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
		qDebug() << file_map->GET_ERRMSG();
		delete[] pInitFail;
		return false;
	}

	if ( !file_map->GET_INITTEST_DATA(slot_no, CPU01, initTestHead[CPU01], pInitFail + nMaxDutPerCpu, sizeof(char) * nMaxDutPerCpu) )
	{
		qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
		qDebug() << file_map->GET_ERRMSG();
		delete[] pInitFail;
		return false;
	}

	uint uInitTotalQty = 0;
	uint uInitPassQty = 0;
	uint uInitFailQty = 0;
	uint uInitMaskQty = 0;
	QString sInitialBinValue = "";

	for (uint i = 0; i < nMaxDut; i++)
	{
		if ( boardMaintInfo.INSERT_MOUNT_MAP[i] == 'x')
		{
			uInitMaskQty++;
			sInitialBinValue += 'x';
			continue;
		}

		if ( boardMaintInfo.INSERT_MOUNT_MAP[i] == '.')
		{
			sInitialBinValue += '.';
			continue;
		}

		if ( boardMaintInfo.INSERT_MOUNT_MAP[i] == '1' && boardMaintInfo.INITIAL_MASK_MAP[i] == '1' )
		{
			uInitTotalQty++;
			if ( pInitFail[i] )
			{
				sInitialBinValue += '0';
				uInitFailQty++;
			}
			else
			{
				sInitialBinValue += '1';
				uInitPassQty++;
			}
		}
	}

	delete[] pInitFail;

	this->setVidValue(VID_INITIAL_TEST_DUT_QTY, uInitTotalQty);
	this->setVidValue(VID_INITIAL_TEST_PASS_QTY, uInitPassQty);
	this->setVidValue(VID_INITIAL_TEST_FAIL_QTY, uInitFailQty);
	this->setVidValue(VID_INITIAL_SOCKET_MASK_QTY, uInitMaskQty);
	this->setVidValue(VID_INITIAL_BIN_VALUE, sInitialBinValue);

	char buffer[15];
	memset(buffer, 0x00, 15);
	time_t timeInitStart = initTestHead[CPU00].tInitBegin;
	if ( difftime(initTestHead[CPU01].tInitBegin,  initTestHead[CPU00].tInitBegin) > 0 )
	{
		timeInitStart = initTestHead[CPU01].tInitBegin;
	}

	strftime(buffer, 15, "%Y%m%d%H%M%S", localtime(&timeInitStart));
	const QString sInitTestStartTime = buffer;
	this->setVidValue(VID_INITIAL_START_TIME, sInitTestStartTime);

	time_t timeInitialEnd = initTestHead[CPU00].tInitEnd;
	if ( difftime(initTestHead[CPU01].tInitEnd, initTestHead[CPU00].tInitEnd) < 0 )
	{
		timeInitialEnd = initTestHead[CPU01].tInitEnd;
	}

	memset(buffer, 0x00, 15);
	strftime(buffer, 15, "%Y%m%d%H%M%S", localtime(&timeInitialEnd));
	const QString sInitialEndTime = buffer;
	this->setVidValue(VID_INITIAL_END_TIME, sInitialEndTime);

	setVidValue(VID_MLB_CODE, boardMaintInfo.MLB_CODE);
	setVidValue(VID_MLB_LOT_LIST, boardMaintInfo.MLB_LOT_LIST);
	setVidValue(VID_LOT_MAP, boardMaintInfo.LOT_MAP);

	return true;
}

void	CHsmsManager::slotFinishAtSaveMap(int exit, QProcess::ExitStatus stat, QString cmd)
{
	LOG_TYPE log_type = (stat == QProcess::NormalExit) ? LOG_TYPE::_NORMAL_LOG : LOG_TYPE::_ERROR_LOG;
	emit sigSysLogMessage( log_type, tr("FINISH %1 with Exit Code [%2]").arg(cmd).arg(exit) );
}


void	CHsmsManager::runAtSaveMap(QString option, uint count)
{
	QString program = QString(ATSAVEMAP_NAME);
	QStringList parameters;
	parameters << option;
	if (count != 0)
		parameters << QString::number(count);

	QString cmd = program;
	for (QString& parameter: parameters)
	{
		cmd += tr(" %1").arg(parameter);
	}

	QProcess* process = new QProcess;
	connect( process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int exitCode, QProcess::ExitStatus exitStatus){this->slotFinishAtSaveMap(exitCode, exitStatus, cmd); process->deleteLater();} );
	connect( process, &QProcess::readyReadStandardOutput                           , [=](){
		QByteArray ba = process->readAllStandardOutput();
		emit sigSysLogMessage(LOG_TYPE::_NORMAL_LOG, tr("[SAVEMAP]%1").arg(QString(ba)));
	} );
	process->start(program, parameters);
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("START %1").arg(cmd) );
	process->waitForStarted(3000);
	if (process->state() == QProcess::ProcessState::Running)
		return;
	else
	{
		emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("PROCESS IS NOT RUNNING %1").arg(cmd) );
		return ;
	}
}

bool	CHsmsManager::writeBinaryFile(const QString &file, const QByteArray *data)
{
	QFile fp(file);
	fp.open(QIODevice::WriteOnly);
	if (!fp.isOpen())
	{
		return false;
	}
	QDataStream outStream(&fp);
	outStream << *data;
	fp.close();
	return true;
}

bool	CHsmsManager::readBinaryFile(const QString &file, QByteArray* data)
{
	QFile fp(file);
	fp.open(QIODevice::ReadOnly);
	if (!fp.isOpen())
	{
		return false;
	}
	QDataStream inStream(&fp);
	inStream >> *data;
	fp.close();
	return true;
}

_RCMD			CHsmsManager::getRcmdFromString(atstring sRcmd)
{
	if (sRcmd.compare("AUTO_DIAG") == 0)						return _RCMD::AUTO_DIAG;
	else if (sRcmd.compare("BI_CLOSE_LOT") == 0)				return _RCMD::BI_CLOSE_LOT;
	else if (sRcmd.compare("BI_INITIAL_END") == 0)				return _RCMD::BI_INITIAL_END;
	else if (sRcmd.compare("BI_INITIAL_START") == 0)			return _RCMD::BI_INITIAL_START;
	else if (sRcmd.compare("BI_OPEN_LOT") == 0)					return _RCMD::BI_OPEN_LOT;
	else if (sRcmd.compare("BI_RECLOSE_LOT") == 0)				return _RCMD::BI_RECLOSE_LOT;
	else if (sRcmd.compare("BI_TEST_TIME") == 0)				return _RCMD::BI_TEST_TIME;
	else if (sRcmd.compare("BOARD_MAINT_INFO") == 0)			return _RCMD::BOARD_MAINT_INFO;
	else if (sRcmd.compare("BOARD_STATUS_INFO") == 0)			return _RCMD::BOARD_STATUS_INFO;
	else if (sRcmd.compare("BOARD_TYPE") == 0)					return _RCMD::BOARD_TYPE;
	else if (sRcmd.compare("CHECK_IN_BOARD") == 0)				return _RCMD::CHECK_IN_BOARD;
	else if (sRcmd.compare("CHECK_OUT_BOARD") == 0)				return _RCMD::CHECK_OUT_BOARD;
	else if (sRcmd.compare("EQ_EFF_INFO") == 0)					return _RCMD::EQ_EFF_INFO;
	else if (sRcmd.compare("HOLD_CHECK") == 0)					return _RCMD::HOLD_CHECK;
	else if (sRcmd.compare("INTERLOCK_LIMIT") == 0)				return _RCMD::INTERLOCK_LIMIT;
	else if (sRcmd.compare("READY_TO_LOAD") == 0)				return _RCMD::READY_TO_LOAD;
	else if (sRcmd.compare("READY_TO_UNLOAD") == 0)				return _RCMD::READY_TO_UNLOAD;
	else if (sRcmd.compare("LOT_CALL") == 0)					return _RCMD::LOT_CALL;
	else if (sRcmd.compare("LOT_SET") == 0)						return _RCMD::LOT_SET;
	else if (sRcmd.compare("LOT_SET_INFO") == 0)				return _RCMD::LOT_SET_INFO;
	else if (sRcmd.compare("MANUAL_MASK_MAP") == 0)				return _RCMD::MANUAL_MASK_MAP;
	else if (sRcmd.compare("PM_CHECK") == 0)					return _RCMD::PM_CHECK;
	else if (sRcmd.compare("RACK_SET_INFO") == 0)				return _RCMD::RACK_SET_INFO;
	else if (sRcmd.compare("RECHECK_IN_BOARD") == 0)			return _RCMD::RECHECK_IN_BOARD;
	else if (sRcmd.compare("AUTO_BYPASS") == 0)					return _RCMD::AUTO_BYPASS;
	else if (sRcmd.compare("REPLY_AUTO_BYPASS_EQP_STATE") == 0)	return _RCMD::REPLY_AUTO_BYPASS_EQP_STATE;
	else if (sRcmd.compare("REQUEST_BOARD_MASK_SET") == 0)		return _RCMD::REQUEST_BOARD_MASK_SET;
	else if (sRcmd.compare("REQUEST_RACK_SET") == 0)			return _RCMD::REQUEST_RACK_SET;
	else if (sRcmd.compare("RMVOU") == 0)						return _RCMD::RMVOU;
	else if (sRcmd.compare("SLOT_MASK_INFO") == 0)				return _RCMD::SLOT_MASK_INFO;
	else if (sRcmd.compare("START_BI_CMD") == 0)				return _RCMD::START_BI_CMD;
	else if (sRcmd.compare("TGET") == 0)						return _RCMD::TGET;
	else if (sRcmd.compare("TMVIN") == 0)						return _RCMD::TMVIN;
	else if (sRcmd.compare("TMVOU") == 0)						return _RCMD::TMVOU;
	else if (sRcmd.compare("VMVOU") == 0)						return _RCMD::VMVOU;
	else														return _RCMD::ERROR;
}

_RCMD			CHsmsManager::getRcmdPairedWithCeid(uint ceid)
{
	if (ceid == 7000)
		return _RCMD::EQ_EFF_INFO;
	if (ceid == 8001)
		return _RCMD::AUTO_BYPASS;
	if (ceid == 7007)
		return _RCMD::REPLY_AUTO_BYPASS_EQP_STATE;
	if (ceid == 5110)
		return _RCMD::READY_TO_LOAD;
	if (ceid == 7100)
		return _RCMD::HOLD_CHECK;
	if (ceid == 3300)
		return _RCMD::BOARD_TYPE;
	if (ceid == 3400)
		return _RCMD::BOARD_MAINT_INFO;
	if (ceid == 7340)
		return _RCMD::SLOT_MASK_INFO;
	if (ceid == 4400)
		return _RCMD::TGET;
	if (ceid == 3410)
		return _RCMD::BOARD_STATUS_INFO;
	if (ceid == 7300)
		return _RCMD::INTERLOCK_LIMIT;
	if (ceid == 6100)
		return _RCMD::BI_INITIAL_START;
	if (ceid == 6200)
		return _RCMD::BI_INITIAL_END;
	if (ceid == 5300)
		return _RCMD::TMVIN;
	if (ceid == 4500)
		return _RCMD::BI_OPEN_LOT;
	if (ceid == 3500)
		return _RCMD::CHECK_OUT_BOARD;
	if (ceid == 3600)
		return _RCMD::CHECK_IN_BOARD;
	if (ceid == 4600)
		return _RCMD::BI_CLOSE_LOT;
	if (ceid == 6600)
		return _RCMD::VMVOU;
	if (ceid == 5400)
		return _RCMD::TMVOU;
	if (ceid == 8300)
		return _RCMD::BI_TEST_TIME;
	if (ceid == 5500)
		return _RCMD::LOT_CALL;
	if (ceid == 7320)
		return _RCMD::PM_CHECK;
	if (ceid == 7360)
		return _RCMD::SLOT_MASK_INFO;
	if (ceid == 7400)
		return _RCMD::AUTO_DIAG;
	if (ceid == 5130)
		return _RCMD::READY_TO_UNLOAD;
	return _RCMD::ERROR;
}


_RCMD			CHsmsManager::saveRcmdIniData(CSecsMsg *secs_msg)
{
	atstring sRcmd("");
	this->secs_gem->GetListItemOpen(secs_msg);
	this->secs_gem->GetAsciiItem(secs_msg, sRcmd);

	_RCMD rcmd(getRcmdFromString(sRcmd));

	if (rcmd != _RCMD::ERROR)
	{
		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(RCMD_INI));
		QString section(sRcmd);
		atstring key("");
		atstring sValue("");
		int list_count(this->secs_gem->GetListItemOpen(secs_msg));
		for (int i = 0; i < list_count; i++)
		{
			this->secs_gem->GetListItemOpen(secs_msg);
			this->secs_gem->GetAsciiItem(secs_msg, key);
			this->secs_gem->GetAsciiItem(secs_msg, sValue );
			UTIL::INI::writeSettings(ini_file, tr("%1/%2").arg(section).arg(QString(key)), QString(sValue));
			this->secs_gem->GetListItemClose(secs_msg);
		}
		this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);
	}
	return rcmd;
}

_RCMD			CHsmsManager::convertToStructFromRcmd(CSecsMsg *secs_msg)
{
	//TEST CODE for data save
//	QByteArray ba;
//	ba.resize(sizeof(stRcmdAutoDiag));
//	memcpy(ba.data(), data, sizeof(stRcmdAutoDiag));
//	writeBinaryFile("/home/ablee/sw/user/hsbae/temp/auto_diag.rcmd", &ba);
//	stRcmdAutoDiag saved_data;
//	QByteArray read_ba;
//	read_ba.resize(sizeof(stRcmdAutoDiag));
//	readBinaryFile("/home/ablee/sw/user/hsbae/temp/auto_diag.rcmd", &read_ba);
//	memcpy(&saved_data, read_ba.data(), sizeof(stRcmdAutoDiag));

	atstring sRcmd("");
	this->secs_gem->GetListItemOpen(secs_msg);
	this->secs_gem->GetAsciiItem(secs_msg, sRcmd);

	_RCMD rcmd(getRcmdFromString(sRcmd));

	QString rcmd_file(QObject::tr("%1/%2.rcmd").arg(m_data_dir_path).arg(QString(sRcmd)));
	QString ini_file(QObject::tr("%1/%2.ini").arg(m_data_dir_path).arg(QString(sRcmd)));

//	QString section(sRcmd);
	atstring key("");
	atstring sValue("");
//	QStringList key_list;
//	QStringList value_list;

//	stRcmdData* rcmd_data = new stRcmdData;

	if (rcmd == _RCMD::AUTO_DIAG)
	{
		stRcmdAutoDiag* data = new stRcmdAutoDiag();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->AUTO_DIAG_FLAG);
					memcpy(data->AUTO_DIAG_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->AUTO_DIAG_DESC);
					memcpy(data->AUTO_DIAG_DESC, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdAutoDiag)];
		memset(write_data, 0, sizeof(stRcmdAutoDiag));
		memcpy(write_data, data, sizeof(stRcmdAutoDiag));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdAutoDiag));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::AUTO_DIAG;
	}
	if (rcmd == _RCMD::BI_CLOSE_LOT)
	{
		stRcmdBiCloseLot* data = new stRcmdBiCloseLot();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_FLAG);
					memcpy(data->MLB_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_PGM_LOT_LIST);
					memcpy(data->MLB_PGM_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBiCloseLot)];
		memset(write_data, 0, sizeof(stRcmdBiCloseLot));
		memcpy(write_data, data, sizeof(stRcmdBiCloseLot));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBiCloseLot));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BI_CLOSE_LOT;
	}
	if (rcmd == _RCMD::BI_INITIAL_END)
	{
		stRcmdBiInitialEnd* data = new stRcmdBiInitialEnd();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBiInitialEnd)];
		memset(write_data, 0, sizeof(stRcmdBiInitialEnd));
		memcpy(write_data, data, sizeof(stRcmdBiInitialEnd));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBiInitialEnd));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BI_INITIAL_END;
	}
	if (rcmd == _RCMD::BI_INITIAL_START)
	{
		stRcmdBiInitialStart* data = new stRcmdBiInitialStart();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBiInitialStart)];
		memset(write_data, 0, sizeof(stRcmdBiInitialStart));
		memcpy(write_data, data, sizeof(stRcmdBiInitialStart));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBiInitialStart));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BI_INITIAL_START;
	}
	if (rcmd == _RCMD::BI_OPEN_LOT)
	{
		stRcmdBiOpenLot* data = new stRcmdBiOpenLot();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_ID);
					rcmd_file = QObject::tr("%1/%2/%3.rcmd").arg(m_data_dir_path).arg(QString(sRcmd)).arg(QString(sValue));
					memcpy(data->LOT_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SNO);
					memcpy(data->SNO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_FLAG);
					memcpy(data->MLB_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_PGM_LOT_LIST);
					memcpy(data->MLB_PGM_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBiOpenLot)];
		memset(write_data, 0, sizeof(stRcmdBiOpenLot));
		memcpy(write_data, data, sizeof(stRcmdBiOpenLot));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBiOpenLot));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BI_OPEN_LOT;
	}
	if (rcmd == _RCMD::BI_RECLOSE_LOT)
	{
		// NO REF
//		stRcmdBiRecloseLot* data = new stRcmdBiRecloseLot();

//		rcmd_data->rcmd = _RCMD::BI_RECLOSE_LOT;
//		rcmd_data->data = static_cast<stRcmdBiRecloseLot*>(data);
		return _RCMD::BI_RECLOSE_LOT;
	}
	if (rcmd == _RCMD::BI_TEST_TIME)
	{
		stRcmdBiTestTime* data = new stRcmdBiTestTime();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBiTestTime)];
		memset(write_data, 0, sizeof(stRcmdBiTestTime));
		memcpy(write_data, data, sizeof(stRcmdBiTestTime));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBiTestTime));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BI_TEST_TIME;
	}
	if (rcmd == _RCMD::BOARD_MAINT_INFO)
	{
		stRcmdBoardMaintInfo* data = new stRcmdBoardMaintInfo();
		ini_file = QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI);
		for (int i = 0; i < MAX_SLOT; i += 1)
		{
			if (this->test_info->getValidSlot() & (0b1 << i) && this->test_info->getCurrentSlot() < i)
			{
				this->test_info->setCurrentSlot(i);
				break;
			}
		}
//		UTIL::INI::readSettings(ini_file, QString::number(1125), slot_num);

		rcmd_file = QObject::tr("%1/%2/SLOT%3.rcmd").arg(m_data_dir_path).arg(QString(sRcmd)).arg(this->test_info->getCurrentSlot());

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_TYPE);
					memcpy(data->BIB_TYPE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_ID);
					memcpy(data->BIB_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_X_SIZE);
					memcpy(data->BIB_X_SIZE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_Y_SIZE);
					memcpy(data->BIB_Y_SIZE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->P_CODE);
					memcpy(data->P_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_CODE);
					memcpy(data->MLB_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_LOT_LIST);
//                    this->test_info->clearLotList();
                    this->test_info->setLotList(QString(sValue).split(","));
					memcpy(data->MLB_LOT_LIST, sValue, sValue.length());
//					this->test_info->setLotList(QString(sValue).split(","));
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->IOC);
					memcpy(data->IOC, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BOC);
					memcpy(data->BOC, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BURN_IN_COUNT);
					memcpy(data->BURN_IN_COUNT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PART_NO);
					memcpy(data->PART_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_NO);
					memcpy(data->LOT_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_MAP);
					memcpy(data->LOT_MAP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TEST_PGM_NO);
					memcpy(data->TEST_PGM_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INSERT_MOUNT_MAP);
					memcpy(data->INSERT_MOUNT_MAP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INITIAL_MASK_MAP);
					memcpy(data->INITIAL_MASK_MAP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MANUAL_MASK_MAP);
					memcpy(data->MANUAL_MASK_MAP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->FUNTION_MASK_MAP);
					memcpy(data->FUNTION_MASK_MAP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PRODUCT_NAME);
					memcpy(data->PRODUCT_NAME, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_LAYOUT_FILE_NAME);
					memcpy(data->BIB_LAYOUT_FILE_NAME, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBoardMaintInfo)];
		memset(write_data, 0, sizeof(stRcmdBoardMaintInfo));
		memcpy(write_data, data, sizeof(stRcmdBoardMaintInfo));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBoardMaintInfo));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BOARD_MAINT_INFO;
	}
	if (rcmd == _RCMD::BOARD_STATUS_INFO)
	{
		stRcmdBoardStatusInfo* data = new stRcmdBoardStatusInfo();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBoardStatusInfo)];
		memset(write_data, 0, sizeof(stRcmdBoardStatusInfo));
		memcpy(write_data, data, sizeof(stRcmdBoardStatusInfo));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBoardStatusInfo));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BOARD_STATUS_INFO;
	}
	if (rcmd == _RCMD::BOARD_TYPE)
	{
		stRcmdBoardType* data = new stRcmdBoardType();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_TYPE);
					memcpy(data->BIB_TYPE, sValue, sValue.length());
					this->setVidValue(VID_BIB_TYPE, QString(sValue));
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_X_SIZE);
					memcpy(data->BIB_X_SIZE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_Y_SIZE);
					memcpy(data->BIB_Y_SIZE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_DESCRIPTION);
					memcpy(data->BIB_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->AUTO_START_CHECK);
					memcpy(data->AUTO_START_CHECK, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INIT_RETRY);
					memcpy(data->INIT_RETRY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SPECIAL_CODE_CHECK);
					memcpy(data->SPECIAL_CODE_CHECK, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SUB_EXCLUDED);
					memcpy(data->SUB_EXCLUDED, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->IOC_CHECK);
					memcpy(data->IOC_CHECK, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_INIT);
					memcpy(data->LOT_INIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_INIT);
					memcpy(data->BIB_INIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MOUNT_RATE);
					memcpy(data->MOUNT_RATE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIBQTY_CHK);
					memcpy(data->BIBQTY_CHK, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIBQTY);
					memcpy(data->BIBQTY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INCLUDED_SPECIAL_CODE);
					memcpy(data->INCLUDED_SPECIAL_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INCLUDED_SUB_LOTID);
					memcpy(data->INCLUDED_SUB_LOTID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INCLUDED_LOT_INIT);
					memcpy(data->INCLUDED_LOT_INIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INCLUDED_BIB_INIT);
					memcpy(data->INCLUDED_BIB_INIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INCLUDED_MOUNT_RATE);
					memcpy(data->INCLUDED_MOUNT_RATE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdBoardType)];
		memset(write_data, 0, sizeof(stRcmdBoardType));
		memcpy(write_data, data, sizeof(stRcmdBoardType));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdBoardType));

		delete[] write_data;

		this->test_info->setInitRetestCount(QString(data->INIT_RETRY).toInt());
		this->test_info->setInitYield(QString(data->LOT_INIT).toDouble());

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::BOARD_TYPE;
	}
	if (rcmd == _RCMD::CHECK_IN_BOARD)
	{
		stRcmdCheckInBoard* data = new stRcmdCheckInBoard();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_TYPE);
					memcpy(data->BIB_TYPE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_ID);
					memcpy(data->BIB_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->P_CODE);
					memcpy(data->P_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_CODE);
					memcpy(data->MLB_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_LOT_LIST);
					memcpy(data->MLB_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_MAP);
					memcpy(data->LOT_MAP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdCheckInBoard)];
		memset(write_data, 0, sizeof(stRcmdCheckInBoard));
		memcpy(write_data, data, sizeof(stRcmdCheckInBoard));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdCheckInBoard));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::CHECK_IN_BOARD;
	}
	if (rcmd == _RCMD::CHECK_OUT_BOARD)
	{
		stRcmdCheckOutBoard* data = new stRcmdCheckOutBoard();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_TYPE);
					memcpy(data->BIB_TYPE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_ID);
					memcpy(data->BIB_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->P_CODE);
					memcpy(data->P_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_CODE);
					memcpy(data->MLB_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_LOT_LIST);
					memcpy(data->MLB_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_MAP);
					memcpy(data->LOT_MAP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdCheckOutBoard)];
		memset(write_data, 0, sizeof(stRcmdCheckOutBoard));
		memcpy(write_data, data, sizeof(stRcmdCheckOutBoard));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdCheckOutBoard));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::CHECK_OUT_BOARD;
	}
	if (rcmd == _RCMD::EQ_EFF_INFO)
	{
		stRcmdEqEffInfo* data = new stRcmdEqEffInfo();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->JAM_GRADE);
					memcpy(data->JAM_GRADE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SINGO_EVENT);
					memcpy(data->SINGO_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->JAM_CONTENT);
					memcpy(data->JAM_CONTENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdEqEffInfo)];
		memset(write_data, 0, sizeof(stRcmdEqEffInfo));
		memcpy(write_data, data, sizeof(stRcmdEqEffInfo));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdEqEffInfo));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::EQ_EFF_INFO;
	}
	if (rcmd == _RCMD::HOLD_CHECK)
	{
		stRcmdHoldCheck* data = new stRcmdHoldCheck();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUIP_NO);
					memcpy(data->EQUIP_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->FIRST_QUARTER_PM_DATA);
					memcpy(data->FIRST_QUARTER_PM_DATA, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SECOND_QUARTER_PM_DATA);
					memcpy(data->SECOND_QUARTER_PM_DATA, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->THIRD_QUARTER_PM_DATA);
					memcpy(data->THIRD_QUARTER_PM_DATA, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->FOURTH_QUARTER_PM_DATA);
					memcpy(data->FOURTH_QUARTER_PM_DATA, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->FIRST_HALF_PM_DATA);
					memcpy(data->FIRST_HALF_PM_DATA, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LATTER_HALF_PM_DATA);
					memcpy(data->LATTER_HALF_PM_DATA, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->HOLD_FLAG);
					memcpy(data->HOLD_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdHoldCheck)];
		memset(write_data, 0, sizeof(stRcmdHoldCheck));
		memcpy(write_data, data, sizeof(stRcmdHoldCheck));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdHoldCheck));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::HOLD_CHECK;
	}
	if (rcmd == _RCMD::INTERLOCK_LIMIT)
	{
		stRcmdInterlockLimit* data = new stRcmdInterlockLimit();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->HOT_LIMIT_VALUE);
					memcpy(data->HOT_LIMIT_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->HOT_LIMIT_EVENT);
					memcpy(data->HOT_LIMIT_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->HOT_LIMIT_FLAG);
					memcpy(data->HOT_LIMIT_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->IOC_VALUE);
					memcpy(data->IOC_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->IOC_EVENT);
					memcpy(data->IOC_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->IOC_FLAG);
					memcpy(data->IOC_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_MV_VALUE);
					memcpy(data->SLOT_MV_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_MV_EVENT);
					memcpy(data->SLOT_MV_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_MV_FLAG);
					memcpy(data->SLOT_MV_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TEMP_DELAY_VALUE);
					memcpy(data->TEMP_DELAY_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TEMP_DELAY_EVENT);
					memcpy(data->TEMP_DELAY_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TEMP_DELAY_FLAG);
					memcpy(data->TEMP_DELAY_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BOC_VALUE);
					memcpy(data->BOC_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BOC_EVENT);
					memcpy(data->BOC_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BOC_FLAG);
					memcpy(data->BOC_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ALL_FAIL_VALUE);
					memcpy(data->ALL_FAIL_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ALL_FAIL_EVENT);
					memcpy(data->ALL_FAIL_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ALL_FAIL_FLAG);
					memcpy(data->ALL_FAIL_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIN1_VALUE);
					memcpy(data->BIN1_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIN1_EVENT);
					memcpy(data->BIN1_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIN1_FLAG);
					memcpy(data->BIN1_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PDA_VALUE);
					memcpy(data->PDA_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PDA_EVENT);
					memcpy(data->PDA_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PDA_FLAG);
					memcpy(data->PDA_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIN9_VALUE);
					memcpy(data->BIN9_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIN9_EVENT);
					memcpy(data->BIN9_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIN9_FLAG);
					memcpy(data->BIN9_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BLOCK_FAIL_VALUE);
					memcpy(data->BLOCK_FAIL_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BLOCK_FAIL_EVENT);
					memcpy(data->BLOCK_FAIL_EVENT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BLOCK_FAIL_FLAG);
					memcpy(data->BLOCK_FAIL_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_PATE_VALUE);
					memcpy(data->SLOT_PATE_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_PATE_EVNET);
					memcpy(data->SLOT_PATE_EVNET, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_PATE_FLAG);
					memcpy(data->SLOT_PATE_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->END_DELAY_VALUE);
					memcpy(data->END_DELAY_VALUE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->END_DELAY_EVNET);
					memcpy(data->END_DELAY_EVNET, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->END_DELAY_FLAG);
					memcpy(data->END_DELAY_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PFAIL_SKIP_FLAG);
					memcpy(data->PFAIL_SKIP_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdInterlockLimit)];
		memset(write_data, 0, sizeof(stRcmdInterlockLimit));
		memcpy(write_data, data, sizeof(stRcmdInterlockLimit));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdInterlockLimit));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::INTERLOCK_LIMIT;
	}
	if (rcmd == _RCMD::READY_TO_LOAD)
	{
		stRcmdReadyToLoad* data = new stRcmdReadyToLoad();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUIP_ID);
					memcpy(data->EQUIP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ZONE);
					memcpy(data->ZONE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->STATUS);
					memcpy(data->STATUS, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOGIS_DESCRIPTION);
					memcpy(data->LOGIS_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdReadyToLoad)];
		memset(write_data, 0, sizeof(stRcmdReadyToLoad));
		memcpy(write_data, data, sizeof(stRcmdReadyToLoad));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdReadyToLoad));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::READY_TO_LOAD;
	}
	if (rcmd == _RCMD::LOT_CALL)
	{
		stRcmdLotCall* data = new stRcmdLotCall();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SORTER_REMOVE_EQUIP_ID);
					memcpy(data->SORTER_REMOVE_EQUIP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdLotCall)];
		memset(write_data, 0, sizeof(stRcmdLotCall));
		memcpy(write_data, data, sizeof(stRcmdLotCall));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdLotCall));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::LOT_CALL;
	}
	if (rcmd == _RCMD::LOT_SET)
	{
		// NO REF
//		stRcmdLotSet* data = new stRcmdLotSet();

//		rcmd_data->rcmd = _RCMD::LOT_SET;
//		rcmd_data->data = static_cast<stRcmdLotSet*>(data);
//		return QString(data->RETURN_FLAG).compare("NG") == 0 ? _RCMD::ERROR : _RCMD::LOT_SET;
		return _RCMD::LOT_SET;
	}
	if (rcmd == _RCMD::LOT_SET_INFO)
	{
		// NO REF
//		stRcmdLotSetInfo* data = new stRcmdLotSetInfo();

//		rcmd_data->rcmd = _RCMD::LOT_SET_INFO;
//		rcmd_data->data = static_cast<stRcmdLotSetInfo*>(data);
		return _RCMD::LOT_SET_INFO;
	}
	if (rcmd == _RCMD::MANUAL_MASK_MAP)
	{
		// NO REF
//		stRcmdManualMaskMap* data = new stRcmdManualMaskMap();

//		rcmd_data->rcmd = _RCMD::MANUAL_MASK_MAP;
//		rcmd_data->data = static_cast<stRcmdManualMaskMap*>(data);
		return _RCMD::MANUAL_MASK_MAP;
	}
	if (rcmd == _RCMD::PM_CHECK)
	{
		stRcmdPmCheck* data = new stRcmdPmCheck();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PM_NAME);
					memcpy(data->PM_NAME, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PM_FLAG);
					memcpy(data->PM_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdPmCheck)];
		memset(write_data, 0, sizeof(stRcmdPmCheck));
		memcpy(write_data, data, sizeof(stRcmdPmCheck));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdPmCheck));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::PM_CHECK;
	}
	if (rcmd == _RCMD::RACK_SET_INFO)
	{
		// NO REF
//		stRcmdRackSetInfo* data = new stRcmdRackSetInfo();

//		rcmd_data->rcmd = _RCMD::RACK_SET_INFO;
//		rcmd_data->data = static_cast<stRcmdRackSetInfo*>(data);
		return _RCMD::RACK_SET_INFO;
	}
	if (rcmd == _RCMD::RECHECK_IN_BOARD)
	{
		// NO REF
//		stRcmdRecheckInBoard* data = new stRcmdRecheckInBoard();

//		rcmd_data->rcmd = _RCMD::RECHECK_IN_BOARD;
//		rcmd_data->data = static_cast<stRcmdRecheckInBoard*>(data);
		return _RCMD::RECHECK_IN_BOARD;
	}
	if (rcmd == _RCMD::AUTO_BYPASS)
	{
		stRcmdAutoByPass* data = new stRcmdAutoByPass();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BYPASS_SELECT);
					memcpy(data->BYPASS_SELECT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUID_ID);
					memcpy(data->EQUID_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdAutoByPass)];
		memset(write_data, 0, sizeof(stRcmdAutoByPass));
		memcpy(write_data, data, sizeof(stRcmdAutoByPass));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdAutoByPass));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::AUTO_BYPASS;
	}
	if (rcmd == _RCMD::REPLY_AUTO_BYPASS_EQP_STATE)
	{
		stRcmdReplyAutoBypassEqpState* data = new stRcmdReplyAutoBypassEqpState();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->BYPASSEQPSTATE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BYPASS_SELECT);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUID_ID);
					memcpy(data->EQUID_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdReplyAutoBypassEqpState)];
		memset(write_data, 0, sizeof(stRcmdReplyAutoBypassEqpState));
		memcpy(write_data, data, sizeof(stRcmdReplyAutoBypassEqpState));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdReplyAutoBypassEqpState));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::REPLY_AUTO_BYPASS_EQP_STATE;
	}
	if (rcmd == _RCMD::REQUEST_BOARD_MASK_SET)
	{
		// NO REF
//		stRcmdRequestBoardMaskSet* data = new stRcmdRequestBoardMaskSet();

//		rcmd_data->rcmd = _RCMD::REQUEST_BOARD_MASK_SET;
//		rcmd_data->data = static_cast<stRcmdRequestBoardMaskSet*>(data);
		return _RCMD::REQUEST_BOARD_MASK_SET;
	}
	if (rcmd == _RCMD::REQUEST_RACK_SET)
	{
		// NO REF
//		stRcmdRequestRackSet* data = new stRcmdRequestRackSet();

//		rcmd_data->rcmd = _RCMD::REQUEST_RACK_SET;
//		rcmd_data->data = static_cast<stRcmdRequestRackSet*>(data);
		return _RCMD::REQUEST_RACK_SET;
	}
	if (rcmd == _RCMD::RMVOU)
	{
		// NO REF
//		stRcmdRMVOU* data = new stRcmdRMVOU();

//		rcmd_data->rcmd = _RCMD::RMVOU;
//		rcmd_data->data = static_cast<stRcmdRMVOU*>(data);
		return _RCMD::RMVOU;
	}
	if (rcmd == _RCMD::SLOT_MASK_INFO)
	{
		stRcmdSlotMaskInfo* data = new stRcmdSlotMaskInfo();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUIP_NO);
					memcpy(data->EQUIP_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->INTERLOCK_NAME);
					memcpy(data->INTERLOCK_NAME, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUIP_MASK);
					memcpy(data->EQUIP_MASK, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TOTAL_SLOT_MASK);
					memcpy(data->TOTAL_SLOT_MASK, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MASK_INFO);
					memcpy(data->MASK_INFO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_USE_CHECK);
					memcpy(data->SLOT_USE_CHECK, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SLOT_USE_LIMIT);
					memcpy(data->SLOT_USE_LIMIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdSlotMaskInfo)];
		memset(write_data, 0, sizeof(stRcmdSlotMaskInfo));
		memcpy(write_data, data, sizeof(stRcmdSlotMaskInfo));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdSlotMaskInfo));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;

		if (this->sent_ceid == 7340)
			emit sigSlotMaskInfo1();
		else
			emit sigSlotMaskInfo2();

		return isNg ? _RCMD::ERROR : _RCMD::SLOT_MASK_INFO;
	}
	if (rcmd == _RCMD::START_BI_CMD)
	{
		stRcmdStartBiCmd* data = new stRcmdStartBiCmd();
		int board_count = 0;

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->BATCH_ID
					memcpy(data->BATCH_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_ID);
					memcpy(data->LOT_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
					if (sValue.length() != 0)
					{
						this->test_info->setLotList(QString(sValue).split(","));
					}
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->GROUP);
					memcpy(data->GROUP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BID_COUNT);
					memcpy(data->BID_COUNT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
					this->test_info->setValidSlotCount(QString(sValue).toUInt());
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->RACK_COUNT);
					memcpy(data->RACK_COUNT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ZONE_NO1);
					memcpy(data->ZONE_NO1, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ZONE_NO2);
					memcpy(data->ZONE_NO2, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ZONE_NO3);
					memcpy(data->ZONE_NO3, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ZONE_NO4);
					memcpy(data->ZONE_NO4, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);

				//2024.08.05 - KWMOON(CHANGE ARRAY)
				for( uint board=0;board<32;board++ )
				{
					this->secs_gem->GetListItemOpen(secs_msg);
						this->secs_gem->GetAsciiItem(secs_msg, key);
						this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BOARD_ID1);
						memcpy(data->BOARD_ID[board], sValue, sValue.length());
						if( board < MAX_SLOT )	//0  ~ 15
							this->test_info->setValidSlot(board_count, sValue.length() != 0);
						this->test_info->setBoardId(board_count++, QString(sValue));
					this->secs_gem->GetListItemClose(secs_msg);
				}
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

		this->setSlotWithBIBIni();

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdStartBiCmd)];
		memset(write_data, 0, sizeof(stRcmdStartBiCmd));
		memcpy(write_data, data, sizeof(stRcmdStartBiCmd));

		//2024.07.22 - KWMOON(REMOVE RCMD/DATA FILES)
		for(int s=0;s<MAX_SLOT;s++ )
		{
			this->file_map->REMOVE_BIBID_FILE(s);
			this->file_map->REMOVE_INITTEST_DATA(s);
			this->file_map->REMOVE_STEPTEST_DATA(s);
		}
		this->file_map->REMOVE_RCMD_FILES();
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdStartBiCmd));

		delete[] write_data;
		delete data;

//		rcmd_data->rcmd = _RCMD::START_BI_CMD;
//		rcmd_data->data = data;
		return _RCMD::START_BI_CMD;
	}
	if (rcmd == _RCMD::TGET)
	{
		stRcmdTGET* data = new stRcmdTGET();
		bool is_error = false;

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SORT);
					memcpy(data->SORT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TGET);
					memcpy(data->TGET, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_NO);
					rcmd_file = QObject::tr("%1/%2/%3.rcmd").arg(m_data_dir_path).arg(QString(sRcmd)).arg(QString(sValue));
					memcpy(data->LOT_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
					QString lot_info(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_FLAG);
					memcpy(data->MLB_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_PGM_LOT_LIST);
					memcpy(data->MLB_PGM_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PART_NO);
					memcpy(data->PART_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TEST_PGM_NO);
					memcpy(data->TEST_PGM_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUIP_NO);
					memcpy(data->EQUIP_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_TYPE);
					memcpy(data->BIB_TYPE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SYSTEM_OP_ID);
					memcpy(data->SYSTEM_OP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->DEVICE_NO);
					memcpy(data->DEVICE_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->OPER);
					memcpy(data->OPER, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->FAB);
					memcpy(data->FAB, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->GRADE);
					memcpy(data->GRADE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->DEVICE_QTY);
					memcpy(data->DEVICE_QTY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SPECIAL_CODE);
					memcpy(data->SPECIAL_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PGM_DIR);
					memcpy(data->PGM_DIR, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->DEVICE_DENSITY);
					memcpy(data->DEVICE_DENSITY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->DEVICE_TECH
					memcpy(data->DEVICE_TECH, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->BIB_QTY);
					memcpy(data->BIB_QTY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TAMS_IP);
					memcpy(data->TAMS_IP, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TAMS_ID);
					memcpy(data->TAMS_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TAMS_PWD);
					memcpy(data->TAMS_PWD, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->POWER_UP_RATE_LIMIT);
					memcpy(data->POWER_UP_RATE_LIMIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->POWER_UP_YIELD_LIMIT);
					memcpy(data->POWER_UP_YIELD_LIMIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PGM_TEST_TIME);
					memcpy(data->PGM_TEST_TIME, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->POWER_UP_PDA_LIMIT);
					memcpy(data->POWER_UP_PDA_LIMIT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->PGM_EQ_MODEL);
					memcpy(data->PGM_EQ_MODEL, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->OS_VERSION);
					memcpy(data->OS_VERSION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->FAMILY);
					memcpy(data->FAMILY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->OWNER);
					memcpy(data->OWNER, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->FACTORY);
					memcpy(data->FACTORY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->RWK_COUNT);
					memcpy(data->RWK_COUNT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->RWK_CODE);
					memcpy(data->RWK_CODE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->RML_GRP_ID);
					memcpy(data->RML_GRP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MPG);
					memcpy(data->MPG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->Temp1);
					memcpy(data->Temp1, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->Temp2);
					memcpy(data->Temp2, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->Temp3);
					memcpy(data->Temp3, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUIP_OWNER);
					memcpy(data->EQUIP_OWNER, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->RML_GRP_ID_BIB_QTY);
					memcpy(data->RML_GRP_ID_BIB_QTY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdTGET)];
		memset(write_data, 0, sizeof(stRcmdTGET));
		memcpy(write_data, data, sizeof(stRcmdTGET));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdTGET));

		delete[] write_data;

		this->setVidValue(VID_SYSTEM_OP_ID, QString(data->SYSTEM_OP_ID));
		this->test_info->addPgm(QString(data->TEST_PGM_NO));

		if (!this->test_info->compareMpgCode(QString(data->MPG)))
		{
			is_error = true;
			qDebug() << "[TGET ERROR] MPG ERROR";
		}
		if (!this->test_info->isCorrectPgm(QString(data->LOT_NO), QString(data->TEST_PGM_NO)))
		{
			is_error = true;
			qDebug() << "[TGET ERROR] Board has over 2 Pgm";
		}
		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));

		delete data;

		if (!isNg && !is_error)
			emit sigTGET(lot_info);
		return (isNg | is_error) ? _RCMD::ERROR : _RCMD::TGET;
	}
	if (rcmd == _RCMD::TMVIN)
	{
		stRcmdTMVIN* data = new stRcmdTMVIN();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SORT);
					memcpy(data->SORT, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TRANS_TYPE);
					memcpy(data->TRANS_TYPE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_NO);
					memcpy(data->LOT_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_FLAG);
					memcpy(data->MLB_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_PGM_LOT_LIST);
					memcpy(data->MLB_PGM_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SYSTEM_OP_ID);
					memcpy(data->SYSTEM_OP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQP_NO);
					memcpy(data->EQP_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TEST_PGM_NO);
					memcpy(data->TEST_PGM_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->DEVICE_FAB);
					memcpy(data->DEVICE_FAB, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->DEVICE_GRADE);
					memcpy(data->DEVICE_GRADE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->DEVICE_TECH);
					memcpy(data->DEVICE_TECH, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->DEVICE_DENSITY);
					memcpy(data->DEVICE_DENSITY, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdTMVIN)];
		memset(write_data, 0, sizeof(stRcmdTMVIN));
		memcpy(write_data, data, sizeof(stRcmdTMVIN));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdTMVIN));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::TMVIN;
	}
	if (rcmd == _RCMD::TMVOU)
	{
		stRcmdTMVOU* data = new stRcmdTMVOU();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TRANS_TYPE);
					memcpy(data->TRANS_TYPE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_NO);
					memcpy(data->LOT_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->MLB_FLAG
					memcpy(data->MLB_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_PGM_LOT_LIST);
					memcpy(data->MLB_PGM_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SYSTEM_OP_ID);
					memcpy(data->SYSTEM_OP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQP_NO);
					memcpy(data->EQP_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdTMVOU)];
		memset(write_data, 0, sizeof(stRcmdTMVOU));
		memcpy(write_data, data, sizeof(stRcmdTMVOU));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdTMVOU));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::TMVOU;
	}
	if (rcmd == _RCMD::VMVOU)
	{
		stRcmdVMVOU* data = new stRcmdVMVOU();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->TRANS_TYPE);
					memcpy(data->TRANS_TYPE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOT_NO);
					memcpy(data->LOT_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->MLB_FLAG
					memcpy(data->MLB_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->MLB_PGM_LOT_LIST);
					memcpy(data->MLB_PGM_LOT_LIST, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->SYSTEM_OP_ID);
					memcpy(data->SYSTEM_OP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQP_NO);
					memcpy(data->EQP_NO, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdVMVOU)];
		memset(write_data, 0, sizeof(stRcmdVMVOU));
		memcpy(write_data, data, sizeof(stRcmdVMVOU));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdVMVOU));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::VMVOU;
	}
	if (rcmd == _RCMD::READY_TO_UNLOAD)
	{
		stRcmdReadyToUnload* data = new stRcmdReadyToUnload();

			this->secs_gem->GetListItemOpen(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->RETURN_FLAG
					memcpy(data->RETURN_FLAG, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ERROR_DESCRIPTION);
					memcpy(data->ERROR_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->EQUIP_ID);
					memcpy(data->EQUIP_ID, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->ZONE);
					memcpy(data->ZONE, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//data->STATUS
					memcpy(data->STATUS, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
				this->secs_gem->GetListItemOpen(secs_msg);
					this->secs_gem->GetAsciiItem(secs_msg, key);
					this->secs_gem->GetAsciiItem(secs_msg, sValue );	//, data->LOGIS_DESCRIPTION);
					memcpy(data->LOGIS_DESCRIPTION, sValue, sValue.length());
//					key_list << QString(key);
//					value_list << QString(sValue);
				this->secs_gem->GetListItemClose(secs_msg);
			this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);

//		UTIL::INI::writeSettings(ini_file, section, key_list, value_list);

		char* write_data = new char[sizeof(stRcmdReadyToUnload)];
		memset(write_data, 0, sizeof(stRcmdReadyToUnload));
		memcpy(write_data, data, sizeof(stRcmdReadyToUnload));
		this->file_map->MAKE_FILE(rcmd_file.toStdString().c_str(), write_data, sizeof(stRcmdReadyToUnload));

		delete[] write_data;

		bool isNg(QString(data->RETURN_FLAG).compare("NG") == 0);
		if (isNg)
			emit sigAppenErrorPopup(tr("[ERROR] %1 : %2").arg(QString(sRcmd)).arg(QString(data->ERROR_DESCRIPTION)));
		delete data;
		return isNg ? _RCMD::ERROR : _RCMD::READY_TO_UNLOAD;
	}
//	delete rcmd_data;
	return _RCMD::UNKNOWN;
}

_RCMD	CHsmsManager::handleRcmd(CSecsMsg *secs_msg)
{
	_RCMD rcmd = convertToStructFromRcmd(secs_msg);
	switch(rcmd)
	{
		case _RCMD::AUTO_DIAG:
			emit sigAutoDiag();
			break;
		case _RCMD::BI_CLOSE_LOT:
			emit sigBiCloseLot();
			break;
		case _RCMD::BI_INITIAL_END:
//			emit sigBiInitialEnd();
			S2F42(true);
			break;
		case _RCMD::BI_INITIAL_START:
//			emit sigBiInitialStart();
			S2F42(true);
			break;
		case _RCMD::BI_OPEN_LOT:
			emit sigBiOpenLot();
			break;
		case _RCMD::BI_RECLOSE_LOT:
			emit sigBiRecloseLot();
			break;
		case _RCMD::BI_TEST_TIME:
			emit sigBiTestTime();
			break;
		case _RCMD::BOARD_MAINT_INFO:
			emit sigBoardMaintInfo(this->request_slot_num);
			break;
		case _RCMD::BOARD_STATUS_INFO:
			emit sigBoardStatusInfo();
			break;
		case _RCMD::BOARD_TYPE:
			emit sigBoardType(this->test_info->getValidSlot());
			break;
		case _RCMD::CHECK_IN_BOARD:
			emit sigCheckInBoard();
			break;
		case _RCMD::CHECK_OUT_BOARD:
			emit sigCheckOutBoard();
			break;
		case _RCMD::EQ_EFF_INFO:
			emit sigEqEffInfo();
			break;
		case _RCMD::HOLD_CHECK:
			emit sigHoldCheck();
			break;
		case _RCMD::INTERLOCK_LIMIT:
			emit sigInterLockLimit();
			break;
		case _RCMD::READY_TO_LOAD:
			emit sigReadyToLoad();
			break;
		case _RCMD::LOT_CALL:
			emit sigLotCall();
			break;
		case _RCMD::LOT_SET:
			emit sigLotSet();
			break;
		case _RCMD::LOT_SET_INFO:
			emit sigLotSetInfo();
			break;
		case _RCMD::MANUAL_MASK_MAP:
			emit sigManualMaskMap();
			break;
		case _RCMD::PM_CHECK:
			emit sigPmCheck();
			break;
		case _RCMD::RACK_SET_INFO:
			emit sigRackSetInfo();
			break;
		case _RCMD::RECHECK_IN_BOARD:
			emit sigRecheckInBoard();
			break;
		case _RCMD::AUTO_BYPASS:
			emit sigAutoBypass();
			break;
		case _RCMD::REPLY_AUTO_BYPASS_EQP_STATE:
			emit sigReplyAutoBypassEqpState();
			break;
		case _RCMD::REQUEST_BOARD_MASK_SET:
			emit sigRequestBoardMaskSet();
			break;
		case _RCMD::REQUEST_RACK_SET:
			emit sigRequestRackSet();
			break;
		case _RCMD::RMVOU:
			emit sigRMVOU();
			break;
//		case _RCMD::SLOT_MASK_INFO:
//			emit sigSlotMaskInfo();
//			break;
		case _RCMD::START_BI_CMD:
			this->test_info->clearTestInfoData();
			// 2024.07.18 - hsbae - hy
			S2F42(true);
			this->setStatusId(_STATUS_ID::_RUN);
			setChamberRackIn();
			if (this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive())
				this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();
//			emit sigStartBiCmd();
			break;
//		case _RCMD::TGET:
//			emit sigTGET();
//			break;
		case _RCMD::TMVIN:
			emit sigTMVIN();
			break;
		case _RCMD::TMVOU:
			emit sigTMVOU();
			break;
		case _RCMD::VMVOU:
			emit sigVMVOU();
			break;
		case _RCMD::READY_TO_UNLOAD:
			emit sigReadyToUnload();
			break;
		case _RCMD::ERROR:
			S2F42(false);
			emit sigSetWorking(false);
			break;
		default:
			break;
	}
	return rcmd;

//	if (rcmd != _RCMD::ERROR)
//	{
//		emit sigSendRcmdStructure(rcmd);
//	}

//	_RCMD rcmd = saveRcmdIniData(secs_msg);
//	emit sigSendRcmd(rcmd);
}

QString convertFromCharToBitsQString(char data)
{
	QString rtn = "";
	for (int i = 7; i >= 0; i -= 1)
		rtn += QString::number((data >> i) & 1);
	return rtn;
}
void	CHsmsManager::handleHsmsMessage(stHsmsElement *element)
{
	uint hsms_len = 0x0;
	hsms_len |= element->hsmsHeader.szMsgLength[0] << 24;
	hsms_len |= element->hsmsHeader.szMsgLength[1] << 16;
	hsms_len |= element->hsmsHeader.szMsgLength[2] <<  8;
	hsms_len |= element->hsmsHeader.szMsgLength[3] <<  0;

	CSecsMsg* secs_msg;

	char* pHsms = new char[hsms_len + 4];

	memcpy(pHsms, element, 4 + sizeof(stHsmsMsgHeader));
	memcpy(pHsms + 4 + sizeof(stHsmsMsgHeader), element->pData, hsms_len - sizeof(stHsmsMsgHeader));
	secs_msg = this->secs_gem->CreateMsg( pHsms, hsms_len + 4);

	short wait = 0x0;
	short stream = 0x0;
	short function = 0x0;
	uint uSysbyte = 0;
	secs_msg->GetMsgInfo(stream, function, wait);
	this->secs_gem->GetSystemByte( secs_msg, uSysbyte );
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[SECS][RECV]S%1F%2 %3 Systembyte:%4").arg(stream).arg(function) .arg(wait?"W":"S").arg(uSysbyte) );
	if (stream == 1)
	{
		if (function == 3)
		{

//			this->addMessageQueue(secs_msg, true);
			emit sigAddNewMessageBlock(secs_msg, 2, false);
//			this->message_queue->AddNewMessageBlock(secs_msg, 2, false);
			S1F3(secs_msg);
			qDebug() << "CHECK : " << convertFromCharToBitsQString(this->auto_setup_status);
//			if (this->auto_setup_status == static_cast<char>(_SETUP_PROCESS::_FINISH))
//				this->setEventId(_EVENT_ID::_IDLE_IN);
//			else
//				qDebug() << "CHECK : " << convertFromCharToBitsQString(this->auto_setup_status);
		}
		else if (function == 13)
		{
//			this->addMessageQueue(secs_msg, true);
			emit sigAddNewMessageBlock(secs_msg, 2, false);
//			this->message_queue->AddNewMessageBlock(secs_msg, 2, false);			
			S1F13(secs_msg);
		}
		else if (function == 17)
		{
//			this->addMessageQueue(secs_msg, true);
			emit sigAddNewMessageBlock(secs_msg, 2, false);
//			this->message_queue->AddNewMessageBlock(secs_msg, 2, false);
			S1F17(secs_msg);
		}
	}
	else if (stream == 2)
	{
		if (function == 31)
		{
//			this->addMessageQueue(secs_msg, true);
			emit sigAddNewMessageBlock(secs_msg, 2, false);
//			this->message_queue->AddNewMessageBlock(secs_msg, 2, false);
			S2F31(secs_msg);
		}
		else if (function == 33)
		{
//			this->addMessageQueue(secs_msg, true);
			emit sigAddNewMessageBlock(secs_msg, 2, false);
//			this->message_queue->AddNewMessageBlock(secs_msg, 2, false);
			S2F33(secs_msg);
		}
		else if (function == 35)
		{
//			this->addMessageQueue(secs_msg, true);
			emit sigAddNewMessageBlock(secs_msg, 2, false);
//			this->message_queue->AddNewMessageBlock(secs_msg, 2, false);
			S2F35(secs_msg);
		}
		else if (function == 37)
		{
//			this->addMessageQueue(secs_msg, true);
			emit sigAddNewMessageBlock(secs_msg, 2, false);
//			this->message_queue->AddNewMessageBlock(secs_msg, 2, false);
			S2F37(secs_msg);
		}
		else if (function == 41)
			S2F41(secs_msg);
	}
	else if (stream == 6)
	{
		if (function == 12)
			S6F12(secs_msg);
	}
}

void	CHsmsManager::slotRecvPacketFromTms(stIpcElement element)
{
	if (element.ipcHeader.cmd == PROTOCOL::TCP_COMMAND::CMD_TMSUI_EAP_CONN_STAT)
	{
		memcpy(&this->m_bConnect_tms_eap, element.pData, element.ipcHeader.datalen);

		if (element.pData != nullptr)
		{
			delete[] element.pData;
			element.pData = nullptr;
		}
		emit sigEapConnectStatus(this->m_bConnect_tms_eap);
		return;
	}

	if (element.ipcHeader.cmd == PROTOCOL::TCP_COMMAND::CMD_TMSUI_WITH_MAIN_MSG)
	{
		unsigned int nLen  = element.ipcHeader.datalen;

		char* pData = nullptr;
		stHsmsElement hsms_element;
		if (element.pData)
		{
			pData = new char[nLen];
			memcpy(pData, element.pData, nLen);
			delete[] element.pData;

			memcpy(&hsms_element, pData, sizeof(stHsmsHeader));
			hsms_element.pData = new char[nLen - sizeof(stHsmsMsgHeader)];
			memcpy(hsms_element.pData, pData + sizeof(stHsmsHeader), nLen - sizeof(stHsmsMsgHeader));

			if (pData)
				delete[] pData;
		}

		this->handleHsmsMessage(&hsms_element);

		if (hsms_element.pData) {
			delete[] hsms_element.pData;
		}

		return;
	}
}


void	CHsmsManager::slotSyncSlotTimeResult(unsigned short result, CSecsMsg* secs_msg)
{
	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);

	short bin_value = (result == 0xFF) ? 0b00000000 : 0b00000001;
	reply_secs_msg->AddBinaryItem(&bin_value, 1);

	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S1F3(CSecsMsg* secs_msg)
{
	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);

	QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
	atstring sValue("");
	ushort uValue(0);

	int list_len = 0;
	ushort vid;

	list_len = this->secs_gem->GetListItemOpen(secs_msg);
	this->secs_gem->AddListItem(reply_secs_msg, list_len);

	if (list_len == 7)
	{
		this->setVidValue(VID_DOOR_OPEN_INFO, m_stChamberStatus.bDoorOpen ? "Y" : "N");
		uint rack_count = 0;
		if (m_stChamberStatus.bUpperRackDetect)
			rack_count += 1;
		if (m_stChamberStatus.bBottomRackDetect)
			rack_count += 1;
		this->setVidValue(VID_RACK_COUNT, rack_count);
		this->setVidValue(VID_ZONE1_FLAG, m_stChamberStatus.bUpperRackDetect ? "Y" : "N");
		this->setVidValue(VID_ZONE2_FLAG, m_stChamberStatus.bBottomRackDetect ? "Y" : "N");
		this->setVidValue(VID_ZONE3_FLAG, "");
		this->setVidValue(VID_ZONE4_FLAG, "");
	}

	for (int i = 0; i < list_len; i++)
	{
		this->secs_gem->GetUI2Item(secs_msg, &vid);

		HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
		if (UTIL::INI::readSettings(ini_file, QString::number(vid), sValue))
		{
			if (format == HSMS_FMT_CODE::FMT_ASCII)
			{
				this->secs_gem->AddAsciiItem(reply_secs_msg, sValue);
			}
			else if (format == HSMS_FMT_CODE::FMT_U2)
			{
				uValue = QString(sValue).toUShort();
				this->secs_gem->AddUI2Item(reply_secs_msg, &uValue, 1);
			}
		}
		else
		{
			if (format == HSMS_FMT_CODE::FMT_ASCII)
			{
				this->secs_gem->AddAsciiItem(reply_secs_msg, "");
			}
			else if (format == HSMS_FMT_CODE::FMT_U2)
			{
				uValue = QString(sValue).toUShort();
				this->secs_gem->AddUI2Item(reply_secs_msg, &uValue, 0);
			}
		}
	}

	if (list_len == 7)
	{
		this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_CHMB_INFO), true);
	}
	else
	{
		this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_STATUS_INFO), true);
		this->next_ceid = 7000;
	}
	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S1F13(CSecsMsg* secs_msg)
{
	QString os_path = "";
	QString section_key = "";
	QString sValue = 0x0;
	char* env_os  = getenv(ATOS_DIR_ENV);
	if( env_os != NULL )
	{
		os_path = QString(env_os);
	}
	else
	{
		os_path = DIR_ATOS_PATH;
	}
	QString ini = QObject::tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(TMS_INI);
	section_key = QObject::tr("%1/%2").arg(SECTION_SYSTEM).arg(KEY_TMS_EQUIPID);

	if( !UTIL::INI::readSettings( ini, section_key, sValue ) )
	{
		UTIL::INI::writeSettings( ini, section_key, DEFAULT_TMS_EQUIPID );
		sValue = DEFAULT_TMS_EQUIPID;
	}

	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);

	this->secs_gem->AddListItem(reply_secs_msg, 2);
	short bin_value = 0b00000000;
	this->secs_gem->AddBinaryItem(reply_secs_msg, &bin_value, 1);
	this->secs_gem->AddListItem(reply_secs_msg, 2);
	this->secs_gem->AddAsciiItem(reply_secs_msg, sValue.toStdString());
	this->secs_gem->AddAsciiItem(reply_secs_msg, EQ_OS_VERSION);

	this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_INIT_INFO), true);

	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S1F17(CSecsMsg* secs_msg)
{
	short bin_value = 0b00000001;
	stIpcHeader ipcHeader;

	if (m_bOnline_mode)
	{
		bin_value = 0b00000010;
	}
	else
	{
		m_bOnline_mode = true;
		bin_value = 0b00000000;
	}

	ipcHeader.cmd = PROTOCOL::TCP_COMMAND::CMD_TMS_SET_ONLINE_MODE;
	ipcHeader.src = PROTOCOL::SOURCE_TYPE::_ATMAIN_UI;
	ipcHeader.dest = PROTOCOL::SOURCE_TYPE::_ATTMS_UI;
	ipcHeader.slot = 0x0;
	ipcHeader.runner = 0x0;
	ipcHeader.error = 0;
	ipcHeader.datalen = sizeof(m_bOnline_mode);

	unsigned int size = sizeof(stIpcHeader) + ipcHeader.datalen;
	char* pBuffer = nullptr;
	pBuffer = new char[size];

	memcpy(pBuffer, (char*)&ipcHeader, sizeof(stIpcHeader));
	memcpy(pBuffer + sizeof(stIpcHeader), (char*)&m_bOnline_mode, sizeof(m_bOnline_mode));

	emit sigSendToTmsUi(pBuffer, size);

	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);
    this->secs_gem->AddBinaryItem(reply_secs_msg, &bin_value, 1);

	this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_ONLINE_MODE), true);

	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S2F31(CSecsMsg* secs_msg)
{
	std::string time;
	this->secs_gem->GetAsciiItem(secs_msg, time);
	QString set_time = QString::fromStdString(time);

	if (setTime(set_time))
	{
		emit sigSyncSlotTime(secs_msg);
		this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_SYNC_TIME), true);
		return ;
	}
	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);

	short bin_value = 0b00000001;
	this->secs_gem->AddBinaryItem(reply_secs_msg, &bin_value, 1);

	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S2F33(CSecsMsg* secs_msg)
{
	int list_len = 0;
	uint data_id;
	list_len = this->secs_gem->GetListItemOpen(secs_msg);
	this->secs_gem->GetUI4Item(secs_msg, &data_id);
	list_len = this->secs_gem->GetListItemOpen(secs_msg);
	if (list_len == 0)
	{
		QVector<uint> empty_vector;
		this->secs_gem->setVIDLISTmap(0, empty_vector, true);
	}
	for (int i = 0; i < list_len; i++)
	{
		this->secs_gem->GetListItemOpen(secs_msg);
		uint rptid = 0;
		this->secs_gem->GetUI4Item(secs_msg, &rptid);
		uint vid_list_count = this->secs_gem->GetListItemOpen(secs_msg);

		QVector<uint> vid_list;
		for (uint j = 0; j < vid_list_count; j++)
		{
			uint vid = 0;
			this->secs_gem->GetUI4Item(secs_msg, &vid);
			vid_list.push_back(vid);
		}
		this->secs_gem->setVIDLISTmap(rptid, vid_list);
		this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);
	}

	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);
	//				short bin_value = result ? 0b00000000 : 0b00000001;
	short bin_value = 0b00000000;
	this->secs_gem->AddBinaryItem(reply_secs_msg, &bin_value, 1);

	this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_SET_VID), list_len == 0 ? false : true);

	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S2F35(CSecsMsg* secs_msg)
{
	int list_len = 0;
	uint data_id;
	list_len = this->secs_gem->GetListItemOpen(secs_msg);
	this->secs_gem->GetUI4Item(secs_msg, &data_id);
	list_len = this->secs_gem->GetListItemOpen(secs_msg);
	bool isInit(false);

	for (int i = 0; i < list_len; i++)
	{
		this->secs_gem->GetListItemOpen(secs_msg);

		uint ceid;
		this->secs_gem->GetUI4Item(secs_msg, &ceid);

		int rptid_list_count = 0;
		rptid_list_count = this->secs_gem->GetListItemOpen(secs_msg);
		if (isInit == false && rptid_list_count != 0)
			isInit = true;
		QVector<uint> rptid_list;
		for (int i = 0; i < rptid_list_count; i++)
		{
			uint rptid;
			this->secs_gem->GetUI4Item(secs_msg, &rptid);
			rptid_list.push_back(rptid);
		}
		this->secs_gem->setRPTIDmap(ceid, rptid_list);

		this->secs_gem->GetListItemClose(secs_msg);
		this->secs_gem->GetListItemClose(secs_msg);
	}

	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);

	short bin_value = 0b00000000;
	this->secs_gem->AddBinaryItem(reply_secs_msg, &bin_value, 1);

	this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_SET_RPTID), isInit);

	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S2F37(CSecsMsg* secs_msg)
{
	int list_len = 0;
	short bool_value;
	list_len = this->secs_gem->GetListItemOpen(secs_msg);
	this->secs_gem->GetBooleanItem(secs_msg, &bool_value);
	list_len = this->secs_gem->GetListItemOpen(secs_msg);

	bool result = true;
	if (list_len == 0)
	{
		result = this->secs_gem->setCEIDmap(bool_value, 0, true);
		int i = 0;
	}
	else
	{
		uint u_value;
		for (int i = 0; i < list_len; i++)
		{
			this->secs_gem->GetUI4Item(secs_msg, &u_value);
			if (result)
				result = this->secs_gem->setCEIDmap(bool_value, u_value);
		}
	}

	CSecsMsg* reply_secs_msg = this->secs_gem->CreateReplyMsg(secs_msg);

	short bin_value = result ? 0b00000000 : 0b00000001;
	this->secs_gem->AddBinaryItem(reply_secs_msg, &bin_value, 1);

	this->setAutoSetupStatus(static_cast<char>(_SETUP_PROCESS::_SET_CEID), list_len != 0);

	emit sigAddMessageBlock(reply_secs_msg, true);
}

void	CHsmsManager::S2F41(CSecsMsg* secs_msg)
{
	this->test_info->setReplyMsg(this->secs_gem->CreateReplyMsg(secs_msg));

//	_RCMD rcmd = this->handleRcmd(secs_msg);
	_RCMD rcmd = convertToStructFromRcmd(secs_msg);
	if (rcmd == _RCMD::START_BI_CMD)
		emit sigAddNewMessageBlock(this->test_info->getReplyMsg(), 2, false, getPairedCEIDfromRCMD(rcmd));
	else
		emit sigAddMessageBlock(this->test_info->getReplyMsg(), false, getPairedCEIDfromRCMD(rcmd));

	switch(rcmd)
	{
		case _RCMD::AUTO_DIAG:
			emit sigAutoDiag();
			break;
		case _RCMD::BI_CLOSE_LOT:
			emit sigBiCloseLot();
			break;
		case _RCMD::BI_INITIAL_END:
//			emit sigBiInitialEnd();
			S2F42(true);
			break;
		case _RCMD::BI_INITIAL_START:
//			emit sigBiInitialStart();
			S2F42(true);
			break;
		case _RCMD::BI_OPEN_LOT:
			emit sigBiOpenLot();
			break;
		case _RCMD::BI_RECLOSE_LOT:
			emit sigBiRecloseLot();
			break;
		case _RCMD::BI_TEST_TIME:
			emit sigBiTestTime();
			break;
		case _RCMD::BOARD_MAINT_INFO:
		{
			int slot_num = this->repaet_slot_list.dequeue();
			emit sigBoardMaintInfo(slot_num);
			break;
		}
		case _RCMD::BOARD_STATUS_INFO:
			emit sigBoardStatusInfo();
			break;
		case _RCMD::BOARD_TYPE:
			emit sigBoardType(this->test_info->getValidSlot());
			break;
		case _RCMD::CHECK_IN_BOARD:
			emit sigCheckInBoard();
			break;
		case _RCMD::CHECK_OUT_BOARD:
			emit sigCheckOutBoard();
			break;
		case _RCMD::EQ_EFF_INFO:
			emit sigEqEffInfo();
			break;
		case _RCMD::HOLD_CHECK:
			emit sigHoldCheck();
			break;
		case _RCMD::INTERLOCK_LIMIT:
			emit sigInterLockLimit();
			break;
		case _RCMD::READY_TO_LOAD:
			emit sigReadyToLoad();
			break;
		case _RCMD::LOT_CALL:
			emit sigLotCall();
			break;
		case _RCMD::LOT_SET:
			emit sigLotSet();
			break;
		case _RCMD::LOT_SET_INFO:
			emit sigLotSetInfo();
			break;
		case _RCMD::MANUAL_MASK_MAP:
			emit sigManualMaskMap();
			break;
		case _RCMD::PM_CHECK:
			emit sigPmCheck();
			break;
		case _RCMD::RACK_SET_INFO:
			emit sigRackSetInfo();
			break;
		case _RCMD::RECHECK_IN_BOARD:
			emit sigRecheckInBoard();
			break;
		case _RCMD::AUTO_BYPASS:
			emit sigAutoBypass();
			break;
		case _RCMD::REPLY_AUTO_BYPASS_EQP_STATE:
			emit sigReplyAutoBypassEqpState();
			break;
		case _RCMD::REQUEST_BOARD_MASK_SET:
			emit sigRequestBoardMaskSet();
			break;
		case _RCMD::REQUEST_RACK_SET:
			emit sigRequestRackSet();
			break;
		case _RCMD::RMVOU:
			emit sigRMVOU();
			break;
//		case _RCMD::SLOT_MASK_INFO:
//			emit sigSlotMaskInfo();
//			break;
		case _RCMD::START_BI_CMD:
			// 2024.07.18 - hsbae - hy
			if (this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive())
				this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();
			memset(this->unload_check, false, sizeof(this->unload_check));	//2024.08.15 - KWMOON
			S2F42(true);
			this->setStatusId(_STATUS_ID::_RUN);
			setChamberRackIn();
//			emit sigStartBiCmd();
			break;
//		case _RCMD::TGET:
//			emit sigTGET();
//			break;
		case _RCMD::TMVIN:
			emit sigTMVIN();
			break;
		case _RCMD::TMVOU:
			emit sigTMVOU();
			break;
		case _RCMD::VMVOU:
			emit sigVMVOU();
			break;
		case _RCMD::READY_TO_UNLOAD:
			emit sigReadyToUnload();
			break;
		case _RCMD::ERROR:
			S2F42(false);
			emit sigSetWorking(false);
			break;
		default:
			emit sigError();
			break;
	}
}

void	CHsmsManager::S2F42(bool ack)
{
	CSecsMsg* reply_msg = this->test_info->getReplyMsg();
	if (!reply_msg)
	{
		qDebug() << __func__ << " : ERROR";
		return;
	}
	this->secs_gem->AddListItem(reply_msg, 2);
	// TODO
	// send correct bin_value
	short bin_value = ack ? 0b00000000 : 0b00000001;
	this->secs_gem->AddBinaryItem(reply_msg, &bin_value, 1);
	this->secs_gem->AddListItem(reply_msg, 0);

	emit sigAddMessageBlock(reply_msg, true);
}

void	CHsmsManager::S6F12(CSecsMsg* secs_msg)
{
	emit sigAddMessageBlock(secs_msg, false);
//	this->message_queue->AddMessageBlock(secs_msg, false);

//	if (this->sent_ceid == 7010)
//	{
//		if (this->event_id == _EVENT_ID::_LOT_IN && this->status_id == _STATUS_ID::_RUN)
//			this->setEventId(_EVENT_ID::_READY);
//		else if (this->event_id == _EVENT_ID::_START && this->status_id == _STATUS_ID::_RUN)
//			this->sendNextCeidMessage();
//		else if (this->event_id == _EVENT_ID::_IDLE_OUT && this->status_id == _STATUS_ID::_IDLE)
//			this->sendNextCeidMessage();
//	}
	//TODO
	//EXCEPTION
}

void	CHsmsManager::sendNextCeidMessage()
{
	if (this->auto_setup_status == static_cast<char>(_SETUP_PROCESS::_FINISH))
	{
		this->auto_setup_status &= static_cast<char>(_SETUP_PROCESS::_INITIAL_STATUS);
		this->setEventId(_EVENT_ID::_IDLE_IN);
		return ;
	}
	if (this->event_id == _EVENT_ID::_IDLE_IN)
	{
		if (this->sent_ceid == 7000)
		{
			this->setStatusId(_STATUS_ID::_IDLE_IN);		//yseom
//			makeCEIDMessage(8001);
//			this->sent_ceid = 8001;
		}
	}
	else if (this->event_id == _EVENT_ID::_LOT_IN_INFO)
	{
		this->setEventId(_EVENT_ID::_LOT_IN);
	}
	else if (this->event_id == _EVENT_ID::_LOT_IN)
	{
		if (this->sent_ceid == 7000)
		{
			// 2024.07.18 - hsbae - hy
//			makeCEIDMessage(5120);
//			this->sent_ceid = 5120;
		}
		else if (this->sent_ceid == 5120)
		{
			this->setEventId(_EVENT_ID::_LOTIN);
		}
	}
	// 2024.07.18 - hsbae - hy
//	else if (this->event_id == _EVENT_ID::_LOTIN)
//	{
//		if (this->sent_ceid == 7010)
//			this->setEventId(_EVENT_ID::_READY);
//			;
//	}
//	this->setStatusId(_STATUS_ID::_RUN);
	else if (this->event_id == _EVENT_ID::_READY)
	{
		if (this->sent_ceid == 7000)
		{
//			this->test_info->setValidSlot(0x03);
			if (makeCEIDMessage(7100))
				this->sent_ceid = 7100;
		}
		else if (this->sent_ceid == 7100)
		{
			this->setEventId(_EVENT_ID::_SETUP);
			this->test_info->setSetupStartTime();
			this->test_info->saveSetupStartTime();
		}
	}
	else if (this->event_id == _EVENT_ID::_SETUP)
	{
		if (this->sent_ceid == 7000)
		{
//			CEID3300();
			if (makeCEIDMessage(3300))
				this->sent_ceid = 3300;
			this->test_info->recursive = 0;
			//TEST 2024 07 19 hsbae
//			this->test_info->setValidSlot(0, true);
		}
		else if (this->sent_ceid == 3300)
		{
//			CEID3400();
			if (sendCEID3400byBib())
				this->sent_ceid = 3400;
		}
		else if (this->sent_ceid == 3400)
		{
			//temp
			this->setSlotWithBIBIni();

			if (makeCEIDMessage(7340))
			{
				this->sent_ceid = 7340;
				this->test_info->setCurrentSlot(-1);
			}
		}
		else if (this->sent_ceid == 7340)
		{
			if (sendCEID4400byLot())
				this->sent_ceid = 4400;
		}
		else if (this->sent_ceid == 4400)
		{
			if (this->test_info->getPgmListCount() > 1)
				this->setVidValue(VID_MPG_FLAG, "Y");
			if (this->sendCEID3410byBib())
				this->sent_ceid = 3410;
		}
		else if (this->sent_ceid == 3410)
		{
			if (makeCEIDMessage(7300))
				this->sent_ceid = 7300;
		}
		else if (this->sent_ceid == 7300)
		{
			this->setEventId(_EVENT_ID::_SETUP_END);
			this->test_info->setSetupEndTime();
			this->test_info->saveSetupEndTime();
			runAtSaveMap("-set", this->test_info->getSetupCount());
			this->test_info->increaseSetupCount();
		}
	}
	else if (this->event_id == _EVENT_ID::_SETUP_END)
	{
		this->setEventId(_EVENT_ID::_PRETEST);
	}
	else if (this->event_id == _EVENT_ID::_PRETEST)
	{
		if (this->sent_ceid == 7000)
		{
			// START INIT TEST
			this->test_info->recursive = 0;
			emit sigStartInitTest();
		}
		else if (this->sent_ceid == 6200)
		{
			this->setEventId(_EVENT_ID::_PRETEST_END);
			runAtSaveMap("-pre", this->test_info->getInitCount());
			this->test_info->increaseInitCount();
		}
	}
	else if (this->event_id == _EVENT_ID::_PRETEST_END)
	{
		this->setEventId(_EVENT_ID::_START);
	}
	else if (this->event_id == _EVENT_ID::_START)
	{
		if (this->sent_ceid == 7000)
		{
			this->setStatusId(_STATUS_ID::_RUN);
		}
		else if (this->sent_ceid == 7010)
		{
			if (sendCEID5300byLot())
				this->sent_ceid = 5300;
		}
		else if (this->sent_ceid == 5300)
		{
			if (sendCEID4500byLot())
				this->sent_ceid = 4500;
		}
		else if (this->sent_ceid == 4500)
		{
            setChamberDoorClose();
            this->sent_ceid = 0;
		}
	}
	else if (this->event_id == _EVENT_ID::_MCPRN)
	{
		if (this->sent_ceid == 7000)
		{
            //TODO
            //send 3600 each slot
//            CEID3600();
//            makeCEIDMessage(3600);
//            this->sent_ceid = 3600;
		}
		else if (this->sent_ceid == 3600)
		{
			if (sendCEID4600byLot())
				this->sent_ceid = 4600;
		}
		else if (this->sent_ceid == 4600)
		{
			if (sendCEID6600byLot())
				this->sent_ceid = 6600;
		}
		else if (this->sent_ceid == 6600)
		{
			if (sendCEID5400byLot())
				this->sent_ceid = 5400;
		}
		else if (this->sent_ceid == 5400)
		{
			this->setEventId(_EVENT_ID::_MCEND);
		}
	}
	else if (this->event_id == _EVENT_ID::_MCEND)
	{
		if (this->sent_ceid == 7000)
		{
			if (sendCEID8300byLot())
				this->sent_ceid = 8300;
		}
		else if (this->sent_ceid == 8300)
		{
			if (sendCEID5500byLot())
				this->sent_ceid = 5500;
		}
		else if (this->sent_ceid == 5500)
		{
			if (makeCEIDMessage(7320))
				this->sent_ceid = 7320;
		}
		else if (this->sent_ceid == 7320)
		{
			if (makeCEIDMessage(7360))
				this->sent_ceid = 7360;
		}
		else if (this->sent_ceid == 7360)
		{
			if (makeCEIDMessage(7400))
				this->sent_ceid = 7400;
		}
		else if (this->sent_ceid == 7400)
		{
			if (sendCEID4200bySlot())
				this->sent_ceid = 4200;
		}
		else if (this->sent_ceid == 4200)
		{
			this->m_pUnloadTimer[_UNLOAD_TIMER::_PRE_UNLOAD]->start(CHECK_TIME_10MS);
			this->sent_ceid = 0;
		}
	}
	else if (this->event_id == _EVENT_ID::_IDLE_OUT)
	{
		if (this->sent_ceid == 7000)
		{
			this->setStatusId(_STATUS_ID::_IDLE_OUT);
		}
		else if (this->sent_ceid == 7010)
		{
//			this->m_bLoadCompleted[_ZONE_NO::_ZONE1] = !m_stChamberStatus.bUpperRackDetect;
//			this->m_bLoadCompleted[_ZONE_NO::_ZONE2] = !m_stChamberStatus.bBottomRackDetect;
//			timeoutReadyToUnload();
//			this->m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD]->start(READY_TO_LOAD_TIME);
//			this->setEventId(_EVENT_ID::_LOT_OUT_INFO);
			this->sent_ceid = 0;
		}
	}
	else if (this->event_id == _EVENT_ID::_LOT_OUT)
	{
		this->status_id = _STATUS_ID::_IDLE_IN;
		this->test_info->clearTestInfoData();
		this->test_info->initSetupCount();
		this->test_info->initInitCount();
	}
}

bool	CHsmsManager::makeCEIDMessage(uint ceid)
{
//	qDebug() << "MAKE CEID : " << ceid;
	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		//std::unique_ptr<CSecsMsg> secs_msg( this->secs_gem->CreateMsg(6, 11, 1) );
		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		this->increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());

		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);
		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		bool b_paired_message( ceid == 7010 || ceid == 5120 || ceid == 8100 || ceid == 4200 || ceid == 5150 || ceid == 5140 );
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, b_paired_message ? 2 : 4, true, ceid);

		//2024.06.21 - KWMOON
//		if( secs_msg != nullptr )
//			delete secs_msg;
	}
	return true;
}

// ALL ASCII
bool	CHsmsManager::CEID7000()
{
	uint ceid(7000);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);


		this->secs_gem->AddListItem(secs_msg, 3);

		this->increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(900), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(910), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(1000), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(1120), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(921), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(922), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(923), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(924), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(925), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(926), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(927), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(928), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(929), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(930), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(210), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9710), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(911), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(912), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(913), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(914), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 7000;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));

		//2024.06.21 - KWMOON
		if( secs_msg != nullptr )
			delete secs_msg;
	}
	return true;
}

bool	CHsmsManager::CEID8001()
{
	uint ceid(8001);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		this->increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(8001), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(8002), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 8001;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

bool	CHsmsManager::CEID7010()
{
	uint ceid(7010);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		this->increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);


		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(990), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9930), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 7010;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

bool	CHsmsManager::CEID7100()
{
	uint ceid(7100);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		this->increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 7100;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

bool	CHsmsManager::CEID3300()
{
	uint ceid(3300);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID3400()
{
	while ( (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive)) == 0)
	{
		qDebug() <<"[" <<this->test_info->recursive <<"]"<< this->test_info->getValidSlot() << " & " << (0b1 << this->test_info->recursive) << " = " << (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive));
		this->test_info->recursive += 1;
		if (this->test_info->recursive == 16)
			break;
	}

	if (this->test_info->recursive == 16)
	{
//		this->next_ceid = 7340;
		this->sent_ceid = 3400;
		return true;
	}

//	qDebug() <<"[" <<this->test_info->recursive <<"]"<< this->test_info->getValidSlot() << " & " << (0b1 << this->test_info->recursive) << " = " << (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive)) << " :PASS ";

    QString sBibId = file_map->GET_BIBID(this->test_info->recursive);
	setVidValue(VID_SLOT_ID, this->test_info->recursive + 1);
    setVidValue(VID_BIB_ID, sBibId);

	this->request_slot_num = this->test_info->recursive;

	uint ceid(3400);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);
		
		this->secs_gem->AddListItem(secs_msg, 3);

		this->increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(210), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(200), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(1125), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(771), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	this->test_info->recursive +=1 ;
	while ( (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive)) == 0)
	{
		qDebug() <<"[" <<this->test_info->recursive <<"]"<< this->test_info->getValidSlot() << " & " << (0b1 << this->test_info->recursive) << " = " << (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive));
		this->test_info->recursive += 1;
		if (this->test_info->recursive == 16)
			break;
	}
	if (this->test_info->recursive == 16)
	{
//		this->next_ceid = 7340;
		this->sent_ceid = 3400;
	}
	return true;

}

bool	CHsmsManager::CEID7340()
{
	uint ceid(7340);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(210), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 7340;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

bool	CHsmsManager::CEID4400()
{
    this->setVidValue(VID_LOT_NO, this->test_info->getLot(this->test_info->recursive));
//    setVidValue(VID_LOT_NO, "4QQLF17QAENC");

	this->test_info->recursive += 1;

	uint ceid(4400);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(1000), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(1120), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(210), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(1090), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(771), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}

	if (this->test_info->recursive == this->test_info->getLotListCount())
	{
		this->sent_ceid = 4400;
		this->test_info->recursive = 0x0;
	}

	return true;
}

bool	CHsmsManager::CEID3410()
{
//	uint temp = (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive));
	while ( (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive)) == 0)
	{
		this->test_info->recursive += 1;
		if (this->test_info->recursive == 16)
			break;
	}


	if (this->test_info->recursive == 16)
	{
//		this->next_ceid = 7300;
		this->sent_ceid = 3410;
		return true;
	}

//	qDebug() <<"[" <<this->test_info->recursive <<"]"<< this->test_info->getValidSlot() << " & " << (0b1 << this->test_info->recursive) << " = " << (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive)) << " :PASS ";
	setVidValue(VID_P_CODE, "SETUP");
	setVidValue(VID_BIB_ID, file_map->GET_BIBID(this->test_info->recursive));

	this->request_slot_num = this->test_info->recursive;

	uint ceid(3410);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(210), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(200), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(800), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	this->test_info->recursive +=1 ;
	while ( (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive)) == 0)
	{
		qDebug() <<"[" <<this->test_info->recursive <<"]"<< this->test_info->getValidSlot() << " & " << (0b1 << this->test_info->recursive) << " = " << (this->test_info->getValidSlot() & (0b1 << this->test_info->recursive));
		this->test_info->recursive += 1;
		if (this->test_info->recursive == 16)
			break;
	}

	if (this->test_info->recursive == 16)
	{
//		this->next_ceid = 7300;
		this->sent_ceid = 3410;
		this->test_info->recursive = 0;
	}
	return true;
}

bool	CHsmsManager::CEID7300()
{
	uint ceid(7300);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(210), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 7300;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

bool	CHsmsManager::CEID6100()
{
	uint ceid(6100);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }		
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID8100()
{
	uint ceid(8100);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(1010), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(1000), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9610), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(2000), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9090), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9620), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9640), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9660), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(7610), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(3000), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(210), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(7640), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 8100;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

bool	CHsmsManager::CEID8300()
{
	uint ceid(8300);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5500()
{
	uint ceid(5500);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID7320()
{
	uint ceid(7320);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID7360()
{
	uint ceid(7360);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID7400()
{
	uint ceid(7400);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID4200()
{
    uint ceid(4200);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 2, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5150()
{
	uint ceid(5150);

	QString fulled_zone("");
	if (this->m_stChamberStatus.bUpperRackDetect && this->m_stChamberStatus.bBottomRackDetect)
		fulled_zone = "1,2";
	else if (this->m_stChamberStatus.bUpperRackDetect)
		fulled_zone = "1";
	else if (this->m_stChamberStatus.bBottomRackDetect)
		fulled_zone = "2";

	this->setVidValue(VID_EMPTY_ZONE, fulled_zone);
	this->setVidValue(VID_EMPTY_DATE_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
	this->setVidValue(VID_MULU_PORT, "A");

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 2, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5130()
{
	uint ceid(5130);

	QString fulled_zone("");
	if (this->m_stChamberStatus.bUpperRackDetect && this->m_stChamberStatus.bBottomRackDetect)
		fulled_zone = "1,2";
	else if (this->m_stChamberStatus.bUpperRackDetect)
		fulled_zone = "1";
	else if (this->m_stChamberStatus.bBottomRackDetect)
		fulled_zone = "2";

	this->setVidValue(VID_EMPTY_ZONE, fulled_zone);
	this->setVidValue(VID_EMPTY_DATE_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
	this->setVidValue(VID_MULU_PORT, "A");

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5110()
{
	uint ceid(5110);

	QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
	QString key("9910");
	QString empty_datetime(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
	UTIL::INI::writeSettings(ini_file, key, empty_datetime);


	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);
		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		atstring sValue("");

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(917), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(9910), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);
		UTIL::INI::readSettings(ini_file, QString::number(918), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 5110;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

// ASCII + U2
bool	CHsmsManager::CEID7007()
{
	uint ceid(7007);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID6200()
{
	uint ceid(6200);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5300()
{
	uint ceid(5300);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID4500()
{
	uint ceid(4500);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID3500()
{
	uint ceid(3500);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID3600()
{
	uint ceid(3600);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID4600()
{
	uint ceid(4600);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID6600()
{
	uint ceid(6600);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5400()
{
	uint ceid(5400);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

        QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
        atstring sValue("");
        ushort uValue(0);
        for (uint j = 0; j < vid_count; j++)
        {
            uint vid = 0;
            if (!this->secs_gem->getVidValue(rptid, j, vid))
                return false;
            HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
            if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, sValue);
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
                }
            }
            else
            {
                if (format == HSMS_FMT_CODE::FMT_ASCII)
                {
                    this->secs_gem->AddAsciiItem(secs_msg, "");
                }
                else if (format == HSMS_FMT_CODE::FMT_U2)
                {
                    uValue = QString(sValue).toUShort();
                    this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
                }
            }
        }

		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5140()
{
	uint ceid(5140);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);
		for (uint j = 0; j < vid_count; j++)
		{
			uint vid = 0;
			if (!this->secs_gem->getVidValue(rptid, j, vid))
				return false;
			HSMS_FMT_CODE format(this->secs_gem->getVidType(vid));
			if ( UTIL::INI::readSettings(ini_file, QString::number(vid), sValue) )
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, sValue);
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);
				}
			}
			else
			{
				if (format == HSMS_FMT_CODE::FMT_ASCII)
				{
					this->secs_gem->AddAsciiItem(secs_msg, "");
				}
				else if (format == HSMS_FMT_CODE::FMT_U2)
				{
					uValue = QString(sValue).toUShort();
					this->secs_gem->AddUI2Item(secs_msg, &uValue, 0);
				}
			}
		}
		qDebug() << "ADD NEW MESSAGE : " << ceid;
		emit sigAddNewMessageBlock(secs_msg, 4, true, ceid);
	}
	return true;
}

bool	CHsmsManager::CEID5120()
{
	uint ceid(5120);

	uint rptid_count = 0;
	if(!this->secs_gem->getRPTIDmapLength(ceid, rptid_count))
		return false;

	for (uint i = 0; i < rptid_count; i++)
	{
		uint rptid = 0;
		if (!this->secs_gem->getRptidValue(ceid, i, rptid))
			return false;

		uint vid_count = 0;
		if (!this->secs_gem->getVIDLISTmapLength(rptid, vid_count))
			return false;

		CSecsMsg* secs_msg = this->secs_gem->CreateMsg(6, 11, 1);

		this->secs_gem->AddListItem(secs_msg, 3);

		increaseDataIdInSharedMemory();
		stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
		this->secs_gem->AddUI4Item(secs_msg, &shm_data->data_id, 1);

		this->secs_gem->AddUI4Item(secs_msg, &ceid, 1);

		this->secs_gem->AddListItem(secs_msg, 1);
		this->secs_gem->AddListItem(secs_msg, 2);
		this->secs_gem->AddUI4Item(secs_msg, &rptid, 1);
		this->secs_gem->AddListItem(secs_msg, vid_count);

		QString ini_file(QObject::tr("%1/%2").arg(m_data_dir_path).arg(VID_VALUE_INI));
		atstring sValue("");
		ushort uValue(0);

		UTIL::INI::readSettings(ini_file, QString::number(100), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

		UTIL::INI::readSettings(ini_file, QString::number(915), sValue);
		uValue = QString(sValue).toUShort();
		this->secs_gem->AddUI2Item(secs_msg, &uValue, 1);

		UTIL::INI::readSettings(ini_file, QString::number(9920), sValue);
		this->secs_gem->AddAsciiItem(secs_msg, sValue);

//		qDebug() << "RPTID : " << rptid;
//		atstring temp="";
//		this->secs_gem->GetMsgSML(secs_msg, temp);
//		qDebug() << QString(temp);
//		emit SECS_MSG
		QByteArray send_data;
		this->secs_gem->GetMsgHSMS(secs_msg, send_data);
		stIpcElement ipc_element;
		ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
		ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
		ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
		ipc_element.ipcHeader.error = 0;
		ipc_element.ipcHeader.slot = 0;
		ipc_element.ipcHeader.runner = 0;
		ipc_element.ipcHeader.datalen = send_data.length();

		char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
		memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
		memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

//		this->sent_ceid = 5120;
		emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));
	}
	return true;
}

//2024.06.10 - KWMOON
/* response example
 * emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER, 0x1 );
 * emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER, 0x1 ); // when start test,,,, init test -> door open
 * emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER, 0x1 );
 * emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER, 0x1 );
 */
void	CHsmsManager::slotFromChmbMgrResponse( PROTOCOL::PLC_COMMAND cmd, bool success)
{
	if (!success)
		return ;

	switch(cmd)
	{
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER:
		{
//			if (this->chmb_check_timer->isActive())
//				this->chmb_check_timer->stop();
//			makeCEIDMessage(5120);
		}

		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET:
		case PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER:
		case PROTOCOL::PLC_COMMAND::CMD_WB_BUZZER_ONOFF:
		case PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE:
		case PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE:
		default:
			break;
	}
}

//2024.06.10 - KWMOON
void	CHsmsManager::slotFromChmbMgrStatus( stChamberStatus chmbstat )
{
//	qDebug() << __func__ << " : " << b_rack_ignore;
//    qDebug() << tr("RACK IGNORE : %1").arg(b_rack_ignore);

//    qDebug() << __func__;
//    qDebug() << "Door Open:" << chmbstat.bDoorOpen;
//    qDebug() << "Door Close:" << chmbstat.bDoorClose;
//    qDebug() << "Door Rack In Complete:" << chmbstat.bRackInComplete;
//    qDebug() << "Door Rack Out Complete:" << chmbstat.bRackOutComplete;
//    qDebug() << "Door Upper Rack Detect:" << chmbstat.bUpperRackDetect;
//    qDebug() << "Door Bottom Rack Detect:" << chmbstat.bBottomRackDetect;

	if( !m_bUiAutoMode )
	{
		return;
	}

	if (b_rack_ignore)
	{
		if (this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive())
			this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();
		return;
	}

//	qDebug() << __func__ << " : " << convertFromCharToBitsQString(this->auto_setup_status);
	memcpy( &m_stChamberStatus, &chmbstat, sizeof(stChamberStatus) );

	if( !m_stChamberStatus.bChmbCon )
	{

	}

	if (this->status_id == _STATUS_ID::_IDLE_IN)
	{
		//RACK OUT STATUS
		if( m_stChamberStatus.bRackOutComplete )
		{
			//READY TO LOAD - RACK OUT & RACK NO DETECT & DOOR OPEN
			if( !m_stChamberStatus.bBottomRackDetect && !m_stChamberStatus.bUpperRackDetect  && m_stChamberStatus.bDoorOpen  && m_stChamberStatus.bRackOutComplete)
			{
				//READY TO LOAD TIMER START....
				if ( !this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive() )
				{
					qDebug() << "READY TO LOAD!";
					timeoutReadyToLoad();
					this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->start(READY_TO_LOAD_TIME);
					m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] = false;
					m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] = false;
					qDebug() << "START TIMER";
				}
			}
			//LOAD COMPLETE
			else if( (m_stChamberStatus.bBottomRackDetect || m_stChamberStatus.bUpperRackDetect) && m_stChamberStatus.bDoorOpen )
			{
				QString log_agv_status = tr("[ 0 ] AGV_COMPLETE[%1]   EQ_READY[%2]   EQ_ABORT[%3]")
								  .arg(m_stChamberStatus.bEqAgvComplete?"Y":"N")
								  .arg(m_stChamberStatus.bEqReadyStatus?"Y":"N")
								  .arg(m_stChamberStatus.bEqAbortStatus?"Y":"N");
				emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, log_agv_status);


	//			if ( this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive() && m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] && m_bLoadCompleted[ _ZONE_NO::_ZONE2 ])
	//			{
	//				emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, "READY TO LOAD TIMER STOP");
	//				this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();
	//			}
				//AGV ACTION COMPLETE & NO ERROR
				if( m_stChamberStatus.bEqReadyStatus )
				{
					emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, "[ 1 ] AGV WORKING...");
				}
				else if( m_stChamberStatus.bEqAgvComplete && !m_stChamberStatus.bEqAbortStatus && !m_stChamberStatus.bEqReadyStatus )
				{
					QString log_agv_status = tr("[ 2 ] AGV_COMPLETE[%1]   EQ_READY[%2]   EQ_ABORT[%3]")
									  .arg(m_stChamberStatus.bEqAgvComplete?"Y":"N")
									  .arg(m_stChamberStatus.bEqReadyStatus?"Y":"N")
									  .arg(m_stChamberStatus.bEqAbortStatus?"Y":"N");
					emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, log_agv_status);
					uint ceid = 5120;
					//CHAMBER PIO(AGV) COMPLETE.
					if( m_stChamberStatus.bUpperRackDetect )
					{
						if( !m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] )
						{
							setVidValue(VID_ZONE_NO, "1");
							if( makeCEIDMessage(ceid) )
							{
								m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] = true;
								if( !m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] )
								{
									this->sent_ceid = ceid;
									this->setEventId(_EVENT_ID::_LOTIN);
								}
							}
						}
					}

					if( m_stChamberStatus.bBottomRackDetect )
					{
						if( !m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] )
						{
							setVidValue(VID_ZONE_NO, "2");
							if( makeCEIDMessage(ceid) )
							{
								m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] = true;
								if( !m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] )
								{
									this->sent_ceid = ceid;
									this->setEventId(_EVENT_ID::_LOTIN);
								}
							}
						}
					}
				}
			}
			else
			{
				if (this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive())
				{
					this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();
				}
			}
		}
		else if( m_stChamberStatus.bRackInComplete )
		{
			//READY TO LOAD TIMER STOP...
			if (this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive())
			{
				// 2024.07.18 - hsbae - hy
				this->makeCEIDMessage(5120);
				this->sent_ceid = 5120;
				this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();
			}
			if( m_stChamberStatus.bRackOutComplete )
			{

			}
		}
		else
		{
			if (this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive())
			{
				this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();
			}
		}
	}
	else if (this->status_id == _STATUS_ID::_IDLE_OUT)
	{
		if (!this->m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD]->isActive() &&
			(m_stChamberStatus.bBottomRackDetect || m_stChamberStatus.bUpperRackDetect) &&
			m_stChamberStatus.bDoorOpen &&
			m_stChamberStatus.bRackOutComplete)
		{
			timeoutReadyToUnload();
			this->m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD]->start(READY_TO_LOAD_TIME);
			m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] = !m_stChamberStatus.bUpperRackDetect;
			m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] = !m_stChamberStatus.bBottomRackDetect;
			this->setEventId(_EVENT_ID::_LOT_OUT_INFO);
		}
		else
		{
			if (m_stChamberStatus.bEqReadyStatus)
				emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, "[ 1 ] AGV WORKING...");
			else if ( m_stChamberStatus.bEqAgvComplete && !m_stChamberStatus.bEqAbortStatus && !m_stChamberStatus.bEqReadyStatus )
			{
				QString log_agv_status = tr("[ 0 ] AGV_COMPLETE[%1]   EQ_READY[%2]   EQ_ABORT[%3]")
								  .arg(m_stChamberStatus.bEqAgvComplete?"Y":"N")
								  .arg(m_stChamberStatus.bEqReadyStatus?"Y":"N")
								  .arg(m_stChamberStatus.bEqAbortStatus?"Y":"N");
				emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, log_agv_status);

				uint ceid = 5140;
				//CHAMBER PIO(AGV) COMPLETE.
				if( !m_stChamberStatus.bUpperRackDetect )
				{
					if( !m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] )
					{
						qDebug() << "SEND 1";
						setVidValue(VID_ZONE_NO, "1");
						if( makeCEIDMessage(ceid) )
						{
							this->sent_ceid = ceid;
							m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] = true;
							if( m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] )
							{
								if (this->m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD]->isActive())
									this->m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD]->stop();
								this->setEventId(_EVENT_ID::_LOT_OUT);
							}
						}
					}
				}

				if( !m_stChamberStatus.bBottomRackDetect )
				{
					if( !m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] )
					{
						qDebug() << "SEND 2";
						setVidValue(VID_ZONE_NO, "2");
						if( makeCEIDMessage(ceid) )
						{
							this->sent_ceid = ceid;
							m_bLoadCompleted[ _ZONE_NO::_ZONE2 ] = true;
							if( m_bLoadCompleted[ _ZONE_NO::_ZONE1 ] )
							{
								if (this->m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD]->isActive())
									this->m_pUnloadTimer[_UNLOAD_TIMER::_READY_TO_UNLOAD]->stop();
								this->setEventId(_EVENT_ID::_LOT_OUT);
							}
						}
					}
				}
			}
		}
	}
}

void CHsmsManager::slotRecvSlotLiveStatus(stSlotLive slot_live_status)
{
	memcpy(&m_stSlotLive, &slot_live_status, sizeof(slot_live_status));
}

void CHsmsManager::slotDownloadPgmResult(bool bSuccess, QString sLotId)
{
	qDebug() << __func__;
	qDebug() << "bSuccess" << bSuccess;
	qDebug() << "sLotId" << sLotId;
}

void CHsmsManager::slotRcmdReply(bool ack)
{
	S2F42(ack);
}

bool CHsmsManager::sendCEID5300byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(this->test_info->getLot(i))
									 .arg(EXT_RCMD);

		stRcmdTGET tGet;
		this->file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		setVidValue(VID_TRANS_TYPE, "TMVIN");
		setVidValue(VID_LOT_NO, this->test_info->getLot(i));
		setVidValue(VID_OUT_QTY, tGet.DEVICE_QTY);
		setVidValue(VID_PGM_NO, tGet.TEST_PGM_NO);
		setVidValue(VID_BIB_QTY, tGet.BIB_QTY);
		setVidValue(VID_BIB_TYPE, tGet.BIB_TYPE);
		setVidValue(VID_TEST_START_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_FAMILY, tGet.FAMILY);
//		setVidValue(VID_MPG_FLAG, "Y");
		setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);
		setVidValue(VID_MLB_PGM_LOT_LIST, tGet.MLB_PGM_LOT_LIST);

		if (!makeCEIDMessage(5300))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID4500byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(this->test_info->getLot(i))
									 .arg(EXT_RCMD);

		stRcmdTGET tGet;
		this->file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		setVidValue(VID_PROCESS, "TEST");               //Need Explanation
		setVidValue(VID_PART_NO, tGet.PART_NO);
		setVidValue(VID_LOT_NO, this->test_info->getLot(i));
		setVidValue(VID_DEVICE_QTY, tGet.DEVICE_QTY);
		setVidValue(VID_PGM_NO, tGet.TEST_PGM_NO);
		setVidValue(VID_BIB_TYPE, tGet.BIB_TYPE);
		setVidValue(VID_START_DATE_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_FAMILY, tGet.FAMILY);
//		setVidValue(VID_MPG_FLAG, "Y");                 //Need Explanation
		setVidValue(VID_MLB_FLAG, tGet.MLB_FLAG);
		setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);
		setVidValue(VID_MLB_PGM_LOT_LIST, tGet.MLB_PGM_LOT_LIST);

		if (!makeCEIDMessage(4500))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID4600byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(this->test_info->getLot(i))
									 .arg(EXT_RCMD);
		stRcmdTGET tGet;

		file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		QString sRcmdBiOpenLotFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(BI_OPEN_LOT_DIR)
									 .arg(this->test_info->getLot(i))
									 .arg(EXT_RCMD);

		stRcmdBiOpenLot biOpenLot;
		this->file_map->READ_FILE(sRcmdBiOpenLotFile.toStdString().c_str(), (char*)&biOpenLot, sizeof(stRcmdBiOpenLot));

		setVidValue(VID_P_CODE, "TEST");
		setVidValue(VID_PART_NO, tGet.PART_NO);
		setVidValue(VID_LOT_NO, tGet.LOT_NO);
		setVidValue(VID_SNO, biOpenLot.SNO);
		setVidValue(VID_DEVICE_QTY, tGet.DEVICE_QTY);
		setVidValue(VID_PGM_NO, tGet.TEST_PGM_NO);
		setVidValue(VID_BIB_TYPE, tGet.BIB_TYPE);
		setVidValue(VID_END_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_AUTO_MODE, "Y");
		setVidValue(VID_FAMILY, tGet.FAMILY);
		setVidValue(VID_POWER_FAULT_COUNT, 0);
		setVidValue(VID_FIRST_INITIAL_BIN, tGet.DEVICE_QTY);
		setVidValue(VID_INITIAL_TEST_TOTAL_COUNT, 1);
		setVidValue(VID_BIN0A, 0);
		setVidValue(VID_SPECIAL_CODE, tGet.SPECIAL_CODE);
		setVidValue(VID_MLB_FLAG, tGet.MLB_FLAG);
		setVidValue(VID_MLB_PGM_LOT_LIST, tGet.MLB_PGM_LOT_LIST);
		setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);

		if (!makeCEIDMessage(4600))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID6600byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		QString lot = this->test_info->getLot(i);

		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(lot)
									 .arg(EXT_RCMD);

		stRcmdTGET tGet;
		this->file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		QString sRcmdOpenLot = tr("%1/%2%3")
									 .arg(m_data_dir_path)
									 .arg(RCMD_BI_OPEN_LOT)
									 .arg(EXT_RCMD);
		stRcmdBiOpenLot biOpenLot;
		file_map->READ_FILE(sRcmdOpenLot.toStdString().c_str(), (char*)&biOpenLot, sizeof(stRcmdBiOpenLot));

		uint in_qty(0);
		uint out_qty(0);
		uint bib_qty(0);
		uint pass_qty(0);
		uint bin[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		QStringList bib_list;
		uint nMaxDut(0);
		uint nMaxDutPerCpu(0);

		for (int slot = 0; slot < MAX_SLOT; slot += 1)
		{
			if ( !(this->test_info->getValidSlot() & (0b1 << slot)) )
				continue;

			QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
										 .arg(m_data_dir_path)
										 .arg(BOARD_MAINT_DIR)
										 .arg(slot)
										 .arg(EXT_RCMD);

			stRcmdBoardMaintInfo boardMaintInfo;
			this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));

			if (!QString(boardMaintInfo.LOT_MAP).contains(lot))
				continue;

			bib_qty += 1;

			bib_list << boardMaintInfo.BIB_ID;
			QStringList lot_map = QString(boardMaintInfo.LOT_MAP).split(",");

			if (nMaxDut == 0)
			{
				nMaxDut = file_map->GET_BIBX(slot, CPU00) * file_map->GET_BIBY(slot, CPU00);
				nMaxDutPerCpu = nMaxDut / MAX_SYS_CPU;
			}

			uint* pSortBinData = new uint[nMaxDut];

			if ( !file_map->GET_SORTBIN_DATA(slot, CPU00, pSortBinData, sizeof(int) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
				delete[] pSortBinData;
				return false;
			}

			if ( !file_map->GET_SORTBIN_DATA(slot, CPU01, pSortBinData + nMaxDutPerCpu, sizeof(int) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
				delete[] pSortBinData;
				return false;
			}

			stInitTestHead initTestHead[MAX_SYS_CPU];
			char* pInitFail = new char[nMaxDut];

			if ( !file_map->GET_INITTEST_DATA(slot, CPU00, initTestHead[CPU00], pInitFail, sizeof(char) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
				delete[] pInitFail;
				delete[] pSortBinData;
				return false;
			}

			if ( !file_map->GET_INITTEST_DATA(slot, CPU01, initTestHead[CPU01], pInitFail + nMaxDutPerCpu, sizeof(char) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
				delete[] pInitFail;
				delete[] pSortBinData;
				return false;
			}

			for (uint dut = 0; dut < nMaxDut; dut += 1)
			{
				if (lot_map[dut].compare(lot) != 0)
					continue;
				// change to comment for bin0 date - hsbae 20240815
				// if (pInitFail[dut])
					// continue;

				if (boardMaintInfo.INSERT_MOUNT_MAP[dut] == '1' && boardMaintInfo.INITIAL_MASK_MAP[i] == '1')
				{
					out_qty += 1;
					if (pInitFail[dut])
					{
						bin[0] += 1;
					}
					else
					{
						int bin_result = pSortBinData[dut];
						bin[bin_result] += 1;
					}
				}
			}
			delete[] pSortBinData;
			delete[] pInitFail;

		}

		QString bin_code("BIN0 BIN1 BIN2 BIN3 BIN4 BIN5 BIN6 BIN7 BIN8 BIN9 BIN10BIN11BIN12BIN13BIN14BIN15BIN16");

		QString bin_qty("");
		for (int i = 0 ; i <= MAX_SLOT; i += 1)
		{
			bin_qty += QStringLiteral("%1").arg(bin[i], 5, 10, QLatin1Char('0'));
		}

		uint bib_qty_of_tget = QString(tGet.BIB_QTY).toUInt();
		if (bib_qty != bib_qty_of_tget)
		{
			emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("BIB_QTY OF LOT [%1] IS NOT MATCH [CHECK : %2] [TGET : %3]").arg(lot).arg(bib_qty).arg(bib_qty_of_tget) );
			emit sigSetWorking(false);
			return false;
		}

		this->setVidValue(VID_TRANS_TYPE, "VMVOU");
		this->setVidValue(VID_LOT_NO, tGet.LOT_NO);
		this->setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);
		this->setVidValue(VID_IN_QTY, in_qty);
		this->setVidValue(VID_OUT_QTY, out_qty);
		this->setVidValue(VID_BIB_QTY, tGet.BIB_QTY);
		this->setVidValue(VID_BIN_CODE, bin_code);
		this->setVidValue(VID_BIN_QTY, bin_qty);
		this->setVidValue(VID_BIB_LIST, bib_list.join(""));
		this->setVidValue(VID_PG_FW_INFO, "");
		this->setVidValue(VID_DRV_FW_INFO, "");
		this->setVidValue(VID_OS_VER, tGet.OS_VERSION);
		this->setVidValue(VID_1RML_GRP_ID, tGet.RML_GRP_ID);
//		this->setVidValue(VID_MPG_FLAG, "Y");
		this->setVidValue(VID_MLB_FLAG, tGet.MLB_FLAG);
		this->setVidValue(VID_MLB_PGM_LOT_LIST, tGet.MLB_PGM_LOT_LIST);
		this->setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);

		if (!makeCEIDMessage(6600))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID5400byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		QString lot = this->test_info->getLot(i);

		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(lot)
									 .arg(EXT_RCMD);

		stRcmdTGET tGet;
		file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		QString sRcmdOpenLot = tr("%1/%2%3")
									 .arg(m_data_dir_path)
									 .arg(RCMD_BI_OPEN_LOT)
									 .arg(EXT_RCMD);
		stRcmdBiOpenLot biOpenLot;
		file_map->READ_FILE(sRcmdOpenLot.toStdString().c_str(), (char*)&biOpenLot, sizeof(stRcmdBiOpenLot));

		uint in_qty(0);
		uint out_qty(0);
		uint bib_qty(0);
		uint pass_qty(0);
		uint bin[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		QStringList bib_list;
		uint nMaxDut(0);
		uint nMaxDutPerCpu(0);

		for (int slot = 0; slot < MAX_SLOT; slot += 1)
		{
			if ( !(this->test_info->getValidSlot() & (0b1 << slot)) )
				continue;

			QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
										 .arg(m_data_dir_path)
										 .arg(BOARD_MAINT_DIR)
										 .arg(slot)
										 .arg(EXT_RCMD);

			stRcmdBoardMaintInfo boardMaintInfo;
			this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));

			if (!QString(boardMaintInfo.LOT_MAP).contains(lot))
				continue;

			bib_qty += 1;

			bib_list << boardMaintInfo.BIB_ID;
			QStringList lot_map = QString(boardMaintInfo.LOT_MAP).split(",");

			if (nMaxDut == 0)
			{
				nMaxDut = file_map->GET_BIBX(slot, CPU00) * file_map->GET_BIBY(slot, CPU00);
				nMaxDutPerCpu = nMaxDut / MAX_SYS_CPU;
			}

			uint* pSortBinData = new uint[nMaxDut];
			uint* pBinMap = new uint[16];

			if ( !file_map->GET_SORTBIN_DATA(slot, CPU00, pSortBinData, sizeof(int) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
				delete[] pSortBinData;
				delete[] pBinMap;
				return false;
			}

			if ( !file_map->GET_SORTBIN_DATA(slot, CPU01, pSortBinData + nMaxDutPerCpu, sizeof(int) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
				delete[] pSortBinData;
				delete[] pBinMap;
				return false;
			}
			if (!file_map->GET_BINMAP_DATA(slot, CPU00, pBinMap, sizeof(uint)*16))
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
				delete[] pSortBinData;
				delete[] pBinMap;
				return false;
			}

			stInitTestHead initTestHead[MAX_SYS_CPU];
			char* pInitFail = new char[nMaxDut];

			if ( !file_map->GET_INITTEST_DATA(slot, CPU00, initTestHead[CPU00], pInitFail, sizeof(char) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
				delete[] pInitFail;
				delete[] pSortBinData;
				delete[] pBinMap;
				return false;
			}

			if ( !file_map->GET_INITTEST_DATA(slot, CPU01, initTestHead[CPU01], pInitFail + nMaxDutPerCpu, sizeof(char) * nMaxDutPerCpu) )
			{
				qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
				delete[] pInitFail;
				delete[] pSortBinData;
				delete[] pBinMap;
				return false;
			}

			for (uint dut = 0; dut < nMaxDut; dut += 1)
			{
				if (lot_map[dut].compare(lot) != 0)
					continue;

				if (boardMaintInfo.INSERT_MOUNT_MAP[dut] == '1' && boardMaintInfo.INITIAL_MASK_MAP[i] == '1')
				{
					out_qty += 1;
					if (pInitFail[dut])
					{
						bin[0] += 1;
					}
					else
					{
						int bin_result = pSortBinData[dut];
						bin[bin_result] += 1;
					}
				}

				// change to comment for bin0 date - hsbae 20240815
				// if (pInitFail[dut])
					// continue;
//				out_qty += 1;
//				int bin_result = pBinMap[pSortBinData[dut]];
//				bin[bin_result] += 1;
			}
			delete[] pSortBinData;
			delete[] pBinMap;
			delete[] pInitFail;

		}

		QString bin_code("BIN0 BIN1 BIN2 BIN3 BIN4 BIN5 BIN6 BIN7 BIN8 BIN9 BIN10BIN11BIN12BIN13BIN14BIN15BIN16");

		QString bin_qty("");
		for (int i = 0 ; i <= MAX_SLOT; i += 1)
		{
			bin_qty += QStringLiteral("%1").arg(bin[i], 5, 10, QLatin1Char('0'));
		}

		uint bib_qty_of_tget = QString(tGet.BIB_QTY).toUInt();
		if (bib_qty != bib_qty_of_tget)
		{
			emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("BIB_QTY OF LOT [%1] IS NOT MATCH [CHECK : %2] [TGET : %3]").arg(lot).arg(bib_qty).arg(bib_qty_of_tget) );
			emit sigSetWorking(false);
			return false;
		}

		this->setVidValue(VID_TRANS_TYPE, "TMVOU");
		this->setVidValue(VID_LOT_NO, tGet.LOT_NO);
		this->setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);
		this->setVidValue(VID_IN_QTY, in_qty);
		this->setVidValue(VID_OUT_QTY, out_qty);
		this->setVidValue(VID_BIB_QTY, tGet.BIB_QTY);
		this->setVidValue(VID_BIN_CODE, bin_code);
		this->setVidValue(VID_BIN_QTY, bin_qty);
		this->setVidValue(VID_BIB_LIST, bib_list.join(""));	//2024.08.15 - KWMOON("," --> "" )
		this->setVidValue(VID_PG_FW_INFO, "");
		this->setVidValue(VID_DRV_FW_INFO, "");
		this->setVidValue(VID_OS_VER, tGet.OS_VERSION);
		this->setVidValue(VID_1RML_GRP_ID, tGet.RML_GRP_ID);
//		this->setVidValue(VID_MPG_FLAG, "Y");
		this->setVidValue(VID_MLB_FLAG, tGet.MLB_FLAG);
		this->setVidValue(VID_MLB_PGM_LOT_LIST, tGet.MLB_PGM_LOT_LIST);
		this->setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);

		if (!makeCEIDMessage(5400))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID8300byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(this->test_info->getLot(i))
									 .arg(EXT_RCMD);

		stRcmdTGET tGet;

		file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		uint nMaxDut(0);
		for (int slot = 0; slot < MAX_SLOT; slot += 1)
		{
			if ( !(this->test_info->getValidSlot() & (0b1 << slot)) )
				continue;
			else
			{
				nMaxDut = file_map->GET_BIBX(slot, CPU00) * file_map->GET_BIBY(slot, CPU00);
				break;
			}
		}

		setVidValue(VID_LOT_NO, tGet.LOT_NO);
		setVidValue(VID_P_CODE, "MCEND");
		setVidValue(VID_PGM_NO, tGet.TEST_PGM_NO);
		setVidValue(VID_SPECIAL_CODE, tGet.SPECIAL_CODE);
		setVidValue(VID_INITIAL_START_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_INITIAL_END_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_BEFORE_BI_TEST_START_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_BEFORE_BI_TEST_END_TIME, "");
		setVidValue(VID_BI_TEST_START_TIME, "");
		setVidValue(VID_BI_TEST_END_TIME, "");
		setVidValue(VID_AFTER_BI_TEST_START_TIME, "");
		setVidValue(VID_AFTER_BI_TEST_END_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_TEMP_UP_START_TIME_1, "");
		setVidValue(VID_TEMP_UP_END_TIME_1, "");
		setVidValue(VID_TEMP_UP_START_TIME_2, "");
		setVidValue(VID_TEMP_UP_END_TIME_2, "");
		setVidValue(VID_TEMP_DOWN_START_TIME_1, "");
		setVidValue(VID_TEMP_DOWN_END_TIME_1, "");
		setVidValue(VID_TEMP_DOWN_START_TIME_2, "");
		setVidValue(VID_TEMP_DOWN_END_TIME_2, "");
		setVidValue(VID_OTHERS_START_TIME, "");
		setVidValue(VID_OTHERS_END_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_BIB_TYPE_DENSITY_INFO, nMaxDut);
		setVidValue(VID_PG_FW_INFO, "");
		setVidValue(VID_DRV_FW_INFO, "");

		if (!makeCEIDMessage(8300))
			return false;
	}
	return true;
}


bool CHsmsManager::sendCEID5500byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		setVidValue(VID_LOT_NO, this->test_info->getLot(i));
		if (!makeCEIDMessage(5500))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID4400byLot()
{
	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
//		QString sRcmdTgetFile = tr("%1/%2/%3%4")
//									 .arg(m_data_dir_path)
//									 .arg(TGET_DIR)
//									 .arg(this->test_info->getLot(i))
//									 .arg(EXT_RCMD);

//		stRcmdTGET tGet;

//		file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		this->setVidValue(VID_LOT_NO, this->test_info->getLot(i));
//		this->setVidValue(VID_BIB_TYPE, tGet.BIB_TYPE);

		if (!makeCEIDMessage(4400))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID4200bySlot()
{
	for (int i = 0; i < MAX_SLOT; i += 1)
	{
//		qDebug() << m_stSlotLive.slot_status[i][CPU00];
//		qDebug() << m_stSlotLive.slot_status[i][CPU01];
//		qDebug() << EN_SLOT_ONLINE;
		if (m_stSlotLive.slot_status[i][CPU00] == EN_SLOT_ONLINE/* && m_stSlotLive.slot_status[i][CPU01] == EN_SLOT_ONLINE*/)
		{
			this->setVidValue(VID_SLOT_ID, i + 1);
			if (this->test_info->getValidSlot() & (0b1 << i))
			{
				QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
											 .arg(m_data_dir_path)
											 .arg(BOARD_MAINT_DIR)
											 .arg(i)
											 .arg(EXT_RCMD);

				stRcmdBoardMaintInfo boardMaintInfo;
				this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));

				this->setVidValue(VID_LOT_NO, boardMaintInfo.LOT_NO);
				this->setVidValue(VID_BIB_ID, boardMaintInfo.BIB_ID);
			}
			else
			{
				this->setVidValue(VID_LOT_NO, "");
				this->setVidValue(VID_BIB_ID, "");
			}
			this->setVidValue(VID_SITE_ID, "000000000");
			this->setVidValue(VID_SITE_FIRMWARE, "000001");
			this->setVidValue(VID_SITE_CPU_OS_VERSION, "01");
			this->setVidValue(VID_CAL_FLAG, "Y");
			this->setVidValue(VID_CAL_DATE, "19000101");

			if (!makeCEIDMessage(4200))
				return false;
		}
	}

	for (int i = 0; i < MAX_SLOT; i += 1)
	{
		if (this->test_info->getValidSlot() & (0b1 << i))
		{
			QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
										 .arg(m_data_dir_path)
										 .arg(BOARD_MAINT_DIR)
										 .arg(i)
										 .arg(EXT_RCMD);

			stRcmdBoardMaintInfo boardMaintInfo;
			this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));
			this->setVidValue(VID_LOT_NO, boardMaintInfo.LOT_NO);
			break;
		}
	}
	return true;
}

bool CHsmsManager::sendCEID3400byBib()
{
	for (int i = 0; i < MAX_SLOT; i += 1)
	{
		if ( !(this->test_info->getValidSlot() & (0b1 << i)) )
			continue;
		QString sBibId = this->file_map->GET_BIBID(i);
		this->setVidValue(VID_BIB_TYPE, sBibId.left(3));
		this->setVidValue(VID_BIB_ID, sBibId);
		this->setVidValue(VID_P_CODE, "SETUP");
		this->setVidValue(VID_SLOT_ID, i + 1);

//		this->test_info->enqueueSlotNumber(i);
		this->repaet_slot_list.enqueue(i);

		if (!makeCEIDMessage(3400))
			return false;
	}
	return true;
}

bool CHsmsManager::sendCEID3410byBib()
{
	for (int i = 0; i < MAX_SLOT; i += 1)
	{
		if ( !(this->test_info->getValidSlot() & (0b1 << i)) )
			continue;
		QString sBibId = this->file_map->GET_BIBID(i);
		this->setVidValue(VID_BIB_TYPE, sBibId.left(3));
		this->setVidValue(VID_BIB_ID, sBibId);
		this->setVidValue(VID_SLOT_ID, i + 1);

		if (!makeCEIDMessage(3410))
			return false;
	}
	return true;
}

void CHsmsManager::slotAutoInitTestStart()
{
	setVidValue(VID_INITIAL_START_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));

	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
	{
		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(this->test_info->getLot(i))
									 .arg(EXT_RCMD);

		stRcmdTGET tGet;
		this->file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));

		setVidValue(VID_LOT_NO, this->test_info->getLot(i));
		setVidValue(VID_PART_NO, QString(tGet.PART_NO));
		setVidValue(VID_TEMP_INIT_FLAG, "N");
		setVidValue(VID_FAMILY, tGet.FAMILY);
		setVidValue(VID_MLB_FLAG, tGet.MLB_FLAG);
		setVidValue(VID_MLB_PGM_LOT_LIST, tGet.MLB_PGM_LOT_LIST);

		CEID6100();
	}
	this->sent_ceid = 6100;
	this->test_info->recursive = 0x0;
}

void CHsmsManager::slotAutoInitTestDone()
{
//	bool b_need_retest(false);
//	QMap<QString, QPair<uint, uint> > lot_result;
//	for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
//		lot_result.insert(this->test_info->getLot(i), QPair<uint, uint>(0, 0));

	this->setVidValue(VID_INITIAL_END_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));

	QString sRcmdBoardTypeFile = tr("%1/%2%3")
								 .arg(m_data_dir_path)
								 .arg(RCMD_BOARD_TYPE)
								 .arg(EXT_RCMD);

	char* pReadBoardTypeInfo = new char[sizeof(stRcmdBoardType)];
	this->file_map->READ_FILE(sRcmdBoardTypeFile.toStdString().c_str(), pReadBoardTypeInfo, sizeof(stRcmdBoardType));

	stRcmdBoardType board_type_info_data;
	memcpy( (char*)&board_type_info_data, pReadBoardTypeInfo, sizeof(stRcmdBoardType) );
	delete[] pReadBoardTypeInfo;

	for (int i = 0; i < MAX_SLOT; i++) {
		uint slot = i;
		if (!(this->test_info->getValidSlot() & (0b1 << slot)))
			continue;
		QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
									 .arg(m_data_dir_path)
									 .arg(BOARD_MAINT_DIR)
									 .arg(slot)
									 .arg(EXT_RCMD);

		char* pReadBoardMaintInfo = new char[sizeof(stRcmdBoardMaintInfo)];
		this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), pReadBoardMaintInfo, sizeof(stRcmdBoardMaintInfo));

		stRcmdBoardMaintInfo board_maint_info_data;
		memcpy( (char*)&board_maint_info_data, pReadBoardMaintInfo, sizeof(stRcmdBoardMaintInfo));
		delete[] pReadBoardMaintInfo;

		//
		//Retrieve Data from File Map
		uint nMaxDut = file_map->GET_BIBX(slot, CPU00) * file_map->GET_BIBY(slot, CPU00);

		if ( nMaxDut < 1)
		{
			qDebug() << "[HsmsMgr][ERROR] MaxDut < 1";
			return ;
		}

		uint nMaxDutPerCpu = nMaxDut / MAX_SYS_CPU;

		stInitTestHead initTestHead[MAX_SYS_CPU];
		char* pInitFail = new char[nMaxDut];

		if ( !file_map->GET_INITTEST_DATA(slot, CPU00, initTestHead[CPU00], pInitFail, sizeof(char) * nMaxDutPerCpu) )
		{
			qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
			delete[] pInitFail;
			return ;
		}

		if ( !file_map->GET_INITTEST_DATA(slot, CPU01, initTestHead[CPU01], pInitFail + nMaxDutPerCpu, sizeof(char) * nMaxDutPerCpu) )
		{
			qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
			delete[] pInitFail;
			return ;
		}

		uint uInitTotalQty = 0;
		uint uInitPassQty = 0;
		uint uInitFailQty = 0;
		uint uInitMaskQty = 0;
		QString sInitialBinValue = "";

		for (uint i = 0; i < nMaxDut; i++)
		{
			if ( board_maint_info_data.INSERT_MOUNT_MAP[i] == 'x')
			{
				uInitMaskQty++;
				sInitialBinValue += 'x';
				continue;
			}

			if ( board_maint_info_data.INSERT_MOUNT_MAP[i] == '.')
			{
				sInitialBinValue += '.';
				continue;
			}

			if ( board_maint_info_data.INSERT_MOUNT_MAP[i] == '1' && board_maint_info_data.INITIAL_MASK_MAP[i] == '1' )
			{
//				lot_result.find(QString(board_maint_info_data.LOT_MAP[i])).value().second += 1;

				uInitTotalQty++;
				if ( pInitFail[i] )
				{
					sInitialBinValue += '0';
					uInitFailQty++;	
				}
				else
				{
					sInitialBinValue += '1';
					uInitPassQty++;
//					lot_result.find(QString(board_maint_info_data.LOT_MAP[i])).value().first += 1;
				}
			}
		}

		delete[] pInitFail;
		//

		this->setVidValue(VID_INSERT_EQP_NO, "");
		this->setVidValue(VID_LOT_NO, QString(board_maint_info_data.LOT_NO));
		this->setVidValue(VID_PART_NO, QString(board_maint_info_data.PART_NO));
		this->setVidValue(VID_BIB_TYPE, QString(board_maint_info_data.BIB_TYPE));
		this->setVidValue(VID_BIB_ID, QString(board_maint_info_data.BIB_ID));
		this->setVidValue(VID_SLOT_ID, slot + 1);
		this->setVidValue(VID_PGM_NO, QString(board_maint_info_data.TEST_PGM_NO));

		this->setVidValue(VID_IOC, 0);						// FIX
		this->setVidValue(VID_TEMP_INIT_FLAG, "N");			// FIX
		this->setVidValue(VID_INITIAL_TEST_TOTAL_COUNT, 1);	// FIX

		this->setVidValue(VID_INITIAL_TEST_DUT_QTY, uInitTotalQty);
		this->setVidValue(VID_INITIAL_TEST_PASS_QTY, uInitPassQty);
		this->setVidValue(VID_INITIAL_TEST_FAIL_QTY, uInitFailQty);
		this->setVidValue(VID_INITIAL_SOCKET_MASK_QTY, uInitMaskQty);

//		this->setVidValue(VID_INITIAL_START_TIME, 1);
		this->setVidValue(VID_INITIAL_BIN_VALUE, sInitialBinValue);

		this->setVidValue(VID_MLB_CODE, QString(board_maint_info_data.MLB_CODE));
		this->setVidValue(VID_MLB_LOT_LIST, QString(board_maint_info_data.MLB_LOT_LIST));
		this->setVidValue(VID_LOT_MAP, QString(board_maint_info_data.LOT_MAP));

		if ( !getInitTestResult(slot) )
		{
			qDebug() << "Failed to get InitTestResult";
			return;
		}

//		if (!b_need_retest)
//		{
//			if ( (static_cast<double>(uInitTotalQty) / nMaxDut) * 100 >= QString(board_type_info_data.MOUNT_RATE).toDouble() &&
//				 (static_cast<double>(uInitPassQty)/uInitTotalQty) * 100 < QString(board_type_info_data.BIB_INIT).toDouble() )
//			{
//				b_need_retest = true;
//			}
//		}
		emit sigUpdateAutoInitResult(slot, uInitPassQty, uInitTotalQty);
		makeCEIDMessage(6200);
	}
//	if (!b_need_retest)
//	{
//		for (int i = 0; i < this->test_info->getLotListCount(); i += 1)
//		{
//			QPair<uint, uint> lot_init_value_pair = lot_result.find(QString(this->test_info->getLot(i))).value();
//			if (lot_init_value_pair.second >= DEFAULT_LOT_YIELD_SKIP_QTY &&
//				static_cast<double>(lot_init_value_pair.first) / lot_init_value_pair.second * 100 < QString(board_type_info_data.LOT_INIT).toDouble())
//			{
//				b_need_retest = true;
//				break;
//			}
//		}
//	}

//	lot_result.clear();

//	if (b_need_retest && this->test_info->getInitRetestCount() > 0)
//	{
//		this->test_info->decreaseInitRetestCount();
//		this->sent_ceid = 7000;
//	}
//	else
//	{
		this->sent_ceid = 6200;
		this->slot_with_bib = 0x0;
//	}
}

void CHsmsManager::slotAutoTestStart()
{
	for (int i = 0; i < MAX_SLOT; i++) {
		uint slot = i;
		if (!(this->test_info->getValidSlot() & (0b1 << slot)))
			continue;

		QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
									 .arg(m_data_dir_path)
									 .arg(BOARD_MAINT_DIR)
									 .arg(slot)
									 .arg(EXT_RCMD);

		stRcmdBoardMaintInfo boardMaintInfo;
		this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));
		uint nMaxDut = file_map->GET_BIBX(slot, CPU00) * file_map->GET_BIBY(slot, CPU00);

		if ( nMaxDut < 1)
		{
			qDebug() << "[HsmsMgr][ERROR] MaxDut < 1";
			return ;
		}

		//Retrieve Data from File Map
		uint nMaxDutPerCpu = nMaxDut / MAX_SYS_CPU;

		stInitTestHead initTestHead[MAX_SYS_CPU];
		char* pInitFail = new char[nMaxDut];

		if ( !file_map->GET_INITTEST_DATA(slot, CPU00, initTestHead[CPU00], pInitFail, sizeof(char) * nMaxDutPerCpu) )
		{
			qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
			delete[] pInitFail;
			return ;
		}

		if ( !file_map->GET_INITTEST_DATA(slot, CPU01, initTestHead[CPU01], pInitFail + nMaxDutPerCpu, sizeof(char) * nMaxDutPerCpu) )
		{
			qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
			delete[] pInitFail;
			return ;
		}

		uint uInitTotalQty = 0;
		uint uInitPassQty = 0;
		uint uInitFailQty = 0;
		uint uInitMaskQty = 0;
		QString sInitialBinValue = "";

		for (uint i = 0; i < nMaxDut; i++)
		{
			if ( boardMaintInfo.INSERT_MOUNT_MAP[i] == 'x')
			{
				uInitMaskQty++;
				sInitialBinValue += 'x';
				continue;
			}

			if ( boardMaintInfo.INSERT_MOUNT_MAP[i] == '.')
			{
				sInitialBinValue += '.';
				continue;
			}

			if ( boardMaintInfo.INSERT_MOUNT_MAP[i] == '1' && boardMaintInfo.INITIAL_MASK_MAP[i] == '1' )
			{
				uInitTotalQty++;
				if ( pInitFail[i] )
				{
					sInitialBinValue += '0';
					uInitFailQty++;
				}
				else
				{
					sInitialBinValue += '1';
					uInitPassQty++;
				}
			}
		}

		delete[] pInitFail;

		QString sRcmdTgetFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(TGET_DIR)
									 .arg(boardMaintInfo.LOT_NO)
									 .arg(EXT_RCMD);

		stRcmdTGET tGet;
		this->file_map->READ_FILE(sRcmdTgetFile.toStdString().c_str(), (char*)&tGet, sizeof(stRcmdTGET));


		setVidValue(VID_BIB_TYPE, boardMaintInfo.BIB_TYPE);
		setVidValue(VID_BIB_ID, boardMaintInfo.BIB_ID);
		setVidValue(VID_P_CODE, "START");
		setVidValue(VID_SYSTEM_START_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_SYSTEM_INSERT_DEVICE_QTY, uInitPassQty);                //Q: Send PassQty or TotalQty
		setVidValue(VID_PART_NO, boardMaintInfo.PART_NO);
		setVidValue(VID_SLOT_ID, slot + 1);
		setVidValue(VID_LOT_NO, boardMaintInfo.LOT_NO);
		setVidValue(VID_SYSTEM_OP_ID, tGet.SYSTEM_OP_ID);
		setVidValue(VID_MLB_CODE, boardMaintInfo.MLB_CODE);


		CEID3500();
	}
	this->sent_ceid = 3500;
	this->slot_with_bib = 0x0;
}

//20240806 for test
void CHsmsManager::setAmbAfterTest()
{
	if (!m_pTimer[_TIMER_ID::_AMB]->isActive())
	{
		double temp_amb_sv = DEFAULT_CHMB_AMB_SV;
		short sAmbSv = static_cast<short>( temp_amb_sv * 10 );
		int nAmbSv = static_cast<int>(sAmbSv&0xFFFF);

		double temp_dry_sv = DEFAULT_CHMB_DRY_SV;
		short sDrySv = static_cast<short>( temp_dry_sv * 10 );
		int nDrySv = static_cast<int>(sDrySv&0xFFFF);

		if (m_stChamberStatus.dPV < 5)
		{
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE, nDrySv );
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER, 0x1 ); // hsbae - 2024.08.06

			m_pTimer[_TIMER_ID::_DRY]->start(CHECK_TIME_10MS);
			m_dtStart[_TIMER_ID::_DRY] = QDateTime::currentDateTime();
		}
		else
		{
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE, nAmbSv );
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER, 0x1 ); // hsbae - 2024.08.06
			m_pTimer[_TIMER_ID::_AMB]->start(CHECK_TIME_10MS);
			m_dtStart[_TIMER_ID::_AMB] = QDateTime::currentDateTime();
		}
	}
}

void CHsmsManager::slotAutoTestStartDone()
{
	setAmbAfterTest();

//	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER, 0x1 ); // hsbae - 2024.08.06

	this->setEventId(_EVENT_ID::_MCPRN);
//	makeCEIDMessage(3600);
//	this->slot_with_bib |= (0b1 << slot);

//	if (this->slot_with_bib == this->test_info->getValidSlot())
//	{
//		this->sent_ceid = 3600;
//		this->slot_with_bib = 0x0;
//	}
	for (int i = 0; i < MAX_SLOT; i++) {
		if (!(this->test_info->getValidSlot() & (0b1 << i)))
			continue;

		QString sRcmdMaintInfoFile = tr("%1/%2/SLOT%3%4")
									 .arg(m_data_dir_path)
									 .arg(BOARD_MAINT_DIR)
									 .arg(i)
									 .arg(EXT_RCMD);

		stRcmdBoardMaintInfo boardMaintInfo;
		this->file_map->READ_FILE(sRcmdMaintInfoFile.toStdString().c_str(), (char*)&boardMaintInfo, sizeof(stRcmdBoardMaintInfo));
		uint nMaxDut = file_map->GET_BIBX(i, CPU00) * file_map->GET_BIBY(i, CPU00);

		if ( nMaxDut < 1)
		{
			qDebug() << "[HsmsMgr][ERROR] MaxDut < 1";
			return ;
		}

		uint nMaxDutPerCpu = nMaxDut / MAX_SYS_CPU;

		stInitTestHead initTestHead[MAX_SYS_CPU];
		char* pInitFail = new char[nMaxDut];

		if ( !file_map->GET_INITTEST_DATA(i, CPU00, initTestHead[CPU00], pInitFail, sizeof(char) * nMaxDutPerCpu) )
		{
			emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("SLOT%1-%2 MAKE CHECKIN init fail data read error").arg(i+1).arg(CPU00) ) ;
			qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU00";
			delete[] pInitFail;
			return ;
		}

		if ( !file_map->GET_INITTEST_DATA(i, CPU01, initTestHead[CPU01], pInitFail + nMaxDutPerCpu, sizeof(char) * nMaxDutPerCpu) )
		{
			emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("SLOT%1-%2 MAKE CHECKIN init fail data read error").arg(i+1).arg(CPU00) ) ;
			qDebug() << "[HsmsMgr][ERROR] Failed GET_INITTEST_DATA CPU01";
			delete[] pInitFail;
			return ;
		}

		////////////////////////////////////////////////////////////////////////////////////
		/// 2024.08.15 - KWMOON( CUM SORT BIN )
		////////////////////////////////////////////////////////////////////////////////////
		uint* pSortBin    = new uint[nMaxDut];
		memset( pSortBin  , 0x0, sizeof(uint)*nMaxDut );
		for( int iCpu=0; iCpu<MAX_SYS_CPU; ++iCpu )
		{
			uint iOffset = iCpu*nMaxDutPerCpu;
			if( !file_map->GET_SORTBIN_DATA(i, iCpu, pSortBin + iOffset, sizeof(uint)*nMaxDutPerCpu) )
			{
				emit sigSysLogMessage( LOG_TYPE::_ERROR_LOG, tr("SLOT%1-%2 MAKE CHECKIN sortbin data read error").arg(i+1).arg(iCpu) ) ;
				delete[] pSortBin;
				return ;
			}
		}

		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////

		uint uInitTotalQty = 0;
		uint uInitPassQty = 0;
		uint uInitFailQty = 0;
		uint uInitMaskQty = 0;
		QString sInitialBinValue = "";
		QString sSystemBinValue = "";

		for (uint j = 0; j < nMaxDut; j++)
		{
			if ( boardMaintInfo.INSERT_MOUNT_MAP[j] == 'x')
			{
				uInitMaskQty++;
				sInitialBinValue += 'x';
				sSystemBinValue  += 'x';	// 2024.08.15 - KWMOON
				continue;
			}

			if ( boardMaintInfo.INSERT_MOUNT_MAP[j] == '.')
			{
				sInitialBinValue += '.';
				sSystemBinValue  += '.';	// 2024.08.15 - KWMOON
				continue;
			}

			if ( boardMaintInfo.INSERT_MOUNT_MAP[j] == '1' && boardMaintInfo.INITIAL_MASK_MAP[j] == '1' )
			{
				uInitTotalQty++;
				if ( pInitFail[j] )
				{
					sInitialBinValue += '0';
					sSystemBinValue  += '0';
					uInitFailQty++;
				}
				else
				{
					sInitialBinValue += '1';
					// 2024.08.15 - KWMOON modify
					if( pSortBin[j] != 0x1 )
					{
						sSystemBinValue  += '9';
						uInitFailQty++;
					}
					else
					{
						sSystemBinValue  += '1';
						uInitPassQty++;
					}
				}
			}
		}
		delete[] pInitFail;
		delete[] pSortBin;

		QString sRcmdBiOpenLotFile = tr("%1/%2/%3%4")
									 .arg(m_data_dir_path)
									 .arg(BI_OPEN_LOT_DIR)
									 .arg(boardMaintInfo.LOT_NO)
									 .arg(EXT_RCMD);

		stRcmdBiOpenLot biOpenLot;
		this->file_map->READ_FILE(sRcmdBiOpenLotFile.toStdString().c_str(), (char*)&biOpenLot, sizeof(stRcmdBiOpenLot));

		setVidValue(VID_BIB_TYPE, boardMaintInfo.BIB_TYPE);
		setVidValue(VID_BIB_ID, boardMaintInfo.BIB_ID);
		setVidValue(VID_BIB_X_SIZE, boardMaintInfo.BIB_X_SIZE);
		setVidValue(VID_BIB_Y_SIZE, boardMaintInfo.BIB_Y_SIZE);
		setVidValue(VID_CHECK_IN_BOARD_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
		setVidValue(VID_P_CODE, "MCEND");
		setVidValue(VID_IOC, 0);	// FIX
		setVidValue(VID_BOC, 0);	// FIX
		setVidValue(VID_BURN_IN_COUNT, boardMaintInfo.BURN_IN_COUNT); // FIX
		setVidValue(VID_PART_NO, boardMaintInfo.PART_NO);
		setVidValue(VID_LOT_NO, boardMaintInfo.LOT_NO);
		setVidValue(VID_SNO, biOpenLot.SNO);                                                                        //NEED TO RETRIEVE SNO
		setVidValue(VID_PGM_NO, boardMaintInfo.TEST_PGM_NO);
	//    setVidValue(VID_SYSTEM_START_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));  //NEED TO RETRIEVE TEST START TIME
		setVidValue(VID_SYSTEM_END_TIME, QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));      //NEED TO RETRIEVE TEST END TIME
		setVidValue(VID_SYSTEM_INSERT_DEVICE_QTY, uInitTotalQty);	// FIX                                        //PassQty or TotalQty
		setVidValue(VID_SLOT_ID, i + 1);
		setVidValue(VID_SYSTEM_BIN_MAP, sSystemBinValue);                                              //2024.08.15 - KWMOON
		setVidValue(VID_INITIAL_BIN_VALUE, sInitialBinValue);
		setVidValue(VID_TEST_TOTAL_QTY, uInitTotalQty);                                                 //NEED TO RETRIEVE TEST TOTAL QTY
		setVidValue(VID_TEST_PASS_QTY, uInitPassQty);                                                   //NEED TO RETRIEVE TEST PASS QTY
		setVidValue(VID_TEST_FAIL_QTY, uInitFailQty);                                                   //NEED TO RETRIEVE TEST FAIL QTY
		setVidValue(VID_MLB_CODE, boardMaintInfo.MLB_CODE);
		setVidValue(VID_MLB_LOT_LIST, boardMaintInfo.MLB_LOT_LIST);
		setVidValue(VID_LOT_MAP, boardMaintInfo.LOT_MAP);
		setVidValue(VID_FIRST_INITIAL_BIN_VALUE, sInitialBinValue);

		CEID3600();
	}
	this->slot_with_bib = 0x0;
	this->sent_ceid = 3600;
}


void CHsmsManager::slotSetRackIgnore(bool rack_ignore)
{
	this->b_rack_ignore = rack_ignore;
}

void CHsmsManager::slotReadyToLoad()
{
	qDebug() << __func__;
	this->makeCEIDMessage(5110);
}

void CHsmsManager::slotLoadComplete()
{
	qDebug() << __func__;
	if ( this->m_pTimer[_TIMER_ID::_LOAD_CMPLETE]->isActive() )
	{
		this->m_pTimer[_TIMER_ID::_LOAD_CMPLETE]->stop();
	}
	this->makeCEIDMessage(5120);
}

//2024.07.23 - KWMOON
void  CHsmsManager::slotBibIdReadDone()
{
	if (this->checkBibType(this->test_info->getValidSlot()) )
		this->SetEventId(_EVENT_ID::_READY);
}

void CHsmsManager::slotSendMessage(CSecsMsg *secs_msg)
{
	QByteArray send_data;
	this->secs_gem->GetMsgHSMS(secs_msg, send_data);

	stIpcElement ipc_element;
	ipc_element.ipcHeader.cmd = PROTOCOL::CMD_TMSUI_WITH_MAIN_MSG;
	ipc_element.ipcHeader.dest = PROTOCOL::_ATTMS_UI;
	ipc_element.ipcHeader.src = PROTOCOL::_ATMAIN_UI;
	ipc_element.ipcHeader.error = 0;
	ipc_element.ipcHeader.slot = 0;
	ipc_element.ipcHeader.runner = 0;
	ipc_element.ipcHeader.datalen = send_data.length();

	char* all_data = new char[send_data.length() + sizeof(stIpcHeader)];
	memcpy(all_data, &ipc_element, sizeof(stIpcHeader));
	memcpy(all_data + sizeof(stIpcHeader), send_data, send_data.length());

	emit sigSendToTmsUi(all_data, send_data.length() + sizeof(stIpcHeader));

	short wait = 0x0;
	short stream = 0x0;
	short function = 0x0;
	uint uSysbyte = 0;
	secs_msg->GetMsgInfo(stream, function, wait);
	this->secs_gem->GetSystemByte( secs_msg, uSysbyte );
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[SECS][SEND]S%1F%2 %3 Systembyte:%4").arg(stream).arg(function) .arg(wait?"W":"S").arg(uSysbyte) );
//	if (secs_msg)
//	{
//		delete secs_msg;
//		secs_msg = nullptr;
//	}
}

void CHsmsManager::slotQueueMonitoring()
{
	emit sigQueueMonitoring();
}

void CHsmsManager::slotSendNextCeidMessage()
{
	this->sendNextCeidMessage();
}

//2024.08.05 - KWMOON
void CHsmsManager::slotAutoManualSetupDone( bool setup_result, int slot_bits )
{
	if( !setup_result )
	{
		//error
	}
	else
	{
		if (this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->isActive())
			this->m_pTimer[_TIMER_ID::_READY_TO_LOAD]->stop();

		emit sigSetWorking(true);

		this->test_info->clearTestInfoData();
		this->test_info->setValidSlot( slot_bits );
		this->setSlotWithBIBIni();

		emit sigToSlotViewUpdateSetup();

		uint valid_slot_count = 0;
		for (int i = 0; i < MAX_SLOT; i += 1)
		{
			if (this->test_info->getValidSlot() & (0b1 << i))
				valid_slot_count += 1;
		}
		this->test_info->setValidSlotCount(valid_slot_count);

		this->setVidValue(VID_MPG_FLAG, "N");
		this->setVidValue(VID_MLB_EQ_FLAG, "Y");
		this->setVidValue(VID_OS_VERSION, "2024_07_25");


		memset(this->unload_check, false, sizeof(this->unload_check));

		this->setStatusId(_STATUS_ID::_RUN);
		if (!m_stChamberStatus.bRackInComplete)
		{
			setChamberRackIn();
		}
		else
		{
			this->checkBibType(slot_bits);
			this->setEventId(_EVENT_ID::_READY, true);
		}
	}
}

void CHsmsManager::slotMakeReportDone(bool done)
{
	this->unload_check[_PRE_UNLOAD_CHECK::_REPORT_DONE] = done;
}

//2024.08.14 - KWMOON
void CHsmsManager::slotChamberTempMonSet( stGetTemp temp )
{
	double sv = temp.dSv;
	double pv = temp.dPv;
	int step_no = temp.nAck;
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][HSMS] TEMP_MON_SET STEP_NO[%1] SV[%2] PV[%3]").arg(step_no).arg(sv).arg(pv) );
}

//2024.08.14 - KWMOON
void CHsmsManager::slotChamberTempMonEnd( stGetTemp temp )
{
	double sv = temp.dSv;
	double pv = temp.dPv;
	int step_no = temp.nAck;
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][HSMS] TEMP_MON_END STEP_NO[%1] SV[%2] PV[%3]").arg(step_no).arg(sv).arg(pv) );
}

void CHsmsManager::SetInterruptMode(bool bInterrupt)
{
	this->m_bInterruptMode = bInterrupt;
	if( bInterrupt )
		emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][HSMS] UI MANUAL ON") );
	else
		emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][HSMS] UI AUTO ON") );
}

//2024.08.19 - KWMOON
bool CHsmsManager::setChamberTowerLampRemote()
{
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_LAMP_MASK_ONOFF, 0x0 );		//remote:0, local:1
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP REMOTE-ON"));
	return true;
}

//2024.08.19 - KWMOON
bool CHsmsManager::setChamberTowerLampLocal()
{
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_LAMP_MASK_ONOFF, 0x1 );		//remote:0, local:1
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP REMOTE-OFF"));
	return true;
}

//2024.08.19 - KWMOON
bool CHsmsManager::setChamberTowerLampOn(_TOWERLAMP_COLOR tcolor)
{
	switch( tcolor )
	{
		case _TOWERLAMP_COLOR::_GREEN:
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP GREEN-ON"));
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_ON , CHMB_TOWERLAMP_ON );
			break;
		case _TOWERLAMP_COLOR::_YELLOW:
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP YELLOW-ON"));
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_ON, CHMB_TOWERLAMP_ON );
			break;
		case _TOWERLAMP_COLOR::_RED:
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP RED-ON"));
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_ON   , CHMB_TOWERLAMP_ON );
			break;
	}

	return true;
}

//2024.08.19 - KWMOON
bool CHsmsManager::setChamberTowerLampBlink(_TOWERLAMP_COLOR tcolor)
{
	switch( tcolor )
	{
		case _TOWERLAMP_COLOR::_GREEN:
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP BLINK GREEN-ON"));
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_BLINK , CHMB_TOWERLAMP_ON );
			break;
		case _TOWERLAMP_COLOR::_YELLOW:
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP BLINK YELLOW-ON"));
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_BLINK, CHMB_TOWERLAMP_ON );
			break;
		case _TOWERLAMP_COLOR::_RED:
			emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP BLINK RED-ON"));
			emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_BLINK   , CHMB_TOWERLAMP_ON );
			break;
	}

	return true;
}

//2024.08.19 - KWMOON
bool CHsmsManager::setChamberTowerLampOff()
{
	emit sigChmbCtrolToChmbMgr( PROTOCOL::PLC_COMMAND::CMD_WB_ALL_LAMP_OFF, 0x1 );
	emit sigSysLogMessage( LOG_TYPE::_NORMAL_LOG, tr("[INFO][CHMB]TOWER LAMP OFF"));
	return true;
}
