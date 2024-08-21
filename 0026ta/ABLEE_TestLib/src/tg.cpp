#include "bitchart.hpp"
#include "sys.hpp"
#include "tg.hpp"
#include "io.hpp"
#include "log.hpp"

extern CPcie g_pcie;
extern CSys  g_sys;
extern CLog  g_log;

CTg::CTg()
{
	memset( m_dRate , 0x0, sizeof(m_dRate)  );
	memset( m_nRate , 0x0, sizeof(m_nRate)  );
	memset( m_dClock, 0x0, sizeof(m_dClock) );
	memset( m_nClock, 0x0, sizeof(m_nClock) );

	InitClockStr();
	InitTimeset ();
	InitStrobeDelay();
}

void CTg::InitClockStr()
{
	m_sClk[DREL1  ] = "DREL1  ";
	m_sClk[DRET1  ] = "DRET1  ";
	m_sClk[BCLK1  ] = "BCLK1  ";
	m_sClk[CCLK1  ] = "CCLK1  ";
	m_sClk[BCLK2  ] = "BCLK2  ";
	m_sClk[CCLK2  ] = "CCLK2  ";
	m_sClk[BCLK3  ] = "BCLK3  ";
	m_sClk[CCLK3  ] = "CCLK3  ";
	m_sClk[BCLK4  ] = "BCLK4  ";
	m_sClk[CCLK4  ] = "CCLK4  ";
	m_sClk[BCLK5  ] = "BCLK5  ";
	m_sClk[CCLK5  ] = "CCLK5  ";
	m_sClk[BCLK6  ] = "BCLK6  ";
	m_sClk[CCLK6  ] = "CCLK6  ";
	m_sClk[BCLK7  ] = "BCLK7  ";
	m_sClk[CCLK7  ] = "CCLK7  ";
	m_sClk[BCLK8  ] = "BCLK8  ";
	m_sClk[CCLK8  ] = "CCLK8  ";
	m_sClk[IOBCLK ] = "IOBCLK ";
	m_sClk[IOCCLK ] = "IOCCLK ";
	m_sClk[ADBCLK ] = "ADBCLK ";
	m_sClk[ADCCLK ] = "ADCCLK ";
	m_sClk[STBCLK ] = "STBCLK ";
	m_sClk[STCCLK ] = "STCCLK ";
	m_sClk[STRB3  ] = "STRB3  ";
	m_sClk[STRB4  ] = "STRB4  ";
	m_sClk[STRB5  ] = "STRB5  ";
	m_sClk[STRB6  ] = "STRB6  ";
	m_sClk[STRB7  ] = "STRB7  ";
	m_sClk[STRB8  ] = "STRB8  ";
	//m_sClk[RSTRB1 ] = "RSTRB1 ";		//2022.11.21 - kwmoon(comment)
	//m_sClk[RSTRB2 ] = "RSTRB2 ";		//2022.11.21 - kwmoon(comment)
	//m_sClk[RSTRB3 ] = "RSTRB3 ";		//2022.11.21 - kwmoon(comment)
	//m_sClk[RSTRB4 ] = "RSTRB4 ";		//2022.11.21 - kwmoon(comment)
	//m_sClk[RSTRB5 ] = "RSTRB5 ";		//2022.11.21 - kwmoon(comment)
	//m_sClk[RSTRB6 ] = "RSTRB6 ";		//2022.11.21 - kwmoon(comment)
	//m_sClk[RSTRB7 ] = "RSTRB7 ";		//2022.11.21 - kwmoon(comment)
	//m_sClk[RSTRB8 ] = "RSTRB8 ";		//2022.11.21 - kwmoon(comment)
	for( int r=0;r<16;r++ )
		m_dStrbROffset[r] = DEFAULT_STROBE_ROFFSET;
}

void CTg::DbgInitTimeset(double rate)
{
	double dCClk = rate/2;
	for( int t=0;t<MAX_TIMESET;t++ )
	{
		m_dRate[t] = rate;

		m_dClock[DREL1] [t] =   0.0;
		m_dClock[DRET1] [t] =  dCClk;
		m_dClock[BCLK1] [t] =   0.0;
		m_dClock[CCLK1] [t] =  dCClk;
		m_dClock[BCLK2] [t] =   0.0;
		m_dClock[CCLK2] [t] =  dCClk;
		m_dClock[BCLK3] [t] =   0.0;
		m_dClock[CCLK3] [t] =  dCClk;
		m_dClock[BCLK4] [t] =   0.0;
		m_dClock[CCLK4] [t] =  dCClk;
		m_dClock[BCLK5] [t] =   0.0;
		m_dClock[CCLK5] [t] =  dCClk;
		m_dClock[BCLK6] [t] =   0.0;
		m_dClock[CCLK6] [t] =  dCClk;
		m_dClock[BCLK7] [t] =   0.0;
		m_dClock[CCLK7] [t] =  dCClk;
		m_dClock[BCLK8] [t] =   0.0;
		m_dClock[CCLK8] [t] =  dCClk;
		m_dClock[IOBCLK][t] =   0.0;
		m_dClock[IOCCLK][t] =  dCClk;
		m_dClock[ADBCLK][t] =   0.0;
		m_dClock[ADCCLK][t] =  dCClk;
		m_dClock[STBCLK][t] =   0.0;
		m_dClock[STCCLK][t] =  dCClk;
		m_dClock[STRB3 ][t] =   0.0;
		m_dClock[STRB4 ][t] =  dCClk;
		m_dClock[STRB5 ][t] =   0.0;
		m_dClock[STRB6 ][t] =  dCClk;
		m_dClock[STRB7 ][t] =   0.0;
		m_dClock[STRB8 ][t] =  dCClk;
		//m_dClock[RSTRB1][t] =   0.0;		//2022.11.21 - kwmoon(comment)
		//m_dClock[RSTRB2][t] =  dCClk;		//2022.11.21 - kwmoon(comment)
		//m_dClock[RSTRB3][t] =   0.0;		//2022.11.21 - kwmoon(comment)
		//m_dClock[RSTRB4][t] =  dCClk;		//2022.11.21 - kwmoon(comment)
		//m_dClock[RSTRB5][t] =   0.0;		//2022.11.21 - kwmoon(comment)
		//m_dClock[RSTRB6][t] =  dCClk;		//2022.11.21 - kwmoon(comment)
		//m_dClock[RSTRB7][t] =   0.0;		//2022.11.21 - kwmoon(comment)
		//m_dClock[RSTRB8][t] =  dCClk;		//2022.11.21 - kwmoon(comment)
	}
	DoRate(true);
	for( int pin=0;pin<MAX_CLOCK_LIST;pin++)
		DoClock( pin, true );
}


//Must BCLK start bigger than 10 NS
void CTg::InitTimeset()
{
	double dBCLK = 10.0;
	double dCCLK = 60.0;
	if( 1 )
	{
		for( int t=0;t<MAX_TIMESET;t++ )
		{
			m_dRate[t] = 100.0;	//100 NS

			m_dClock[DREL1] [t] =  dBCLK;
			m_dClock[DRET1] [t] =  dCCLK;

			m_dClock[BCLK1] [t] =  dBCLK;
			m_dClock[CCLK1] [t] =  dCCLK;
			m_dClock[BCLK2] [t] =  dBCLK;
			m_dClock[CCLK2] [t] =  dCCLK;
			m_dClock[BCLK3] [t] =  dBCLK;
			m_dClock[CCLK3] [t] =  dCCLK;
			m_dClock[BCLK4] [t] =  dBCLK;
			m_dClock[CCLK4] [t] =  dCCLK;
			m_dClock[BCLK5] [t] =  dBCLK;
			m_dClock[CCLK5] [t] =  dCCLK;
			m_dClock[BCLK6] [t] =  dBCLK;
			m_dClock[CCLK6] [t] =  dCCLK;
			m_dClock[BCLK7] [t] =  dBCLK;
			m_dClock[CCLK7] [t] =  dCCLK;
			m_dClock[BCLK8] [t] =  dBCLK;
			m_dClock[CCLK8] [t] =  dCCLK;
			m_dClock[IOBCLK][t] =  dBCLK;
			m_dClock[IOCCLK][t] =  dCCLK;
			m_dClock[ADBCLK][t] =  dBCLK;
			m_dClock[ADCCLK][t] =  dCCLK;
			m_dClock[STBCLK][t] =  dBCLK;
			m_dClock[STCCLK][t] =  dCCLK;
			m_dClock[STRB3 ][t] =  dBCLK;
			m_dClock[STRB4 ][t] =  dCCLK;
			m_dClock[STRB5 ][t] =  dBCLK;
			m_dClock[STRB6 ][t] =  dCCLK;
			m_dClock[STRB7 ][t] =  dBCLK;
			m_dClock[STRB8 ][t] =  dCCLK;
			//m_dClock[RSTRB1][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB2][t] =  60.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB3][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB4][t] =  60.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB5][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB6][t] =  60.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB7][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB8][t] =  60.0;		//2022.11.21 - kwmoon(comment)
		}
	}

	DoRate(false);
	for( int pin=0;pin<MAX_CLOCK_LIST;pin++)
		DoClock( pin, false );
}

/*
//Must BCLK start bigger than 10 NS
//2023.08.25 - kwmoon( backup before modify )
void CTg::InitTimeset()
{
	if( 1 )
	{
		for( int t=0;t<MAX_TIMESET;t++ )
		{
			m_dRate[t] = 100.0;	//100 NS

			m_dClock[DREL1] [t] =  10.0;
			m_dClock[DRET1] [t] =  60.0;

			m_dClock[BCLK1] [t] =  10.0;
			m_dClock[CCLK1] [t] =  60.0;
			m_dClock[BCLK2] [t] =  10.0;
			m_dClock[CCLK2] [t] =  60.0;
			m_dClock[BCLK3] [t] =  10.0;
			m_dClock[CCLK3] [t] =  60.0;
			m_dClock[BCLK4] [t] =  10.0;
			m_dClock[CCLK4] [t] =  60.0;
			m_dClock[BCLK5] [t] =  10.0;
			m_dClock[CCLK5] [t] =  60.0;
			m_dClock[BCLK6] [t] =  10.0;
			m_dClock[CCLK6] [t] =  60.0;
			m_dClock[BCLK7] [t] =  10.0;
			m_dClock[CCLK7] [t] =  60.0;
			m_dClock[BCLK8] [t] =  10.0;
			m_dClock[CCLK8] [t] =  60.0;
			m_dClock[IOBCLK][t] =  10.0;
			m_dClock[IOCCLK][t] =  60.0;
			m_dClock[ADBCLK][t] =  10.0;
			m_dClock[ADCCLK][t] =  60.0;
			m_dClock[STBCLK][t] =  10.0;
			m_dClock[STCCLK][t] =  60.0;
			m_dClock[STRB3 ][t] =  10.0;
			m_dClock[STRB4 ][t] =  60.0;
			m_dClock[STRB5 ][t] =  10.0;
			m_dClock[STRB6 ][t] =  60.0;
			m_dClock[STRB7 ][t] =  10.0;
			m_dClock[STRB8 ][t] =  60.0;
			//m_dClock[RSTRB1][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB2][t] =  60.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB3][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB4][t] =  60.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB5][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB6][t] =  60.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB7][t] =  10.0;		//2022.11.21 - kwmoon(comment)
			//m_dClock[RSTRB8][t] =  60.0;		//2022.11.21 - kwmoon(comment)
		}
	}

	DoRate(false);
	for( int pin=0;pin<MAX_CLOCK_LIST;pin++)
		DoClock( pin, false );
}
*/

void CTg::InitTg()
{
	TgStart();
	g_sys.SysDelay( 10 MS );
	TgStop();
	TgMemClear();
	InitClockIDelay();
}

bool CTg::RATE(double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15 )
{
	m_dRate[1 ] = r0 ;
	m_dRate[2 ] = r1 ;
	m_dRate[3 ] = r2 ;
	m_dRate[4 ] = r3 ;
	m_dRate[5 ] = r4 ;
	m_dRate[6 ] = r5 ;
	m_dRate[7 ] = r6 ;
	m_dRate[8 ] = r7 ;
	m_dRate[9 ] = r8 ;
	m_dRate[10] = r9 ;
	m_dRate[11] = r10;
	m_dRate[12] = r11;
	m_dRate[13] = r12;
	m_dRate[14] = r13;
	m_dRate[15] = r14;
	m_dRate[16] = r15;
	//m_dRate[17] = r15;
	//m_dRate[18] = r15;
	//m_dRate[19] = r15;
	//m_dRate[20] = r15;
	//m_dRate[21] = r15;
	//m_dRate[22] = r15;
	//m_dRate[23] = r15;
	//m_dRate[24] = r15;
	//m_dRate[25] = r15;
	//m_dRate[26] = r15;
	//m_dRate[27] = r15;
	//m_dRate[28] = r15;
	//m_dRate[29] = r15;
	//m_dRate[30] = r15;
	//m_dRate[31] = r15;

	bool bValid = true;
	for( int ts=1;ts<17;ts++)
	{
		if( !IsValidRate( ts, m_dRate[ts] ) )
			bValid = false;
	}
	if( !bValid ) return false;
	return DoRate();
}

bool CTg::RATE(double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15,
		  double r16, double r17, double r18, double r19, double r20, double r21, double r22, double r23, double r24, double r25, double r26, double r27, double r28, double r29, double r30 )
{
	m_dRate[1 ] = r0 ;
	m_dRate[2 ] = r1 ;
	m_dRate[3 ] = r2 ;
	m_dRate[4 ] = r3 ;
	m_dRate[5 ] = r4 ;
	m_dRate[6 ] = r5 ;
	m_dRate[7 ] = r6 ;
	m_dRate[8 ] = r7 ;
	m_dRate[9 ] = r8 ;
	m_dRate[10] = r9 ;
	m_dRate[11] = r10;
	m_dRate[12] = r11;
	m_dRate[13] = r12;
	m_dRate[14] = r13;
	m_dRate[15] = r14;
	m_dRate[16] = r15;
	m_dRate[17] = r16;
	m_dRate[18] = r17;
	m_dRate[19] = r18;
	m_dRate[20] = r19;
	m_dRate[21] = r20;
	m_dRate[22] = r21;
	m_dRate[23] = r22;
	m_dRate[24] = r23;
	m_dRate[25] = r24;
	m_dRate[26] = r25;
	m_dRate[27] = r26;
	m_dRate[28] = r27;
	m_dRate[29] = r28;
	m_dRate[30] = r29;
	m_dRate[31] = r30;

	bool bValid = true;
	for( int ts=1;ts<MAX_TIMESET;ts++)
	{
		if( !IsValidRate( ts, m_dRate[ts] ) )
			bValid = false;
	}
	if( !bValid ) return false;
	return DoRate();
}

bool CTg::SET_RATE( int ts, double r )
{
	int nTS = ts+1;
	if( !IsValidRate( nTS, r) ) return false;

	m_dRate[nTS] = r;
	return DoRate();
/*
	bool bPass = true;
	int nCoarse = static_cast<int>( m_dRate[nTS] / COARSE_DELAY_LSB );
	double dCoarseMod = std::fmod ( m_dRate[nTS] , COARSE_DELAY_LSB );
	int nFine   = static_cast<int>( dCoarseMod / FINE_DELAY_LSB );
	if( nCoarse > COARSE_DELAY_MAX )
	{
		//error procedure
		nCoarse = COARSE_DELAY_MAX;
		bPass = false;
	}
	if( nFine > FINE_DELAY_MAX )
	{
		//error procedure
		nFine = FINE_DELAY_MAX;
		bPass = false;
	}
	m_nRate[nTS] = (nCoarse<<8) | (nFine&0x3F);

	g_pcie.write_user( BITCHART::ALPG::RATE_DRELT_BCLK_REG::_RATE + (nTS*4), m_nRate[nTS] );
	return bPass;
*/
}

bool CTg::IsValidRate( int ts, double dRate )
{
	if( (dRate>RATE_LIMIT_1MS) || (dRate<RATE_LIMIT_4NS) )	// 1MS ~ 5NS
	{
		fprintf( stderr, "RATE TS%d RATE(%6.3f NS) Range Error [ %7.1f NS ~ %6.3f NS]\n", ts, dRate, RATE_LIMIT_1MS, RATE_LIMIT_4NS );
		return false;
	}
	return true;
}

bool CTg::IsBClk(int clk)
{
	bool bFirst = false;
	switch ( clk )
	{
		case DREL1:
		case BCLK1:
		case BCLK2:
		case BCLK3:
		case BCLK4:
		case BCLK5:
		case BCLK6:
		case BCLK7:
		case BCLK8:
		case IOBCLK:
		case ADBCLK:
		case STBCLK:
		case STRB3:
		case STRB5:
		case STRB7:
		//case RSTRB1:		//2022.11.21 - kwmoon(comment)
		//case RSTRB3:		//2022.11.21 - kwmoon(comment)
		//case RSTRB5:		//2022.11.21 - kwmoon(comment)
		//case RSTRB7:		//2022.11.21 - kwmoon(comment)
			bFirst = true;
			break;
		default:
			bFirst = false;
			break;
	}
	return bFirst;
}

bool CTg::IsValidClock( int clk, int ts, double dClk )
{
	double dPeriodS = 0.0;
	double dPeriodE = 0.0;
	if( IsBClk(clk) )
	{
		dPeriodS = 0.0;
		dPeriodE = m_dRate[ts];
	}
	else
	{
		dPeriodS = RATE_LIMIT_2_5NS;
		dPeriodE = m_dRate[ts];
	}
	if( (dClk>dPeriodE) || (dClk<dPeriodS) )	// 1MS ~ 5NS
	{
		fprintf( stderr, "%-10s TS%d Period(%6.3f NS) Range Error [ %7.1f NS ~ %6.3f NS]\n",
				 m_sClk[clk].c_str(), ts, dClk, dPeriodS, dPeriodE );
		return false;
	}
	return true;
}

bool CTg::CLOCK(int pin, double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15 )
{
	if( (pin < DREL1) || (pin >MAX_CLOCK_LIST) )
	{
		fprintf( stderr, "Invalid CLOCK pin index %d, between[%d ~ %d]\n", pin, DREL1, MAX_CLOCK_LIST-1 );
		return false;
	}

	if( !IsValidClock( pin, 1 , r0  )) return false; else m_dClock[pin][1 ] = r0 ;
	if( !IsValidClock( pin, 2 , r1  )) return false; else m_dClock[pin][2 ] = r1 ;
	if( !IsValidClock( pin, 3 , r2  )) return false; else m_dClock[pin][3 ] = r2 ;
	if( !IsValidClock( pin, 4 , r3  )) return false; else m_dClock[pin][4 ] = r3 ;
	if( !IsValidClock( pin, 5 , r4  )) return false; else m_dClock[pin][5 ] = r4 ;
	if( !IsValidClock( pin, 6 , r5  )) return false; else m_dClock[pin][6 ] = r5 ;
	if( !IsValidClock( pin, 7 , r6  )) return false; else m_dClock[pin][7 ] = r6 ;
	if( !IsValidClock( pin, 8 , r7  )) return false; else m_dClock[pin][8 ] = r7 ;
	if( !IsValidClock( pin, 9 , r8  )) return false; else m_dClock[pin][9 ] = r8 ;
	if( !IsValidClock( pin, 10, r9  )) return false; else m_dClock[pin][10] = r9 ;
	if( !IsValidClock( pin, 11, r10 )) return false; else m_dClock[pin][11] = r10;
	if( !IsValidClock( pin, 12, r11 )) return false; else m_dClock[pin][12] = r11;
	if( !IsValidClock( pin, 13, r12 )) return false; else m_dClock[pin][13] = r12;
	if( !IsValidClock( pin, 14, r13 )) return false; else m_dClock[pin][14] = r13;
	if( !IsValidClock( pin, 15, r14 )) return false; else m_dClock[pin][15] = r14;
	if( !IsValidClock( pin, 16, r15 )) return false; else m_dClock[pin][16] = r15;

	//2022.12.03 - kwmmon( io -> dre )
	if( pin == IOBCLK )
	{
		m_dClock[DREL1][1 ] = r0 ;
		m_dClock[DREL1][2 ] = r1 ;
		m_dClock[DREL1][3 ] = r2 ;
		m_dClock[DREL1][4 ] = r3 ;
		m_dClock[DREL1][5 ] = r4 ;
		m_dClock[DREL1][6 ] = r5 ;
		m_dClock[DREL1][7 ] = r6 ;
		m_dClock[DREL1][8 ] = r7 ;
		m_dClock[DREL1][9 ] = r8 ;
		m_dClock[DREL1][10] = r9 ;
		m_dClock[DREL1][11] = r10;
		m_dClock[DREL1][12] = r11;
		m_dClock[DREL1][13] = r12;
		m_dClock[DREL1][14] = r13;
		m_dClock[DREL1][15] = r14;
		m_dClock[DREL1][16] = r15;
		DoClock( DREL1 );
	}
	//2022.12.03 - kwmmon( io -> dre )
	if( pin == IOCCLK )
	{
		m_dClock[DRET1][1 ] = r0 ;
		m_dClock[DRET1][2 ] = r1 ;
		m_dClock[DRET1][3 ] = r2 ;
		m_dClock[DRET1][4 ] = r3 ;
		m_dClock[DRET1][5 ] = r4 ;
		m_dClock[DRET1][6 ] = r5 ;
		m_dClock[DRET1][7 ] = r6 ;
		m_dClock[DRET1][8 ] = r7 ;
		m_dClock[DRET1][9 ] = r8 ;
		m_dClock[DRET1][10] = r9 ;
		m_dClock[DRET1][11] = r10;
		m_dClock[DRET1][12] = r11;
		m_dClock[DRET1][13] = r12;
		m_dClock[DRET1][14] = r13;
		m_dClock[DRET1][15] = r14;
		m_dClock[DRET1][16] = r15;
		DoClock( DRET1 );
	}

	int nStrbArray[2][3] = { {STRB3, STRB5, STRB7}, {STRB4, STRB6, STRB8} };
	//2022.11.21 - kwmoon
	//STBCLK - STRB3/STRB5/STRB7
	if( pin == TS_CLOCK_LIST::STBCLK )
	{
		for( int iStb=0;iStb<3;iStb++ )
		{
			m_dClock[nStrbArray[0][iStb]][1 ] = r0 ;
			m_dClock[nStrbArray[0][iStb]][2 ] = r1 ;
			m_dClock[nStrbArray[0][iStb]][3 ] = r2 ;
			m_dClock[nStrbArray[0][iStb]][4 ] = r3 ;
			m_dClock[nStrbArray[0][iStb]][5 ] = r4 ;
			m_dClock[nStrbArray[0][iStb]][6 ] = r5 ;
			m_dClock[nStrbArray[0][iStb]][7 ] = r6 ;
			m_dClock[nStrbArray[0][iStb]][8 ] = r7 ;
			m_dClock[nStrbArray[0][iStb]][9 ] = r8 ;
			m_dClock[nStrbArray[0][iStb]][10] = r9 ;
			m_dClock[nStrbArray[0][iStb]][11] = r10;
			m_dClock[nStrbArray[0][iStb]][12] = r11;
			m_dClock[nStrbArray[0][iStb]][13] = r12;
			m_dClock[nStrbArray[0][iStb]][14] = r13;
			m_dClock[nStrbArray[0][iStb]][15] = r14;
			m_dClock[nStrbArray[0][iStb]][16] = r15;
			DoClock( nStrbArray[0][iStb] );
		}
	}

	//2022.11.21 - kwmoon
	//STCCLK - STRB4/STRB6/STRB8
	if( pin == TS_CLOCK_LIST::STCCLK )
	{
		for( int iStb=0;iStb<3;iStb++ )
		{
			m_dClock[nStrbArray[1][iStb]][1 ] = r0 ;
			m_dClock[nStrbArray[1][iStb]][2 ] = r1 ;
			m_dClock[nStrbArray[1][iStb]][3 ] = r2 ;
			m_dClock[nStrbArray[1][iStb]][4 ] = r3 ;
			m_dClock[nStrbArray[1][iStb]][5 ] = r4 ;
			m_dClock[nStrbArray[1][iStb]][6 ] = r5 ;
			m_dClock[nStrbArray[1][iStb]][7 ] = r6 ;
			m_dClock[nStrbArray[1][iStb]][8 ] = r7 ;
			m_dClock[nStrbArray[1][iStb]][9 ] = r8 ;
			m_dClock[nStrbArray[1][iStb]][10] = r9 ;
			m_dClock[nStrbArray[1][iStb]][11] = r10;
			m_dClock[nStrbArray[1][iStb]][12] = r11;
			m_dClock[nStrbArray[1][iStb]][13] = r12;
			m_dClock[nStrbArray[1][iStb]][14] = r13;
			m_dClock[nStrbArray[1][iStb]][15] = r14;
			m_dClock[nStrbArray[1][iStb]][16] = r15;
			DoClock( nStrbArray[1][iStb] );
		}
	}

	return DoClock( pin );
}

bool CTg::CLOCK(int pin, double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15,
		  double r16, double r17, double r18, double r19, double r20, double r21, double r22, double r23, double r24, double r25, double r26, double r27, double r28, double r29, double r30 )
{
	if( (pin < DREL1) || (pin >MAX_CLOCK_LIST) )
	{
		fprintf( stderr, "Invalid CLOCK pin index %d, between[%d ~ %d]\n", pin, DREL1, MAX_CLOCK_LIST-1 );
		return false;
	}

	if( !IsValidClock( pin, 1 , r0  )) return false; else m_dClock[pin][1 ] = r0 ;
	if( !IsValidClock( pin, 2 , r1  )) return false; else m_dClock[pin][2 ] = r1 ;
	if( !IsValidClock( pin, 3 , r2  )) return false; else m_dClock[pin][3 ] = r2 ;
	if( !IsValidClock( pin, 4 , r3  )) return false; else m_dClock[pin][4 ] = r3 ;
	if( !IsValidClock( pin, 5 , r4  )) return false; else m_dClock[pin][5 ] = r4 ;
	if( !IsValidClock( pin, 6 , r5  )) return false; else m_dClock[pin][6 ] = r5 ;
	if( !IsValidClock( pin, 7 , r6  )) return false; else m_dClock[pin][7 ] = r6 ;
	if( !IsValidClock( pin, 8 , r7  )) return false; else m_dClock[pin][8 ] = r7 ;
	if( !IsValidClock( pin, 9 , r8  )) return false; else m_dClock[pin][9 ] = r8 ;
	if( !IsValidClock( pin, 10, r9  )) return false; else m_dClock[pin][10] = r9 ;
	if( !IsValidClock( pin, 11, r10 )) return false; else m_dClock[pin][11] = r10;
	if( !IsValidClock( pin, 12, r11 )) return false; else m_dClock[pin][12] = r11;
	if( !IsValidClock( pin, 13, r12 )) return false; else m_dClock[pin][13] = r12;
	if( !IsValidClock( pin, 14, r13 )) return false; else m_dClock[pin][14] = r13;
	if( !IsValidClock( pin, 15, r14 )) return false; else m_dClock[pin][15] = r14;
	if( !IsValidClock( pin, 16, r15 )) return false; else m_dClock[pin][16] = r15;
	if( !IsValidClock( pin, 17, r16 )) return false; else m_dClock[pin][17] = r16;
	if( !IsValidClock( pin, 18, r17 )) return false; else m_dClock[pin][18] = r17;
	if( !IsValidClock( pin, 19, r18 )) return false; else m_dClock[pin][19] = r18;
	if( !IsValidClock( pin, 20, r19 )) return false; else m_dClock[pin][20] = r19;
	if( !IsValidClock( pin, 21, r20 )) return false; else m_dClock[pin][21] = r20;
	if( !IsValidClock( pin, 22, r21 )) return false; else m_dClock[pin][22] = r21;
	if( !IsValidClock( pin, 23, r22 )) return false; else m_dClock[pin][23] = r22;
	if( !IsValidClock( pin, 24, r23 )) return false; else m_dClock[pin][24] = r23;
	if( !IsValidClock( pin, 25, r24 )) return false; else m_dClock[pin][25] = r24;
	if( !IsValidClock( pin, 26, r25 )) return false; else m_dClock[pin][26] = r25;
	if( !IsValidClock( pin, 27, r26 )) return false; else m_dClock[pin][27] = r26;
	if( !IsValidClock( pin, 28, r27 )) return false; else m_dClock[pin][28] = r27;
	if( !IsValidClock( pin, 29, r28 )) return false; else m_dClock[pin][29] = r28;
	if( !IsValidClock( pin, 30, r29 )) return false; else m_dClock[pin][30] = r29;
	if( !IsValidClock( pin, 31, r30 )) return false; else m_dClock[pin][31] = r30;

	//2022.12.03 - kwmmon( io -> dre )
	if( pin == IOBCLK )
	{
		m_dClock[DREL1][1 ] = r0 ;
		m_dClock[DREL1][2 ] = r1 ;
		m_dClock[DREL1][3 ] = r2 ;
		m_dClock[DREL1][4 ] = r3 ;
		m_dClock[DREL1][5 ] = r4 ;
		m_dClock[DREL1][6 ] = r5 ;
		m_dClock[DREL1][7 ] = r6 ;
		m_dClock[DREL1][8 ] = r7 ;
		m_dClock[DREL1][9 ] = r8 ;
		m_dClock[DREL1][10] = r9 ;
		m_dClock[DREL1][11] = r10;
		m_dClock[DREL1][12] = r11;
		m_dClock[DREL1][13] = r12;
		m_dClock[DREL1][14] = r13;
		m_dClock[DREL1][15] = r14;
		m_dClock[DREL1][16] = r15;
		m_dClock[DREL1][17] = r16;
		m_dClock[DREL1][18] = r17;
		m_dClock[DREL1][19] = r18;
		m_dClock[DREL1][20] = r19;
		m_dClock[DREL1][21] = r20;
		m_dClock[DREL1][22] = r21;
		m_dClock[DREL1][23] = r22;
		m_dClock[DREL1][24] = r23;
		m_dClock[DREL1][25] = r24;
		m_dClock[DREL1][26] = r25;
		m_dClock[DREL1][27] = r26;
		m_dClock[DREL1][28] = r27;
		m_dClock[DREL1][29] = r28;
		m_dClock[DREL1][30] = r29;
		m_dClock[DREL1][31] = r30;
		DoClock( DREL1 );
	}
	//2022.12.03 - kwmmon( io -> dre )
	if( pin == IOCCLK )
	{
		m_dClock[DRET1][1 ] = r0 ;
		m_dClock[DRET1][2 ] = r1 ;
		m_dClock[DRET1][3 ] = r2 ;
		m_dClock[DRET1][4 ] = r3 ;
		m_dClock[DRET1][5 ] = r4 ;
		m_dClock[DRET1][6 ] = r5 ;
		m_dClock[DRET1][7 ] = r6 ;
		m_dClock[DRET1][8 ] = r7 ;
		m_dClock[DRET1][9 ] = r8 ;
		m_dClock[DRET1][10] = r9 ;
		m_dClock[DRET1][11] = r10;
		m_dClock[DRET1][12] = r11;
		m_dClock[DRET1][13] = r12;
		m_dClock[DRET1][14] = r13;
		m_dClock[DRET1][15] = r14;
		m_dClock[DRET1][16] = r15;
		m_dClock[DRET1][17] = r16;
		m_dClock[DRET1][18] = r17;
		m_dClock[DRET1][19] = r18;
		m_dClock[DRET1][20] = r19;
		m_dClock[DRET1][21] = r20;
		m_dClock[DRET1][22] = r21;
		m_dClock[DRET1][23] = r22;
		m_dClock[DRET1][24] = r23;
		m_dClock[DRET1][25] = r24;
		m_dClock[DRET1][26] = r25;
		m_dClock[DRET1][27] = r26;
		m_dClock[DRET1][28] = r27;
		m_dClock[DRET1][29] = r28;
		m_dClock[DRET1][30] = r29;
		m_dClock[DRET1][31] = r30;
		DoClock( DRET1 );
	}

	int nStrbArray[2][3] = { {STRB3, STRB5, STRB7}, {STRB4, STRB6, STRB8} };
	//2022.11.21 - kwmoon
	//STBCLK - STRB3/STRB5/STRB7
	if( pin == TS_CLOCK_LIST::STBCLK )
	{
		for( int iStb=0;iStb<3;iStb++ )
		{
			m_dClock[nStrbArray[0][iStb]][1 ] = r0 ;
			m_dClock[nStrbArray[0][iStb]][2 ] = r1 ;
			m_dClock[nStrbArray[0][iStb]][3 ] = r2 ;
			m_dClock[nStrbArray[0][iStb]][4 ] = r3 ;
			m_dClock[nStrbArray[0][iStb]][5 ] = r4 ;
			m_dClock[nStrbArray[0][iStb]][6 ] = r5 ;
			m_dClock[nStrbArray[0][iStb]][7 ] = r6 ;
			m_dClock[nStrbArray[0][iStb]][8 ] = r7 ;
			m_dClock[nStrbArray[0][iStb]][9 ] = r8 ;
			m_dClock[nStrbArray[0][iStb]][10] = r9 ;
			m_dClock[nStrbArray[0][iStb]][11] = r10;
			m_dClock[nStrbArray[0][iStb]][12] = r11;
			m_dClock[nStrbArray[0][iStb]][13] = r12;
			m_dClock[nStrbArray[0][iStb]][14] = r13;
			m_dClock[nStrbArray[0][iStb]][15] = r14;
			m_dClock[nStrbArray[0][iStb]][16] = r15;
			m_dClock[nStrbArray[0][iStb]][17] = r16;
			m_dClock[nStrbArray[0][iStb]][18] = r17;
			m_dClock[nStrbArray[0][iStb]][19] = r18;
			m_dClock[nStrbArray[0][iStb]][20] = r19;
			m_dClock[nStrbArray[0][iStb]][21] = r20;
			m_dClock[nStrbArray[0][iStb]][22] = r21;
			m_dClock[nStrbArray[0][iStb]][23] = r22;
			m_dClock[nStrbArray[0][iStb]][24] = r23;
			m_dClock[nStrbArray[0][iStb]][25] = r24;
			m_dClock[nStrbArray[0][iStb]][26] = r25;
			m_dClock[nStrbArray[0][iStb]][27] = r26;
			m_dClock[nStrbArray[0][iStb]][28] = r27;
			m_dClock[nStrbArray[0][iStb]][29] = r28;
			m_dClock[nStrbArray[0][iStb]][30] = r29;
			m_dClock[nStrbArray[0][iStb]][31] = r30;
			DoClock( nStrbArray[0][iStb] );

		}
	}

	//2022.11.21 - kwmoon
	//STCCLK - STRB4/STRB6/STRB8
	if( pin == TS_CLOCK_LIST::STCCLK )
	{
		for( int iStb=0;iStb<3;iStb++ )
		{
			m_dClock[nStrbArray[1][iStb]][1 ] = r0 ;
			m_dClock[nStrbArray[1][iStb]][2 ] = r1 ;
			m_dClock[nStrbArray[1][iStb]][3 ] = r2 ;
			m_dClock[nStrbArray[1][iStb]][4 ] = r3 ;
			m_dClock[nStrbArray[1][iStb]][5 ] = r4 ;
			m_dClock[nStrbArray[1][iStb]][6 ] = r5 ;
			m_dClock[nStrbArray[1][iStb]][7 ] = r6 ;
			m_dClock[nStrbArray[1][iStb]][8 ] = r7 ;
			m_dClock[nStrbArray[1][iStb]][9 ] = r8 ;
			m_dClock[nStrbArray[1][iStb]][10] = r9 ;
			m_dClock[nStrbArray[1][iStb]][11] = r10;
			m_dClock[nStrbArray[1][iStb]][12] = r11;
			m_dClock[nStrbArray[1][iStb]][13] = r12;
			m_dClock[nStrbArray[1][iStb]][14] = r13;
			m_dClock[nStrbArray[1][iStb]][15] = r14;
			m_dClock[nStrbArray[1][iStb]][16] = r15;
			m_dClock[nStrbArray[1][iStb]][17] = r16;
			m_dClock[nStrbArray[1][iStb]][18] = r17;
			m_dClock[nStrbArray[1][iStb]][19] = r18;
			m_dClock[nStrbArray[1][iStb]][20] = r19;
			m_dClock[nStrbArray[1][iStb]][21] = r20;
			m_dClock[nStrbArray[1][iStb]][22] = r21;
			m_dClock[nStrbArray[1][iStb]][23] = r22;
			m_dClock[nStrbArray[1][iStb]][24] = r23;
			m_dClock[nStrbArray[1][iStb]][25] = r24;
			m_dClock[nStrbArray[1][iStb]][26] = r25;
			m_dClock[nStrbArray[1][iStb]][27] = r26;
			m_dClock[nStrbArray[1][iStb]][28] = r27;
			m_dClock[nStrbArray[1][iStb]][29] = r28;
			m_dClock[nStrbArray[1][iStb]][30] = r29;
			m_dClock[nStrbArray[1][iStb]][31] = r30;
			DoClock( nStrbArray[1][iStb] );
		}
	}
	return DoClock( pin );
}

bool CTg::SET_CLK( int pin, int ts, double tval )
{
	bool bPass = true;
	int nTS = ts+1;
	if( (pin < DREL1) || (pin >MAX_CLOCK_LIST) )
	{
		fprintf( stderr, "Invalid CLOCK pin index %d, between[%d ~ %d]\n", pin, DREL1, MAX_CLOCK_LIST-1 );
		return false;
	}
	m_dClock[pin][nTS] = tval ;

	int nCoarse = static_cast<int>( m_dClock[pin][nTS] / COARSE_DELAY_LSB );
	double dCoarseMod = std::fmod ( m_dClock[pin][nTS] , COARSE_DELAY_LSB );
	int nFine   = static_cast<int>( dCoarseMod / FINE_DELAY_LSB );
	if( nCoarse > COARSE_DELAY_MAX )
	{
		//error procedure
		nCoarse = COARSE_DELAY_MAX;
		bPass = false;
	}
	if( nFine > FINE_DELAY_MAX )
	{
		nFine = FINE_DELAY_MAX;
		bPass = false;
	}
	m_nClock[pin][nTS] = (nCoarse<<8) | (nFine&0x3F);

	int offset_p = pin * CLK_ADDR_OFFSET;
	g_pcie.write_user( (BITCHART::ALPG::RATE_DRELT_BCLK_REG::_DREL1 + offset_p ) + (nTS*4), m_nClock[pin][nTS] );

	//2022.12.03 - kwmoon
	if( pin == IOBCLK )
	{
		m_nClock[DREL1][nTS] = m_nClock[pin][nTS];
		int offset_p = DREL1 * CLK_ADDR_OFFSET;
		g_pcie.write_user( (BITCHART::ALPG::RATE_DRELT_BCLK_REG::_DREL1 + offset_p ) + (nTS*4), m_nClock[DREL1][nTS] );
	}

	//2022.12.03 - kwmoon
	if( pin == IOCCLK )
	{
		m_nClock[DRET1][nTS] = m_nClock[pin][nTS];
		int offset_p = DRET1 * CLK_ADDR_OFFSET;
		g_pcie.write_user( (BITCHART::ALPG::RATE_DRELT_BCLK_REG::_DREL1 + offset_p ) + (nTS*4), m_nClock[DRET1][nTS] );
	}

	//2022.11.21 - kwmoon
	//STBCLK - STRB3/STRB5/STRB7
	int nStrbArray[2][3] = { {STRB3, STRB5, STRB7}, {STRB4, STRB6, STRB8} };	//BClk,CClk
	if( pin == TS_CLOCK_LIST::STBCLK )
	{
		for( int iStb=0;iStb<3;iStb++ )
		{
			int iClk = nStrbArray[0][iStb];
			m_nClock[iClk][nTS] = m_nClock[pin][nTS];
			int offset_p = iClk * CLK_ADDR_OFFSET;
			g_pcie.write_user( (BITCHART::ALPG::RATE_DRELT_BCLK_REG::_DREL1 + offset_p ) + (nTS*4), m_nClock[iClk][nTS] );
		}
	}

	//2022.11.21 - kwmoon
	//STCCLK - STRB4/STRB6/STRB8
	if( pin == TS_CLOCK_LIST::STCCLK )
	{
		for( int iStb=0;iStb<3;iStb++ )
		{
			int iClk = nStrbArray[1][iStb];
			m_nClock[iClk][nTS] = m_nClock[pin][nTS];
			int offset_p = iClk * CLK_ADDR_OFFSET;
			g_pcie.write_user( (BITCHART::ALPG::RATE_DRELT_BCLK_REG::_DREL1 + offset_p ) + (nTS*4), m_nClock[iClk][nTS] );
		}
	}

	return bPass;
}

bool CTg::SET_CLK_DELAY( int pin, double tdelay )
{
	bool bPass = true;
	bool bValidClock = false;
	int offset_p = 0x0;
	switch ( pin )
	{
		case DREL1:
			bValidClock = true;
			offset_p = 0x28;
			break;
		case BCLK1:
			bValidClock = true;
			offset_p = 0x0;
			break;
		case BCLK2:
			bValidClock = true;
			offset_p = 0x4;
			break;
		case BCLK3:
			bValidClock = true;
			offset_p = 0x8;
			break;
		case BCLK4:
			bValidClock = true;
			offset_p = 0xC;
			break;
		case BCLK5:
			bValidClock = true;
			offset_p = 0x10;
			break;
		case BCLK6:
			bValidClock = true;
			offset_p = 0x14;
			break;
		case BCLK7:
			bValidClock = true;
			offset_p = 0x18;
			break;
		case BCLK8:
			bValidClock = true;
			offset_p = 0x1C;
			break;
		case IOBCLK:
			bValidClock = true;
			offset_p = 0x20;
			break;
		case ADBCLK:
			bValidClock = true;
			offset_p = 0x24;
			break;
	}
	if( !bValidClock )
	{
		if( (pin>=DREL1) && (pin<MAX_CLOCK_LIST) )
			fprintf( stderr, "CLK(%s) does not support DELAY, use DREL1/BCLK1~8/IOBCLK/ADBCLK\n", m_sClk[pin].c_str() );
		else
			fprintf( stderr, "Invalid CLK(%d) does not support DELAY, use DREL1/BCLK1~8/IOBCLK/ADBCLK\n", pin );
		return false;
	}

	m_dClockDelay[pin] = tdelay ;

	int nCoarse = static_cast<int>( m_dClockDelay[pin] / DELAY_COARSE_DELAY_LSB );
	double dCoarseMod = std::fmod ( m_dClockDelay[pin] , DELAY_COARSE_DELAY_LSB );
	int nFine   = static_cast<int>( dCoarseMod / DELAY_FINE_DELAY_LSB );
	if( nCoarse > COARSE_DELAY_MAX )
	{
		//error procedure
		nCoarse = COARSE_DELAY_MAX;
		bPass = false;
	}
	if( nFine > FINE_DELAY_MAX )
	{
		nFine = FINE_DELAY_MAX;
		bPass = false;
	}
	m_nClockDelay[pin] = (nCoarse<<8) | (nFine&0xFF);
	g_pcie.write_user( BITCHART::ALPG::DELAY_OFFSET_REG::_BCLK1_OFFSET + offset_p, m_nClockDelay[pin] );
	if( 0 )
	{
		fprintf( stdout, "CLK(%s) Delay:%6.1f PS Write(0x%08X,0x%02X)\n", m_sClk[pin].c_str(), tdelay, BITCHART::ALPG::DELAY_OFFSET_REG::_BCLK1_OFFSET + offset_p, m_nClockDelay[pin] );
	}
	return bPass;
}

void CTg::SetStrbROffset(uint row, double delay)
{
	if( (row<0) || (row>15) )
	{
		fprintf( stderr, "%s error : invalid row %d [ 0 ~ 15]\n", __FUNCTION__, row );
		return;
	}
	m_dStrbROffset[row] = DEFAULT_STROBE_ROFFSET + delay;
}

bool CTg::SET_STROBE_DELAY( uint row, double tdelay )
{
	bool bPass = true;

	if( (row<0) || (row>15) )
	{
		fprintf( stderr, "%s error : invalid row %d [ 0 ~ 15]\n", __FUNCTION__, row );
		return false;
	}
	if( tdelay < 0.0 )
	{
		tdelay = 0.0;
		fprintf( stderr, "%s error : delay value(%7.3f PS) error, use [ 0 ~ %d]\n", __FUNCTION__, tdelay, STROBE_MAX_DELAY_VALUE );
		return false;
	}
	if( tdelay > RSTORBE_MAX_VALUE )
	{
		tdelay = RSTORBE_MAX_VALUE;
		fprintf( stderr, "%s error : delay value(%7.3f PS) error, use [ 0 ~ %d]\n", __FUNCTION__, tdelay, STROBE_MAX_DELAY_VALUE );
	}

	double total_delay = m_dStrbROffset[row] + tdelay;	//2022.12.05 - KWMOON
	int nCoarse = static_cast<int>( total_delay / DELAY_COARSE_DELAY_LSB );
	double dCoarseMod = std::fmod ( total_delay , DELAY_COARSE_DELAY_LSB );
	int nFine   = static_cast<int>( dCoarseMod / DELAY_FINE_DELAY_LSB );
	if( nCoarse > STROBE_COARSE_DELAY_MAX )
	{
		//error procedure
		nCoarse = STROBE_COARSE_DELAY_MAX;
		bPass = false;
	}
	if( nFine > STROBE_FINE_DELAY_MAX )
	{
		nFine = STROBE_FINE_DELAY_MAX;
		bPass = false;
	}
	int nDelay = (nCoarse<<8) | (nFine&0xFF);

	int Address = BITCHART::ALPG::DELAY_OFFSET_REG::_R1_OFFSET + (row*4);
	g_pcie.write_user( Address , nDelay );
	if( 0 )
		fprintf( stdout, "%s ROW:%02d %7.2f PS (0x%08X,0x%04X)\n", __FUNCTION__, row, tdelay, Address, nDelay);
	g_sys.SysDelay( 1 US );
	return bPass;
}

//X
bool CTg::SET_STROBE_DELAY( uint start, uint end, double tdelay )
{
	bool bPass = true;
	if( start < end )
	{
		fprintf( stderr, "%s start scan number% is invalid[0~%d]\n ", __FUNCTION__, start, SCAN_MEM_DEPTH-1 );
		return false;
	}
	if( (start<0) || (start>SCAN_MEM_DEPTH-1) )
	{
		fprintf( stderr, "%s start scan number% is invalid[0~%d]\n ", __FUNCTION__, start, SCAN_MEM_DEPTH-1 );
		return false;
	}
	if( (end<0) || (end>SCAN_MEM_DEPTH-1) )
	{
		fprintf( stderr, "%s start scan number% is invalid[0~%d]\n ", __FUNCTION__, start, SCAN_MEM_DEPTH-1 );
		return false;
	}

	for( uint s=start;s<end;s++ )
	{
		m_dStrbDelay[s] = tdelay;
		int nCoarse = static_cast<int>( m_dStrbDelay[s] / DELAY_COARSE_DELAY_LSB );
		double dCoarseMod = std::fmod ( m_dStrbDelay[s] , DELAY_COARSE_DELAY_LSB );
		int nFine   = static_cast<int>( dCoarseMod / DELAY_FINE_DELAY_LSB );
		if( nCoarse > COARSE_DELAY_MAX )
		{
			//error procedure
			nCoarse = COARSE_DELAY_MAX;
			bPass = false;
		}
		if( nFine > FINE_DELAY_MAX )
		{
			nFine = FINE_DELAY_MAX;
			bPass = false;
		}
		m_nStrbDelay[s] = (nCoarse<<8) | (nFine&0x3F);
	}
	return bPass;
}

//X
bool CTg::APPLY_STROBE_DELAY(uint scan_no)
{
	fprintf( stdout, "Apply STROBE_DELAY ScanNo:%d[%x], Delay:%7.3f PS[0x%04X]\n",
			 scan_no, scan_no, m_dStrbDelay[scan_no], m_nStrbDelay[scan_no] );

	g_pcie.write_user( BITCHART::ALPG::_STRB1_2_OFFSET , m_nStrbDelay[scan_no] );
	g_pcie.write_user( BITCHART::ALPG::_STRB3_4_OFFSET , m_nStrbDelay[scan_no] );
	g_pcie.write_user( BITCHART::ALPG::_STRB5_6_OFFSET , m_nStrbDelay[scan_no] );
	g_pcie.write_user( BITCHART::ALPG::_STRB7_8_OFFSET , m_nStrbDelay[scan_no] );
	g_pcie.write_user( BITCHART::ALPG::_RSTRB1_2_OFFSET, m_nStrbDelay[scan_no] );
	g_pcie.write_user( BITCHART::ALPG::_RSTRB3_4_OFFSET, m_nStrbDelay[scan_no] );
	g_pcie.write_user( BITCHART::ALPG::_RSTRB5_6_OFFSET, m_nStrbDelay[scan_no] );
	g_pcie.write_user( BITCHART::ALPG::_RSTRB7_8_OFFSET, m_nStrbDelay[scan_no] );
	return true;
}

bool CTg::DoRate( bool bWrite )
{
	bool bPass = true;
	for( int ts=0;ts<MAX_TIMESET;ts++ )
	{
		int nCoarse = static_cast<int>( m_dRate[ts] / COARSE_DELAY_LSB )-1;
		double dCoarseMod = std::fmod ( m_dRate[ts] , COARSE_DELAY_LSB );
		int nFine   = static_cast<int>( dCoarseMod / FINE_DELAY_LSB );
		if( nCoarse > COARSE_DELAY_MAX )
		{
			//error procedure
			nCoarse = COARSE_DELAY_MAX;
			bPass = false;
		}
		if( nFine > FINE_DELAY_MAX )
		{
			nFine = FINE_DELAY_MAX;
			bPass = false;
		}
		m_nRate[ts] = (nCoarse<<8) | (nFine&0x3F);
	}
	if( bWrite )
	{
		for( int ts=0;ts<MAX_TIMESET;ts++ )
		{
			g_pcie.write_user( BITCHART::ALPG::RATE_DRELT_BCLK_REG::_RATE + (ts*4), m_nRate[ts] );
			if( 0 )
			{
				fprintf( stdout, "RATE  TS%02d  Write(0x%08X,0x%08X)\n",
						 ts, BITCHART::ALPG::RATE_DRELT_BCLK_REG::_RATE + (ts*4), m_nRate[ts] );
			}

		}
	}
	return bPass;
}

bool CTg::DoClock( int pin, bool bWrite )
{
	bool bPass = true;
	for( int ts=0;ts<MAX_TIMESET;ts++ )
	{
		int nCoarse = static_cast<int>( m_dClock[pin][ts] / COARSE_DELAY_LSB );
		double dCoarseMod = std::fmod ( m_dClock[pin][ts] , COARSE_DELAY_LSB );
		int nFine   = static_cast<int>( dCoarseMod / FINE_DELAY_LSB );
		if( nCoarse > COARSE_DELAY_MAX )
		{
			//error procedure
			nCoarse = COARSE_DELAY_MAX;
			bPass = false;
		}
		if( nFine > FINE_DELAY_MAX )
		{
			nFine = FINE_DELAY_MAX;
			bPass = false;
		}
		m_nClock[pin][ts] = (nCoarse<<8) | (nFine&0x3F);
	}
	if( bWrite )
	{
		int offset_p = pin * CLK_ADDR_OFFSET;
		for( int ts=0;ts<MAX_TIMESET;ts++ )
		{
			uint address = (BITCHART::ALPG::RATE_DRELT_BCLK_REG::_DREL1 + offset_p ) + (ts*4);
			g_pcie.write_user( address, m_nClock[pin][ts] );
			if( 0 )
			{
				fprintf( stdout, "%-10s TS%02d %7.3f NS Write(0x%08X,0x%08X)\n",
						 m_sClk[pin].c_str(), ts, m_dClock[pin][ts], address, m_nClock[pin][ts] );
			}
		}
	}
	return bPass;
}

bool CTg::WriteTimeset()
{
	for( int ts=0;ts<MAX_TIMESET;ts++ )
	{
		g_pcie.write_user( BITCHART::ALPG::RATE_DRELT_BCLK_REG::_RATE + (ts*4), m_nRate[ts] );
	}

	for( int pin=0;pin<TS_CLOCK_LIST::MAX_CLOCK_LIST;pin++ )
	{
		int offset_p = pin * CLK_ADDR_OFFSET;
		for( int ts=0;ts<MAX_TIMESET;ts++ )
		{
			g_pcie.write_user( (BITCHART::ALPG::RATE_DRELT_BCLK_REG::_DREL1 + offset_p ) + (ts*4), m_nClock[pin][ts] );
		}
	}
	return true;
}

void CTg::PrintRate()
{
	g_log.Printf( "\n\n%-15s\n", "RATE" );
	for( int ts=0;ts<MAX_USE_TIMESET;ts++ )
		g_log.Printf( "%10s%02d  ", "TS", ts );
	g_log.Printf( "\n" );

	for( int ts=0;ts<MAX_USE_TIMESET;ts++ )
		g_log.Printf( "%9.3f NS  ", m_dRate[ts] );
	g_log.Printf( "\n" );

	for( int ts=0;ts<MAX_USE_TIMESET;ts++ )
		g_log.Printf( "COARSE[%04X]  ", (m_nRate[ts]>>8)&0xFFFF );
	g_log.Printf( "\n" );

	for( int ts=0;ts<MAX_USE_TIMESET;ts++ )
		g_log.Printf( "FINE  [%04X]  ", m_nRate[ts]&0x3F );
	g_log.Printf( "\n" );
	g_log.Printf( NULL );
}

void CTg::PrintClock()
{
	for( int clk=0;clk<MAX_CLOCK_LIST;clk++ )
	{
		g_log.Printf( "\n\n%-15s\n", m_sClk[clk].c_str() );
		for( int ts=0;ts<MAX_USE_TIMESET;ts++ )
			g_log.Printf( "%9.3f NS  ", m_dClock[clk][ts] );
		g_log.Printf( "\n" );

		for( int ts=0;ts<MAX_USE_TIMESET;ts++ )
			g_log.Printf( "COARSE[%04X]  ", (m_nClock[clk][ts]>>8)&0xFFFF );
		g_log.Printf( "\n" );

		for( int ts=0;ts<MAX_USE_TIMESET;ts++ )
			g_log.Printf( "FINE  [%04X]  ", m_nClock[clk][ts]&0x3F );
		g_log.Printf( "\n" );
		g_log.Printf( NULL );
	}
}

void CTg::TgStart()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_TG_START, 0x1 );
	g_sys.SysDelay( 10 US );
}

void CTg::TgStop()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_TG_STOP, 0x1 );
	g_sys.SysDelay( 10 US );
}

void CTg::TgBclkReset()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_BCLK_RESET, 0x1 );		//BCLK RESET
	g_sys.SysDelay( 20 US );
}

void CTg::TgMemClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_TG_MEM_CLEAR, 0x1 );		//Memory Clear
	g_sys.SysDelay( 20 US );
}

void CTg::InitStrobeDelay()
{
	for(int s=0;s<SCAN_MEM_DEPTH;s++ )
	{
		m_dStrbDelay[s] = 0.0;
		m_nStrbDelay[s] = 0x0;
	}
}

/*
//2023.08.25 - kwmoon cpe except, all other  +500ps
void CTg::InitClockIDelay()
{
	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_IOA  );
	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_IOB  );
	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_UCLK );

	m_dIDelay[IOA_IOCLK   ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOA_ADCLK   ] = 2234.0 + 500.0;	//PS
	m_dIDelay[IOA_CPE0    ] = 1000.0;	//PS	//2023-01-04 fatric
	m_dIDelay[IOA_CPE1    ] = 1000.0;	//PS	//2023-01-04 fatric
	m_dIDelay[IOA_STRB1_0 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOA_STRB1_1 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOA_STRB2_0 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOA_STRB2_1 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOB_IOCLK   ] = 2156.0 + 500.0;	//PS
	m_dIDelay[IOB_ADCLK   ] = 2312.0 + 500.0;	//PS
	m_dIDelay[IOB_CPE0    ] = 1000.0;	//PS
	m_dIDelay[IOB_CPE1    ] = 1000.0;	//PS
	m_dIDelay[IOB_STRB1_0 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOB_STRB1_1 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOB_STRB2_0 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[IOB_STRB2_1 ] = 2000.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK1  ] = 2156.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK2  ] = 2156.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK3  ] = 2390.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK4  ] = 2624.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK5  ] = 2234.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK6  ] = 2234.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK7  ] = 2312.0 + 500.0;	//PS
	m_dIDelay[UCLK_BCLK8  ] = 2390.0 + 500.0;	//PS
	m_dIDelay[UCLK_STRB1_0] = 2000.0 + 500.0;	//PS
	m_dIDelay[UCLK_STRB1_1] = 2000.0 + 500.0;	//PS
	m_dIDelay[UCLK_CPE    ] = 1000.0;	//PS	//2023-01-04 fatric

	SetClockIDelay( IOA_IOCLK    , m_dIDelay[IOA_IOCLK   ] );
	SetClockIDelay( IOA_ADCLK    , m_dIDelay[IOA_ADCLK   ] );
	SetClockIDelay( IOA_CPE0     , m_dIDelay[IOA_CPE0    ] );
	SetClockIDelay( IOA_CPE1     , m_dIDelay[IOA_CPE1    ] );
	SetClockIDelay( IOA_STRB1_0  , m_dIDelay[IOA_STRB1_0 ] );
	SetClockIDelay( IOA_STRB1_1  , m_dIDelay[IOA_STRB1_1 ] );
	SetClockIDelay( IOA_STRB2_0  , m_dIDelay[IOA_STRB2_0 ] );
	SetClockIDelay( IOA_STRB2_1  , m_dIDelay[IOA_STRB2_1 ] );
	SetClockIDelay( IOB_IOCLK    , m_dIDelay[IOB_IOCLK   ] );
	SetClockIDelay( IOB_ADCLK    , m_dIDelay[IOB_ADCLK   ] );
	SetClockIDelay( IOB_CPE0     , m_dIDelay[IOB_CPE0    ] );
	SetClockIDelay( IOB_CPE1     , m_dIDelay[IOB_CPE1    ] );
	SetClockIDelay( IOB_STRB1_0  , m_dIDelay[IOB_STRB1_0 ] );
	SetClockIDelay( IOB_STRB1_1  , m_dIDelay[IOB_STRB1_1 ] );
	SetClockIDelay( IOB_STRB2_0  , m_dIDelay[IOB_STRB2_0 ] );
	SetClockIDelay( IOB_STRB2_1  , m_dIDelay[IOB_STRB2_1 ] );
	SetClockIDelay( UCLK_BCLK1   , m_dIDelay[UCLK_BCLK1  ] );
	SetClockIDelay( UCLK_BCLK2   , m_dIDelay[UCLK_BCLK2  ] );
	SetClockIDelay( UCLK_BCLK3   , m_dIDelay[UCLK_BCLK3  ] );
	SetClockIDelay( UCLK_BCLK4   , m_dIDelay[UCLK_BCLK4  ] );
	SetClockIDelay( UCLK_BCLK5   , m_dIDelay[UCLK_BCLK5  ] );
	SetClockIDelay( UCLK_BCLK6   , m_dIDelay[UCLK_BCLK6  ] );
	SetClockIDelay( UCLK_BCLK7   , m_dIDelay[UCLK_BCLK7  ] );
	SetClockIDelay( UCLK_BCLK8   , m_dIDelay[UCLK_BCLK8  ] );
	SetClockIDelay( UCLK_STRB1_0 , m_dIDelay[UCLK_STRB1_0] );
	SetClockIDelay( UCLK_STRB1_1 , m_dIDelay[UCLK_STRB1_1] );
	SetClockIDelay( UCLK_CPE     , m_dIDelay[UCLK_CPE    ] );

	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_IOA  );
	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_IOB  );
	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_UCLK );
}
*/

////2023.08.25 - original backup
//void CTg::InitClockIDelay()
//{
//	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_IOA  );
//	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_IOB  );
//	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_UCLK );
//
//    double dOffset = 0.0;
//	if( 1 )
//	{
//        m_dIDelay[IOA_IOCLK   ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOA_ADCLK   ] = 2234.0 + dOffset;	//PS
//        m_dIDelay[IOA_CPE0    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
//        m_dIDelay[IOA_CPE1    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
//        m_dIDelay[IOA_STRB1_0 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOA_STRB1_1 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOA_STRB2_0 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOA_STRB2_1 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOB_IOCLK   ] = 2156.0 + dOffset;	//PS
//        m_dIDelay[IOB_ADCLK   ] = 2312.0 + dOffset;	//PS
//        m_dIDelay[IOB_CPE0    ] = 1000.0 + dOffset;	//PS
//        m_dIDelay[IOB_CPE1    ] = 1000.0 + dOffset;	//PS
//        m_dIDelay[IOB_STRB1_0 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOB_STRB1_1 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOB_STRB2_0 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[IOB_STRB2_1 ] = 2000.0 + dOffset;	//PS
//        m_dIDelay[UCLK_BCLK1  ] = 2156.0 + dOffset;	//PS
//        m_dIDelay[UCLK_BCLK2  ] = 2156.0 + dOffset;	//PS
//        m_dIDelay[UCLK_BCLK3  ] = 2390.0 + dOffset;	//PS
//        m_dIDelay[UCLK_BCLK4  ] = 2624.0 + dOffset;	//PS
//		m_dIDelay[UCLK_BCLK5  ] = 2234.0 + 400.0;		//2023.10.27 - KWMOON(CLE + 400 PS )
//		m_dIDelay[UCLK_BCLK6  ] = 2234.0 + 200.0;		//2023.10.27 - KWMOON(ALE + 100 PS )
//        m_dIDelay[UCLK_BCLK7  ] = 2312.0 + dOffset;	//PS
//        m_dIDelay[UCLK_BCLK8  ] = 2390.0 + dOffset;	//PS
//        m_dIDelay[UCLK_STRB1_0] = 2000.0 + dOffset;	//PS
//        m_dIDelay[UCLK_STRB1_1] = 2000.0 + dOffset;	//PS
//        m_dIDelay[UCLK_CPE    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
//	}
//	else
//	{
//		m_dIDelay[IOA_IOCLK   ] = 2500.0;	//PS
//		m_dIDelay[IOA_ADCLK   ] = 2500.0;	//PS
//		m_dIDelay[IOA_CPE0    ] = 2500.0;	//PS
//		m_dIDelay[IOA_CPE1    ] = 2500.0;	//PS
//		m_dIDelay[IOA_STRB1_0 ] = 2500.0;	//PS
//		m_dIDelay[IOA_STRB1_1 ] = 2500.0;	//PS
//		m_dIDelay[IOA_STRB2_0 ] = 2500.0;	//PS
//		m_dIDelay[IOA_STRB2_1 ] = 2500.0;	//PS
//		m_dIDelay[IOB_IOCLK   ] = 2500.0;	//PS
//		m_dIDelay[IOB_ADCLK   ] = 2500.0;	//PS
//		m_dIDelay[IOB_CPE0    ] = 2500.0;	//PS
//		m_dIDelay[IOB_CPE1    ] = 2500.0;	//PS
//		m_dIDelay[IOB_STRB1_0 ] = 2500.0;	//PS
//		m_dIDelay[IOB_STRB1_1 ] = 2500.0;	//PS
//		m_dIDelay[IOB_STRB2_0 ] = 2500.0;	//PS
//		m_dIDelay[IOB_STRB2_1 ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK1  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK2  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK3  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK4  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK5  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK6  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK7  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_BCLK8  ] = 2500.0;	//PS
//		m_dIDelay[UCLK_STRB1_0] = 2500.0;	//PS
//		m_dIDelay[UCLK_STRB1_1] = 2500.0;	//PS
//		m_dIDelay[UCLK_CPE    ] = 2500.0;	//PS
//
//	}
//
//	SetClockIDelay( IOA_IOCLK    , m_dIDelay[IOA_IOCLK   ] );
//	SetClockIDelay( IOA_ADCLK    , m_dIDelay[IOA_ADCLK   ] );
//	SetClockIDelay( IOA_CPE0     , m_dIDelay[IOA_CPE0    ] );
//	SetClockIDelay( IOA_CPE1     , m_dIDelay[IOA_CPE1    ] );
//	SetClockIDelay( IOA_STRB1_0  , m_dIDelay[IOA_STRB1_0 ] );
//	SetClockIDelay( IOA_STRB1_1  , m_dIDelay[IOA_STRB1_1 ] );
//	SetClockIDelay( IOA_STRB2_0  , m_dIDelay[IOA_STRB2_0 ] );
//	SetClockIDelay( IOA_STRB2_1  , m_dIDelay[IOA_STRB2_1 ] );
//	SetClockIDelay( IOB_IOCLK    , m_dIDelay[IOB_IOCLK   ] );
//	SetClockIDelay( IOB_ADCLK    , m_dIDelay[IOB_ADCLK   ] );
//	SetClockIDelay( IOB_CPE0     , m_dIDelay[IOB_CPE0    ] );
//	SetClockIDelay( IOB_CPE1     , m_dIDelay[IOB_CPE1    ] );
//	SetClockIDelay( IOB_STRB1_0  , m_dIDelay[IOB_STRB1_0 ] );
//	SetClockIDelay( IOB_STRB1_1  , m_dIDelay[IOB_STRB1_1 ] );
//	SetClockIDelay( IOB_STRB2_0  , m_dIDelay[IOB_STRB2_0 ] );
//	SetClockIDelay( IOB_STRB2_1  , m_dIDelay[IOB_STRB2_1 ] );
//	SetClockIDelay( UCLK_BCLK1   , m_dIDelay[UCLK_BCLK1  ] );
//	SetClockIDelay( UCLK_BCLK2   , m_dIDelay[UCLK_BCLK2  ] );
//	SetClockIDelay( UCLK_BCLK3   , m_dIDelay[UCLK_BCLK3  ] );
//	SetClockIDelay( UCLK_BCLK4   , m_dIDelay[UCLK_BCLK4  ] );
//	SetClockIDelay( UCLK_BCLK5   , m_dIDelay[UCLK_BCLK5  ] );
//	SetClockIDelay( UCLK_BCLK6   , m_dIDelay[UCLK_BCLK6  ] );
//	SetClockIDelay( UCLK_BCLK7   , m_dIDelay[UCLK_BCLK7  ] );
//	SetClockIDelay( UCLK_BCLK8   , m_dIDelay[UCLK_BCLK8  ] );
//	SetClockIDelay( UCLK_STRB1_0 , m_dIDelay[UCLK_STRB1_0] );
//	SetClockIDelay( UCLK_STRB1_1 , m_dIDelay[UCLK_STRB1_1] );
//	SetClockIDelay( UCLK_CPE     , m_dIDelay[UCLK_CPE    ] );
//
//	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_IOA  );
//	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_IOB  );
//	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_UCLK );
//}

void CTg::InitClockIDelay()
{
	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_IOA  );
	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_IOB  );
	SetIDelayReset( EN_LOGIC_TYPE::LOGIC_UCLK );

	double dOffset = 0.0;
	if( 1 )
	{
		m_dIDelay[IOA_IOCLK   ] = 1950.0 + dOffset;	//PS
		m_dIDelay[IOA_ADCLK   ] = 1600.0 + dOffset;	//PS
		m_dIDelay[IOA_CPE0    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
		m_dIDelay[IOA_CPE1    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
		m_dIDelay[IOA_STRB1_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_STRB1_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_STRB2_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_STRB2_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_IOCLK   ] = 1950.0 + dOffset;	//PS
		m_dIDelay[IOB_ADCLK   ] = 1600.0 + dOffset;	//PS
		m_dIDelay[IOB_CPE0    ] = 1000.0 + dOffset;	//PS
		m_dIDelay[IOB_CPE1    ] = 1000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB1_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB1_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB2_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB2_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK1  ] = 2300.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK2  ] = 2300.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK3  ] = 2300.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK4  ] = 2300.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK5  ] = 2400.0 + dOffset;	//2023.11.08 - KWMOON(+0)	//2023.11.03 - KWMOON(+400)	//2023.10.27 - KWMOON(CLE + 400 PS )
		m_dIDelay[UCLK_BCLK6  ] = 2650.0 + dOffset;	//2023.11.08 - KWMOON(+0)	//2023.11.03 - KWMOON(+400)	//2023.10.27 - KWMOON(ALE + 100 PS )
		m_dIDelay[UCLK_BCLK7  ] = 2500.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK8  ] = 2500.0 + dOffset;	//PS
		m_dIDelay[UCLK_STRB1_0] = 2000.0 + dOffset;	//PS
		m_dIDelay[UCLK_STRB1_1] = 2000.0 + dOffset;	//PS
		m_dIDelay[UCLK_CPE    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric

		/*   -hyk
		m_dIDelay[IOA_IOCLK   ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_ADCLK   ] = 2234.0 + dOffset;	//PS
//		m_dIDelay[IOA_CPE0    ] = 2000.0 + dOffset;	//PS
//		m_dIDelay[IOA_CPE1    ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_CPE0    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
		m_dIDelay[IOA_CPE1    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
		m_dIDelay[IOA_STRB1_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_STRB1_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_STRB2_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOA_STRB2_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_IOCLK   ] = 2156.0 + dOffset;	//PS
		m_dIDelay[IOB_ADCLK   ] = 2312.0 + dOffset;	//PS
//		m_dIDelay[IOB_CPE0    ] = 2000.0 + dOffset;	//PS	//2023-01-04 fatric
//		m_dIDelay[IOB_CPE1    ] = 2000.0 + dOffset;	//PS	//2023-01-04 fatric
		m_dIDelay[IOB_CPE0    ] = 1000.0 + dOffset;	//PS
		m_dIDelay[IOB_CPE1    ] = 1000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB1_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB1_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB2_0 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[IOB_STRB2_1 ] = 2000.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK1  ] = 2156.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK2  ] = 2156.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK3  ] = 2390.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK4  ] = 2624.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK5  ] = 2234.0 + 400.0;		//2023.10.27 - KWMOON(CLE + 400 PS )
		m_dIDelay[UCLK_BCLK6  ] = 2234.0 + 100.0;		//2023.10.27 - KWMOON(ALE + 100 PS )
		m_dIDelay[UCLK_BCLK7  ] = 2312.0 + dOffset;	//PS
		m_dIDelay[UCLK_BCLK8  ] = 2390.0 + dOffset;	//PS
		m_dIDelay[UCLK_STRB1_0] = 2000.0 + dOffset;	//PS
		m_dIDelay[UCLK_STRB1_1] = 2000.0 + dOffset;	//PS
//		m_dIDelay[UCLK_CPE    ] = 2000.0 + dOffset;	//PS
		m_dIDelay[UCLK_CPE    ] = 1000.0 + dOffset;	//PS	//2023-01-04 fatric
		 */
	}
	else
	{
		m_dIDelay[IOA_IOCLK   ] = 2500.0;	//PS
		m_dIDelay[IOA_ADCLK   ] = 2500.0;	//PS
		m_dIDelay[IOA_CPE0    ] = 2500.0;	//PS
		m_dIDelay[IOA_CPE1    ] = 2500.0;	//PS
		m_dIDelay[IOA_STRB1_0 ] = 2500.0;	//PS
		m_dIDelay[IOA_STRB1_1 ] = 2500.0;	//PS
		m_dIDelay[IOA_STRB2_0 ] = 2500.0;	//PS
		m_dIDelay[IOA_STRB2_1 ] = 2500.0;	//PS
		m_dIDelay[IOB_IOCLK   ] = 2500.0;	//PS
		m_dIDelay[IOB_ADCLK   ] = 2500.0;	//PS
		m_dIDelay[IOB_CPE0    ] = 2500.0;	//PS
		m_dIDelay[IOB_CPE1    ] = 2500.0;	//PS
		m_dIDelay[IOB_STRB1_0 ] = 2500.0;	//PS
		m_dIDelay[IOB_STRB1_1 ] = 2500.0;	//PS
		m_dIDelay[IOB_STRB2_0 ] = 2500.0;	//PS
		m_dIDelay[IOB_STRB2_1 ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK1  ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK2  ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK3  ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK4  ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK5  ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK6  ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK7  ] = 2500.0;	//PS
		m_dIDelay[UCLK_BCLK8  ] = 2500.0;	//PS
		m_dIDelay[UCLK_STRB1_0] = 2500.0;	//PS
		m_dIDelay[UCLK_STRB1_1] = 2500.0;	//PS
		m_dIDelay[UCLK_CPE    ] = 2500.0;	//PS

	}

	SetClockIDelay( IOA_IOCLK    , m_dIDelay[IOA_IOCLK   ] );
	SetClockIDelay( IOA_ADCLK    , m_dIDelay[IOA_ADCLK   ] );
	SetClockIDelay( IOA_CPE0     , m_dIDelay[IOA_CPE0    ] );
	SetClockIDelay( IOA_CPE1     , m_dIDelay[IOA_CPE1    ] );
	SetClockIDelay( IOA_STRB1_0  , m_dIDelay[IOA_STRB1_0 ] );
	SetClockIDelay( IOA_STRB1_1  , m_dIDelay[IOA_STRB1_1 ] );
	SetClockIDelay( IOA_STRB2_0  , m_dIDelay[IOA_STRB2_0 ] );
	SetClockIDelay( IOA_STRB2_1  , m_dIDelay[IOA_STRB2_1 ] );
	SetClockIDelay( IOB_IOCLK    , m_dIDelay[IOB_IOCLK   ] );
	SetClockIDelay( IOB_ADCLK    , m_dIDelay[IOB_ADCLK   ] );
	SetClockIDelay( IOB_CPE0     , m_dIDelay[IOB_CPE0    ] );
	SetClockIDelay( IOB_CPE1     , m_dIDelay[IOB_CPE1    ] );
	SetClockIDelay( IOB_STRB1_0  , m_dIDelay[IOB_STRB1_0 ] );
	SetClockIDelay( IOB_STRB1_1  , m_dIDelay[IOB_STRB1_1 ] );
	SetClockIDelay( IOB_STRB2_0  , m_dIDelay[IOB_STRB2_0 ] );
	SetClockIDelay( IOB_STRB2_1  , m_dIDelay[IOB_STRB2_1 ] );
	SetClockIDelay( UCLK_BCLK1   , m_dIDelay[UCLK_BCLK1  ] );
	SetClockIDelay( UCLK_BCLK2   , m_dIDelay[UCLK_BCLK2  ] );
	SetClockIDelay( UCLK_BCLK3   , m_dIDelay[UCLK_BCLK3  ] );
	SetClockIDelay( UCLK_BCLK4   , m_dIDelay[UCLK_BCLK4  ] );
	SetClockIDelay( UCLK_BCLK5   , m_dIDelay[UCLK_BCLK5  ] );
	SetClockIDelay( UCLK_BCLK6   , m_dIDelay[UCLK_BCLK6  ] );
	SetClockIDelay( UCLK_BCLK7   , m_dIDelay[UCLK_BCLK7  ] );
	SetClockIDelay( UCLK_BCLK8   , m_dIDelay[UCLK_BCLK8  ] );
	SetClockIDelay( UCLK_STRB1_0 , m_dIDelay[UCLK_STRB1_0] );
	SetClockIDelay( UCLK_STRB1_1 , m_dIDelay[UCLK_STRB1_1] );
	SetClockIDelay( UCLK_CPE     , m_dIDelay[UCLK_CPE    ] );

	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_IOA  );
	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_IOB  );
	SetIDelayRequest( EN_LOGIC_TYPE::LOGIC_UCLK );
}

void CTg::SetIDelayReset (EN_LOGIC_TYPE logic)
{
	switch ( logic )
	{
		case LOGIC_IOA:
			g_pcie.write_user( BITCHART::DRV_IOA::_IO_DELAY_RESET, 0x1 );
			break;
		case LOGIC_IOB:
			g_pcie.write_user( BITCHART::DRV_IOB::_IO_DELAY_RESET, 0x1 );
			break;
		case LOGIC_UCLK:
			g_pcie.write_user( BITCHART::DRV_UCLK::_IO_DELAY_RESET, 0x1 );
			break;
		default:
			break;
	}
	g_sys.SysDelay( 150 US );
}

void CTg::SetIDelayRequest(EN_LOGIC_TYPE logic)
{
	switch ( logic )
	{
		case LOGIC_IOA:
			g_pcie.write_user( BITCHART::DRV_IOA::_IO_DELAY_REQ, 0x1 );
			break;
		case LOGIC_IOB:
			g_pcie.write_user( BITCHART::DRV_IOB::_IO_DELAY_REQ, 0x1 );
			break;
		case LOGIC_UCLK:
			g_pcie.write_user( BITCHART::DRV_UCLK::_IO_DELAY_REQ, 0x1 );
			break;
		default:
			break;
	}
	g_sys.SysDelay( 150 US );
}

bool CTg::SetClockIDelay( int pin, double ps )
{
	if( (pin<IOA_IOCLK) || (pin>UCLK_CPE) ) return false;
	int quotient  = 0;
	double delay = ps;
	double d1 = delay/IDELAY_RESOLUTION;
	double d2 = std::fmod( delay, IDELAY_RESOLUTION );
	quotient  = static_cast<int>(d1);
	if( d2 > (IDELAY_RESOLUTION/2) )
		quotient += 1;
	if( quotient > 0x3F ) quotient = 0x3F;	//6bit
	if( 0 ) printf("CLK DELAY(%03d) Delay:%8.3f PS (%d Step / %8.3f PS)\n", pin, delay, quotient, quotient*78.0 );

	switch( pin )
	{
		case IOA_IOCLK    :
			g_pcie.write_user( BITCHART::DRV_IOA::_BCLK9_IDELAY, quotient );
			break;
		case IOA_ADCLK    :
			g_pcie.write_user( BITCHART::DRV_IOA::_BCLK10_IDELAY, quotient );
			break;
		case IOA_CPE0     :
			g_pcie.write_user( BITCHART::DRV_IOA::_CPE0_IDELAY, quotient );
			break;
		case IOA_CPE1     :
			g_pcie.write_user( BITCHART::DRV_IOA::_CPE1_IDELAY, quotient );
			break;
		case IOA_STRB1_0  :
			g_pcie.write_user( BITCHART::DRV_IOA::_STRB1_0_IDELAY, quotient );
			break;
		case IOA_STRB1_1  :
			g_pcie.write_user( BITCHART::DRV_IOA::_STRB1_1_IDELAY, quotient );
			break;
		case IOA_STRB2_0  :
			g_pcie.write_user( BITCHART::DRV_IOA::_STRB2_0_IDELAY, quotient );
			break;
		case IOA_STRB2_1  :
			g_pcie.write_user( BITCHART::DRV_IOA::_STRB2_1_IDELAY, quotient );
			break;
		case IOB_IOCLK    :
			g_pcie.write_user( BITCHART::DRV_IOB::_BCLK9_IDELAY, quotient );
			break;
		case IOB_ADCLK    :
			g_pcie.write_user( BITCHART::DRV_IOB::_BCLK10_IDELAY, quotient );
			break;
		case IOB_CPE0     :
			g_pcie.write_user( BITCHART::DRV_IOB::_CPE2_IDELAY, quotient );
			break;
		case IOB_CPE1     :
			g_pcie.write_user( BITCHART::DRV_IOB::_CPE3_IDELAY, quotient );
			break;
		case IOB_STRB1_0  :
			g_pcie.write_user( BITCHART::DRV_IOB::_STRB3_0_IDELAY, quotient );
			break;
		case IOB_STRB1_1  :
			g_pcie.write_user( BITCHART::DRV_IOB::_STRB3_1_IDELAY, quotient );
			break;
		case IOB_STRB2_0  :
			g_pcie.write_user( BITCHART::DRV_IOB::_STRB4_0_IDELAY, quotient );
			break;
		case IOB_STRB2_1  :
			g_pcie.write_user( BITCHART::DRV_IOB::_STRB4_1_IDELAY, quotient );
			break;
		case UCLK_BCLK1   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK1_IDELAY, quotient );
			break;
		case UCLK_BCLK2   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK2_IDELAY, quotient );
			break;
		case UCLK_BCLK3   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK3_IDELAY, quotient );
			break;
		case UCLK_BCLK4   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK4_IDELAY, quotient );
			break;
		case UCLK_BCLK5   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK5_IDELAY, quotient );
			break;
		case UCLK_BCLK6   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK6_IDELAY, quotient );
			break;
		case UCLK_BCLK7   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK7_IDELAY, quotient );
			break;
		case UCLK_BCLK8   :
			g_pcie.write_user( BITCHART::DRV_UCLK::_BCLK8_IDELAY, quotient );
			break;
		case UCLK_STRB1_0 :
			g_pcie.write_user( BITCHART::DRV_UCLK::_STRB1_0_IDELAY, quotient );
			break;
		case UCLK_STRB1_1 :
			g_pcie.write_user( BITCHART::DRV_UCLK::_STRB1_1_IDELAY, quotient );
			break;
		case UCLK_CPE     :
			g_pcie.write_user( BITCHART::DRV_UCLK::_CPE_IDELAY, quotient );
			break;
	}

	return true;
}
