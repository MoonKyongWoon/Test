#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/serial.h>
#include <termios.h>
#include <sys/signal.h>
#include <sys/types.h>

int main()
{
	int fd, res;
	struct termios oldtio,newtio;
	char buf[255];
	int modemctlline;
	int txemptystate;

	fd = open( "/dev/ttyS2", O_RDWR | O_NOCTTY );
	if (fd <0)
	{
		printf( "Device OPEN FAIL\n");
		return -1;
	}
	// 통신 환경를 설정한다.
	tcgetattr(fd, &oldtio);

	// 현재 설정을 oldtio에 저장
	memset(&newtio, 0, sizeof(newtio));
	newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD ;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	// set input mode (non-canonical, no echo,...)
	newtio.c_lflag = 0;

	// 다음 두라인은 통신 방식에 따라서 다르게 설정하여야 한다.
	newtio.c_cc[VTIME] = 1; // 문자 사이의 timer를 disable
	newtio.c_cc[VMIN]  = 0; // 최소 5 문자 받을 때까진 blocking
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	for( int i=0;i<5;i++ )
	{
		// 현재 설정된 모뎀 제어 레지스터를 읽는다.
		ioctl( fd, TIOCMBIS, &modemctlline);

		// RTS를 HIGH로 만든다.(Request To Send)
		modemctlline = TIOCM_RTS;
		ioctl( fd, TIOCMBIS, &modemctlline );
		// Read the RTS pin status.
		ioctl(fd, TIOCMGET, &modemctlline);
		if (modemctlline & TIOCM_RTS)
			printf("RTS pin is set\n");
		else
			printf("RTS pin is reset\n");

		// 데이타를 출력한다.
		memset( buf, 0x0, 32 );
		strncpy( buf, "*IDN03?\n", 8 );
		buf[32]=0;
		int nWritten = write( fd, buf, strlen(buf) );
		printf( "WRITE %d Bytes : %s", nWritten, buf );

		// 모든 데이타가 전송되어 완전하게 버퍼가
		// 비워질때까지 대기한다.
		while( 1 )
		{
			ioctl( fd, TIOCSERGETLSR, &txemptystate);
			if( txemptystate )
				break;
		}

		// RTS를 LOW로 만든다.
		modemctlline = TIOCM_RTS;
		ioctl( fd, TIOCMBIC, &modemctlline );
		// Read the RTS pin status.
		ioctl(fd, TIOCMGET, &modemctlline);
		if (modemctlline & TIOCM_RTS)
			printf("RTS pin is set\n");
		else
			printf("RTS pin is reset\n");
		// 잠시 대기한다.
		sleep(1);

		// 데이타를 읽는다.
		res = read(fd,buf,sizeof(buf));
		buf[res]=0;
		printf("--> %s\n", buf);
	}
	// 통신상태를 원 위치한다.
	tcsetattr(fd,TCSANOW,&oldtio);
	close( fd );

	return 0;
}
