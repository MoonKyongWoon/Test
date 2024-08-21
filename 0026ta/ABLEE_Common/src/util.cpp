#include "util.hpp"

namespace UTIL
{
    //DATE
    QString DATE::yyyyMMDDHHmmsszzz( bool bracket )
    {
        QString datetime = "";
        QString time_format = "yyyy-MM-dd HH:mm:ss.zzz";
        QDateTime local(QDateTime::currentDateTime());
        if( bracket ) datetime = "[" + local.toString(time_format) + "]";
        else          datetime = local.toString(time_format);
        return datetime;
    }
    QString DATE::yyyyMMDDHHmmss( bool bracket)
    {
        QString datetime = "";
        QString time_format = "yyyy-MM-dd HH:mm:ss";
        QDateTime local(QDateTime::currentDateTime());
        if( bracket ) datetime = "[" + local.toString(time_format) + "]";
        else          datetime = local.toString(time_format);
        return datetime;
    }
    QString DATE::yyMMDDHHmmss( bool bracket)
    {
        QString datetime = "";
        QString time_format = "yy-MM-dd HH:mm:ss";
        QDateTime local(QDateTime::currentDateTime());
        if( bracket ) datetime = "[" + local.toString(time_format) + "]";
        else          datetime = local.toString(time_format);
        return datetime;

    }
    QString DATE::yyMMDD( bool bracket)
    {
        QString datetime = "";
        QString time_format = "yy-MM-dd";
        QDateTime local(QDateTime::currentDateTime());
        if( bracket ) datetime = "[" + local.toString(time_format) + "]";
        else          datetime = local.toString(time_format);
        return datetime;
    }
    QString DATE::yyyyMMDD( bool bracket)
    {
        QString datetime = "";
        QString time_format = "yyyyMMdd";
        QDateTime local(QDateTime::currentDateTime());
        if( bracket ) datetime = "[" + local.toString(time_format) + "]";
        else          datetime = local.toString(time_format);
        return datetime;
    }

    //FILE
    bool FILE::AppendFile( QString file, QString msg, bool bNl )
    {
        QFile fp(file);
        fp.open( QIODevice::Append | QIODevice::Text );
        if( !fp.isOpen() ) return false;
        QTextStream outStream(&fp);
        outStream << msg;
        if( bNl ) outStream << LF;
        fp.close();
        return true;
    }

    bool FILE::WriteFile( QString file, QString msg, bool bNl)
    {
        QFile fp(file);
        fp.open( QIODevice::WriteOnly | QIODevice::Text );
        if( !fp.isOpen() ) return false;
        QTextStream outStream(&fp);
        outStream << msg;
        if( bNl ) outStream << LF;
        fp.close();
        return true;
    }

    //VALIDATION
    bool VALIDATION::IsValidIpV4( std::string sIpV4)
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
    bool VALIDATION::IsExistDir( QString path )
    {
        QDir dir(path);
        return dir.exists();
    }
    bool VALIDATION::IsExistFile( QString f )
    {
        return QFile::exists(f);
    }
    bool VALIDATION::IsExecutable( QString file )
    {
        QFileInfo info( file );
        if( !info.isFile() ) return false;
        return info.isExecutable();
    }

    //SOCKET ERROR
	QString TCPIP::GetSocketErrDescription( QAbstractSocket::SocketError errCode )
    {
        QString errDescription = "No error occurred.";
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

	//PROTOCOL
	QString TCPIP::CMD_STR(PROTOCOL::TCP_COMMAND cmd)
	{
		QString scmd = "";
		switch ( cmd )
		{
			case PROTOCOL::CMD_SLOT_INFO_REQ		: { scmd = "CMD_SLOT_INFO_REQ        "; break; }
			case PROTOCOL::CMD_SLOT_INFO_ACK		: { scmd = "CMD_SLOT_INFO_ACK        "; break; }
			case PROTOCOL::CMD_LIVE_SLOT_REQ		: { scmd = "CMD_LIVE_SLOT_REQ        "; break; }
			case PROTOCOL::CMD_LIVE_SLOT_ACK		: { scmd = "CMD_LIVE_SLOT_ACK        "; break; }
			case PROTOCOL::CMD_LIVE_PGM_REQ	    	: { scmd = "CMD_LIVE_PGM_REQ         "; break; }
			case PROTOCOL::CMD_LIVE_PGM_ACK  		: { scmd = "CMD_LIVE_PGM_ACK         "; break; }
			case PROTOCOL::CMD_SLOT_CPU_INFO_SEND   : { scmd = "CMD_SLOT_CPU_INFO_SEND   "; break; }
			case PROTOCOL::CMD_SLOT_CPU_INFO_ACK    : { scmd = "CMD_SLOT_CPU_INFO_ACK    "; break; }
			case PROTOCOL::CMD_EWS_FTP_INFO_SEND    : { scmd = "CMD_EWS_FTP_INFO_SEND    "; break; }
			case PROTOCOL::CMD_EWS_FTP_INFO_ACK     : { scmd = "CMD_EWS_FTP_INFO_ACK     "; break; }
			case PROTOCOL::CMD_LOADER_INFO_REQ		: { scmd = "CMD_LOADER_INFO_REQ      "; break; }
			case PROTOCOL::CMD_LOADER_INFO_ACK		: { scmd = "CMD_LOADER_INFO_ACK      "; break; }
			//case PROTOCOL::CMD_SETUP_BIBID_REQ		: { scmd = "CMD_SETUP_BIBID_REQ      "; break; }
			//case PROTOCOL::CMD_SETUP_BIBID_ACK		: { scmd = "CMD_SETUP_BIBID_ACK      "; break; }
			case PROTOCOL::CMD_PGM_LOAD_REQ			: { scmd = "CMD_PGM_LOAD_REQ         "; break; }
			case PROTOCOL::CMD_PGM_LOAD_STATUS  	: { scmd = "CMD_PGM_LOAD_STATUS      "; break; }
			case PROTOCOL::CMD_PGM_LOAD_ACK			: { scmd = "CMD_PGM_LOAD_ACK         "; break; }
			case PROTOCOL::CMD_PGM_RELOAD_REQ		: { scmd = "CMD_PGM_RELOAD_REQ       "; break; }
			case PROTOCOL::CMD_PGM_RELOAD_ACK		: { scmd = "CMD_PGM_RELOAD_ACK       "; break; }
			case PROTOCOL::CMD_PGM_UNLOAD_REQ		: { scmd = "CMD_PGM_UNLOAD_REQ       "; break; }
			case PROTOCOL::CMD_PGM_UNLOAD_ACK		: { scmd = "CMD_PGM_UNLOAD_ACK       "; break; }
			case PROTOCOL::CMD_PGM_FINISHED  		: { scmd = "CMD_PGM_FINISHED         "; break; }
			case PROTOCOL::CMD_VERSION_REQ			: { scmd = "CMD_VERSION_REQ          "; break; }
			case PROTOCOL::CMD_VERSION_ACK			: {	scmd = "CMD_VERSION_ACK          "; break; }
			//case PROTOCOL::CMD_TEST_DATA_INIT_REQ	: { scmd = "CMD_TEST_DATA_INIT_REQ   "; break; }
			//case PROTOCOL::CMD_TEST_DATA_INIT_ACK	: {	scmd = "CMD_TEST_DATA_INIT_ACK   "; break; }
			case PROTOCOL::CMD_UI_CHMBUSE_REQ   	: { scmd = "CMD_UI_CHMBUSE_REQ       "; break; }
			case PROTOCOL::CMD_UI_CHMBUSE_ACK    	: {	scmd = "CMD_UI_CHMBUSE_ACK       "; break; }

			case PROTOCOL::CMD_READ_BIBID_REQ    	: {	scmd = "CMD_READ_BIBID_REQ       "; break; }	//2024.06.17 - KWMOON
			case PROTOCOL::CMD_READ_BIBID_ACK    	: {	scmd = "CMD_READ_BIBID_ACK       "; break; }	//2024.06.17 - KWMOON
			case PROTOCOL::CMD_ENGR_SETUP_REQ       : {	scmd = "CMD_ENGR_SETUP_REQ       "; break; }	//2024.06.17 - KWMOON
			case PROTOCOL::CMD_ENGR_SETUP_ACK    	: {	scmd = "CMD_ENGR_SETUP_ACK       "; break; }	//2024.06.17 - KWMOON
			case PROTOCOL::CMD_AUTO_SETUP_REQ       : {	scmd = "CMD_AUTO_SETUP_REQ       "; break; }	//2024.06.17 - KWMOON
			case PROTOCOL::CMD_AUTO_SETUP_ACK    	: {	scmd = "CMD_AUTO_SETUP_ACK       "; break; }	//2024.06.17 - KWMOON
			case PROTOCOL::CMD_PFM_SYSMONUI_SEND	: {	scmd = "CMD_PFM_SYSMONUI_SEND    "; break; }	//2024.06.17 - KWMOON
			case PROTOCOL::CMD_PFM_SYSMONUI_ACK  	: {	scmd = "CMD_PFM_SYSMONUI_ACK     "; break; }	//2024.06.17 - KWMOON

			case PROTOCOL::CMD_LOT_ID_REQ          	: { scmd = "CMD_LOT_ID_REQ           "; break; }
			case PROTOCOL::CMD_LOT_ID_ACK          	: { scmd = "CMD_LOT_ID_ACK           "; break; }
			case PROTOCOL::CMD_PART_NAME_REQ       	: { scmd = "CMD_PART_NAME_REQ        "; break; }
			case PROTOCOL::CMD_PART_NAME_ACK       	: { scmd = "CMD_PART_NAME_ACK        "; break; }
			case PROTOCOL::CMD_INIT_TEST_START_REQ 	: { scmd = "CMD_INIT_TEST_START_REQ  "; break; }
			case PROTOCOL::CMD_INIT_TEST_START_ACK 	: { scmd = "CMD_INIT_TEST_START_ACK  "; break; }
			case PROTOCOL::CMD_TEST_START_REQ      	: { scmd = "CMD_TEST_START_REQ       "; break; }
			case PROTOCOL::CMD_TEST_START_ACK      	: { scmd = "CMD_TEST_START_ACK       "; break; }
			case PROTOCOL::CMD_ITEM_TEST_START_REQ 	: { scmd = "CMD_ITEM_TEST_START_REQ  "; break; }
			case PROTOCOL::CMD_ITEM_TEST_START_ACK 	: { scmd = "CMD_ITEM_TEST_START_ACK  "; break; }
			case PROTOCOL::CMD_TEST_STOP_REQ       	: { scmd = "CMD_TEST_STOP_REQ        "; break; }
			case PROTOCOL::CMD_TEST_STOP_ACK       	: { scmd = "CMD_TEST_STOP_ACK        "; break; }

			case PROTOCOL::CMD_INIT_TEST_DATA    	: { scmd = "CMD_INIT_TEST_DATA       "; break; }
			case PROTOCOL::CMD_CHIPID_INFO       	: { scmd = "CMD_CHIPID_INFO          "; break; }
			case PROTOCOL::CMD_TEST_STEP_BEGIN     	: { scmd = "CMD_TEST_STEP_BEGIN      "; break; }
			case PROTOCOL::CMD_TEST_STEP_END     	: { scmd = "CMD_TEST_STEP_END        "; break; }
			case PROTOCOL::CMD_STEP_FAIL_DATA     	: { scmd = "CMD_STEP_FAIL_DATA       "; break; }
			case PROTOCOL::CMD_STEP_FAIL_BLOCK_DATA : { scmd = "CMD_STEP_FAIL_BLOCK_DATA "; break; }
			case PROTOCOL::CMD_STEP_FAIL_CHIP_DATA  : { scmd = "CMD_STEP_FAIL_CHIP_DATA  "; break; }
			case PROTOCOL::CMD_INIT_TEST_DONE       : { scmd = "CMD_INIT_TEST_DONE       "; break; }
			case PROTOCOL::CMD_CUM_FAIL_BLOCK_DATA  : { scmd = "CMD_CUM_FAIL_BLOCK_DATA  "; break; }
			case PROTOCOL::CMD_CUM_FAIL_CHIP_DATA   : { scmd = "CMD_CUM_FAIL_CHIP_DATA   "; break; }
			case PROTOCOL::CMD_CUM_FAIL_DATA        : { scmd = "CMD_CUM_FAIL_DATA        "; break; }
			case PROTOCOL::CMD_STEP_SORTBIN_DATA    : { scmd = "CMD_STEP_SORTBIN_DATA    "; break; }
			case PROTOCOL::CMD_CUM_SORTBIN_DATA     : { scmd = "CMD_CUM_SORTBIN_DATA     "; break; }
			case PROTOCOL::CMD_CHIPID_DATA       	: { scmd = "CMD_CHIPID_DATA          "; break; }
			case PROTOCOL::CMD_STEP_IN_INFO         : { scmd = "CMD_STEP_IN_INFO         "; break; }
			case PROTOCOL::CMD_STEP_OUT_INFO        : { scmd = "CMD_STEP_OUT_INFO        "; break; }
			case PROTOCOL::CMD_CYCLE_IN_INFO        : { scmd = "CMD_CYCLE_IN_INFO        "; break; }
			case PROTOCOL::CMD_CYCLE_OUT_INFO       : { scmd = "CMD_CYCLE_OUT_INFO       "; break; }
			case PROTOCOL::CMD_GET_BLOCK_DATA       : { scmd = "CMD_GET_BLOCK_DATA       "; break; }	//2024.06.24 - KWMOON
			case PROTOCOL::CMD_CUM_FAIL_CATEGORY    : { scmd = "CMD_CUM_FAIL_CATEGORY    "; break; }	//2024.06.24 - KWMOON

			case PROTOCOL::CMD_STEP_FCATE_DATA      : { scmd = "CMD_STEP_FCATE_DATA      "; break; }	//2024.07.22 fatric
			case PROTOCOL::CMD_STEP_PCATE_DATA      : { scmd = "CMD_STEP_PCATE_DATA      "; break; }	//2024.07.22 fatric
			case PROTOCOL::CMD_BINMAP_DATA          : { scmd = "CMD_BINMAP_DATA          "; break; }	//2024.07.22 fatric
			case PROTOCOL::CMD_STEP_FAIL_MODE       : { scmd = "CMD_STEP_FAIL_MODE       "; break; }	//2024.07.22 fatric
			case PROTOCOL::CMD_STEP_USE_MODE        : { scmd = "CMD_STEP_USE_MODE        "; break; }	//2024.08.18 fatric

			case PROTOCOL::CMD_TEST_DONE            : { scmd = "CMD_TEST_DONE            "; break; }

			case PROTOCOL::CMD_SLOT_RUN_NO_SEND 	: { scmd = "CMD_SLOT_RUN_NO_SEND     "; break; }
			case PROTOCOL::CMD_PGM_LOAD_STATUS_SEND : { scmd = "CMD_PGM_LOAD_STATUS_SEND "; break; }
			case PROTOCOL::CMD_PGM_ITEM_LISTUP      : { scmd = "CMD_PGM_ITEM_LISTUP      "; break; }
			case PROTOCOL::CMD_LOG_PRINT            : { scmd = "CMD_LOG_PRINT            "; break; }
			case PROTOCOL::CMD_PGM_RUNTIME_ERROR    : { scmd = "CMD_PGM_RUNTIME_ERROR    "; break; }

			case PROTOCOL::CMD_DEV_PGM_VARIABLE     : { scmd = "CMD_DEV_PGM_VARIABLE     "; break; }
			case PROTOCOL::CMD_VAR_SET_BITIME       : { scmd = "CMD_VAR_SET_BITIME       "; break; }
			case PROTOCOL::CMD_KEEP_ALIVE           : { scmd = "CMD_KEEP_ALIVE           "; break; }

			case PROTOCOL::CMD_CHMB_SETSV_VALID_REQ : { scmd = "CMD_CHMB_SETSV_VALID_REQ "; break; }
			case PROTOCOL::CMD_CHMB_SETSV_VALID_ACK : { scmd = "CMD_CHMB_SETSV_VALID_ACK "; break; }
			case PROTOCOL::CMD_CHMB_ENTER_SYNC_REQ  : { scmd = "CMD_CHMB_ENTER_SYNC_REQ  "; break; }
			case PROTOCOL::CMD_CHMB_ENTER_SYNC_ACK  : { scmd = "CMD_CHMB_ENTER_SYNC_ACK  "; break; }
			case PROTOCOL::CMD_CHMB_READ_TEMP_REQ   : { scmd = "CMD_CHMB_READ_TEMP_REQ   "; break; }
			case PROTOCOL::CMD_CHMB_READ_TEMP_ACK 	: { scmd = "CMD_CHMB_READ_TEMP_ACK   "; break; }
			case PROTOCOL::CMD_CHMB_READ_ALARM_REQ	: { scmd = "CMD_CHMB_READ_ALARM_REQ  "; break; }
			case PROTOCOL::CMD_CHMB_READ_ALARM_ACK	: { scmd = "CMD_CHMB_READ_ALARM_ACK  "; break; }
			case PROTOCOL::CMD_CHMB_READ_STAT1_REQ	: { scmd = "CMD_CHMB_READ_STAT1_REQ  "; break; }
			case PROTOCOL::CMD_CHMB_READ_STAT1_ACK	: { scmd = "CMD_CHMB_READ_STAT1_ACK  "; break; }
			case PROTOCOL::CMD_CHMB_READ_STAT2_REQ	: { scmd = "CMD_CHMB_READ_STAT2_REQ  "; break; }
			case PROTOCOL::CMD_CHMB_READ_STAT2_ACK	: { scmd = "CMD_CHMB_READ_STAT2_ACK  "; break; }
			case PROTOCOL::CMD_CHMB_INIT_TEMP_REQ  	: { scmd = "CMD_CHMB_INIT_TEMP_REQ   "; break; }
			case PROTOCOL::CMD_CHMB_INIT_TEMP_ACK 	: { scmd = "CMD_CHMB_INIT_TEMP_ACK   "; break; }
			case PROTOCOL::CMD_CHMB_RUN_SYNC_REQ    : { scmd = "CMD_CHMB_RUN_SYNC_REQ    "; break; }
			case PROTOCOL::CMD_CHMB_RUN_SYNC_ACK    : { scmd = "CMD_CHMB_RUN_SYNC_ACK    "; break; }
			case PROTOCOL::CMD_CHMB_DOOR_CLOSE_REQ  : { scmd = "CMD_CHMB_DOOR_CLOSE_REQ  "; break; }
			case PROTOCOL::CMD_CHMB_DOOR_CLOSE_ACK  : { scmd = "CMD_CHMB_DOOR_CLOSE_ACK  "; break; }
			case PROTOCOL::CMD_CHMB_DOOR_OPEN_REQ   : { scmd = "CMD_CHMB_DOOR_OPEN_REQ   "; break; }
			case PROTOCOL::CMD_CHMB_DOOR_OPEN_ACK   : { scmd = "CMD_CHMB_DOOR_OPEN_ACK   "; break; }
			case PROTOCOL::CMD_CHMB_RACKIN_REQ      : { scmd = "CMD_CHMB_RACKIN_REQ      "; break; }
			case PROTOCOL::CMD_CHMB_RACKIN_ACK      : { scmd = "CMD_CHMB_RACKIN_ACK      "; break; }
			case PROTOCOL::CMD_CHMB_RACKOUT_REQ     : { scmd = "CMD_CHMB_RACKOUT_REQ     "; break; }
			case PROTOCOL::CMD_CHMB_RACKOUT_ACK     : { scmd = "CMD_CHMB_RACKOUT_ACK     "; break; }
			case PROTOCOL::CMD_CHMB_SET_SV_REQ      : { scmd = "CMD_CHMB_SET_SV_REQ      "; break; }
			case PROTOCOL::CMD_CHMB_SET_SV_ACK      : { scmd = "CMD_CHMB_SET_SV_ACK      "; break; }
			case PROTOCOL::CMD_CHMB_SET_AMB_REQ     : { scmd = "CMD_CHMB_SET_AMB_REQ     "; break; }
			case PROTOCOL::CMD_CHMB_SET_AMB_ACK     : { scmd = "CMD_CHMB_SET_AMB_ACK     "; break; }
			case PROTOCOL::CMD_CHMB_RESET_ALARM_REQ : { scmd = "CMD_CHMB_RESET_ALARM_REQ "; break; }
			case PROTOCOL::CMD_CHMB_RESET_ALARM_ACK : { scmd = "CMD_CHMB_RESET_ALARM_ACK "; break; }

			case PROTOCOL::CMD_CHMB_TEMP_MON_SET    : { scmd = "CMD_CHMB_TEMP_MON_SET    "; break; }	//2024.08.14 - KWMOON
			case PROTOCOL::CMD_CHMB_TEMP_MON_END    : { scmd = "CMD_CHMB_TEMP_MON_END    "; break; }	//2024.08.14 - KWMOON


			default									: {	scmd = "CMD_UNKNOWN              "; break; }
		}
		return scmd;
	}
	//2023.05.09 - KWMOON
	QString TCPIP::SOURCE_STR(PROTOCOL::SOURCE_TYPE src)
	{
		QString ssrc = "";
		switch ( src )
		{
			case PROTOCOL::_ATMAIN_UI		: { ssrc = "_ATMAIN_UI		"; break; }
			case PROTOCOL::_ATMGR		    : { ssrc = "_ATMGR		    "; break; }
			case PROTOCOL::_ATLODER		    : { ssrc = "_ATLODER		"; break; }
			case PROTOCOL::_ATMAIN_UI_CHMB	: { ssrc = "_ATMAIN_UI_CHMB	"; break; }
			case PROTOCOL::_ATCHMB_UI		: { ssrc = "_ATCHMB_UI		"; break; }
			case PROTOCOL::_ATCHMB_COMM		: { ssrc = "_ATCHMB_COMM	"; break; }
			case PROTOCOL::_ATPPSMON_UI		: { ssrc = "_ATPPSMON_UI	"; break; }
			case PROTOCOL::_ATPPSMON_COMM   : { ssrc = "_ATPPSMON_COMM  "; break; }
			case PROTOCOL::_ATSYSMON_UI		: { ssrc = "_ATSYSMON_UI	"; break; }
			case PROTOCOL::_ATSYSMON_COMM	: { ssrc = "_ATSYSMON_COMM	"; break; }
			case PROTOCOL::_ATTMS_UI		: { ssrc = "_ATTMS_UI		"; break; }
			case PROTOCOL::_ATSLOTS		    : { ssrc = "_ATSLOTS		"; break; }
			case PROTOCOL::_ATSLOT01		: { ssrc = "_ATSLOT01		"; break; }
			case PROTOCOL::_ATSLOT02		: { ssrc = "_ATSLOT02		"; break; }
			case PROTOCOL::_ATSLOT03		: { ssrc = "_ATSLOT03		"; break; }
			case PROTOCOL::_ATSLOT04		: { ssrc = "_ATSLOT04		"; break; }
			case PROTOCOL::_ATSLOT05		: { ssrc = "_ATSLOT05		"; break; }
			case PROTOCOL::_ATSLOT06		: { ssrc = "_ATSLOT06		"; break; }
			case PROTOCOL::_ATSLOT07		: { ssrc = "_ATSLOT07		"; break; }
			case PROTOCOL::_ATSLOT08		: { ssrc = "_ATSLOT08		"; break; }
			case PROTOCOL::_ATSLOT09		: { ssrc = "_ATSLOT09		"; break; }
			case PROTOCOL::_ATSLOT10		: { ssrc = "_ATSLOT10		"; break; }
			case PROTOCOL::_ATSLOT11		: { ssrc = "_ATSLOT11		"; break; }
			case PROTOCOL::_ATSLOT12		: { ssrc = "_ATSLOT12		"; break; }
			case PROTOCOL::_ATSLOT13		: { ssrc = "_ATSLOT13		"; break; }
			case PROTOCOL::_ATSLOT14		: { ssrc = "_ATSLOT14		"; break; }
			case PROTOCOL::_ATSLOT15		: { ssrc = "_ATSLOT15		"; break; }
			case PROTOCOL::_ATSLOT16		: { ssrc = "_ATSLOT16		"; break; }
			default                         : { ssrc = "SRC_UNKNOWN	    "; break; }
		}
		return ssrc;
	}

	std::string TCPIP::GetHostName()
	{
		char hostname[1024];
		hostname[1023] = '\0';
		gethostname(hostname, 1023);
		return std::string(hostname);
	}

	void INI::syncIni( QString file )
	{
		QSettings settings(file, QSettings::IniFormat);
		settings.sync();
	}

	bool INI::writeSettings( QString file, QString key, const QVariant val, bool sync)
	{
        QSettings settings(file, QSettings::IniFormat);
        if( !settings.isWritable() ) return false;
        settings.setValue(key, val);
		if( sync ) settings.sync();
        return true;
    }

	bool INI::writeSettings( QString file, QString section, QStringList key_list, QStringList value_list)
	{
		QSettings settings(file, QSettings::IniFormat);
		if( !settings.isWritable() ) return false;
		for (int i = 0; i < key_list.length(); i++)
		{
			settings.setValue(QObject::tr("%1/%2").arg(section).arg(key_list[i]), value_list[i]);
		}
		settings.sync();
		return true;
	}

    bool INI::readSettings (QString file, QString key, QVariant& val)
    {
        QSettings settings(file, QSettings::IniFormat);
        if( !settings.contains(key) ) return false;
        val = settings.value(key);
        return true;
    }

    bool INI::readSettings (QString file, QString key, uint& val)
    {
        bool bOk = false;
        QSettings settings(file, QSettings::IniFormat);
        if( !settings.contains(key) ) return false;
        val = settings.value(key).toUInt(&bOk);
        if( bOk ) return true;
        else      return false;
    }

    bool INI::readSettings (QString file, QString key, int& val)
    {
        bool bOk = false;
        QSettings settings(file, QSettings::IniFormat);
        if( !settings.contains(key) ) return false;
        val = settings.value(key).toInt(&bOk);
        if( bOk ) return true;
        else      return false;
    }

    bool INI::readSettings (QString file, QString key, QString& val)
    {
        QSettings settings(file, QSettings::IniFormat);
        if( !settings.contains(key) ) return false;
        val = settings.value(key).toString();
        return true;
    }

    bool INI::readSettings (QString file, QString key, std::string& val)
    {
        QSettings settings(file, QSettings::IniFormat);
        if( !settings.contains(key) ) return false;
        val = settings.value(key).toString().toStdString();
        return true;
    }

    bool INI::readSettings (QString file, QString key, double& val)
    {
        bool bOk = false;
        QSettings settings(file, QSettings::IniFormat);
        if( !settings.contains(key) ) return false;
        val = settings.value(key).toDouble(&bOk);
        if( bOk ) return true;
        else      return false;
    }

	bool INI::readSettings (QString file, QString key, bool& val)
	{
		QSettings settings(file, QSettings::IniFormat);
		if( !settings.contains(key) ) return false;
		val = settings.value(key).toBool();
		return true;
	}

    QStringList INI::getAllGroups(QString file)
    {
        QSettings settings(file, QSettings::IniFormat);
        return settings.childGroups();
    }
    QStringList INI::getAllKeys(QString file, QString section)
    {
        QSettings settings(file, QSettings::IniFormat);
        settings.beginGroup(section);
        const QStringList childKeys = settings.childKeys();
        settings.endGroup();
        if( childKeys.count() > 0 ) return childKeys;
        else return QStringList();
    }
    QStringList INI::getAllKeys(QString file, std::string stdsection)
    {
        QString section = stdsection.c_str();
        QSettings settings(file, QSettings::IniFormat);
        settings.beginGroup(section);
        const QStringList childKeys = settings.childKeys();
        settings.endGroup();
        if( childKeys.count() > 0 ) return childKeys;
        else return QStringList();
    }
	bool INI::writeSettings(std::string file, std::string section, const double value, bool sync)
	{
		QSettings settings( file.c_str(), QSettings::IniFormat);
		if( !settings.isWritable() ) return false;
		settings.setValue(section.c_str(), value);
		if( sync ) settings.sync();
		return true;
	}

	bool INI::writeSettings(std::string file, std::string section, const int value, bool sync)
	{
		QSettings settings( file.c_str(), QSettings::IniFormat);
		if( !settings.isWritable() ) return false;
		settings.setValue(section.c_str(), value);
		if( sync ) settings.sync();
		return true;
	}

	bool INI::writeSettings(std::string file, std::string section, std::string value, bool sync)
	{
		QSettings settings( file.c_str(), QSettings::IniFormat);
		if( !settings.isWritable() ) return false;
		settings.setValue(section.c_str(), value.c_str());
		if( sync ) settings.sync();
		return true;
	}

	bool INI::readSettings (std::string file, std::string section, double& rValue)
	{
		bool bOk = false;
		QSettings settings(file.c_str(), QSettings::IniFormat);
		if( !settings.contains(section.c_str()) ) return false;
		rValue = settings.value(section.c_str()).toDouble(&bOk);
		if( bOk ) return true;
		else      return false;
	}

	bool INI::readSettings (std::string file, std::string section, int& rValue)
	{
		bool bOk = false;
		QSettings settings(file.c_str(), QSettings::IniFormat);
		if( !settings.contains(section.c_str()) )
		{
			rValue = 0;
			return false;
		}
		rValue = settings.value(section.c_str()).toInt(&bOk);
		if( bOk ) return true;
		else      return false;
	}

	bool INI::readSettings (std::string file, std::string section, std::string& rValue)
	{
		QSettings settings(file.c_str(), QSettings::IniFormat);
		if( !settings.contains(section.c_str()) )
		{
			rValue = std::string("");
			return false;
		}
		rValue = settings.value(section.c_str()).toString().toStdString();
		return true;
	}

	bool INI::removeSection( QString ini, QString section )
	{
		QSettings settings( ini, QSettings::IniFormat);
		settings.beginGroup(section);
		settings.remove("");
		settings.endGroup();
		return true;
	}

	bool INI::removeKey( QString ini, QString section, QString key )
	{
		QSettings settings( ini, QSettings::IniFormat);
		QString secskey = QObject::tr("%1/%2").arg(section).arg(key);
		if( settings.contains(secskey) )
		{
			settings.remove( secskey );
			return true;
		}
		return false;
	}


	std::string _SIGNAL::GetSigName( int signum )
    {
        std::string signame = "";
        switch( signum )
        {
        case 1:  signame = "SIGHUP"   ; break;
        case 2:  signame = "SIGINT"   ; break;
        case 3:  signame = "SIGQUIT"  ; break;
        case 4:  signame = "SIGILL"   ; break;
        case 5:  signame = "SIGTRAP"  ; break;
        case 6:  signame = "SIGABRT"  ; break;
        case 7:  signame = "SIGBUS"   ; break;
        case 8:  signame = "SIGFPE"   ; break;
        case 9:  signame = "SIGKILL"  ; break;
        case 10: signame = "SIGUSR1"  ; break;
        case 11: signame = "SIGSEGV"  ; break;
        case 12: signame = "SIGUSR2"  ; break;
        case 13: signame = "SIGPIPE"  ; break;
        case 14: signame = "SIGALRM"  ; break;
        case 15: signame = "SIGTERM"  ; break;
        case 16: signame = "SIGSTKFLT"; break;
        case 17: signame = "SIGCHLD"  ; break;
        case 18: signame = "SIGCONT"  ; break;
        case 19: signame = "SIGSTOP"  ; break;
        case 20: signame = "SIGTSTP"  ; break;
        case 21: signame = "SIGTTIN"  ; break;
        case 22: signame = "SIGTTOUT" ; break;
        case 23: signame = "SIGURG"   ; break;
        case 24: signame = "SIGXCPU"  ; break;
        case 25: signame = "SIGXFSZ"  ; break;
        case 26: signame = "SIGVTALRM"; break;
        case 27: signame = "SIGPROF"  ; break;
        case 28: signame = "SIGWINCH" ; break;
        case 29: signame = "SIGIO"    ; break;
        case 30: signame = "SIGPWR"   ; break;
        case 31: signame = "SIGSYS"   ; break;
        default: signame = "SIGUNDEF" ; break;
        }
        return signame;
    }

	//2022.10.28 - TIMER Appended by fatric.
	TIMER::TIMER()
	{
	}
	TIMER::TIMER(double dSec)
	{
		setTimer( dSec );
	}
	void TIMER::setTimer(double dSec)
	{
		setTime = dSec;
	}
	void TIMER::start()
	{
		begin = std::chrono::steady_clock::now();
	}
	bool TIMER::isEnd()
	{
		double dElapsedTime = getElapsedTime();
		return ( dElapsedTime >= setTime );
	}
	double TIMER::getElapsedTime()
	{
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		long long elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();	//US
		return elapsed_time * 1.0e-6;
	}

	bool PROCESS::IsRun( long mypid, std::string program, std::string arg )
	{
		(void)arg;
		char namebuf[128] = {0x0,};
		char filebuf[256] = {0x0,};

		memset( namebuf, 0x0, sizeof(namebuf) );
		memset( filebuf, 0x0, sizeof(filebuf) );

		snprintf(namebuf, sizeof(namebuf), "/proc/%ld/cmdline", mypid);
		::FILE* fp = ::fopen(namebuf, "r");
		if ( fp )
		{
			if ( fgets(filebuf, sizeof(filebuf), fp) != NULL )
			{
				char* next_ptr = nullptr;
				char* ret_ptr  = strtok_r (filebuf, " ", &next_ptr);
				bool bFoundPrg = false;
				while( ret_ptr )
				{
					if ( strstr(ret_ptr, program.c_str()) != NULL )
					{
						bFoundPrg = true;
					}
					ret_ptr  = strtok_r (NULL, " ", &next_ptr);
				}
				if( bFoundPrg )
				{
					fclose(fp);
					return true;
				}
			}
			fclose(fp);
		}
		return false;
	}

	bool PROCESS::IsRun( std::string program, std::string arg )
	{
		DIR* dir;
		struct dirent* ent;
		char* endptr;
		char namebuf[128];
		char filebuf[256];
		bool bArgCheck = arg.empty()?false:true;

		if (!(dir = opendir("/proc"))) {
			perror("can't open /proc");
			return -1;
		}

		while( (ent = readdir(dir)) != NULL )
		{
			long lpid = strtol(ent->d_name, &endptr, 10);
			if (*endptr != '\0') {
				continue;
			}

			snprintf(namebuf, sizeof(namebuf), "/proc/%ld/cmdline", lpid);
			::FILE* fp = ::fopen(namebuf, "r");
			if ( fp )
			{
				if ( fgets(filebuf, sizeof(filebuf), fp) != NULL )
				{
					char* next_ptr = nullptr;
					char* ret_ptr  = strtok_r (filebuf, " ", &next_ptr);
					bool bFoundPrg = false;
					bool bFoundArg = bArgCheck?false:true;
					while( ret_ptr )
					{
						if ( strstr(ret_ptr, program.c_str()) != NULL )
						{
							bFoundPrg = true;
						}
						if( bArgCheck )
						{
							if( !arg.empty() && ( strstr(ret_ptr, arg.c_str()) != NULL ) )
							{
								bFoundArg = true;
							}
						}
						ret_ptr  = strtok_r (NULL, " ", &next_ptr);
					}
					if( bFoundPrg && bFoundArg )
					{
						fclose(fp);
						closedir(dir);
						return true;
					}
				}
				fclose(fp);
			}
		}
		closedir(dir);
		return false;
	}

	bool PROCESS::DetachRun( std::string prg, QStringList args, qint64& pid )
	{
		QProcess process;
		QString program = QString( prg.c_str() );
		//qDebug() << prg.c_str();
		//qDebug() << args;
		return process.startDetached( program, args, "", &pid );
	}
	bool PROCESS::Kill( qint64 pid )
	{
		QStringList args = { "-9", QObject::tr("%1").arg(pid) };
		qint64 rPid = 0;
		return DetachRun( "kill", args, rPid);
	}

	bool PROCESS::Kill( std::string program, std::string arg )
	{
		DIR* dir;
		struct dirent* ent;
		char* endptr;
		char namebuf[128];
		char filebuf[256];
		bool bArgCheck = arg.empty()?false:true;

		if (!(dir = opendir("/proc"))) {
			perror("can't open /proc");
			return -1;
		}

		while( (ent = readdir(dir)) != NULL )
		{
			long lpid = strtol(ent->d_name, &endptr, 10);
			if (*endptr != '\0') {
				continue;
			}

			snprintf(namebuf, sizeof(namebuf), "/proc/%ld/cmdline", lpid);
			::FILE* fp = ::fopen(namebuf, "r");
			if ( fp )
			{
				if ( fgets(filebuf, sizeof(filebuf), fp) != NULL )
				{
					char* next_ptr = nullptr;
					char* ret_ptr  = strtok_r (filebuf, " ", &next_ptr);
					bool bFoundPrg = false;
					bool bFoundArg = bArgCheck?false:true;
					while( ret_ptr )
					{
						if ( strstr(ret_ptr, program.c_str()) != NULL )
						{
							bFoundPrg = true;
						}
						if( bArgCheck )
						{
							if( !arg.empty() && ( strstr(ret_ptr, arg.c_str()) != NULL ) )
							{
								bFoundArg = true;
							}
						}
						ret_ptr  = strtok_r (NULL, " ", &next_ptr);
					}
					if( bFoundPrg && bFoundArg )
					{
						QStringList args = { "-9", QObject::tr("%1").arg(lpid) };
						qint64 pid = 0;
						fprintf( stdout, "kill %s[pid:%ld]\n", program.c_str(), lpid );
						DetachRun( "kill", args, pid );
					}
				}
				fclose(fp);
			}
		}
		closedir(dir);
		return false;
	}

	//2023.07.10 - kwmoon
	bool PROCESS::writePid( QString atname, const QVariant pid)
	{
		char* env_os  = getenv(ATOS_DIR_ENV);
		QString os_path = "";
		if( env_os != NULL )
		{
			os_path = QString(env_os);
		}
		else
		{
			os_path = DIR_ATOS_PATH;
		}
		QString ini_file = QObject::tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(PID_INI);

		QSettings settings(ini_file, QSettings::IniFormat);
		QString key = QObject::tr("%1/%2").arg(SECTION_ATSYS_PID).arg(atname);
		if( !settings.isWritable() ) return false;
		settings.setValue(key, pid);
		settings.sync();
		return true;
	}

	//2023.07.10 - kwmoon
	bool PROCESS::readPid( QString atname, qint64& pid)
	{
		char* env_os  = getenv(ATOS_DIR_ENV);
		QString os_path = "";
		if( env_os != NULL )
		{
			os_path = QString(env_os);
		}
		else
		{
			os_path = DIR_ATOS_PATH;
		}
		QString ini_file = QObject::tr("%1/%2/%3").arg(os_path).arg(CONFIG_DIR).arg(PID_INI);

		bool bOk = false;
		QSettings settings(ini_file, QSettings::IniFormat);
		QString key = QObject::tr("%1/%2").arg(SECTION_ATSYS_PID).arg(atname);
		if( !settings.contains(key) ) return false;
		pid = settings.value(key).toLongLong(&bOk);
		if( bOk ) return true;
		else      return false;
		return true;
	}

	//CHMB PROTOCOL
	QString CHAMBER::PLCCMD_STR(PROTOCOL::PLC_COMMAND cmd)
	{
		QString scmd = "";
		switch ( cmd )
		{
			case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_RUN_TRIGGER          : { scmd = "CMD_WB_TEMP_RUN_TRIGGER          "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_TEMP_STOP_TRIGGER         : { scmd = "CMD_WB_TEMP_STOP_TRIGGER         "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_MODE_TRIGGER          : { scmd = "CMD_WB_AMB_MODE_TRIGGER          "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_AMB_AUTO_TRIGGER          : { scmd = "CMD_WB_AMB_AUTO_TRIGGER          "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_ALARM_RESET               : { scmd = "CMD_WB_ALARM_RESET               "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_MASK_ONOFF_TRIGGER        : { scmd = "CMD_WB_MASK_ONOFF_TRIGGER        "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_OPEN_TRIGGER         : { scmd = "CMD_WB_DOOR_OPEN_TRIGGER         "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_CLOSE_TRIGGER        : { scmd = "CMD_WB_DOOR_CLOSE_TRIGGER        "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_IN_TRIGGER           : { scmd = "CMD_WB_RACK_IN_TRIGGER           "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_OUT_TRIGGER          : { scmd = "CMD_WB_RACK_OUT_TRIGGER          "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_RACK_NOP_TRIGGER          : { scmd = "CMD_WB_RACK_NOP_TRIGGER          "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TIME_SET         : { scmd = "CMD_WW_WATCHDOG_TIME_SET         "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_TOGGLE_TRIGGER   : { scmd = "CMD_WW_WATCHDOG_TOGGLE_TRIGGER   "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WW_WATCHDOG_ENB_DSB          : { scmd = "CMD_WW_WATCHDOG_ENB_DSB          "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WW_SV_WRITE                  : { scmd = "CMD_WW_SV_WRITE                  "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WW_AMB_SV_WRITE              : { scmd = "CMD_WW_AMB_SV_WRITE              "; break; }

			case PROTOCOL::PLC_COMMAND::CMD_RB_RACK_IN                   : { scmd = "CMD_RB_RACK_IN                   "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RB_RACK_OUT                  : { scmd = "CMD_RB_RACK_OUT                  "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RB_DOOR_OPEN                 : { scmd = "CMD_RB_DOOR_OPEN                 "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RB_DOOR_CLOSE                : { scmd = "CMD_RB_DOOR_CLOSE                "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RB_TEMP_RUN                  : { scmd = "CMD_RB_TEMP_RUN                  "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RB_TEMP_STOP                 : { scmd = "CMD_RB_TEMP_STOP                 "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RB_AMBIENT                   : { scmd = "CMD_RB_AMBIENT                   "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_PLC_VER                   : { scmd = "CMD_RW_PLC_VER                   "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_PV                        : { scmd = "CMD_RW_PV                        "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_SV                        : { scmd = "CMD_RW_SV                        "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_PCW_TEMP                  : { scmd = "CMD_RW_PCW_TEMP                  "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_WPS_PRESSURE              : { scmd = "CMD_RW_WPS_PRESSURE              "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_PCW_FLOW                  : { scmd = "CMD_RW_PCW_FLOW                  "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_STATUS1                   : { scmd = "CMD_RW_STATUS1                   "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_STATUS2                   : { scmd = "CMD_RW_STATUS2                   "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM0                    : { scmd = "CMD_RW_ALARM0                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM1                    : { scmd = "CMD_RW_ALARM1                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM2                    : { scmd = "CMD_RW_ALARM2                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM3                    : { scmd = "CMD_RW_ALARM3                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM4                    : { scmd = "CMD_RW_ALARM4                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM5                    : { scmd = "CMD_RW_ALARM5                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM6                    : { scmd = "CMD_RW_ALARM6                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM7                    : { scmd = "CMD_RW_ALARM7                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM8                    : { scmd = "CMD_RW_ALARM8                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RW_ALARM9                    : { scmd = "CMD_RW_ALARM9                    "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_SYS_PLC_CONN                 : { scmd = "CMD_SYS_PLC_CONN                 "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_BUZZER_ONOFF              : { scmd = "CMD_WB_BUZZER_ONOFF              "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_LAMP_MASK_ONOFF           : { scmd = "CMD_WB_LAMP_MASK_ONOFF           "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_ON               : { scmd = "CMD_WB_RED_LAMP_ON               "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_ON            : { scmd = "CMD_WB_YELLOW_LAMP_ON            "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_ON             : { scmd = "CMD_WB_GREEN_LAMP_ON             "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_RED_LAMP_BLINK            : { scmd = "CMD_WB_RED_LAMP_BLINK            "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_YELLOW_LAMP_BLINK         : { scmd = "CMD_WB_YELLOW_LAMP_BLINK         "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_GREEN_LAMP_BLINK          : { scmd = "CMD_WB_GREEN_LAMP_BLINK          "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_WB_ALL_LAMP_OFF              : { scmd = "CMD_WB_ALL_LAMP_OFF              "; break; }
			case PROTOCOL::PLC_COMMAND::CMD_RB_RACK_NOP_TRIGGER          : { scmd = "CMD_RB_RACK_NOP_TRIGGER          "; break; }	//2023.09.08 - KWMOON
			case PROTOCOL::PLC_COMMAND::CMD_RB_FINGER_UP_STAT_RACKIN     : { scmd = "CMD_RB_FINGER_UP_STAT_RACKIN     "; break; }	//2023.09.08 - KWMOON
			case PROTOCOL::PLC_COMMAND::CMD_RB_DOOR_IGNORE_RACKINOUT     : { scmd = "CMD_RB_DOOR_IGNORE_RACKINOUT     "; break; }	//2023.09.08 - KWMOON
			case PROTOCOL::PLC_COMMAND::CMD_WB_FINGER_UP_STAT_RACKIN_ON  : { scmd = "CMD_WB_FINGER_UP_RACKIN_ON       "; break; }	//2023.09.08 - KWMOON
			case PROTOCOL::PLC_COMMAND::CMD_WB_FINGER_UP_STAT_RACKIN_OFF : { scmd = "CMD_WB_FINGER_UP_RACKIN_OFF      "; break; }	//2023.09.08 - KWMOON
			case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_IGNORE_RACK_INOUT_ON : { scmd = "CMD_WB_DOOR_IGNORE_RACK_INOUT_ON "; break; }	//2023.09.08 - KWMOON
			case PROTOCOL::PLC_COMMAND::CMD_WB_DOOR_IGNORE_RACK_INOUT_OFF: { scmd = "CMD_WB_DOOR_IGNORE_RACK_INOUT_OFF"; break; }	//2023.09.08 - KWMOON
			case PROTOCOL::PLC_COMMAND::CMD_WB_SYSTEM_DC_ONOFF           : { scmd = "CMD_WB_SYSTEM_DC_ONOFF           "; break; }	//2023.09.08 - KWMOON
			default                                                      : { scmd = "CMD_PLC_UNKNOWN                  "; break; }
		}
		return scmd;
	}

	//CHMB PROTOCOL
	QString CHAMBER::PLCADDR_STR(PLC_ADDRESS_MAP addr)
	{
		QString scmd = "";
		switch ( addr )
		{
			case PLC_ADDRESS_MAP::ADDR_PLC_W_TEMP_RUN                 :  { scmd = "ADDR_PLC_W_TEMP_RUN                 "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_TEMP_STOP                :  { scmd = "ADDR_PLC_W_TEMP_STOP                "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_AMBIENT_RUN              :  { scmd = "ADDR_PLC_W_AMBIENT_RUN              "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_AMBIENT_AUTO_RUN         :  { scmd = "ADDR_PLC_W_AMBIENT_AUTO_RUN         "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_ALARM_RESET              :  { scmd = "ADDR_PLC_W_ALARM_RESET              "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_MASK_ON_OFF              :  { scmd = "ADDR_PLC_W_MASK_ON_OFF              "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_DOOR_OPEN_RUN            :  { scmd = "ADDR_PLC_W_DOOR_OPEN_RUN            "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_DOOR_CLOSE_RUN           :  { scmd = "ADDR_PLC_W_DOOR_CLOSE_RUN           "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_RACK_IN_RUN              :  { scmd = "ADDR_PLC_W_RACK_IN_RUN              "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_RACK_OUT_RUN             :  { scmd = "ADDR_PLC_W_RACK_OUT_RUN             "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_RACK_NOP                 :  { scmd = "ADDR_PLC_W_RACK_NOP                 "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_WATCHDOG_TIME_SET        :  { scmd = "ADDR_PLC_W_WATCHDOG_TIME_SET        "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_WATCHDOG_TOGGLE          :  { scmd = "ADDR_PLC_W_WATCHDOG_TOGGLE          "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_WATCHDOG_ENABLE          :  { scmd = "ADDR_PLC_W_WATCHDOG_ENABLE          "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_SV_SET                   :  { scmd = "ADDR_PLC_W_SV_SET                   "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_AMBIENT_SV_SET           :  { scmd = "ADDR_PLC_W_AMBIENT_SV_SET           "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_WORD_STATUS1             :  { scmd = "ADDR_PLC_R_WORD_STATUS1             "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_MASK_OFF                 :  { scmd = "ADDR_PLC_R_MASK_OFF                 "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_MASK_ON                  :  { scmd = "ADDR_PLC_R_MASK_ON                  "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_WATCHDOG_ENABLE          :  { scmd = "ADDR_PLC_R_WATCHDOG_ENABLE          "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_WATCHDOG_DISABLE         :  { scmd = "ADDR_PLC_R_WATCHDOG_DISABLE         "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_DOOR_OPEN                :  { scmd = "ADDR_PLC_R_DOOR_OPEN                "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_DOOR_CLOSE               :  { scmd = "ADDR_PLC_R_DOOR_CLOSE               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_UPPER_RACK_DETECT        :  { scmd = "ADDR_PLC_R_UPPER_RACK_DETECT        "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_BOTTOM_RACK_DETECT       :  { scmd = "ADDR_PLC_R_BOTTOM_RACK_DETECT       "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_RACK_IN_COMPLETE         :  { scmd = "ADDR_PLC_R_RACK_IN_COMPLETE         "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_RACK_OUT_COMPLETE        :  { scmd = "ADDR_PLC_R_RACK_OUT_COMPLETE        "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_EQ_READY                 :  { scmd = "ADDR_PLC_R_EQ_READY                 "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_EQ_ABORT                 :  { scmd = "ADDR_PLC_R_EQ_ABORT                 "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_AVG_COMPLETE             :  { scmd = "ADDR_PLC_R_AVG_COMPLETE             "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_WORD_STATUS2             :  { scmd = "ADDR_PLC_R_WORD_STATUS2             "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_TEMP_RUN                 :  { scmd = "ADDR_PLC_R_TEMP_RUN                 "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_TEMP_STOP                :  { scmd = "ADDR_PLC_R_TEMP_STOP                "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_AMBIENT                  :  { scmd = "ADDR_PLC_R_AMBIENT                  "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_SYSTEM_DC_POWER_ON       :  { scmd = "ADDR_PLC_R_SYSTEM_DC_POWER_ON       "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_VERSION                  :  { scmd = "ADDR_PLC_R_VERSION                  "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_PV                       :  { scmd = "ADDR_PLC_R_PV                       "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_SV                       :  { scmd = "ADDR_PLC_R_SV                       "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_PCW_TEMPERATURE          :  { scmd = "ADDR_PLC_R_PCW_TEMPERATURE          "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_WPS_PRESSURE             :  { scmd = "ADDR_PLC_R_WPS_PRESSURE             "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_PCW_FLOW                 :  { scmd = "ADDR_PLC_R_PCW_FLOW                 "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D150               :  { scmd = "ADDR_PLC_R_ALARM_D150               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D151               :  { scmd = "ADDR_PLC_R_ALARM_D151               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D152               :  { scmd = "ADDR_PLC_R_ALARM_D152               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D153               :  { scmd = "ADDR_PLC_R_ALARM_D153               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D154               :  { scmd = "ADDR_PLC_R_ALARM_D154               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D155               :  { scmd = "ADDR_PLC_R_ALARM_D155               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D156               :  { scmd = "ADDR_PLC_R_ALARM_D156               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D157               :  { scmd = "ADDR_PLC_R_ALARM_D157               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D158               :  { scmd = "ADDR_PLC_R_ALARM_D158               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_ALARM_D159               :  { scmd = "ADDR_PLC_R_ALARM_D159               "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_BUZZER_ON_OFF            :  { scmd = "ADDR_PLC_W_BUZZER_ON_OFF            "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_LAMP_LOCAL_REMOTE        :  { scmd = "ADDR_PLC_W_LAMP_LOCAL_REMOTE        "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_RED_LAMP_ON              :  { scmd = "ADDR_PLC_W_RED_LAMP_ON              "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_YELLOW_LAMP_ON           :  { scmd = "ADDR_PLC_W_YELLOW_LAMP_ON           "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_GREEN_LAMP_ON            :  { scmd = "ADDR_PLC_W_GREEN_LAMP_ON            "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_RED_LAMP_BLINK           :  { scmd = "ADDR_PLC_W_RED_LAMP_BLINK           "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_YELLOW_LAMP_BLINK        :  { scmd = "ADDR_PLC_W_YELLOW_LAMP_BLINK        "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_GREEN_LAMP_BLINK         :  { scmd = "ADDR_PLC_W_GREEN_LAMP_BLINK         "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_W_ALL_LAMP_OFF             :  { scmd = "ADDR_PLC_W_ALL_LAMP_OFF             "; break; }
			case PLC_ADDRESS_MAP::ADDR_PLC_R_RACK_NOP                 :  { scmd = "ADDR_PLC_R_RACK_NOP                 "; break; }	//2023.09.09 - KWMOON
			case PLC_ADDRESS_MAP::ADDR_PLC_W_FINGER_RACKIN_ON         :  { scmd = "ADDR_PLC_W_FINGER_RACKIN_ON         "; break; }	//2023.09.09 - KWMOON
			case PLC_ADDRESS_MAP::ADDR_PLC_W_FINGER_RACKIN_OFF        :  { scmd = "ADDR_PLC_W_FINGER_RACKIN_OFF        "; break; }	//2023.09.09 - KWMOON
			case PLC_ADDRESS_MAP::ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_ON :  { scmd = "ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_ON "; break; }	//2023.09.09 - KWMOON
			case PLC_ADDRESS_MAP::ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_OFF:  { scmd = "ADDR_PLC_W_DOOR_IGNORE_RACKINOUT_OFF"; break; }	//2023.09.09 - KWMOON
			default                                                   :  { scmd = "ADDR_PLC_ADUNKNOWN                  "; break; }
		}
		return scmd;
	}

	int FTP::PUT(const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFile, const char *pszMode)
	{
		int ret;
		char buf[PATH_MAX] = { 0 };

		snprintf(buf, sizeof(buf), "atftp.cc \"open %s\" \"user %s %s\" \"cd %s\" \"lcd %s\" \"%s\" \"put %s\" \"bye\"",
				 pszIp  ,
				 pszId  ,
				 pszPass,
				 pszDirR,
				 pszDirL,
				 pszMode,
				 pszFile );
		ret = system(buf);
		printf("%s\n", buf);
		return ret;
	}

	int FTP::GET(const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFile, const char *pszMode)
	{
		int ret;
		char buf[PATH_MAX] = { 0 };

		snprintf(buf, sizeof(buf), "atftp.cc \"open %s\" \"user %s %s\" \"cd %s\" \"lcd %s\" \"%s\" \"get %s\" \"bye\"",
				 pszIp  ,
				 pszId  ,
				 pszPass,
				 pszDirR,
				 pszDirL,
				 pszMode,
				 pszFile );
		ret = system(buf);
		printf("%s\n", buf);
		return ret;
	}

	int FTP::MPUT(const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFiles, const char *pszMode)
	{
		int ret;
		char buf[PATH_MAX] = { 0 };

		snprintf(buf, sizeof(buf), "atftp.cc \"open %s\" \"user %s %s\" \"cd %s\" \"lcd %s\" \"%s\" \"prompt\" \"mput %s\" \"bye\"",
				 pszIp  ,
				 pszId  ,
				 pszPass,
				 pszDirR,
				 pszDirL,
				 pszMode,
				 pszFiles );
		ret = system(buf);
//		printf("%s: RTN:%d %s\n", __FUNCTION__, ret, buf );
		printf("%s\n", buf);
		return ret;
	}

	int FTP::MGET(const char *pszIp, const char *pszId, const char *pszPass, const char *pszDirR, const char *pszDirL, const char *pszFiles, const char *pszMode)
	{
		int ret;
		char buf[PATH_MAX] = { 0 };

		snprintf(buf, sizeof(buf), "atftp.cc \"open %s\" \"user %s %s\" \"cd %s\" \"lcd %s\" \"%s\" \"prompt\" \"mget %s\" \"bye\"",
				 pszIp  ,
				 pszId  ,
				 pszPass,
				 pszDirR,
				 pszDirL,
				 pszMode,
				 pszFiles );
		ret = system(buf);
		printf("%s\n", buf);
		return ret;
	}

};
