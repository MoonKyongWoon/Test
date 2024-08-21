#ifndef COMMON_HPP
#define COMMON_HPP

#include <QString>
#include "sysver.hpp"
#include "chmbprotocol.hpp"

#define PROGRAM_NAME				ATCHMBUI_NAME
#define PROGRAM_VERSION				ATCHMBUI_VER
#define PROGRAM_RELEASE_DATE		ATCHMBUI_DATE

//////////////////////////////////////////////////////////////////////
///  DISPLAY COLOR DEFINE
//////////////////////////////////////////////////////////////////////
#define PASS_QCOLOR					QColor(  0,  0,  0)
#define FAIL_QCOLOR					QColor(255,  0,  0)
#define ODD_ROW_QCOLOR				QColor(245,245,245)
#define EVEN_ROW_QCOLOR				QColor(255,255,255)
#define TP_ODD_ROW_QCOLOR			QColor(236,251,252)
#define TP_EVEN_ROW_QCOLOR			QColor(255,235,217)
#define SODD_ROW_QCOLOR				"QColor(245,245,245);"
#define SEVEN_ROW_QCOLOR			"QColor(255,255,255);"

//////////////////////////////////////////////////////////////////////
///  ETC
//////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(p)				if( p ) { delete p; p=nullptr; }
#define MAX_LOG_LINE_NUM			(65535)
#define MAX_ALARM_WORD_ADDR			(10)

enum EN_ALARM_LEVEL
{
	EN_NO_WARN_ERROR    = 0x0,
	EN_WARNING_LEVEL         ,
	EN_HEAVY_LEVEL           ,
};

typedef struct _stAlarmData
{
	EN_ALARM_LEVEL level;
	QString sAlarmName;
}stAlarmData;


typedef struct _stAlarmList
{
	int index;
	int bit;
	QString sDateTime;
    QString sUpdateTime;
    EN_ALARM_LEVEL level;
	QString sAlarmName;
}stAlarmList;

#endif // COMMON_HPP
