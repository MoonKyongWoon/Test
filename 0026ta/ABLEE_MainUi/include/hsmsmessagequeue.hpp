#ifndef CHSMSMESSAGEQUEUE_HPP
#define CHSMSMESSAGEQUEUE_HPP

#include <QObject>
#include <QQueue>
#include <QVector>
#include <QTimer>
#include <QSharedMemory>
#include "secsgem.hpp"

enum MESSAGE_TYPE {
	SEND = 0,
	RECV	,
};

struct MessageBlock {
	bool			process;
	MESSAGE_TYPE	message_type;
	CSecsMsg*		secs_msg;
	uint			system_byte;
};

struct MessageQueueItem {
	bool					process;
	uint					ceid;
	int						block_count;
	QVector<MessageBlock*>	message_block;
};

class CHsmsMessageQueue : public QObject
{
	Q_OBJECT
private:
	QQueue<MessageQueueItem*>	message_queue;
	CSecsGem					*secs_gem;
	QTimer						*queue_timer;
	QSharedMemory				*m_pShmHsms;
	bool						is_working;

private:
	void initVariables();
	void initSharedMemory();
	void increaseSystemByteInSharedMemory();
	void clearMessageQueue();
	void addNewMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid);
	void addMessageBlock(CSecsMsg* secs_msg, bool is_send, uint ceid);
	void addMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid);
	void checkMessageBlockComplete();
	void removeFirstItem();
	void queueMonitoring();

public:
	explicit CHsmsMessageQueue(CSecsGem* secs_gem, QObject *parent = nullptr);
	~CHsmsMessageQueue();
	void ClearMessageQueue();
	void AddNewMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid);
	void AddMessageBlock(CSecsMsg* secs_msg, bool is_send, uint ceid);
	void AddMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid);

signals:
	void sigSendMessage(CSecsMsg* secs_msg);
	void sigSendNextCeidMessage();

public slots:
	void slotAddNewMessageBlock(CSecsMsg* secs_msg, int block_count, bool is_send, uint ceid);
	void slotAddMessageBlock(CSecsMsg* secs_msg, bool is_send, uint ceid);
	void slotQueueMonitoring();
	void slotSetWorking(bool is_work);
};

#endif // CHSMSMESSAGEQUEUE_HPP
