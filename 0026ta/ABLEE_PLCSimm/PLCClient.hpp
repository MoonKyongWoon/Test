#ifndef CPLCCLIENT_HPP
#define CPLCCLIENT_HPP

#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>

class CPLCClient : public QObject
{
    Q_OBJECT
public:
    explicit CPLCClient(int, qintptr, QObject *parent = nullptr);
    ~CPLCClient();

private:
    void CloseSocket();
    bool IsValidSocket();
public:
    void DoEmit();
    QString GetIp();
signals:
    void SigClientDisconnected(int);
    void SigClientConnected(int, QString);
    void SigTcpConfigLog ( QString, bool );
    void SigTcpMessageLog( QString, bool );
    void SigRecvPacket(int,QByteArray);
public slots:
    void SlotNewConnection(qintptr);
    void SlotReadyRead();
    void SlotDisconnected();
    void SlotSendPacket(int, QByteArray,int);
private:
    int m_nClientNo;
    QString m_sClientIp;
    qintptr m_handle;
    QTcpSocket* m_pSocket;
};

#endif // CPLCCLIENT_HPP
