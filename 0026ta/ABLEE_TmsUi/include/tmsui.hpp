#ifndef TMSUI_HPP
#define TMSUI_HPP

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QSplitter>
#include <QTimer>
#include <QThread>
#include <QPushButton>
#include <QWidget>
#include <QSharedMemory>

#include "logger.hpp"
#include "localserver.hpp"
#include "localclient.hpp"
#include "tcpserver.hpp"
#include "hsmsclient.hpp"
#include "syslog.hpp"
#include "util.hpp"
//#include "hsms_cnv.hpp"
#include "secsgem.hpp"
#include "hsmstimeoutchecker.hpp"

#define MAX_LOG_LINE_NUM			(65535*2)
#define PASS_TEXT_QCOLOR			QColor(  0,  0,  0)
#define FAIL_TEXT_QCOLOR			QColor(255, 77, 77)
#define FAULT_TEXT_QCOLOR			QColor(255,  0,  0)	//2023.03.18


namespace Ui {
	class CTmsUi;
}

class CTmsUi : public QWidget
{
	Q_OBJECT

public:
	explicit CTmsUi(QWidget *parent = nullptr);
	~CTmsUi();
private slots:
	void SlotSplitterMoved(int, int);
	void SlotCustomContextMenuRequested( QPoint );
	void SlotScrollStatusChanged();

public slots:
	void SlotStart();
	void SlotRestart();
	void SlotSysLog(LOG_TYPE, QString);
	void SlotIpcConnected(quintptr);
	void SlotIpcStatusChanged(bool);
	void SlotIpcRecvPacket(stIpcElement);
	void SlotHsmsConnected(qintptr);
	void SlotHsmsStatusChanged(bool);
	void SlotHsmsReceivePacket(stHsmsElement);
	void SlotHsmsTimeOut();
	void SlotResendHsmsPacket(char*, unsigned int);

signals:
	void SigIpcStatusChanged(bool);
	void SigIpcSendPacket(char*, uint);
	void SigTcpStatusChanged(bool);
	void SigHsmsStatusChanged(bool);
	void SigSaveSysLog( LOG_TYPE, QString );
	void SigSaveHsmsLog( LOG_TYPE, QString );
	void SigSendHsmsPacket( char*, uint );

private:
	void InitVar();
	void InitUi();
	void InitLog();
	void InitTimeOutChecker();
	void InitTimer();
	bool StartTimer();
	bool StopTimer();
	void InitSharedMemory();
	bool LoadTmsIni();
	bool RunIpcServer();
	bool CloseIpcServer();
	bool DeleteIpcServer();
	bool RunTcpServer();
	bool ListenTcpServer();
	bool CloseTcpServer();
	bool DeleteTcpServer();
	void DisplaySysLog( LOG_TYPE, QString );
	void DisplayHsmsLog( LOG_TYPE, QString );
	void SetTcpStatus(bool);
	void SetSelectStatus(bool);
	void SetHsmsStatus(HSMS_STATUS);
	void SetOnlineMode(bool);
	void SendTcpStatus();
	void SendHsmsSelectRsp(stHsmsElement);
	void SendHsmsDeselectRsp(stHsmsElement);
	void SendHsmsLinkTestRsp(stHsmsElement);
	void SendHsmsRejectReq(stHsmsElement);
	void SendHsmsSeparatetReq(stHsmsElement);

private:
	QString m_sOsDir;
	QString m_sTmsIni;
	CLocalServer* m_pIpcSvr;
	CTcpServer* m_pTcpSvr;
	CLocalClient* m_pIpcClient;
	CHsmsClient* m_pHsmsClient;
	CLogger*  m_pLoggerSys;
	CLogger*  m_pLoggerHsms;
	bool m_bAutoScrollSys;
	bool m_bAutoScrollHsms;
	HSMS_STATUS	m_HsmsStatus;
	bool m_bTcpStatus;
	bool m_bOnlineMode;

private:
	quint16 m_uPort;
	uint    m_uDevId;
	QTimer* m_pTmrTimeOut;
	CHsmsTimeOutChecker* m_pTimeOutChecker;
	QThread* m_pTimeOutCheckerTh;
	QSharedMemory* m_pShmMainUi;

private:
	Ui::CTmsUi *ui;
};

#endif // TMSUI_HPP
