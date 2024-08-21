#ifndef CTG_HPP
#define CTG_HPP

#include <cmath>
#include "testlib_global.h"
#include "sys.hpp"
#include "sysdef.hpp"
#include "swcontrol.h"
#include "atstring.hpp"

#define FINE_DELAY_LSB					(0.1)		//100 PS
#define COARSE_DELAY_LSB				(4.0)		//4.0 NS
#define FINE_DELAY_MAX					(0x27)		//3.9 NS
#define COARSE_DELAY_MAX				(0x3fffe)	//1048572 NS
#define CLK_ADDR_OFFSET					(0x80)
#define RATE_LIMIT_1MS					(1000000.0)
#define RATE_LIMIT_4NS					(4.0)
#define RATE_LIMIT_2_5NS				(2.5)
#define MAX_USE_TIMESET					(17)
#define IDELAY_RESOLUTION				(78.0)		//78PS

#define DELAY_FINE_DELAY_LSB			( 100)		//100 PS
#define DELAY_COARSE_DELAY_LSB			(4000)		//4.0 NS
#define STROBE_COARSE_DELAY_MAX			(0xFF)		//60NS
#define STROBE_FINE_DELAY_MAX			(0xFF)		//3.9NS
#define STROBE_MAX_DELAY_VALUE			(60000)		//60NS
//#define DEFAULT_STROBE_ROFFSET			(35000)		//35NS Default Strobe Offset
#define DEFAULT_STROBE_ROFFSET			(26000)		//26NS Default Strobe Offset
#define RSTORBE_MAX_VALUE				(STROBE_MAX_DELAY_VALUE-DEFAULT_STROBE_ROFFSET)

class TESTLIBSHARED_EXPORT CTg
{
public:
    CTg();

public:
	bool RATE(double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15 );
	bool RATE(double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15,
			  double r16, double r17, double r18, double r19, double r20, double r21, double r22, double r23, double r24, double r25, double r26, double r27, double r28, double r29, double r30 );
	bool SET_RATE( int ts, double r );
	bool CLOCK(int pin, double t0 , double t1, double t2, double t3, double t4, double t5, double t6, double t7, double t8, double t9, double t10, double t11, double t12, double t13, double t14, double t15 );
	bool CLOCK(int pin, double t0 , double t1, double t2, double t3, double t4, double t5, double t6, double t7, double t8, double t9, double t10, double t11, double t12, double t13, double t14, double t15 ,
						double t16, double t17, double t18, double t19, double t20, double t21, double t22, double t23, double t24, double t25, double t26, double t27, double t28, double t29, double t30);
	bool SET_CLK( int, int, double );
	bool SET_CLK_DELAY (int, double );
	bool SET_STROBE_DELAY(uint,uint,double);
	bool SET_STROBE_DELAY(uint,double);
	bool APPLY_STROBE_DELAY(uint);
public:
	void InitTg();
	bool WriteTimeset();
	void TgStart();
	void TgStop();
	void TgBclkReset();
	void TgMemClear();
	void InitClockIDelay();
	void InitStrobeDelay();
	void DbgInitTimeset(double);		//debug
	void SetStrbROffset(uint,double);
public:
	void PrintRate();
	void PrintClock();
public:
	bool SetClockIDelay( int, double );	//Debug
	void SetIDelayReset (EN_LOGIC_TYPE);
	void SetIDelayRequest(EN_LOGIC_TYPE);
private:
	void InitClockStr();
	void InitTimeset();
	bool IsBClk(int);
	bool IsValidRate (int,double);
	bool IsValidClock(int,int,double);
	bool DoRate(bool bWrite=true);
	bool DoClock(int,bool bWrite=true);
public:
	double m_dIDelay[MAX_IDELAY_LIST];
	double m_dStrbROffset[16];	//row 0 ~ 16
private:
	atstring m_sClk[TS_CLOCK_LIST::MAX_CLOCK_LIST];
	double m_dRate [MAX_TIMESET];
	double m_dClock[TS_CLOCK_LIST::MAX_CLOCK_LIST][MAX_TIMESET];
	double m_dClockDelay[TS_CLOCK_LIST::MAX_CLOCK_LIST];
	double m_dStrbDelay[SCAN_MEM_DEPTH];
	int m_nRate [MAX_TIMESET];
	int m_nClock[TS_CLOCK_LIST::MAX_CLOCK_LIST][MAX_TIMESET];
	int m_nClockDelay[TS_CLOCK_LIST::MAX_CLOCK_LIST];
	int m_nStrbDelay[SCAN_MEM_DEPTH];
};

#endif // TG_HPP
