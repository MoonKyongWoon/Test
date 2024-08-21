#ifndef CPLCSERVER_HPP
#define CPLCSERVER_HPP

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class CPLCServer : public QTcpServer
{
    Q_OBJECT
public:
    CPLCServer(QObject* parent=nullptr);
signals:
    void SigIncommingConnection( qintptr );
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif // CPLCSERVER_HPP
