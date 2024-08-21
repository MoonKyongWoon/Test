#include "syspwrth.hpp"

CSysPwrTh::CSysPwrTh(SIDE_AB ch, std::map<QString, stPmubusCommand>& isocom, std::map<QString, stPmubusCommand>& module, QObject *parent)
    : QObject(parent), m_ch(ch), m_mapPbCmdIsoComm(isocom), m_mapPbCmdModule(module), m_pRs485(nullptr)
{
	CreateRs485();
}

CSysPwrTh::~CSysPwrTh()
{
	if( m_pRs485 )
	{
		if( m_pRs485->IsOpen() )
		{
			m_pRs485->CloseDevice();
		}
		delete m_pRs485;
		m_pRs485 = nullptr;
	}
}

void CSysPwrTh::CreateRs485()
{
	m_pRs485 = new CRS485(m_ch, this);
	connect( m_pRs485, &CRS485::SigLogMessage, [=](SIDE_AB ch, LOG_TYPE t, QString l) { emit SigLogMessage(ch,t,l);} );
}

void CSysPwrTh::SetSerialInfo(QString tty)
{
	if( m_pRs485 == nullptr )
	{
		//error message
		return;
	}
	m_stty = tty;
	m_pRs485->SetSerialInfo( m_stty );
	SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1]port info : %2").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
}


void CSysPwrTh::SlotOpenDevice()
{
    if( IsOpen() )
    {
        emit SigOpenDeviceResult( m_ch, true );
    }
    else
    {
        m_pRs485->OpenDevice();
        emit SigOpenDeviceResult( m_ch, m_pRs485->IsOpen() );
    }
}

void CSysPwrTh::SlotCloseDevice()
{
    if( IsOpen() )
    {
        m_pRs485->CloseDevice();
    }
	SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1]Close Device : %2").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
}

bool CSysPwrTh::IsOpen()
{
	return m_pRs485->IsOpen();
}

long long int CSysPwrTh::Delay(double dTime)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    long long wait_time = static_cast<long long>( dTime * 1.0e6 );	//US
    while ( true )
    {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        long long now_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        if( now_time > wait_time ) break;
    }
    return 0;
}


void CSysPwrTh::SlotManualTest(stUiRequest request)
{
	if( request.uSide != m_ch ) return;
	if( !IsOpen() ) return;

	char  szRecvData[128] = {0x0,};
	char  szSendData[128] = {0x0,};
	char  szSendBuffer[128] = {0x0,};

	unsigned char cmd     = request.stPmbusCmd.uCommand;
	std::string scmd      = request.stPmbusCmd.sCommand;
	PMBUS_OPENRATION opr  = request.stPmbusCmd.enCmdType;
	PMBUS_DATA_FORMAT fmt = request.stPmbusCmd.enDataFormat;
	unsigned char offset  = request.stPmbusCmd.uOffsetData;
	unsigned char bytes   = request.stPmbusCmd.uNumOfByte + offset;
	unsigned short multi  = request.stPmbusCmd.uMultiplier;

	int device_address = request.uDevice;
	int data           = request.uData;

	int nWriteSize = 0;
	if( opr == PMBUS_OPENRATION::PMBUS_READ )
	{
		BuildPMBusRequest( cmd, opr, (DEVICE_ADDRESS)device_address, 0x0  );
		memcpy(szSendBuffer, &m_stPmbusReqHead, sizeof(stPmbusReqHead));
		nWriteSize = sizeof(m_stPmbusReqHead);
	}
	else
	{
		BuildPMBusRequest( cmd, opr, (DEVICE_ADDRESS)device_address, bytes  );
        if( request.stPmbusCmd.uNumOfByte > 0 )
        {
            szSendData[0] = m_stUiRequest.stPmbusCmd.uNumOfByte;
            int nPos=0;
            for (int i = offset; i<bytes; i++)
            {
                szSendData[i] = (data >> (nPos * 8)) & 0xFF;
                nPos++;
            }
            memcpy(szSendBuffer, &m_stPmbusReqHead, sizeof(stPmbusReqHead));
            memcpy(szSendBuffer + sizeof(stPmbusReqHead), &szSendData, bytes);
            nWriteSize = sizeof(stPmbusReqHead) + bytes;
        }
        else
        {
            memcpy(szSendBuffer, &m_stPmbusReqHead, sizeof(stPmbusReqHead));
            nWriteSize = sizeof(stPmbusReqHead);
        }
	}
	if( !PMBusWrite( szSendBuffer, nWriteSize) )
	{
		emit SigLogMessage(m_ch, _ERROR_LOG, tr("[CH%1][ ERR] %2 send error!").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()) );
		return;
	}
	emit SigLogMessage(m_ch, _NORMAL_LOG, tr("[CH%1][SEND] %2 : %3 Bytes!").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()).arg(nWriteSize) );

	uchar errCode = PMBusRead( cmd, device_address, szRecvData);
	if (errCode != ERR_SUCCESS)
	{
		emit SigLogMessage(m_ch, _ERROR_LOG, tr("[CH%1][RECV] %2 : [ErrCode:%3]").arg((m_ch==SIDE_A)?"A":"B").arg(request.stPmbusCmd.sCommand.c_str()).arg(ErrorCodeString(errCode)) );
        return;
	}

	uchar uSize = request.stPmbusCmd.uNumOfByte;
	uint uResponseSize = 0x0;
	uint uData = 0x0;
	double dData = 0x0;
	char szAscii[32] = {0x0,};

	if (opr == PMBUS_READ) uResponseSize = m_stPmbusResponse.ResponseDataLen - offset;
	else                   uResponseSize = m_stPmbusResponse.ResponseDataLen;

	switch( fmt )
    {
		case FORMAT_BITMAPPED:
			for ( unsigned int i = 0; i<uResponseSize; i++)
			{
				uint32_t data = (uint32_t)szRecvData[i + offset] & 0xFF;
				uData |= (data << (i * 8));
			}
			emit SigLogMessage( m_ch, _NORMAL_LOG,
								tr("[CH%1][RECV] %2 : 0x%3").arg((m_ch==SIDE_A)?"A":"B").arg(request.stPmbusCmd.sCommand.c_str()).arg(uData,0,16) );
			break;
		break;

		case FORMAT_LINEAR:
			for (unsigned int i = 0; i<uResponseSize; i++)
			{
				uint32_t data = (uint32_t)szRecvData[i + offset] & 0xFF;
				uData |= (data << (i * 8));
			}
			emit SigLogMessage( m_ch, _NORMAL_LOG,
								tr("[CH%1][RECV] %2 : %3").arg((m_ch==SIDE_A)?"A":"B").arg(request.stPmbusCmd.sCommand.c_str()).arg(L11ToDouble(uData)) );
			break;
		case FORMAT_ASCII:
			for (unsigned int i = 0; i<uResponseSize - 1; i++)
				szAscii[i] = szRecvData[1 + i];
			emit SigLogMessage( m_ch, _NORMAL_LOG,
								tr("[CH%1][RECV] %2 : %3").arg((m_ch==SIDE_A)?"A":"B").arg(request.stPmbusCmd.sCommand.c_str()).arg(szAscii) );
			break;
		case FORMAT_DIRECT:
			for (unsigned int i = 0; i<uResponseSize; i++)
			{
				uint32_t data = (uint32_t)szRecvData[i + offset] & 0xFF;
				uData |= (data<<(i*8));
			}
			dData = static_cast<double>(uData) / multi;
			emit SigLogMessage( m_ch, _NORMAL_LOG,
								tr("[CH%1][RECV] %2 : %3").arg((m_ch==SIDE_A)?"A":"B").arg(request.stPmbusCmd.sCommand.c_str()).arg(dData) );
			break;
        default:
                break;
	}
}

void CSysPwrTh::SlotTextTest( SIDE_AB side, QString sCmd)
{
	if( side != m_ch ) return;

	char  szRecvData[128] = {0x0,};
	char  szSendData[128] = {0x0,};
	int   nWriteSize = 0x0;
	snprintf( szSendData, sizeof(szSendData), "%s", sCmd.toStdString().c_str() );

	nWriteSize = strlen(szSendData);
	emit SigLogMessage(m_ch, _NORMAL_LOG, tr("[CH%1][SEND] %2 : %3 Bytes!").arg((m_ch==SIDE_A)?"A":"B").arg(sCmd).arg(nWriteSize) );
	if( !PMBusWrite( szSendData, nWriteSize) )
	{
		emit SigLogMessage(m_ch, _ERROR_LOG, tr("[CH%1][ ERR] %2 send error!").arg((m_ch==SIDE_A)?"A":"B").arg(sCmd) );
		return;
	}
    Delay( 100 MS );
	//Delay( 0.5 );

	//uchar length = m_pRs485->ReadData( szRecvData );
	//QString sRecvData = QString(szRecvData).replace("\n","");
	//emit SigLogMessage(m_ch, _NORMAL_LOG, tr("[CH%1][RECV] %2 : %3 [Length:%4]").arg((m_ch==SIDE_A)?"A":"B").arg(sCmd).arg(sRecvData).arg(length) );
}

void CSysPwrTh::BuildPMBusRequest( int pmbuscmd, PMBUS_OPENRATION opr, DEVICE_ADDRESS addr, int size )
{
	static uint32_t m_uMsgId = 0;

    memset( &m_stPmbusReqHead, 0x0, sizeof(stPmbusReqHead) );
	//BYTE #1-4 MSG ID
	m_stPmbusReqHead.szMsgId[0] = (m_uMsgId >> 24) & 0xFF;
	m_stPmbusReqHead.szMsgId[1] = (m_uMsgId >> 16) & 0xFF;
	m_stPmbusReqHead.szMsgId[2] = (m_uMsgId >>  8) & 0xFF;
	m_stPmbusReqHead.szMsgId[3] = (m_uMsgId >>  0) & 0xFF;
	//m_uMsgId++;
	if (m_uMsgId > 0xFFFFFFE) m_uMsgId = 0;

	//BYTE #5 (0x81)
    m_stPmbusReqHead.Type      = 0x1;	//Fix
    m_stPmbusReqHead.Reserved0 = 0x0;
    m_stPmbusReqHead.SplitType = 0x0;
    m_stPmbusReqHead.Reserved1 = 0x0;
    m_stPmbusReqHead.NoOfCmds  = 0x1;	//Fix

	//BYTE #6 (RACK/MODULE ADDR)
	m_stPmbusReqHead.iHpRackAddr     = 0x0;		//Rack
	m_stPmbusReqHead.InternalDevAddr = addr;	//Module

	//BYTE #7
	m_stPmbusReqHead.Operation = opr;
	if (m_stPmbusReqHead.Operation == PMBUS_READ)
		m_stPmbusReqHead.CmdDataLen = 0x0;
	else
		m_stPmbusReqHead.CmdDataLen = size;

	//BYTE #8 PMBus Command
	m_stPmbusReqHead.Command = pmbuscmd;
}

bool CSysPwrTh::PMBusWrite( char* buffer, int size )
{
	return m_pRs485->SendData( buffer, size);
}

bool CSysPwrTh::SerialRead()
{
	char szRecv[64] = {0x0,};
    m_pRs485->SetRTS( 0 );
	return m_pRs485->RecvData( szRecv, sizeof(szRecv) );
}

uchar CSysPwrTh::PMBusRead( uchar cmd, int dev, char* pRecv )
{
    m_pRs485->SetRTS( 0 );
	char szBuffer[128] = { 0x0, };
	memset( &m_stPmbusResponse, 0x0, sizeof(stPmbusResponse) );
	if( !m_pRs485->RecvData( (char*)&m_stPmbusResponse, sizeof(m_stPmbusResponse)) )
	{
		emit SigLogMessage( m_ch, _ERROR_LOG,  tr("[CH%1][ ERR] Recv Head Error!").arg((m_ch==SIDE_A)?"A":"B") );
        return ERR_CUSTOM_INVALID;
	}
	memcpy(szBuffer, &m_stPmbusResponse, sizeof(m_stPmbusResponse));

	int dLen = m_stPmbusResponse.ResponseDataLen + 2;
	uchar errcode = IsValidResponse();
	if ( errcode == ERR_SUCCESS )
	{
		if( !m_pRs485->RecvData( pRecv, dLen ) )
		{
			emit SigLogMessage( m_ch, _ERROR_LOG,  tr("[CH%1][ ERR] Recv Data Error!").arg((m_ch==SIDE_A)?"A":"B") );
			return ERR_CUSTOM_INVALID;
		}
	}
	return errcode;
}

uchar CSysPwrTh::IsValidResponse()
{
	uchar uRtnCode = ERR_SUCCESS;
	if ( m_stPmbusResponse.ErrorCode != ERR_SUCCESS )
	{
		uRtnCode = m_stPmbusResponse.ErrorCode;
		emit SigLogMessage( m_ch, _ERROR_LOG,
							tr("[CH%1][ ERR] Code : 0x%2, Desc : %3\n").arg((m_ch==SIDE_A)?"A":"B").arg(m_stPmbusResponse.ErrorCode,16).arg( ErrorCodeString(m_stPmbusResponse.ErrorCode)) );;
	}
	return uRtnCode;
}

QString CSysPwrTh::ErrorCodeString(uchar ErrCode)
{
	QString strErrCode;
	switch (ErrCode)
	{
	case ERR_RACK_NOT_EXISTING:
		strErrCode = "ERR_RACK_NOT_EXISTING";
		break;
	case ERR_DEVICE_NOT_EXISTING:
		strErrCode = "ERR_DEVICE_NOT_EXISTING";
		break;
	case ERR_UNSUPPORTED_CMD:
		strErrCode = "ERR_UNSUPPORTED_CMD";
		break;
	case ERR_OPERATION_INVALID:
		strErrCode = "ERR_OPERATION_INVALID";
		break;
	case ERR_LENGTH_INVALID:
		strErrCode = "ERR_LENGTH_INVALID";
		break;
	case ERR_DATA_INVALID:
		strErrCode = "ERR_DATA_INVALID";
		break;
	case ERR_WRITE_PROTECT:
		strErrCode = "ERR_WRITE_PROTECT";
		break;
	case ERR_PROTOCOL_INVALID:
		strErrCode = "ERR_PROTOCOL_INVALID";
		break;
	default:
		strErrCode = "UNKNOWN_ERROR_CODE";
		break;
	}
	return strErrCode;
}

double CSysPwrTh::L11ToDouble(uint16_t input_val)
{
	// extract exponent as MS 5 bits
	uchar exponent = input_val >> 11;
	// extract mantissa as LS 11 bits
	uint16_t mantissa = input_val & 0x7ff;
	// sign extend exponent from 5 to 8 bits
	if (exponent > 0x0F) exponent |= 0xE0;
	// sign extend mantissa from 11 to 16 bits
	if (mantissa > 0x03FF) mantissa |= 0xF800;

	// compute value as mantissa * 2^(exponent)
	return mantissa * pow(2, exponent);
}

uint16_t CSysPwrTh::DoubleToL11(double input_val)
{
	// set exponent to -16
	int exponent = -16;
	// extract mantissa from input value
	int mantissa =
		(int)(input_val / pow(2.0, exponent));
	// Search for an exponent that produces
	// a valid 11-bit mantissa
	do
	{
		if ((mantissa >= -1024) &&
			(mantissa <= +1023))
		{
			break; // stop if mantissa valid
		}
		exponent++;
		mantissa =
			(int)(input_val / pow(2.0, exponent));
	} while (exponent < +15);

	// Format the exponent of the L11
	uint16_t uExponent = exponent << 11;

	// Format the mantissa of the L11
	uint16_t uMantissa = mantissa & 0x07FF;
	// Compute value as exponent | mantissa
	return uExponent | uMantissa;
}

void CSysPwrTh::SlotMonitoring(SIDE_AB ab)
{
    if( ab != m_ch ) return;

    GetIsoComm();
    GetModules();
}

bool CSysPwrTh::GetIsoComm()
{
    QString sCmd[2][MAX_LINE] = { {"R_READ_VIN1","R_READ_VIN2","R_READ_VIN3"}, {"R_READ_IIN1","R_READ_IIN2","R_READ_IIN3"} };
    stUiRequest request;
    stLineMeas LineMeas;
    char  szRecvData[128]   = {0x0,};
    char  szSendBuffer[128] = {0x0,};

    memset( &LineMeas, 0x0, sizeof(stLineMeas) );
    for( int vi=0;vi<MAX_LINE_ITEM;vi++ )
    {
        int nWriteSize = 0;
        for( int line=0;line<MAX_LINE;line++ )
        {
            std::map<QString, stPmubusCommand>::iterator it = m_mapPbCmdIsoComm.find( sCmd[vi][line] );
            if( it == m_mapPbCmdIsoComm.end() ) continue;

            request.stPmbusCmd = it->second;
            std::string scmd      = request.stPmbusCmd.sCommand;
            unsigned char cmd     = request.stPmbusCmd.uCommand;
            PMBUS_OPENRATION opr  = request.stPmbusCmd.enCmdType;
            //PMBUS_DATA_FORMAT fmt = request.stPmbusCmd.enDataFormat;
            unsigned char offset  = request.stPmbusCmd.uOffsetData;
            unsigned short multi  = request.stPmbusCmd.uMultiplier;
            int devaddr           = DEVICE_ADDRESS::DEV_ISOCOMM;    //request.uDevice;

            BuildPMBusRequest( cmd, opr, (DEVICE_ADDRESS)devaddr, 0x0 );
            memset( szSendBuffer, 0x0, sizeof(szSendBuffer) );
            memcpy(szSendBuffer, &m_stPmbusReqHead, sizeof(stPmbusReqHead));
            nWriteSize = sizeof(m_stPmbusReqHead);

            if( !PMBusWrite( szSendBuffer, nWriteSize) )
            {
                emit SigLogMessage(m_ch, _ERROR_LOG, tr("[CH%1][ ERR] %2 send error!").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()) );
                return false;
            }
            uchar errCode = PMBusRead( cmd, devaddr, szRecvData);
            if (errCode != ERR_SUCCESS)
            {
                emit SigLogMessage(m_ch, _ERROR_LOG, tr("[CH%1][RECV] %2 : [ErrCode:%3]").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()).arg(ErrorCodeString(errCode)) );
                return false;
            }
            //uchar uSize   = request.stPmbusCmd.uNumOfByte;
            uint uResSize = 0x0;
            uint uData    = 0x0;
            if (opr == PMBUS_READ) uResSize = m_stPmbusResponse.ResponseDataLen - offset;
            else                   uResSize = m_stPmbusResponse.ResponseDataLen;

            //Direct Format
            for (unsigned int i = 0; i<uResSize; i++)
            {
                uint32_t data = (uint32_t)szRecvData[i + offset] & 0xFF;
                uData |= (data<<(i*8));
            }
            double dData = static_cast<double>(uData) / multi;
            if( vi == 0 )
                LineMeas.Voltage[line] = dData;
            else
                LineMeas.Current[line] = dData;
            emit SigLogMessage( m_ch, _NORMAL_LOG,
                                tr("[CH%1][RECV] %2 : %3").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()).arg(dData) );
            Delay( 50 MS );
        }
    }
    emit SigIsoComm( m_ch, LineMeas );
    return true;
}


bool CSysPwrTh::GetModules()
{
    QString sCmd[MAX_MODULE_ITEM] = { "R_READ_VOUT", "R_READ_IOUT", "R_POUT_MAX", "R_TEMPERATUR_2", "R_STATUS_WORD" };
    stUiRequest request;
    stModulelMeas ModuleData;
    char  szRecvData[128]   = {0x0,};
    char  szSendBuffer[128] = {0x0,};

    memset( &ModuleData, 0x0, sizeof(stModulelMeas) );
    for( int module=0;module<MAX_MODULE;module++ )
    {
        int nWriteSize = 0;
        for( int item=0;item<MAX_MODULE_ITEM;item++ )
        {
            std::map<QString, stPmubusCommand>::iterator it = m_mapPbCmdModule.find( sCmd[item] );
            if( it == m_mapPbCmdModule.end() ) continue;

            request.stPmbusCmd = it->second;
            std::string scmd      = request.stPmbusCmd.sCommand;
            unsigned char cmd     = request.stPmbusCmd.uCommand;
            PMBUS_OPENRATION opr  = request.stPmbusCmd.enCmdType;
            PMBUS_DATA_FORMAT fmt = request.stPmbusCmd.enDataFormat;
            unsigned char offset  = request.stPmbusCmd.uOffsetData;
            unsigned short multi  = request.stPmbusCmd.uMultiplier;
            int devaddr           = DEVICE_ADDRESS::DEV_MODULE1+module;    //request.uDevice;

            BuildPMBusRequest( cmd, opr, (DEVICE_ADDRESS)devaddr, 0x0 );
            memset( szSendBuffer, 0x0, sizeof(szSendBuffer) );
            memcpy(szSendBuffer, &m_stPmbusReqHead, sizeof(stPmbusReqHead));
            nWriteSize = sizeof(m_stPmbusReqHead);

            if( !PMBusWrite( szSendBuffer, nWriteSize) )
            {
                emit SigLogMessage(m_ch, _ERROR_LOG, tr("[CH%1][ ERR] %2 send error!").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()) );
                return false;
            }
            uchar errCode = PMBusRead( cmd, devaddr, szRecvData);
            if (errCode != ERR_SUCCESS)
            {
                emit SigLogMessage(m_ch, _ERROR_LOG, tr("[CH%1][RECV] %2 : [ErrCode:%3]").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()).arg(ErrorCodeString(errCode)) );
                return false;
            }
            //uchar uSize   = request.stPmbusCmd.uNumOfByte;
            uint uResSize = 0x0;
            uint uData    = 0x0;
            if (opr == PMBUS_READ) uResSize = m_stPmbusResponse.ResponseDataLen - offset;
            else                   uResSize = m_stPmbusResponse.ResponseDataLen;

            double dData = 0.0;
            if( fmt == PMBUS_DATA_FORMAT::FORMAT_DIRECT )
            {
                //Direct Format
                for (unsigned int i = 0; i<uResSize; i++)
                {
                    uint32_t data = (uint32_t)szRecvData[i + offset] & 0xFF;
                    uData |= (data<<(i*8));
                }
                dData = static_cast<double>(uData) / multi;
            }
            else if( fmt == PMBUS_DATA_FORMAT::FORMAT_BITMAPPED )
            {
                for ( unsigned int i = 0; i<uResSize; i++)
                {
                    uint32_t data = (uint32_t)szRecvData[i + offset] & 0xFF;
                    uData |= (data << (i * 8));
                }
            }
            else if( fmt == PMBUS_DATA_FORMAT::FORMAT_LINEAR )
            {
                for (unsigned int i = 0; i<uResSize; i++)
                {
                    uint32_t data = (uint32_t)szRecvData[i + offset] & 0xFF;
                    uData |= (data << (i * 8));
                }
                dData = L11ToDouble(uData);
            }

                 if( item == MODULE_VOUT ) { ModuleData.Voltage[module] = dData; emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1][MDL%2] %3 : %4").arg((m_ch==SIDE_A)?"A":"B").arg(module).arg(scmd.c_str()).arg(dData) );}
            else if( item == MODULE_IOUT ) { ModuleData.Current[module] = dData; emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1][MDL%2] %3 : %4").arg((m_ch==SIDE_A)?"A":"B").arg(module).arg(scmd.c_str()).arg(dData) );}
            else if( item == MODULE_POUT ) { ModuleData.Watt   [module] = dData; emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1][MDL%2] %3 : %4").arg((m_ch==SIDE_A)?"A":"B").arg(module).arg(scmd.c_str()).arg(dData) );}
            else if( item == MODULE_TEMP ) { ModuleData.Temp   [module] = dData; emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1][MDL%2] %3 : %4").arg((m_ch==SIDE_A)?"A":"B").arg(module).arg(scmd.c_str()).arg(dData) );}
            else if( item == MODULE_STAT ) { ModuleData.Status [module] = uData; emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1][MDL%2] %3 : %4").arg((m_ch==SIDE_A)?"A":"B").arg(module).arg(scmd.c_str()).arg(uData) );}
            else {}

            emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1][RECV] %2 : %3").arg((m_ch==SIDE_A)?"A":"B").arg(scmd.c_str()).arg(dData) );
            Delay( 50 MS );
        }
    }
    emit SigModules( m_ch, ModuleData );
    return true;
}
