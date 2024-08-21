#include "tcpserver.hpp"

CTcpServer::CTcpServer(QObject* parent) : QTcpServer (parent)
{

}

void CTcpServer::incomingConnection(qintptr handle)
{
    emit SigIncommingConnection( handle );
}
