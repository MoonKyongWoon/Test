#ifndef CRS485_HPP
#define CRS485_HPP

#include <QObject>
#include <chrono>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <asm/ioctls.h>
#include <errno.h>
#include <termios.h>
#include "pmbus.hpp"
#include "syslog.hpp"

#define US	* 1.0e-6
#define MS	* 1.0e-3
#define  S	* 1.0


class CRS485 : public QObject
{
	Q_OBJECT
public:
	explicit CRS485(SIDE_AB ch, QObject *parent = nullptr);
    ~CRS485();

public:
	void SetSerialInfo( QString );	//port,baud
	bool OpenDevice();
	bool CloseDevice();
	bool IsOpen();
    //2023.03.31 - kwmoon
    bool OpenRs485();
	long long int Delay(double dTime);
public:
	bool SendData( char*, int );
	bool RecvData( char*, int );
    int  ReadData( char* );
    void SetRTS(uint);
public slots:
	void SlotOpenDevice();
	void SlotCloseDevice();

signals:
	void SigLogMessage( SIDE_AB, LOG_TYPE, QString );
private:
	SIDE_AB m_ch;
	QString m_stty;
	bool m_bOpen;
	bool m_bDebug;
private:
	struct termios m_oldtio, m_newtio;
	int m_rs485;

};

#endif // CRS485_HPP
