#ifndef CEAPUTIL_HPP
#define CEAPUTIL_HPP

#include <bitset>
#include <QObject>
#include <QString>
#include <QTimer>
#include "commstruct.hpp"
#include "secsgem.hpp"
#include "secsmsg.hpp"

//#include "hsms_cnv.hpp"

#define TEST_IP				"192.168.62.62"
#define TEST_PORT			5000

#define T3_TYPICAL_VALUE	45
#define T5_TYPICAL_VALUE	10
#define T6_TYPICAL_VALUE	5
#define T7_TYPICAL_VALUE	10
#define T8_TYPICAL_VALUE	5



enum class TIME_OUT_TYPE {
	T3	 = 0,
	T5		,
	T6		,
	T7		,
	T8		,
};

typedef struct _stControlListItem {
    TIME_OUT_TYPE	timeout_type;
	uint			system_byte;
    int				count;
}stControlListItem;


namespace CEapUtil {
	namespace Convert {
			QString convertFromCharToBitsQString(char data);
			QString convertFromIntToBitsQString(int data);
			QString convertFromControlMessageToQString(stHsmsElement element);
	}
}

#endif // CEAPUTIL_HPP
