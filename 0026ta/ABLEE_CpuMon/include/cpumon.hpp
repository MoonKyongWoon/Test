#ifndef CSYSPWR_HPP
#define CSYSPWR_HPP

#include <QObject>
#include <QThread>
#include "monitor.hpp"
#include "logger.hpp"

#include "./../ABLEE_Common/tcp/tcpserver.hpp"
#include "./../ABLEE_Common/tcp/tcpclient.hpp"
#include "./../ABLEE_Common/tool/util.hpp"

class CCpuMon : public QObject
{
	Q_OBJECT
public:
	explicit CCpuMon(QObject *parent = nullptr);
	~CCpuMon();
	void CreateInstance();
	void Start();
	void PRINTF(LOG_TYPE,QString);
	void initIniFile();
	bool runTcpServer();
	bool AckVersion();
	void setSlotCpuInfo(stTcpElement& tcp_element);
signals:
	void SigOpenHW();
	void SigRun(uint cpu);
	void SigPfmSendPacket(char* data, uint len);
	void sigSendPacket(char* data, int len);

public slots:
	void SlotOpenHWResult(uint, bool);
	void SlotRunResult( uint, bool);
	void SlotLogMessage( uint, LOG_TYPE, QString );

private:
	CMonitor* m_pMonitor[2];
	QThread*  m_pMonitorTh[2];
	CLogger* m_pLogger;
	CTcpServer*	m_server; // hsbae
	CTcpClient*	m_client; // hsbae
	int	m_slot;

public slots:
	void	SlotIncommingConnection( qintptr handle );
	void	SlotStatusChanged( bool bConn );
	void	SlotReceivePacket( stTcpElement tcpelement );
};

#endif // CSYSPWR_HPP
