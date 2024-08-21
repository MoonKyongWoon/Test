#include "localserver.hpp"

CLocalServer::CLocalServer( QObject* parent ) : QLocalServer(parent)
{

}

void CLocalServer::incomingConnection(quintptr handle)
{
    emit SigIncommingConnection( handle );
}
