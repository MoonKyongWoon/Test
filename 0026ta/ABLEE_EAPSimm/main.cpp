#include <QApplication>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTextStream>

#include "mainwindow.hpp"
#include "sysini.hpp"
#include "sysver.hpp"
#include "util.hpp"

//#include "hsms_cnv.hpp"

bool EapSimm_RunCheck()
{
	qint64 pid = 0;
	qint64 mypid = QCoreApplication::applicationPid();
	if( UTIL::PROCESS::readPid( ATEAPSIMM_NAME, pid ) )
	{
		if( UTIL::PROCESS::IsRun( pid, ATEAPSIMM_NAME ) )
		{
			return true;
		}
		else
		{
			if( UTIL::PROCESS::writePid( ATEAPSIMM_NAME, mypid ) )
				return false;
		}
	}
	else
	{
		if( UTIL::PROCESS::writePid( ATEAPSIMM_NAME, mypid ) )
			return false;
	}
	return true;
}

void SML_TO_BIN()
{
	atstring sml;
	sml =
	"<S6F11 S                                   \n"
	"	<L 2									\n"
	"		<F4 1 -1.0>	    					\n"
	"		<F8 1 -1.0>   						\n"
	"    >										\n"
	">											\n";
	CSecsGem secsgem;
	CSecsMsg* pMsg = secsgem.CompileSML( sml );
	if( pMsg )
	{
		QByteArray ba;
		if( secsgem.GetMsgHSMS ( pMsg, ba ) > 0 )
		{
			qDebug() << ba;
			qDebug() << ba.toHex();
		}
		else
		{
			std::vector<atstring> vecError = secsgem.getCompileErrors();
			for( atstring err : vecError )
			{
				printf("%s\n", err.c_str() );
			}
		}
		delete pMsg;
		pMsg = nullptr;
	}
}

void BIN_TO_SML()
{
	stHsmsHeader hsmsheader;
	char szHsms[128]={0x0,};
	char szData[64]={0x0,};
	hsmsheader.stMsgHeader.cRbit = 1;
	hsmsheader.stMsgHeader.cDevidMsb = 0;
	hsmsheader.stMsgHeader.cDevidLsb = 0;

	hsmsheader.stMsgHeader.cWbit     = 1;
	hsmsheader.stMsgHeader.cStream   = 6;
	hsmsheader.stMsgHeader.cFunction =11;

	hsmsheader.stMsgHeader.cPType    = 0;
	hsmsheader.stMsgHeader.cSType    = 0;
    //hsmsheader.stMsgHeader.nSystemBytes  = 0x01010101;
    hsmsheader.stMsgHeader.cSystemBytes[0]  = 0x01;
    hsmsheader.stMsgHeader.cSystemBytes[1]  = 0x01;
    hsmsheader.stMsgHeader.cSystemBytes[2]  = 0x01;
    hsmsheader.stMsgHeader.cSystemBytes[3]  = 0x01;

	szData[0] = (HSMS_FMT_CODE::FMT_LIST<<2) | 0x1;
	szData[1] = 6;
	szData[2] = (HSMS_FMT_CODE::FMT_ASCII<<2) | 0x1;
	szData[3] = 11;	//KOREA-ABLEE
	szData[4] = 'K';
	szData[5] = 'O';
	szData[6] = 'R';
	szData[7] = 'E';
	szData[8] = 'A';
	szData[9] = '-';
	szData[10] = 'A';
	szData[11] = 'B';
	szData[12] = 'L';
	szData[13] = 'E';
	szData[14] = 'E';
	szData[15] = (HSMS_FMT_CODE::FMT_U4<<2) | 0x1;
	szData[16] = 8;
	szData[17] = 0b00000001;
	szData[18] = 0b10000000;
	szData[19] = 0b00000001;
	szData[20] = 0b10000000;
	szData[21] = 0b00000001;
	szData[22] = 0b10000000;
	szData[23] = 0b00000001;
	szData[24] = 0b10000000;
	szData[25] = (HSMS_FMT_CODE::FMT_U1<<2) | 0x1;
	szData[26] = 1;
	szData[27] = 0b00000001;
	szData[28] = (HSMS_FMT_CODE::FMT_U2<<2) | 0x1;
	szData[29] = 2;
	szData[30] = 0b00000000;
	szData[31] = 0b00000010;
	szData[32] = (HSMS_FMT_CODE::FMT_I1<<2) | 0x1;
	szData[33] = 1;
	szData[34] = 0b00000011;
	szData[35] = (HSMS_FMT_CODE::FMT_I2<<2) | 0x1;
	szData[36] = 2;
	szData[37] = 0b00000000;
	szData[38] = 0b00000100;

//	int total_len = sizeof(stHsmsHeader) + sizeof(szData);
	int total_len = sizeof(stHsmsHeader) + 39;//strlen(szData);

	hsmsheader.szMsgLength[0] = (total_len >> 24 ) & 0xFF;
	hsmsheader.szMsgLength[1] = (total_len >> 16 ) & 0xFF;
	hsmsheader.szMsgLength[2] = (total_len >>  8 ) & 0xFF;
	hsmsheader.szMsgLength[3] = (total_len >>  0 ) & 0xFF;

	memcpy( szHsms, &hsmsheader, sizeof(stHsmsHeader) );
//	memcpy( szHsms+sizeof(stHsmsHeader), szData, sizeof(szData) );
	memcpy( szHsms+sizeof(stHsmsHeader), szData, 39);//strlen(szData) );

	qDebug() << "---LEN---";
	qDebug() << "HEADER : " << sizeof(stHsmsHeader);
	qDebug() << "DATA   : " << strlen(szData);
	qDebug() << "TOTAL  : " << total_len;
	qDebug() << "---------";

	CSecsGem secsgem;
	CSecsMsg* pMsg = secsgem.CreateMsg( szHsms, total_len );
	if( pMsg == nullptr )
	{
		qDebug() << "CreateMsg error!";
	}
	else
	{
		atstring sml = "";
		if( secsgem.GetMsgSML( pMsg, sml ) < 0 )
		{
			qDebug() << "GetMsgSML error!";
		}
		else
		{
			qDebug() << QString(sml);
		}
		delete pMsg;
		pMsg = nullptr;
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);      //Dialog '?' Help Disable
	app.setWindowIcon( QIcon(":/icon/icon/ablee.ico") );

	QString qss = "";
	//QFile f(":/qss/qss/MailSy.qss");
	QFile f(":/qss/qss/Ubuntu.qss" );
	if ( !f.exists() )
	{
		fprintf( stderr, "Unable to set stylesheet, file not found\n" );
	}
	else
	{
		f.open( QFile::ReadOnly | QFile::Text );
		QTextStream ts(&f);
		qss = ts.readAll();
		app.setStyleSheet(qss);
	}

	/*
	if( EapSimm_RunCheck() )
	{
		QMessageBox::critical( 0, qApp->applicationName(), QObject::tr("%1 is running!").arg(ATEAPSIMM_NAME), QMessageBox::Ok );
		exit(-1);
	}
	*/

	CMainWindow w;
	w.setWindowFlags( w.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	bool bRestored = true;
	if( 1 )
	{
		QSettings settings( CONFIG_INI_KEY, ATEAPSIMM_NAME );
		QByteArray saved_geometry = settings.value("geometry").toByteArray();
		if ( !saved_geometry.isEmpty() ) w.restoreGeometry( saved_geometry );
		else bRestored = false;
		QByteArray saved_state = settings.value("windowState").toByteArray();
		if ( !saved_state.isEmpty() )
			w.restoreState(saved_state);
		else
			bRestored = false;
		//qDebug() << settings.fileName();
		if( !bRestored ) w.resize( 1024, 768);
	}
	w.show();
//	SML_TO_BIN();
//	BIN_TO_SML();
	return app.exec();
}
