#ifndef CTCPSERVER_HPP
#define CTCPSERVER_HPP

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "protocol.hpp"

class CTcpServer : public QTcpServer
{
    Q_OBJECT
public:
	CTcpServer(QObject* parent=nullptr);
signals:
    void SigIncommingConnection( qintptr );
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif
