#ifndef CLOCALSERVER_HPP
#define CLOCALSERVER_HPP

#include <QObject>
#include <QLocalServer>
#include "protocol.hpp"

class CLocalServer : public QLocalServer
{
    Q_OBJECT
public:
    CLocalServer(QObject* parent);
signals:
    void SigIncommingConnection( quintptr );
protected:
    void incomingConnection(quintptr socketDescriptor) override;
};

#endif
