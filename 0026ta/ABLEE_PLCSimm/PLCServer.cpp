#include "PLCServer.hpp"

CPLCServer::CPLCServer(QObject* p): QTcpServer(p)
{

}

void CPLCServer::incomingConnection(qintptr handle)
{
    emit SigIncommingConnection( handle );
}
