#ifndef CYOKOGAWAPLC_HPP
#define CYOKOGAWAPLC_HPP

#include <QObject>
#include <QThread>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QTcpSocket>

#include "commstruct.hpp"
#include "protocol.hpp"
#include "syslog.hpp"
#include "sysini.hpp"
#include "util.hpp"

#define PLC_CPU						( 1)
#define PLC_MAX_ADDR_DATA			(32)

typedef struct _stExitErrorCode
{
	PLC_EXIT_ERROR_CODE code;
	QString             description;
}stExitErrorCode;


class CYokogawaPlc : public QObject
{
	Q_OBJECT
public:
	explicit CYokogawaPlc(QObject *parent = nullptr);
	~CYokogawaPlc();
public:
	void SetPlcTcpInfo(QString, uint);
	bool PlcConnect();
	bool IsPlcConn();
	bool PlcClose() ;
	void PlcSocketControl( PLC_ON_OFFLINE );
public:
	void SetMsgLoopMode(bool);		//2023.05.20 - KWMOON
	void SetChamberTempLimit(double,double);
//Instance Mode
private:
	stPlcResponse ResponseCmd(PROTOCOL::PLC_COMMAND cmd, YKG_CMD_LIST plc_cmd, PLC_RESPONSE_TYPE bData, bool bPass, int* Data = nullptr, int nDataLen = 1);
public:
	bool RequestCmd ( PROTOCOL::PLC_COMMAND cmd, stPlcResponse&, int data, uint len );
	QString GetErrMsg();
private:
	void CreateYkgCommandMap();
	void CreateYkgExitErrorCodeMap();
	PLC_ADDRESS_MAP GetPlcCmdToAddr(PROTOCOL::PLC_COMMAND);
	QString GetPlcAddrToString(PLC_ADDRESS_MAP);
	bool MakePlcFormatCommand( YKG_CMD_LIST, QString* sAddr, int* nData, int nLen=1 );
	PLC_COMM_RESULT_CODE DoSendLinePacket(QByteArray );
	PLC_COMM_RESULT_CODE DoRecvLinePacket(QByteArray&);
	PLC_EXIT_ERROR_CODE DoChamberReceiveWriteAck( YKG_CMD_LIST );
	PLC_EXIT_ERROR_CODE DoChamberReceiveReadAck ( YKG_CMD_LIST, int*, int );
	void DoChamberResponseEmit( uint, PROTOCOL::PLC_COMMAND cmd, YKG_CMD_LIST plc_cmd, PLC_RESPONSE_TYPE bData, bool bPass, int* Data = nullptr, int nDataLen = 1 );

signals:
	void SigPlcConnStatus( PLC_ON_OFFLINE );
	void SigPlcLogMessage( LOG_TYPE, QString );
	void SigPlcChmbResponse( uint, stPlcResponse );
public slots:
	void SlotPlcDisconnected();
	void SlotPlcSocketControl( PLC_ON_OFFLINE );
	void SlotPlcControlByEmt( uint, stIpcElement );
private:
	QMap< YKG_CMD_LIST, QString > m_mapCmd;
	QMap< QString, stExitErrorCode > m_mapExitErrCode;
	QByteArray m_SendByteArray;
	double m_dTempLimitLow;
	double m_dTempLimitHigh;
private:
	bool m_bMsgLoop;
	QTcpSocket* m_pSocket;
	QString m_sIp;
	quint16 m_uPort;
	bool m_bPlcCon;
	QString m_sErrMsg;
};

#endif // CYOKOGAWAPLC_HPP
