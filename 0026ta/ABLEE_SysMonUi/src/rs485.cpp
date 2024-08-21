#include "rs485.hpp"

CRS485::CRS485(SIDE_AB ch, QObject *parent) : QObject(parent), m_ch(ch)
{
	m_bOpen = false;
    m_bDebug = false;
	if( m_ch == SIDE_A ) m_stty = "/dev/ttyS2";
	else                 m_stty = "/dev/ttyS3";
}

CRS485::~CRS485()
{
    CloseDevice();
}

void CRS485::SetSerialInfo( QString tty )
{
	m_stty = tty;
}

bool CRS485::OpenDevice()
{
    //struct serial_rs485 rs485conf;
	if( m_bOpen )
	{
		emit SigLogMessage( m_ch, _NORMAL_LOG,
							tr("[CH%1] %2 alreay open!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
		return true;
	}

	m_bOpen = false;
	m_rs485 = open( m_stty.toStdString().c_str(), O_RDWR | O_NOCTTY );
	if ( m_rs485 <0 )
	{
		emit SigLogMessage( m_ch, _ERROR_LOG, tr("[CH%1] %2 open error!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
		return false;
	}
	// 통신 환경를 설정한다.
	tcgetattr( m_rs485, &m_oldtio );

	// 현재 설정을 oldtio에 저장
	memset(&m_newtio, 0, sizeof(m_newtio));
	m_newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD ;
	m_newtio.c_iflag = IGNPAR;
	m_newtio.c_oflag = 0;
	m_newtio.c_lflag = 0;

	// 다음 두라인은 통신 방식에 따라서 다르게 설정하여야 한다.
    m_newtio.c_cc[VTIME] = 5;	// timeout * 0.1 = 0.5 sec
    m_newtio.c_cc[VMIN ] = 0;	// immediately return 0 character
	tcflush( m_rs485, TCIFLUSH );
	tcsetattr( m_rs485, TCSANOW, &m_newtio );

	emit SigLogMessage( m_ch, _NORMAL_LOG,
						tr("[CH%1] %2 open success!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
	m_bOpen = true;
    printf("[CH%c] open device\n", (m_ch==SIDE_A)?'A':'B' );
	return true;
}

bool CRS485::OpenRs485()
{
    //emit SigLogMessage( m_ch, _NORMAL_LOG, __FUNCTION__ );
    if( m_bOpen )
    {
        emit SigLogMessage( m_ch, _NORMAL_LOG,
                            tr("[CH%1] %2 alreay open!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
        return true;
    }

    int ret = 0;
    int rtsflag = 0;

    m_bOpen = false;
    m_rs485 = open( m_stty.toStdString().c_str(), O_RDWR | O_NOCTTY );
    //m_rs485 = open( m_stty.toStdString().c_str(), O_RDWR | O_DSYNC | O_SYNC );
    if ( m_rs485 <0 )
    {
        emit SigLogMessage( m_ch, _ERROR_LOG, tr("[CH%1] %2 open error!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
        return false;
    }

    tcgetattr( m_rs485, &m_oldtio );
    // 현재 설정을 oldtio에 저장
    memset(&m_newtio, 0, sizeof(m_newtio));
    m_newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD ;
    m_newtio.c_iflag = IGNPAR;
    m_newtio.c_oflag = 0;
    m_newtio.c_lflag = 0;

    // 다음 두라인은 통신 방식에 따라서 다르게 설정하여야 한다.
    m_newtio.c_cc[VTIME] = 10;	// timeout * 0.1 Second = 1 second
    m_newtio.c_cc[VMIN ] =  0;	// immediately return 0 character
    tcflush( m_rs485, TCIFLUSH );

    ret = tcsetattr( m_rs485, TCSANOW, &m_newtio );
    if( ret < 0 )
    {
        emit SigLogMessage( m_ch, _ERROR_LOG,
                            tr("[CH%1]Can not tcsetattr TCSANOW(errno:%d)").arg((m_ch==SIDE_A)?"A":"B").arg(errno) );
        CloseDevice();
        return false;
    }
    struct serial_rs485 rs485conf;
    ret = ioctl (m_rs485, TIOCGRS485, &rs485conf);
    if (ret < 0)
    {
        emit SigLogMessage( m_ch, _ERROR_LOG,
                            tr("[CH%1]Can not get rs485 mode, ioctl failed %2 (errno: %3)").arg((m_ch==SIDE_A)?"A":"B").arg(ret).arg(errno) );
        printf("Can not get rs485 mode, ioctl failed %d (errno: %d)\n", ret, errno);
        CloseDevice();
        return false;
    }

    //test rtsflag = TIOCM_RTS;
    //test ret = ioctl(m_rs485, TIOCMBIC, &rtsflag);
    //test if (ret < 0)
    //test {
    //test     emit SigLogMessage( m_ch, _ERROR_LOG, tr("[CH%1]Can not set rts pint, octl failed %2, (errno: %3)").arg((m_ch==SIDE_A)?"A":"B").arg(ret).arg(errno) );
    //test     printf("Can not set rts pint, ioctl failed %d, (errno: %d)\n", ret, errno);
    //test     CloseDevice();
    //test     return false;
    //test }

    /* Enable RS485 mode */
    rs485conf.flags |= SER_RS485_ENABLED;

    /* Set RTS when sending */
    rs485conf.flags |= SER_RS485_RTS_ON_SEND;

    /* Cleare RTS after sending */
    rs485conf.flags &= ~SER_RS485_RTS_AFTER_SEND;

    /* Set RTS delay before send ms */
    rs485conf.delay_rts_before_send = 2;

    /* Set RTS delay after send ms */
    rs485conf.delay_rts_after_send  = 2;

    /* Don't receive data while sending */
    rs485conf.flags &= ~SER_RS485_RX_DURING_TX;

    ret = ioctl (m_rs485, TIOCSRS485, &rs485conf);
    if (ret < 0)
    {
        emit SigLogMessage( m_ch, _ERROR_LOG,
                            tr("Can not send rs485 mode, ioctl failed %1 (errno: %2)").arg(ret).arg(errno) );
        printf("Can not send rs485 mode, ioctl failed %d (errno: %d)\n", ret, errno);
        CloseDevice();
        return false;
    }
    emit SigLogMessage( m_ch, _NORMAL_LOG,
                        tr("[CH%1] %2 open success!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
    m_bOpen = true;
    return true;
}

bool CRS485::CloseDevice()
{
    if( !m_bOpen ) return true;

	if( m_bOpen )
	{
        tcsetattr( m_rs485, TCSANOW, &m_oldtio );
		close(m_rs485);
		m_bOpen = false;
	}
	emit SigLogMessage( m_ch, _NORMAL_LOG,
						tr("[CH%1] %2 close success!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty) );
    printf("[CH%c] close device\n", (m_ch==SIDE_A)?'A':'B' );
	return true;
}

bool CRS485::IsOpen()
{
	return m_bOpen;
}

long long int CRS485::Delay(double dTime)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	long long wait_time = static_cast<long long>( dTime * 1.0e6 );	//US
	while ( true )
	{
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        long long elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        if( elapsed_time > wait_time ) break;
	}
	return 0;
}

//not use
void CRS485::SlotOpenDevice()
{
    //OpenDevice();
    OpenRs485();
	emit SigLogMessage( m_ch, _NORMAL_LOG,
						tr("[CH%1] open %2 %3!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty).arg(m_bOpen?"success":"fail") );
}

//not use
void CRS485::SlotCloseDevice()
{
	CloseDevice();
	emit SigLogMessage( m_ch, _NORMAL_LOG,
						tr("[CH%1] close %2 %3!").arg((m_ch==SIDE_A)?"A":"B").arg(m_stty).arg(m_bOpen?"fail":"success") );
}

void CRS485::SetRTS(uint rts)
{
    int modemctlline = 0;

    // 현재 설정된 모뎀 제어 레지스터를 읽는다.
    ioctl( m_rs485, TIOCMBIS, &modemctlline);

    modemctlline = TIOCM_RTS;	//TIOCM_RTS       RTS (request to send)
    if( rts == 1 )
    {
        // RTS를 HIGH로 만든다.(Request To Send)
        if( ioctl( m_rs485, TIOCMBIS, &modemctlline ) < 0 )
        {
            emit SigLogMessage( m_ch, _ERROR_LOG, tr("[CH%1] RTS High error!" ).arg((m_ch==SIDE_A)?"A":"B") );
        }
    }
    else
    {
        // RTS를 LOW 만든다.
        if( ioctl( m_rs485, TIOCMBIC, &modemctlline ) < 0 )
        {
            emit SigLogMessage( m_ch, _ERROR_LOG, tr("[CH%1] RTS Low error!" ).arg((m_ch==SIDE_A)?"A":"B") );
        }
    }

    // Read the RTS pin status.
    //ioctl(m_rs485, TIOCMGET, &modemctlline);
    //if (modemctlline & TIOCM_RTS)
    //    emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1] RTS pin is High" ).arg((m_ch==SIDE_A)?"A":"B") );
    //else
    //    emit SigLogMessage( m_ch, _NORMAL_LOG, tr("[CH%1] RTS pin is Low" ).arg((m_ch==SIDE_A)?"A":"B") );
    Delay( 1 MS );
}

bool CRS485::SendData( char* sendbuf, int size)
{
	int txemptystate = 0;

    SetRTS( 1 );
	int nWritten = 0;
	int nWrittenSum = 0;
	int nWriteSize  = size;
	do
	{
		nWritten = write( m_rs485, sendbuf + nWrittenSum, nWriteSize );
		if( nWritten < 0 )
		{
			emit SigLogMessage( m_ch, _ERROR_LOG,
								tr("[CH%1] %2 write error!").arg((m_ch==SIDE_A)?"A":"B").arg(sendbuf) );
			return false;
		}
		nWrittenSum += nWritten;
		nWriteSize  -= nWritten;
	}while( nWrittenSum < size );

	// 모든 데이타가 전송되어 완전하게 버퍼가
	// 비워질때까지 대기한다.
    //int nWait = 1;
    while( 1 )
    {
        if( ioctl( m_rs485, TIOCSERGETLSR, &txemptystate) < 0 )
        {
            emit SigLogMessage( m_ch, _ERROR_LOG,
                                tr("[CH%1][INFO] TIOCSERGETLSR:%2 error!").arg((m_ch==SIDE_A)?"A":"B").arg(txemptystate,2,16,QChar('0')) );
            continue;
        }
        if( txemptystate )
            break;
        //if( (nWait++) > 10 )
        //{
        //    emit SigLogMessage( m_ch, _ERROR_LOG,
        //                        tr("[CH%1][ ERR] send buffer flush error!").arg((m_ch==SIDE_A)?"A":"B") );
        //    return false;
        //}
        Delay( 1 US );
    }
    Delay( 1 MS );
    //SetRTS( 0 );

    if ( m_bDebug )
	{
        printf("[CH%c][SEND] Size=%02d ", (m_ch==SIDE_A)?'A':'B', nWrittenSum);
        for ( int i = 0; i < nWrittenSum; i++) printf("[%02X] ", sendbuf[i]&0xFF);
		printf("\n");
	}
    //Delay( 10 MS );
	return true;
}

bool CRS485::RecvData( char* readbuf, int size )
{
    int nRetry = 1;
	int nRead = 0;
	int nReadSum = 0;
	int nReadSize  = size;

    //SetRTS( 0 );
	do
	{
		nRead = read( m_rs485, readbuf+nReadSum, nReadSize );
		if ( nRead <= 0 )
		{
            if( nRetry > 2 ) return false;
			//usleep( 100 );
			nRetry++;
			continue;
		}
		nReadSum += nRead;
		nReadSize -= nRead;
	} while (nReadSum < size);

    if ( m_bDebug )
    {
		printf("[CH%c][RECV] Size=%02d ", (m_ch==SIDE_A)?'A':'B', size);
        for ( int i = 0; i < size; i++) printf("[%02X] ", readbuf[i]&0xFF);
        printf("\n");
    }
    //Delay( 10 MS );
	//emit SigLogMessage( m_ch, LOG_TYPE::ERROR_LOG, tr("[READ] %1").arg(readbuf) );
	return true;
}

//not use
int CRS485::ReadData( char* pBuffer )
{
    const int BUFFER_SIZE = 64;
	char szBuffer[BUFFER_SIZE] = {0x0,};
	int nRead = read( m_rs485, szBuffer, BUFFER_SIZE );
	if ( m_bDebug )
	{
		printf("[CH%c][RECV] Size=%02d ", (m_ch==SIDE_A)?'A':'B', nRead);
		for ( int i = 0; i < nRead; i++) printf("[%02X] ", szBuffer[i]&0xFF);
		printf("\n");
	}
    snprintf( pBuffer, BUFFER_SIZE, "%s", szBuffer );
    //printf("BUF:%s\n", szBuffer);
    return nRead;
}
