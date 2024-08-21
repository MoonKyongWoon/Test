#ifndef CCHAMBERMGR_HPP
#define CCHAMBERMGR_HPP

#include <QObject>
#include <QTimer>
#include <QThread>

#include "common.hpp"
#include "commstruct.hpp"
#include "localsocket.hpp"
#include "syslog.hpp"
#include "sysver.hpp"
#include "protocol.hpp"

enum _TIMER_CHMB_VALUE
{
	_TIME_CHMB_COMM        = 5000,
	_TIME_CHMB_STAT_D100   = 2000,
	_TIME_CHMB_STAT_D101   = 2500,
	_TIME_CHMB_TEMPERATURE = 3000,
};

enum _CHMB_CHECK_ITEM
{
	_ITEM_IPC_CONN      = 0x0,
	_ITEM_PLC_CONN           ,
	_ITEM_RACK_DETECT        ,
	_ITEM_RACK_INOUT         ,
	_ITEM_DOOR_OPEN_CLOSE    ,
	_ITEM_CHECK_MAX          ,
};

class CChamberMgr : public QObject
{
	Q_OBJECT
public:
	explicit CChamberMgr(QObject *parent = nullptr);
	~CChamberMgr();

private:
	void InitInstance();
	bool GetChmbStatus   ( PROTOCOL::PLC_COMMAND );
	bool SetChmbBitWrite ( PROTOCOL::PLC_COMMAND, int );
	bool SetChmbWordWrite( PROTOCOL::PLC_COMMAND, int );
signals:
	void SigControllerChmbMsg(LOG_TYPE, QString);
	void SigIpcChmbCommConnect();
	void SigIpcChmbCommSendPacket( char* pData, uint size );
	void SigToMainuiChmbControlRes(PROTOCOL::PLC_COMMAND, bool);
	void SigToMainuiChmbStatus(stChamberStatus);
	void SigToMainuiLogMessage( LOG_TYPE, QString );
public slots:
	void SlotStartMgr(bool);
	void SlotIpcChmbCommLogMessage (LOG_TYPE, QString);
	void SlotIpcChmbCommError ( QLocalSocket::LocalSocketError, QString );
	void SlotIpcChmbCommConnStatus(bool);
	void SlotIpcChmbCommRecvPacket( stIpcElement );
	void SlotTimeoutIpcCon();
	void SlotTimeoutStatusD100();
	void SlotTimeoutStatusD101();
	void SlotTimeoutTemperature();
	void SlotFromMainuiChmbControl( PROTOCOL::PLC_COMMAND, int );
private:
	bool m_bAutoMode;
	bool m_bConnIpc;
	bool m_bConnPlc;
	stChamberStatus m_stChamberStatus;
	CLocalSocket* m_pIpcChmb;
	QTimer* m_tmrIpcCon;
	QTimer* m_tmrStatusD100;
	QTimer* m_tmrStatusD101;
	QTimer* m_tmrTemperature;
};

#endif // CCHMBDLGCONTROLLER_HPP
