#ifndef CDEVICEINSTANCE_HPP
#define CDEVICEINSTANCE_HPP

#include <QDir>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QTcpSocket>
#include <iostream>

#include "api.hpp"
#include "commstruct.hpp"
#include "elementqueue.hpp"
#include "protocol.hpp"
#include "sysdef.hpp"
#include "util.hpp"
#include "tcpsocket.hpp"

enum EN_EXPECT_CHIP
{
	CHIP1_IOA_VER   = 0x3,
	CHIP2_IOA_VER   = 0x5,
};

class CDeviceInstance : public QObject
{
    Q_OBJECT
public:
    CDeviceInstance();
    ~CDeviceInstance();
	void SetProgramName(std::string);	//2023.05.03 - kwmoon
    void SetDeviceIf(CApi*);
    void SetStop(bool);
	bool IsStopMode();
    void Menu();
	void InitTest();
	void TestEntry();
	void SetTerminate();	//2023.04.03 - KWMOON
	bool WaitTerminated();	//2023.04.03 - KWMOON
signals:
	void SigOpenDeviceResult(int,int,bool);
	void SigPcieValidCheckResult(int,int,int,bool);
	void SigConnectToHostResult( int, int, bool );
public slots:
	void SlotOpenDevice(int,int);
	void SlotPcieValidCheck(int,int);
	void SlotConnectToHost( int,int, bool );
	void SlotInitProgram();
	void SlotInitTest( stUiInitInfo );
	void SlotTest(stUiTestInfo);
	void SlotJumpTest( stTestItem );
	void SlotStopTest();
    void SlotManualRun();
private:
    CApi* m_pApi;
	bool m_bOpen;
	bool m_bToolPgm;
};

#endif
