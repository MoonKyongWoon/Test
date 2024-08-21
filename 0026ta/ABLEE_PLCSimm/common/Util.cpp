#include "Util.hpp"
namespace Ablee
{
    QString CUtil::getCurrentDate( bool bBracket )
    {
        QString datetime = "";
        QString time_format = "yyyy-MM-dd";
        QDateTime local(QDateTime::currentDateTime());
        if( bBracket ) datetime = "[" + local.toString(time_format) + "] ";
        else           datetime = local.toString(time_format);
        return datetime;
    }

    QString CUtil::getCurrentDateTime( bool bBracket )
    {
        QString datetime = "";
        QString time_format = "yy-MM-dd HH:mm:ss.zzz";
        QDateTime local(QDateTime::currentDateTime());
        if( bBracket ) datetime = "[" + local.toString(time_format) + "] ";
        else           datetime = local.toString(time_format) + ", ";
        return datetime;
    }

    QString CUtil::getOnlyCurrentDateTime()
    {
        QString datetime = "";
        QString time_format = "yyyy-MM-dd_HHmmss";
        QDateTime local(QDateTime::currentDateTime());
        datetime = local.toString(time_format);
        return datetime;
    }

    QString CUtil::getyyMMDDHHmmss()
    {
        QString datetime = "";
        QString time_format = "yy-MM-dd HH:mm:ss";
        QDateTime local(QDateTime::currentDateTime());
        datetime = local.toString(time_format);
        return datetime;
    }

    bool CUtil::AppendFile( QString file, QString msg)
    {
        QFile fp(file);
        fp.open( QIODevice::Append | QIODevice::Text );
        if( !fp.isOpen() ) return false;
        QTextStream outStream(&fp);
        outStream << msg;
        fp.close();
        return true;
    }
    bool CUtil::IsValidIpV4Format( std::string sIpV4)
    {
        int nZeroCnt = 0x0;
        int nIp[4]   = {0x0,};
        if( sscanf(sIpV4.c_str(),"%d.%d.%d.%d", &nIp[0], &nIp[1], &nIp[2], &nIp[3]) != 4 )
            return false;
        for( int i=0;i<4;i++)
        {
            if( nIp[i] == 0 ) nZeroCnt++;
            if( (nIp[i]<0) || (nIp[i]>254) ) return false;
        }
        if( nZeroCnt == 4 ) return false;
        return true;
    }

    QString CSockErr::GetSocketErrDescription( QAbstractSocket::SocketError errCode )
    {
        QString errDescription = "";
        switch( errCode )
        {
        case QAbstractSocket::ConnectionRefusedError:
            errDescription = "The connection was refused by the peer (or timed out).";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            errDescription = "The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.";
            break;
        case QAbstractSocket::HostNotFoundError:
            errDescription = "The host address was not found.";
            break;
        case QAbstractSocket::SocketAccessError:
            errDescription = "The socket operation failed because the application lacked the required privileges.";
            break;
        case QAbstractSocket::SocketResourceError:
            errDescription = "The local system ran out of resources (e.g., too many sockets).";
            break;
        case QAbstractSocket::SocketTimeoutError:
            errDescription = "The socket operation timed out.";
            break;
        case QAbstractSocket::DatagramTooLargeError:
            errDescription = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).";
            break;
        case QAbstractSocket::NetworkError:
            errDescription = "An error occurred with the network (e.g., the network cable was accidentally plugged out).";
            break;
        case QAbstractSocket::AddressInUseError:
            errDescription = "The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.";
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            errDescription = "he address specified to QAbstractSocket::bind() does not belong to the host.";
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            errDescription = "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).";
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            errDescription = "The socket is using a proxy, and the proxy requires authentication.";
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            errDescription = "The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)";
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            errDescription = "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).";
            break;
        case QAbstractSocket::ProxyConnectionRefusedError:
            errDescription = "Could not contact the proxy server because the connection to that server was denied";
            break;
        case QAbstractSocket::ProxyConnectionClosedError:
            errDescription = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";
            break;
        case QAbstractSocket::ProxyConnectionTimeoutError:
            errDescription = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.";
            break;
        case QAbstractSocket::ProxyNotFoundError:
            errDescription = "The proxy address set with setProxy() (or the application proxy) was not found.";
            break;
        case QAbstractSocket::ProxyProtocolError:
            errDescription = "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.";
            break;
        case QAbstractSocket::OperationError:
            errDescription = "An operation was attempted while the socket was in a state that did not permit it.";
            break;
        case QAbstractSocket::SslInternalError:
            errDescription = "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.";
            break;
        case QAbstractSocket::SslInvalidUserDataError:
            errDescription = "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.";
            break;
        case QAbstractSocket::TemporaryError:
            errDescription = "A temporary error occurred (e.g., operation would block and socket is non-blocking).";
            break;
        case QAbstractSocket::UnknownSocketError:
            errDescription = "An unidentified error occurred.";
            break;
        }
        return errDescription;
    }

}
