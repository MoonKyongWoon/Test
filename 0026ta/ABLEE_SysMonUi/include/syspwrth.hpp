#ifndef CSYSPWRTH_HPP
#define CSYSPWRTH_HPP

#include <QObject>
#include <QThread>
#include <math.h>
#include "rs485.hpp"

class CSysPwrTh : public QObject
{
	Q_OBJECT
public:
    explicit CSysPwrTh(SIDE_AB, std::map<QString, stPmubusCommand>&, std::map<QString, stPmubusCommand>&, QObject *parent = nullptr);
	~CSysPwrTh();
public:
	bool IsOpen();
	void SetSerialInfo(QString);
    long long int Delay(double dTime);
public slots:
	void SlotOpenDevice();
	void SlotCloseDevice();
	void SlotManualTest(stUiRequest);
	void SlotTextTest(SIDE_AB, QString);
    void SlotMonitoring(SIDE_AB);
signals:
	void SigLogMessage( SIDE_AB, LOG_TYPE, QString );
    void SigOpenDeviceResult( SIDE_AB, bool);
    void SigIsoComm( SIDE_AB, stLineMeas );
    void SigModules( SIDE_AB, stModulelMeas );
private:
	void CreateRs485();
	void BuildPMBusRequest( int, PMBUS_OPENRATION, DEVICE_ADDRESS, int );
	bool PMBusWrite( char*, int );
	uchar PMBusRead( uchar, int, char* );
	bool SerialRead();
	uchar IsValidResponse();
	QString ErrorCodeString(uchar ErrCode);
	double L11ToDouble(uint16_t input_val);
	uint16_t DoubleToL11(double input_val);
private:
    bool GetIsoComm();
    bool GetModules();
private:
	SIDE_AB m_ch;
	QString m_stty;
    std::map<QString, stPmubusCommand>& m_mapPbCmdIsoComm;
    std::map<QString, stPmubusCommand>& m_mapPbCmdModule;
    CRS485* m_pRs485;	//2ea
private:
	stUiRequest m_stUiRequest;
	stPmbusReqHead m_stPmbusReqHead;
	stPmbusResponse m_stPmbusResponse;
	char m_szRecvBuf[64];
};

#endif // CSYSPWRTH_HPP
