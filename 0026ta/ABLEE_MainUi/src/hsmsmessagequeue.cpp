#include "hsmsmessagequeue.hpp"

CHsmsMessageQueue::CHsmsMessageQueue(CSecsGem* secs_gem, QObject *parent) : QObject(parent)
{
	this->secs_gem = secs_gem;
	this->initVariables();
	this->initSharedMemory();
}

CHsmsMessageQueue::~CHsmsMessageQueue()
{
	this->clearMessageQueue();
	if (this->queue_timer)
	{
		if (this->queue_timer->isActive())
			this->queue_timer->stop();
		this->queue_timer->deleteLater();
	}
}

void CHsmsMessageQueue::initVariables()
{
	this->is_working = true;
	this->message_queue.clear();
	this->queue_timer = new QTimer();
	connect(this->queue_timer,	&QTimer::timeout,	this,	&CHsmsMessageQueue::slotQueueMonitoring);
	this->queue_timer->start(1000);
}

void CHsmsMessageQueue::initSharedMemory()
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

void CHsmsMessageQueue::increaseSystemByteInSharedMemory()
{
	stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());

	this->m_pShmHsms->lock();
	if (shm_data->system_byte == 0xffffffff)
		shm_data->system_byte = 0;
	else
		shm_data->system_byte += 1;
	this->m_pShmHsms->unlock();
}

void CHsmsMessageQueue::clearMessageQueue()
{
	while (!this->message_queue.isEmpty())
	{
		MessageQueueItem* message_queue_item = this->message_queue.dequeue();

//		QVector<MessageBlock*>::iterator iter = message_queue_item->message_block.begin();

//		for (MessageBlock* message_block_item: message_queue_item->message_block)
//		{
//			if (message_block_item->secs_msg)
//			{
//				delete[] message_block_item->secs_msg;
//				message_block_item->secs_msg = nullptr;
//			}
//		}
		for (MessageBlock* block: message_queue_item->message_block)
		{
			if (block->secs_msg)
				delete block->secs_msg;
			delete block;
		}
		message_queue_item->message_block.clear();
		delete message_queue_item;
	}
	this->message_queue.clear();
}

void CHsmsMessageQueue::addNewMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid)
{
	if (!this->is_working)
		return ;
	uint system_byte(0);

	if (is_send)
	{
		short stream(0);
		short function(0);
		short wait(0);
		this->secs_gem->GetMsgInfo(secs_msg, stream, function, wait);

		if (function % 2 == 1)
		{
			increaseSystemByteInSharedMemory();
			stShmHsms* shm_data = static_cast<stShmHsms*>(this->m_pShmHsms->data());
			system_byte = shm_data->system_byte;
			this->secs_gem->SetSystemByte(secs_msg, system_byte);
		}
		else
		{
			this->secs_gem->GetSystemByte(secs_msg, system_byte);
		}
	}
	else
	{
		this->secs_gem->GetSystemByte(secs_msg, system_byte);
	}


	MessageBlock* message_block_item = new MessageBlock;
	message_block_item->process = ( is_send ? false : true );
	message_block_item->message_type = ( is_send ? MESSAGE_TYPE::SEND : MESSAGE_TYPE::RECV );
	message_block_item->secs_msg = secs_msg;
	message_block_item->system_byte = system_byte;

	MessageQueueItem* message_queue_item = new MessageQueueItem;
	message_queue_item->process = false;
	message_queue_item->ceid = ceid;
	message_queue_item->block_count = block_count;
	message_queue_item->message_block.push_back(message_block_item);

	this->message_queue.enqueue(message_queue_item);
}

void CHsmsMessageQueue::addMessageBlock(CSecsMsg *secs_msg, bool is_send, uint ceid)
{
	if (!this->is_working)
		return ;

	uint system_byte(0);
	this->secs_gem->GetSystemByte(secs_msg, system_byte);

	MessageBlock* message_block_item = new MessageBlock;
	message_block_item->process = ( is_send ? false : true );
	message_block_item->message_type = ( is_send ? MESSAGE_TYPE::SEND : MESSAGE_TYPE::RECV );
	message_block_item->secs_msg = secs_msg;
	message_block_item->system_byte = system_byte;

	for (auto message_queue_item: this->message_queue)
	{
		uint data_order = message_queue_item->message_block.length();

		if (data_order % 2 == 1)
		{
			if (message_queue_item->message_block[data_order - 1]->system_byte == system_byte)
			{
				message_queue_item->message_block.push_back(message_block_item);
				return ;
			}
		}
		else
		{
			if (message_queue_item->ceid == ceid)
			{
				message_queue_item->message_block.push_back(message_block_item);
				return ;
			}
		}
	}
}

void CHsmsMessageQueue::addMessageBlock(CSecsMsg *secs_msg, int block_count, bool is_send, uint ceid)
{
	if (!this->is_working)
		return ;

	uint system_byte(0);
	this->secs_gem->GetSystemByte(secs_msg, system_byte);

	MessageBlock* message_block_item = new MessageBlock;
	message_block_item->process = ( is_send ? false : true );
	message_block_item->message_type = ( is_send ? MESSAGE_TYPE::SEND : MESSAGE_TYPE::RECV );
	message_block_item->secs_msg = secs_msg;
	message_block_item->system_byte = system_byte;

	for (auto message_queue_item: this->message_queue)
	{
		uint data_order = message_queue_item->message_block.length();

		if (data_order % 2 == 1)
		{
			if (message_queue_item->message_block[data_order - 1]->system_byte == system_byte)
			{
				message_queue_item->message_block.push_back(message_block_item);
				return ;
			}
		}
		else
		{
			if (message_queue_item->ceid == ceid)
			{
				message_queue_item->message_block.push_back(message_block_item);
				return ;
			}
		}
	}

	MessageQueueItem* message_queue_item = new MessageQueueItem;
	message_queue_item->process = false;
	message_queue_item->ceid = ceid;
	message_queue_item->block_count = block_count;
	message_queue_item->message_block.push_back(message_block_item);
}


void CHsmsMessageQueue::checkMessageBlockComplete()
{
	if (this->message_queue.empty())
		return;
	if (this->message_queue.head()->process)
		removeFirstItem();
}

void CHsmsMessageQueue::removeFirstItem()
{
	if (this->message_queue.isEmpty())
		return;
	MessageQueueItem* message_queue_item = this->message_queue.dequeue();
	for (MessageBlock* message_block_item: message_queue_item->message_block)
	{
//		if (message_block_item->secs_msg)
//		{
//			delete[] message_block_item->secs_msg;
//			message_block_item->secs_msg = nullptr;
//		}
		delete[] message_block_item;
	}
	message_queue_item->message_block.clear();
	delete message_queue_item;
}

void CHsmsMessageQueue::queueMonitoring()
{
	if (this->message_queue.isEmpty())
		return ;
	if (!this->is_working)
	{
		this->clearMessageQueue();
		return ;
	}
	MessageQueueItem* message_queue_item = this->message_queue.head();

	for (MessageBlock* message_block_item: message_queue_item->message_block)
	{
		if ( (!message_block_item->process) && (message_block_item->message_type == MESSAGE_TYPE::SEND) )
		{
			message_block_item->process = true;
			emit sigSendMessage(message_block_item->secs_msg);

			break;
		}
	}

	if (message_queue_item->block_count == message_queue_item->message_block.count())
	{
		for (int i = 0; i < message_queue_item->block_count; i++)
		{
			if (message_queue_item->message_block[i]->process == false)
				return ;
		}
		removeFirstItem();
		if (this->message_queue.isEmpty())
			emit sigSendNextCeidMessage();
	}
}

void CHsmsMessageQueue::ClearMessageQueue()
{
	this->clearMessageQueue();
}

void CHsmsMessageQueue::AddNewMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid)
{
	this->addNewMessageBlock(secs_msg, block_count, is_send, ceid);
//	queueMonitoring();
}

void CHsmsMessageQueue::AddMessageBlock(CSecsMsg* secs_msg, bool is_send, uint ceid)
{
	this->addMessageBlock(secs_msg, is_send, ceid);
//	queueMonitoring();
}

void CHsmsMessageQueue::AddMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid)
{
	this->addMessageBlock(secs_msg, block_count, is_send, ceid);
//	queueMonitoring();
}

void CHsmsMessageQueue::slotSetWorking(bool is_work)
{
	this->is_working = is_work;
}

void CHsmsMessageQueue::slotAddNewMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid)
{
	this->addNewMessageBlock(secs_msg, block_count, is_send, ceid);
//	queueMonitoring();
}

void CHsmsMessageQueue::slotAddMessageBlock(CSecsMsg* secs_msg, bool is_send, uint ceid)
{
	this->addMessageBlock(secs_msg, is_send, ceid);
//	queueMonitoring();
}

void CHsmsMessageQueue::slotQueueMonitoring()
{
	queueMonitoring();
}
