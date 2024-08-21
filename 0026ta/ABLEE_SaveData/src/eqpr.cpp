#include "eqpr.hpp"

CEqpr::CEqpr(COrganizedData* pOd) : m_pOd(pOd)
{
	m_strFile = "";
}

bool CEqpr::GenEqpr()
{
	bool bMake = false;
	for ( LOT_LIST::iterator itTotLot = m_pOd->m_Lot.begin(); itTotLot != m_pOd->m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		bMake = true;

		if ( !CreateEqprFile(pLot) )
			bMake = false;

		if( ! m_strFile.empty() && m_pOd->m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("EQPR") )
				UTIL::FTP::PUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFile.c_str(), "ascii" );
			m_strFile.clear();
		}
	}

	printf("[INFO] EQPR MAKE %s...\n", bMake?"OK":"NG" );
	return true;
}

uint CEqpr::GetPara(T_LOT* pLot)
{
	uint nSlot=0;	//LOT Inserted slot count

	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

		nSlot++;
	}

	return m_pOd->m_nBibX[pLot->m_uRS] * m_pOd->m_nBibY[pLot->m_uRS] * nSlot;
}

const char* CEqpr::LotProc()
{
	//xxsystem.ini / LOT_PROCESS
	static const char* szLotProc = "T3C1";

	return szLotProc;
}

bool CEqpr::IsPreFail(uint iSlot, uint iDut)
{
	if ( m_pOd->m_pInitFail[iSlot]==nullptr ) return false;

	return static_cast<bool>( m_pOd->m_pInitFail[iSlot][iDut] );
}

bool CEqpr::CreateEqprFile(T_LOT* pLot)
{
	int fd = 0;
	char szDir [PATH_MAX];
	char szFile[PATH_MAX];
	char szPath[PATH_MAX];

	CFileMap* pDataMgr  = m_pOd->m_pDataMgr;
	time_t    tStart    = pLot->START_TIME;
	time_t    tEnd      = pLot->END_TIME  ;

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%d",
			  m_pOd->m_szReportRoot   ,
			  pLot->PART_NO           ,
			  pLot->LOT_NO            ,
			  pLot->SNO                );

	snprintf( szFile, PATH_MAX, "%s_%04d%02d%02d%02d%02d_%s%s",
			  pDataMgr->GET_STATION() ,
			  YYYY(tEnd  )            ,
			  MM  (tEnd  )            ,
			  DD  (tEnd  )            ,
			  hh  (tStart)            ,
			  mm  (tStart)            ,
			  pLot->LOT_NO            ,
			  EQPR_EXT              );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );

	m_strDir = szDir ;
	m_sFile  = szPath;

	fd = open(szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
		printf("[ ERR] %s open error!\n", szPath );

	if ( ! MakeHeader(pLot, fd) )
	{
		close(fd);
		return false;
	}

	if ( ! AppendEqpr(pLot, fd) )
	{
		close(fd);
		return false;
	}

	close(fd);
	printf("%s  gerneration complet.\n", m_sFile.c_str() );

	m_strFile = szFile;

	return true;
}

bool CEqpr::MakeHeader(T_LOT* pLot, int fd)
{
	if ( fd<0 ) return false;

	string strData;
	char   szLine[1024];

	CFileMap* pDataMgr  = m_pOd->m_pDataMgr;
	time_t    tStart    = pLot->START_TIME;
	time_t    tEnd      = pLot->END_TIME  ;
	char      szStart[1024];
	char      szEnd  [1024];

	strData.clear();
	{
		sprintf(szStart, "%04d%02d%02d%02d%02d", YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) );
		sprintf(szEnd  , "%04d%02d%02d%02d%02d", YYYY(tEnd  ), MM(tEnd  ), DD(tEnd  ), hh(tEnd  ), mm(tEnd  ) );
	}
	sprintf(szLine, "N0 %s %s %s AT6200 %s ST1 %d",
			szStart                      ,
			szEnd                        ,
			pLot->TEST_PGM_NO            ,
			pDataMgr->GET_STATION()      ,
			m_pOd->m_nMaxDut[pLot->m_uRS]                ); strData += szLine + STR_LF;

	sprintf(szLine, "%-21.21s %s %s %.1lf %s 0 0",
			pLot->LOT_NO ,
			pLot->PART_NO,
			LotProc()    ,
			-5.0         ,
			pLot->OP_ID                                  ); strData += szLine + STR_LF;

	sprintf(szLine, "%s", m_pOd->m_szVersion[pLot->m_uRS]); strData += szLine + STR_LF;

	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		return false;
	}

	return true;
}

bool CEqpr::AppendEqpr(T_LOT* pLot, int fd)
{
	if ( fd<0 ) return false;

	uint nBinCnt[16]    = { 0, };
	uint nInQty         =   0;
	uint nCateCnt[1000] = { 0, };	//MAX_CATEGORY == 1000

	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

		for ( uint c=0; c<m_pOd->m_nBibY[iSlot]; c++ )
		{
			for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
			{
				uint dut = c*m_pOd->m_nBibX[iSlot] + r;
				if ( ! pLot->S[iSlot].sdut[dut] ) continue;

				++nInQty;

				if (   IsPreFail( iSlot, dut )  ) continue;

				uint nBin = m_pOd->m_Binmap[iSlot][ m_pOd->m_pSortBin[iSlot][dut]%16 ];
//				nBinCnt[ m_pOd->m_pSortBin[iSlot][dut]%16 ]++;
				nBinCnt[ nBin ]++;
				for(uint iStep=1; iStep<=m_pOd->m_nMaxStep[iSlot]; ++iStep)
				{
					uint uStep = iStep-1;
					uint fcate = m_pOd->m_pStepFCate[iSlot][m_pOd->m_nMaxDut[iSlot]*uStep + dut];
					uint pcate = m_pOd->m_pStepPCate[iSlot][m_pOd->m_nMaxDut[iSlot]*uStep + dut];

					if ( fcate>0 && fcate<1000 ) nCateCnt[fcate]++;
					if ( pcate>0 && pcate<1000 ) nCateCnt[pcate]++;
				} //End of iStep
			} //End of r
		} //End of c
	} //End of slot

	//make data
	string strData;
	char   szLine[1024];
	strData.clear();

	double dPassRate = static_cast<double>(nBinCnt[1]) / nInQty;
	sprintf(szLine, "%07d %07d %07d %.2lf %05d %05d %05d %05d %05d %05d %05d %05d",
			nInQty, nInQty, nBinCnt[1], dPassRate,
			nBinCnt[1], nBinCnt[2], nBinCnt[3], nBinCnt[4], nBinCnt[5], nBinCnt[6],
			nBinCnt[7]+ nBinCnt[9], nBinCnt[8] ); strData += szLine + STR_LF;

	for( int iCate=1; iCate<=600; iCate+=6 )
	{
		sprintf(szLine, "%04d %07d %04d %07d %04d %07d %04d %07d %04d %07d %04d %07d",
				iCate    , nCateCnt[iCate    ],
				iCate + 1, nCateCnt[iCate + 1],
				iCate + 2, nCateCnt[iCate + 2],
				iCate + 3, nCateCnt[iCate + 3],
				iCate + 4, nCateCnt[iCate + 4],
				iCate + 5, nCateCnt[iCate + 5] ); strData += szLine + STR_LF;
	}

	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		close( fd );
		return false;
	}

	return true;
}

int CEqpr::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CEqpr::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CEqpr::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CEqpr::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CEqpr::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CEqpr::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}

