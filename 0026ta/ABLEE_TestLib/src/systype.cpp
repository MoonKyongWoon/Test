#include "systype.hpp"
#include "io.hpp"
#include "pg.hpp"

extern CPcie g_pcie;
extern CPg   g_pg;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CGlobalReg::CGlobalReg(GLOBAL_REG GR,int addr, bool b32)
{
	m_nVal       = 0x0;
	m_nReg       = GR;
	m_nHwAddress = addr;
    m_isBigger32 = b32;
}

CGlobalReg& CGlobalReg::operator=(ll64 val)
{
    if( !m_isBigger32 )
	{
		m_nVal = (int)val&0xFFFFFFFF;
	}
    else
	{
		m_nVal = (int)val&0xFFFFFFFFFFFFFFFFll;
	}
	g_pg.SetGlobalRegister( this->m_nHwAddress, this->m_nVal );
    return *this;
}

//2022.12.19 - kwmoon
int CGlobalReg::operator=(CGlobalReg& reg)
{
	m_nVal = reg.m_nVal;	//2023.06.13 - kwmoon(add fatric)
	return static_cast<int>(this->m_nVal);
}

//2023.06.13 - kwmoon(add fatric)
int CGlobalReg::operator=(CGlobalSet& set)
{
	m_nVal = set.GetValue32();
	return m_nVal;
}

GLOBAL_REG CGlobalReg::GetReg()
{
    return m_nReg;
}

int CGlobalReg::GetHwAddress()
{
	return m_nHwAddress;
}

ll64 CGlobalReg::GetValue64()
{
	return m_nVal&0xFFFFFFFFFFFFFFFFll;
}

uint CGlobalReg::GetValue32()
{
	return static_cast<uint>(m_nVal&0xFFFFFFFF);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SET/DSET
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CGlobalSet::CGlobalSet(GLOBAL_SET GR, int g, int i)
{
	m_nVal   = 0x0;
	m_nReg   = GR;
	m_nGroup = g;
	m_nIndex = i;
}

CGlobalSet& CGlobalSet::operator=(int val)
{
	unsigned short mask[4]= { 0xf, 0xf0,0xf00,0xf000 } ;
	int i = 0;
	for(i=0;i<16;i++)
	{
		if( !(g_pg.m_SetReg[i].reg&mask[m_nGroup]) ) break ;
	}
	if( i == 16 ) return *this;

	g_pg.m_SetReg[i].reg |= m_nIndex;
	if     ( m_nGroup == 0 ) g_pg.m_SetReg[i].a = val;
	else if( m_nGroup == 1 ) g_pg.m_SetReg[i].b = val;
	else if( m_nGroup == 2 ) g_pg.m_SetReg[i].c = val;
	else if( m_nGroup == 3 ) g_pg.m_SetReg[i].d = val;
	m_nVal = val;
	return *this;
}

//2023.06.13 - kwmoon(comment fatric)
//CGlobalSet& CGlobalSet::operator=(ll64 llval)
//{
//	unsigned short mask[4]= { 0xf, 0xf0,0xf00,0xf000 } ;
//	int i = 0;
//	for(i=0;i<16;i++)
//	{
//		if( !(g_pg.m_SetReg[i].reg&mask[m_nGroup]) ) break ;
//	}
//	if( i == 16 ) return *this;
//
//	g_pg.m_SetReg[i].reg |= m_nIndex;
//		 if( m_nGroup == 2 ) g_pg.m_SetReg[i].c = llval;
//	else if( m_nGroup == 3 ) g_pg.m_SetReg[i].d = llval;
//	m_nVal = (int)llval;
//	return *this;
//}

//2022.12.19 - kwmoon
int CGlobalSet::operator=(CGlobalSet& set)
{
	unsigned short mask[4]= { 0xf, 0xf0,0xf00,0xf000 } ;
	int i = 0;
	for(i=0;i<16;i++)
	{
		if( (g_pg.m_SetReg[i].reg&mask[m_nGroup]) ) break ;
	}
	if( i == 16 ) return 0;

	if     ( m_nGroup == 0 ) return g_pg.m_SetReg[i].a;
	else if( m_nGroup == 1 ) return g_pg.m_SetReg[i].b;
	else if( m_nGroup == 2 ) return static_cast<int>(g_pg.m_SetReg[i].c);
	else if( m_nGroup == 3 ) return static_cast<int>(g_pg.m_SetReg[i].d);
	return 0;
}

//2023.06.13 - kwmoon(add fatric)
int CGlobalSet::operator=(CGlobalReg& set)
{
	m_nVal = set.GetValue32();
	return m_nVal;
}

GLOBAL_SET CGlobalSet::GetReg()
{
	return m_nReg;
}

int CGlobalSet::GetGroup()
{
	return m_nGroup;
}

int CGlobalSet::GetIndex()
{
	return m_nIndex;
}

ll64 CGlobalSet::GetValue64()
{
	return m_nVal&0xFFFFFFFFFFFFFFFFll;
}

uint CGlobalSet::GetValue32()
{
	return static_cast<uint>(m_nVal&0xFFFFFFFF);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// REGISTER
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPin::CPin(PIN_TYPE t, int reg)
{
	m_type = t;
    m_nReg = reg;
    m_nPos = 0;
    for( int cyp=0;cyp<MAX_CYP;cyp++ )
		m_nCyp[cyp] = PDS::FIXL;
	m_nFmt = FMT_NORM::FL;
    m_bModule = false;
}

CPin& CPin::operator=(int val)
{
	m_nPos = 0;
	return *this, val;
}

CPin& CPin::operator,(int val)
{
	int nPDS = val;
	int nFMT = val;
	if( m_bModule )
	{
		if( this->GetType() == PIN_DRE )
		{
////////////////////////////////////////////////////////////////////////////////////////////
//		NRZB   -> DRENRZB
//		NRZC   -> DRENRZC
//				if( nFMT == FMT_NORM::RZO  ) m_nFmt = FMT_DRE::DRERZ;
//	//			else if( nFMT == FMT_NORM::NRZB ) m_nFmt = FMT_DRE::DRENRZ;		//2024-01-12 fatric
//		   else if( nFMT == FMT_NORM::NRZB ) m_nFmt = FMT_DRE::DRENRZB;	//2024-01-12 fatric
//		   else if( nFMT == FMT_NORM::NRZC ) m_nFmt = FMT_DRE::DRENRZC;	//2024-01-12 fatric
//		   else if( nFMT == PDS::FIXH      ) m_nFmt = FMT_DRE::DREFIXH;
//		   else if( nFMT == PDS::FIXL      ) m_nFmt = FMT_DRE::DREFIXL;
//		   else
//		   {
//			   fprintf( stderr, "DRE(%d) error : use invalid format %d\n", this->GetIndex(), nFMT );
//		   }
////////////////////////////////////////////////////////////////////////////////////////////
//		2024-03-05 fatric
//		NRZB   -> DRENRZC
//		NRZB_B -> DRENRZB
//		NRZC   -> DRENRZC

				 if( nFMT == FMT_NORM::RZO    ) m_nFmt = FMT_DRE::DRERZ;
//			else if( nFMT == FMT_NORM::NRZB   ) m_nFmt = FMT_DRE::DRENRZ;	//2024-01-12 fatric
			else if( nFMT == FMT_NORM::NRZB   ) m_nFmt = FMT_DRE::DRENRZC;	//2024-01-12 fatric
			else if( nFMT == FMT_NORM::NRZC   ) m_nFmt = FMT_DRE::DRENRZC;	//2024-01-12 fatric
			else if( nFMT == FMT_NORM::NRZB_B ) m_nFmt = FMT_DRE::DRENRZB;	//2024-01-12 fatric
			else if( nFMT == PDS::FIXH        ) m_nFmt = FMT_DRE::DREFIXH;
			else if( nFMT == PDS::FIXL        ) m_nFmt = FMT_DRE::DREFIXL;
			else
			{
				fprintf( stderr, "DRE(%d) error : use invalid format %d\n", this->GetIndex(), nFMT );
			}
////////////////////////////////////////////////////////////////////////////////////////////
		}
		else
		{
			if( (nFMT < RZO) || ( nFMT > _DNRZ) )
			{
				if( nFMT==PDS::FIXH )
				{
					m_nFmt = FMT_NORM::FH;
				}
				else if( nFMT==PDS::FIXL )
				{
					m_nFmt = FMT_NORM::FL;
				}
				else
				{
					//fprintf( stderr, "MODULE ");
				}
			}
			else
				m_nFmt = nFMT;
		}
	}
	else
	{
		if( m_type == PIN_DR )
		{
			if( (nPDS>=PDS::C0) && (nPDS<=PDS::Y17) )
			{
				m_nCyp[m_nPos++] = nPDS;
			}
			else if( (nPDS>=PDS::S0) && (nPDS<=PDS::S63) )
			{
				m_nCyp[m_nPos++] = nPDS;
			}
			else
			{
				//error
				fprintf( stderr, "%s%d error : PDS Set C0~C23, FIXH, FIXL, X0-17, Y0-17, Z0-17, D0-15, SD0-15, S0-63, V0-7\n", "PDR", this->GetIndex() );
				m_nCyp[m_nPos++] = PDS::FIXL;
			}
		}
		else if( m_type == PIN_IO )
		{
			if( (nPDS>=PDS::C0) && (nPDS<=PDS::Y17) )
			{
				m_nCyp[m_nPos++] = nPDS;
			}
			else if( (nPDS>=PDS::Z0) && (nPDS<=PDS::D15) )
			{
				m_nCyp[m_nPos++] = nPDS - 0x42;	//minus offset
			}
			else if( (nPDS>=PDS::SD0) && (nPDS<=PDS::SD15) )
			{
				m_nCyp[m_nPos++] = nPDS - 0x42;	//minus offset
			}
			else if( (nPDS>=PDS::V0) && (nPDS<=PDS::V7) )
			{
				m_nCyp[m_nPos++] = nPDS - 0x42;	//minus offset
			}
			else
			{
				//error
				fprintf( stderr, "%s error : PDS Set C0~C23, FIXH, FIXL\n", "PIO" );
				m_nCyp[m_nPos++] = PDS::FIXL;
			}
		}
		else if( m_type == PIN_UCLK )
		{
			if( (nPDS>=PDS::C0) && (nPDS<=PDS::Y17) )
			{
				m_nCyp[m_nPos++] = nPDS;
			}
			else
			{
				//error
				fprintf( stderr, "%s error : PDS Set C0~C23, FIXH, FIXL\n", "UCLK" );
				m_nCyp[m_nPos++] = PDS::FIXL;
			}
		}
		else if( m_type == PIN_FMA )
		{
			if( (nPDS>=PDS::C0) && (nPDS<=PDS::Y17) )
			{
				m_nCyp[m_nPos++] = nPDS;
			}
			else
			{
				//error
				fprintf( stderr, "%s error : PDS Set C0~C23, FIXH, FIXL\n", "FMA" );
				m_nCyp[m_nPos++] = PDS::FIXL;
			}
		}
		else if( m_type == PIN_MUT )
		{
			if( (nPDS>=PDS::C0) && (nPDS<=PDS::Y17) )
			{
				m_nCyp[m_nPos++] = nPDS;
			}
			else
			{
				//error
				fprintf( stderr, "%s error : PDS Set C0~C23, FIXH, FIXL\n", "FMA" );
				m_nCyp[m_nPos++] = PDS::FIXL;
			}
		}
		else if( (m_type == PIN_DRE) || (m_type == PIN_CPE) )
		{
			if( (nPDS>=PDS::C0) && (nPDS<=PDS::FIXL) )
			{
				m_nCyp[m_nPos++] = nPDS;
			}
			else
			{
				fprintf( stderr, "%s error : PDS Set C0~C23, FIXH, FIXL\n", (m_type == PIN_DRE)?"DRE":"CPE" );
				m_nCyp[m_nPos++] = PDS::FIXL;
			}
		}
	}
    return *this;
}

void CPin::SetModule(bool b)
{
	m_bModule = b;
}

PIN_TYPE CPin::GetType()
{
	return m_type;
}

int CPin::GetIndex()
{
	return m_nReg;
}

int CPin::GetModule()
{
	return m_nFmt;
}

int CPin::GetCyp(int idx)
{
	if( (idx<0) || (idx>MAX_CYP) )
	{
		//error
		return PDS::FIXL;
	}
	return m_nCyp[idx];
}
