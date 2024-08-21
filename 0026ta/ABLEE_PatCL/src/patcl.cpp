#include "../include/patcl.hpp"

CPatCl::CPatCl()
	: m_iCompOpt(0)
{
	m_pComp = new CAlpgCompiler();

}

CPatCl::~CPatCl()
{
	if ( m_pComp ) { delete m_pComp; m_pComp = NULL; }
}

void CPatCl::Reset()
{
	m_pComp->Init();
	m_vecErrors.clear();
}

void CPatCl::SetCompOpt(unsigned int iCompOpt)
{
	m_iCompOpt = iCompOpt;
	m_pComp->SetCompOpt( m_iCompOpt );
}

bool CPatCl::SetDefine( atstring funcname, std::vector<pat_line_data> lines )	//funcname, line data
{
	return m_pComp->SetDefine( funcname, lines );
}

bool CPatCl::SetSubCompile( atstring subname , std::vector<pat_line_data> lines )	//subname , line data
{
	return m_pComp->SetSubCompile( subname, lines );
}

bool CPatCl::ProcNoLabelRemain()
{
	return m_pComp->ProcNoLabelRemain();
}

bool CPatCl::GetSubPc( atstring subname, int& pc )		//"#100", "#200" PC
{
	for( AlpgList::iterator it=m_pComp->pattern.module.begin(); it!=m_pComp->pattern.module.end(); ++it)
	{
		ALPG *alpg = *it;
		if( 0 ) printf("[PGC]Sublist=%s[PC:0x%04X]\n", alpg->name, alpg->pc );
	}

	if ( subname.Trim().Left(1).compare("#")==0 )
	{
		for( AlpgList::iterator it=m_pComp->pattern.module.begin(); it!=m_pComp->pattern.module.end(); ++it)
		{
			ALPG *alpg = *it;
//			if ( alpg->sta==(int)dw_value )
			if ( subname.compare(alpg->name) == 0 )
			{
				pc = alpg->pc;
				if( 0 ) printf("[PGC]Sub=%s(%s)[PC:0x%04X]\n", subname.c_str(), alpg->name, alpg->pc );
				return true;
			}
		}

//		char			szTmp[32];
//		unsigned int	dw_value;
//		char			*endptr;
//
//		sprintf(szTmp, "0x%s", (char *)subname.Trim().Mid(2) );
//		dw_value = strtoul( szTmp, &endptr, 0);
//
//		if ( *endptr )
//		{
//			pc = -1;
//			return false;
//		}
//		else
//		{
//			for( AlpgList::iterator it=m_pComp->pattern.module.begin(); it!=m_pComp->pattern.module.end(); ++it)
//			{
//				ALPG *alpg = *it;
//				if ( alpg->sta==(int)dw_value )
//				{
//					pc = alpg->pc;
//					return true;
//				}
//			}
//		}

		return false;
	}

	if ( m_pComp->sub_map.find(subname) != m_pComp->sub_map.end() )
	{
		pc = m_pComp->sub_map.find(subname)->second;
		if( 0 ) fprintf( stdout, "%s sub_map subname=%s[pc:%x]\n", __FUNCTION__,  subname.c_str(), pc );
		return true;
	}

	return false;
}

unsigned int CPatCl::GetMaxPc()
{
	if ( m_pComp->m_iStartPc > 0 )
		return m_pComp->m_iStartPc -1;

	return m_pComp->m_iStartPc;
}

int CPatCl::GetSubRegister(atstring subname)	//need to define return type(register list)
{
	return 0xff;
}

PGDATA* CPatCl::GetPatternData()		//Compiled pattern data
{
	return &m_pComp->pattern;
}

unsigned int CPatCl::GetErrCnt()
{
	return m_pComp->m_iErrCnt;
}

std::vector<atstring> CPatCl::GetErrors()
{
	atstring strErr;
	strErr = m_pComp->m_strErr;

	strErr.Split(m_vecErrors, "\n");
	return m_vecErrors;
}
