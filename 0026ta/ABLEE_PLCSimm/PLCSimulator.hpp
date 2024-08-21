#ifndef CPLCSIMULATOR_HPP
#define CPLCSIMULATOR_HPP

#include <iostream>
#include <random>
#include <map>
#include <QAction>
#include <QDir>
#include <QHostAddress>
#include <QMessageBox>
#include <QMenu>
#include <QNetworkInterface>
#include <QObject>
#include <QPlainTextEdit>
#include <QResizeEvent>
#include <QSocketNotifier>
#include <QTableWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include "alarmview.hpp"
#include "IniHandler.hpp"
#include "PLCServer.hpp"
#include "PLCClient.hpp"
#include "CommPLC.hpp"
#include "Util.hpp"
#include "sysver.hpp"

extern CIniHandler* g_pIniPLC;        //Global Instance

QT_BEGIN_NAMESPACE
namespace Ui { class CPLCSimulator; }
QT_END_NAMESPACE

class CPLCSimulator : public QWidget
{
    Q_OBJECT

public:
    CPLCSimulator(QWidget *parent = nullptr);
    ~CPLCSimulator();
private:
    void InitVariables();
    void InitUi();
    void InitUiControls();
    void InitUiIpListPort();
    void InitUiConnectionTable();
    void InitPlcView();
    void InitSigSlots();
    void InitTcpServer();
    void InitLogViewContextMenuRequest();
    void InitDirectory();
    void InitPlcAddressMap();
    bool ListenTcpServer( QString, quint16 );
    bool CloseTcpServer();
    void InitWorkerThread();
    void AppendTcpLog ( QString, bool );
    void AppendPlcLog( QString, bool );
    int GetGeneratedId();
    void UpdateConnectionTable();
private:
    bool DoBRD( int, QString, QString, QString );
    bool DoBRR( int, QString, QString, QString );
    bool DoBWR( int, QString, QString, QString );
    bool DoBRW( int, QString, QString, QString );
    bool DoWRD( int, QString, QString, QString );
    bool DoWRR( int, QString, QString, QString );
    bool DoWWR( int, QString, QString, QString );
    bool DoWRW( int, QString, QString, QString );
    QString GetExitCode(EN_PLC_EXIT_ERROR_CODE);
public slots:
    void SlotBtnOpenClicked();
    void SlotBtnCloseClicked();
    void SlotCustomContextMenuRequested( QPoint );
public slots:
    void SlotAcceptError(QAbstractSocket::SocketError);
    void SlotNewConnection(qintptr);
    void SlotClientConnected(int,QString);
    void SlotClientDisconnected(int);
    void SlotClientRecvPacket(int, QByteArray);
    void SlotReloadConfigIni();
signals:
    void SigTcpDescriptor(qintptr);
    void SigTcpServerListen( QString, quint16 );
    void SigTcpServerClose();
    void SigClientSendPacket(int,QByteArray,int);
    void SigDeleteAddress(QString);
    void SigUpdateAddressValue(QString,QString);
private:
    Ui::CPLCSimulator *ui;
    int m_nClientConn;
    QString m_sSvrIp;
    quint16 m_uSvrPort;
    CPLCServer* m_pPlcServer;
    CCommPLC* m_pCommPlcView;
	CAlarmView* m_pAlarmView;
    QVector<CPLCClient*> m_ClientVector;
    std::map<int,CPLCClient*> m_mapId;
    std::map<EN_PLC_EXIT_ERROR_CODE, QString> m_mapExitCode;
    std::map<QString, _stPlcAddrMap> m_mapPlcAddr;

};
#endif // CPLCSIMULATOR_HPP
