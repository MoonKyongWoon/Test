#ifndef COMMON_HPP
#define COMMON_HPP

#include <QString>
#include "sysini.hpp"
#include "commstruct.hpp"

#define SLOT_VIEW_MAX_ROWS			(8)
#define SLOT_VIEW_COUNT				(2)
#define MAX_LOG_LINE_NUM			(262140)        //2023.03.28 - kwmoon(65535 -> 262140)

#define PASS_TEST_QCOLOR			QColor(  0,255,  0)
#define PASS_BLOCK_QCOLOR			QColor(  0,128,  0)
#define FAIL_TEST_QCOLOR			QColor(255,  0,  0)
#define INIT_TEST_QCOLOR			QColor(  0,  0,  0)
#define MASK_TEST_QCOLOR			QColor(243,119, 72)
#define NODEVICE_TEST_QCOLOR		QColor( 99, 99, 99)

#define PASS_TEST_RGB				"rgb(  0,255,  0)"
#define PASS_BLOCK_RGB				"rgb(  0,128,  0)"
#define FAIL_TEST_RGB				"rgb(255,  0,  0)"
#define INIT_TEST_RGB				"rgb(  0,  0,  0)"
#define MASK_TEST_RGB				"rgb(243,119, 72)"
#define NODEVICE_TEST_RGB			"rgb( 99, 99, 99)"

#define PASS_TEXT_QCOLOR			QColor(  0,  0,  0)
#define FAIL_TEXT_QCOLOR			QColor(255, 77, 77)
#define FAULT_TEXT_QCOLOR			QColor(255,  0,  0)	//2023.03.18

#define PASS_BG_QCOLOR				QColor(255,255,255)
#define FAIL_BG_QCOLOR				QColor(255,  0,  0)

#define PASS_STBAR_QCOLOR			"rgb(243,243,243)"
#define FAIL_STBAR_QCOLOR			"rgb(255, 43, 43)"

#define PIXMAP_ONLINE				QPixmap(":/icon/icon/green_marble.ico")
#define PIXMAP_OFFLINE				QPixmap(":/icon/icon/red_marble.ico")
#define PIXMAP_MASK					QPixmap(":/icon/icon/black_marble.ico")

#define STR_MASK					"MASK"
#define STR_BIBID_IO_ERR			"I/O ERROR"
#define STR_BIBID_PARITY_ERR		"PARITY ERROR"

#define PASS_PRGBAR_QSS				"QProgressBar{ background-color: #FEFFFC; text-align:center;" "border: 0px;" "border-radius: 3px;} " "QProgressBar::chunk" "{ background-color: #20bfa9;" "width: 10px;"  "}"
#define FAIL_PRGBAR_QSS				"QProgressBar{ background-color: #FF0000; text-align:center;" "border: 0px;" "border-radius: 3px;} " "QProgressBar::chunk" "{ background-color: #20bfa9;" "width: 10px;"  "}"

#define SELECT_BTN_QSS				"QPushButton{ background-color:#F37748; color:#000000; margin: 0px; padding: 2px; border-radius: 3px; border: 2px solid rgb(179,179,179);}"
#define UNSELECT_BTN_QSS			"QPushButton{ background-color:#84bcda; color:#000000; margin: 1px; padding: 2px; border-radius: 5px; border: 2px solid rgb(192,192,192);}"
#define DISABLE_BTN_QSS				"QPushButton{ background-color:#7d7d7d; color:#000000; margin: 1px; padding: 2px; border-radius: 5px; border: 2px solid rgb(192,192,192);}"


enum EN_SLOTINFO_COL
{
	EN_SI_BIBID       ,
	EN_SI_PT          ,
	EN_SI_PARTNAME    ,
	EN_SI_LOTNAME     ,
	EN_SI_MLB         ,
	EN_SI_PROGRESS    ,
	EN_SI_SLOTINFO_MAX
};

enum EN_LOTINFO_COL
{
	EN_PART,
	EN_LOT,
	EN_INS,
	EN_MNT,
	EN_PASS,
	EN_FAIL,
	EN_YIELD,
	EN_LOTINFO_MAX
};

enum EN_PPSVIEW_COL
{
	EN_GR,
	EN_CH,
	EN_SV,
	EN_RV,
	EN_RC,
	EN_STA,
	EN_PPSVIEW_MAX
};

enum EN_LOG_TAB
{
	EN_SYS_LOG_TAB   ,
	EN_CHMB_LOG_TAB  ,
	EN_SITE0_LOG_TAB ,
};

#endif // COMMON_HPP
