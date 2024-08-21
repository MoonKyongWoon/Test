#include "fls.hpp"

CFls::CFls(COrganizedData* pOd) : m_pOd(pOd)
{
	m_strFiles= "";
}

bool CFls::GenFls()
{
	bool bMake = false;
	for ( LOT_LIST::iterator itTotLot = m_pOd->m_Lot.begin(); itTotLot != m_pOd->m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		//iFileType==0 : fls, iFileType==1 : flsh
		for(uint iFileType=0; iFileType<2; ++iFileType)
		{
			if( GenFileName( iFileType, pLot ) )
			{
				if( MakeSortingHead(iFileType, pLot) && MakeHistoryHead(iFileType, pLot) )
				{
					bMake = true;
					MakeSortingDataSummary  ( iFileType, pLot, bMake );
					MakeSortingDataBoardMap ( iFileType, pLot, bMake );
					MakeTestHistoryReport   ( iFileType, pLot, bMake );
					MakeTestBlockCountReport( iFileType, pLot, bMake );
					MakeFailSocketReport    ( iFileType, pLot, bMake );
				}
				close(m_fd);
				printf("%s  gerneration complet.\n", m_sFile.c_str() );
			}
		}

		if ( ! m_strFiles.empty() && m_pOd->m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("FLS") )
				UTIL::FTP::MPUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFiles.c_str(), "ascii" );

			m_strFiles.clear();
		}
	}

	printf("[INFO] FLS MAKE %s...\n", bMake?"OK":"NG" );
	return bMake;
}

bool CFls::GenFileName(uint iFileType, T_LOT* pLot)
{
	char szDir   [PATH_MAX];
	char szFile  [PATH_MAX];
	char szPath  [PATH_MAX];
	char szExt[2][PATH_MAX] = { FLS_EXT, FLSH_EXT };

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%d",
			  m_pOd->m_szReportRoot,
			  pLot->PART_NO        ,
			  pLot->LOT_NO         ,
			  pLot->SNO             );

	snprintf( szFile, PATH_MAX, "%s-%03d-%03d_%s%s",
			  pLot->LOT_NO         ,
			  pLot->SNO            ,
			  1                    ,
			  pLot->TEST_PGM_NO    ,
			  szExt[iFileType]      );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );

	m_fd = open( szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if( m_fd == -1 )
	{
		printf("[ ERR] %s open error!\n", szFile );
		return false;
	}
	//printf("[INFO] %s start...\n", szFile );
	m_strDir = szDir ; m_strFiles += szFile + string(" ");
	m_sFile  = szPath;
	return true;
}

bool CFls::MakeSortingHead( uint iFileType, T_LOT* pLot )
{
	if ( iFileType >0 ) return true;

	std::string strData   = "";
	char szLine[PATH_MAX] = {0x0,};
	CFileMap* pDataMgr = m_pOd->m_pDataMgr;

	strData.clear();
	m_sHeadSorting.clear();

	time_t tStart = pLot->START_TIME;
	time_t tEnd   = pLot->END_TIME  ;

	sprintf(szLine, "START-TIME %04d/%02d/%02d %02d:%02d ",
			YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) ); strData += szLine;
	sprintf(szLine, "END-TIME %04d/%02d/%02d %02d:%02d "  ,
			YYYY(tEnd  ), MM(tEnd  ), DD(tEnd  ), hh(tEnd  ), mm(tEnd  ) ); strData += szLine;

	sprintf(szLine, "PARTS %-17s "     , pLot->PART_NO                   ); strData += szLine;
	sprintf(szLine, "LOTNO %s "        , pLot->LOT_NO                    ); strData += szLine;
	sprintf(szLine, "SUBLOT  %d "      , pLot->SNO                       ); strData += szLine;
	sprintf(szLine, "TP %-8s"          , pLot->TEST_PGM_NO               ); strData += szLine + STR_LF;

	sprintf(szLine, "STATION %s "      , pDataMgr->GET_STATION()         ); strData += szLine;
	sprintf(szLine, "CHAMBER%1d "      , 1                               ); strData += szLine;
	sprintf(szLine, "SUBLOT-STATUS %s ", "CLOSE"                         ); strData += szLine;
	sprintf(szLine, "REMARK %s "       , m_pOd->m_szVersion[pLot->m_uRS] ); strData += szLine;
	sprintf(szLine, "OPER %s "         , pLot->OPER                      ); strData += szLine;
	sprintf(szLine, "MPG_FLAG:%c"      , pLot->MPG_FLAG                  ); strData += szLine + STR_LF;
	sprintf(szLine, "OID %s"           , pLot->OP_ID                     ); strData += szLine + STR_LF;
	m_sHeadSorting = strData;
	return true;
}

bool CFls::MakeHistoryHead( uint iFileType, T_LOT* pLot )
{
	if ( iFileType >0 ) return true;

	std::string strData   = "";
	char szLine[PATH_MAX] = {0x0,};
	CFileMap* pDataMgr = m_pOd->m_pDataMgr;

	strData.clear();
	m_sHeadHistory.clear();

	sprintf(szLine, " PARTS %s   "  , pLot->PART_NO            ); strData += szLine;
	sprintf(szLine, "LOTNO %s "     , pLot->LOT_NO             ); strData += szLine;
	sprintf(szLine, "BATCH %04d    ", 0                        ); strData += szLine;
	sprintf(szLine, "TP %s    "     , pLot->TEST_PGM_NO        ); strData += szLine;
	sprintf(szLine, "STATION %s   " , pDataMgr->GET_STATION()  ); strData += szLine;
	sprintf(szLine, "CHAMBER%1d   " , 1                        ); strData += szLine;
	time_t tEnd = pLot->END_TIME;
	sprintf(szLine, "END-TIME %04d/%02d/%02d %02d:%02d",
			YYYY(tEnd), MM(tEnd), DD(tEnd), hh(tEnd), mm(tEnd) ); strData += szLine + STR_LF;
	m_sHeadHistory = strData;
	return true;
}

std::string CFls::GetSortHead()
{
	return m_sHeadSorting;
}

std::string CFls::GetHistoryHead()
{
	return m_sHeadHistory;
}

bool CFls::MakeSortingDataSummary(uint iFileType, T_LOT* pLot, bool& bMake)
{
	if( !bMake ) return false;

	char szLine[PATH_MAX] = {0x0,};
	string strLine = "";
	string strData = "";
	time_t tNow;
	struct tm* t;

	strData.clear();
	sprintf(szLine, "%-100s", "<< SORTING DATA SUMMARY >>"); strData += szLine;
	tNow = time(NULL);
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", YYYY(tNow), MM(tNow), DD(tNow), hh(tNow), mm(tNow) );
	strData += szLine + STR_LF;
	strData += GetSortHead();

	sprintf(szLine, "%38s"             , "SORTING-LINE"       );	strData += szLine + STR_LF;
	strLine = "  BID      TOTAL    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16"; strData += strLine + STR_LF;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + STR_LF;

	uint nTotal = 0;
	uint nSortBinSum[17]={0,};
	memset( nSortBinSum, 0x0, sizeof(nSortBinSum) );

	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

//		uint nSlotTotal = pLot->S[iSlot].nInQty;
//		for( uint iDut=0; iDut<m_pOd->m_nMaxDut[iSlot]; iDut++ )
//		{
//			if ( ! pLot->S[iSlot].sdut[iDut] ) continue;
//
//			if( m_pOd->m_pInitFail[iSlot][iDut] ) nSlotTotal--;
//		}

		uint nBin0Cnt    = 0;
		uint nSlotTotal = 0;
		for( uint iDut=0; iDut<m_pOd->m_nMaxDut[iSlot]; iDut++ )
		{
			if( ! pLot->S[iSlot].sdut[iDut]     ) continue;

			nSlotTotal++;

			if( m_pOd->m_pInitFail[iSlot][iDut] ) nBin0Cnt++;
		}
		pLot->S[iSlot].nSortBinCnt [0] = nBin0Cnt;
		pLot->S[iSlot].nSortBinHCnt[0] = nBin0Cnt;

		sprintf(szLine, "0%-8s ", pLot->S[iSlot].BIB_ID );	strData += szLine;
		sprintf(szLine, "%6d"   , nSlotTotal            );	strData += szLine;
		for(int iSortBin=0; iSortBin<17; ++iSortBin )
		{
			uint nBinCnt = 0;
				 if ( iFileType==0 ) nBinCnt = pLot->S[iSlot].nSortBinCnt [iSortBin]; //fls
			else if ( iFileType==1 ) nBinCnt = pLot->S[iSlot].nSortBinHCnt[iSortBin]; //flsh

			sprintf(szLine, " %4d", nBinCnt );	strData += szLine;
			nSortBinSum[iSortBin] += nBinCnt;
		}
		nTotal += nSlotTotal;
		strData += STR_LF;
	}
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + STR_LF;
	sprintf(szLine, "%-9s ", "TOTAL" );	strData += szLine;
	sprintf(szLine, "%6d"  , nTotal );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4d", nSortBinSum[iSortBin] );	strData += szLine;
	}
	strData += STR_LF;
	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + STR_LF;
	sprintf(szLine, "%-16s", "RATE (%)" );	strData += szLine;
	for(int iSortBin=0; iSortBin<17; ++iSortBin )
	{
		sprintf(szLine, " %4.1f", nSortBinSum[iSortBin]*100.0/nTotal );	strData += szLine;
	}
	strData += STR_LF + STR_LF;
	uint strSize = strData.length();
	ssize_t written = write( m_fd, strData.c_str(), strSize );
	if( written != strSize )
	{
		printf("[ ERR]file(%s) %s write error", m_sFile.c_str(), __FUNCTION__ );
		bMake = false;
		return bMake;
	}
	bMake = true;
	return bMake;
}

bool CFls::MakeSortingDataBoardMap(uint iFileType, T_LOT* pLot, bool& bMake)
{
	if( !bMake ) return false;

	char szLine[PATH_MAX] = {0x0,};
	string strData = "";
	time_t tNow;

	sprintf(szLine, "%-100s", "<< SORTING DATA BOARD MAP >>"); strData += szLine;
	tNow = time(NULL);
	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", YYYY(tNow), MM(tNow), DD(tNow), hh(tNow), mm(tNow) );
	strData += szLine + STR_LF;
	strData += GetSortHead();

	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

//		uint nTotal = pLot->S[iSlot].nInQty;
//		for( uint iDut=0; iDut<m_pOd->m_nMaxDut[iSlot]; iDut++ )
//		{
//			if( ! pLot->S[iSlot].sdut[iDut] ) continue;
//
//			if( m_pOd->m_pInitFail[iSlot][iDut] ) nTotal--;
//		}
		uint nTotal = 0;
		for( uint iDut=0; iDut<m_pOd->m_nMaxDut[iSlot]; iDut++ )
		{
			if( ! pLot->S[iSlot].sdut[iDut]     ) continue;
//			if( m_pOd->m_pInitFail[iSlot][iDut] ) continue;

			nTotal++;
		}

		sprintf(szLine, "BID 0%s "   , pLot->S[iSlot].BIB_ID  ); strData += szLine;
		sprintf(szLine, "Z1S%02d "   , iSlot+1                ); strData += szLine;
		sprintf(szLine, "IOC=%s "    , pLot->S[iSlot].IOC     ); strData += szLine;
		sprintf(szLine, "BOC=%s "    , pLot->S[iSlot].BOC     ); strData += szLine;
		sprintf(szLine, "MLB_CODE=%s", pLot->S[iSlot].MLB_CODE); strData += szLine + STR_LF;;
		sprintf(szLine, "Total %d"   , nTotal                 ); strData += szLine + STR_LF;

		strData += "   ";
		for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
		{
			sprintf( szLine, " %2d", r+1 ); strData += szLine;
		}
		strData += STR_LF;

		for ( int c=m_pOd->m_nBibY[iSlot]-1; c>=0; c-- )
		{
			sprintf( szLine, " %2d", c+1 ); strData += szLine;
			for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
			{
				int dut = c*m_pOd->m_nBibX[iSlot] + r;

//				if (  m_pOd->m_pInitFail[iSlot][dut] || !pLot->S[iSlot].sdut[dut] )
					 if ( !pLot->S[iSlot].sdut[dut]      ) sprintf( szLine, "  .");
				else if ( m_pOd->m_pInitFail[iSlot][dut] ) sprintf( szLine, "  0");
				else
				{
					uint nBin = m_pOd->m_pSortBin[iSlot][dut];										//iFileType==1, flsh
					if ( iFileType==0 ) nBin = m_pOd->m_Binmap[iSlot][ m_pOd->m_pSortBin[iSlot][dut] ];	//iFileType==0, fls

					sprintf( szLine, " %2d", nBin );
				}
				strData += szLine;
			}
			strData += STR_LF;
		}
		strData += STR_LF;
	}
	uint strSize = strData.length();
	ssize_t written = write( m_fd, strData.c_str(), strSize );
	if( written != strSize )
	{
		printf("[ ERR]file(%s) %s write error", m_sFile.c_str(), __FUNCTION__ );
		bMake = false;
		return bMake;
	}
	bMake = true;
	return bMake;
}

bool CFls::MakeTestHistoryReport(uint iFileType, T_LOT* pLot, bool& bMake)
{
	if( !bMake ) return false;

	if ( iFileType==0 )
	{
		std::string strData = "";
		std::string strLine = "";
		char szLine[PATH_MAX] = {0x0,};
		time_t tNow;

		sprintf(szLine, "%-100s", "<<TEST HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
		tNow = time(NULL);
		sprintf(szLine, "%04d/%02d/%02d %02d:%02d", YYYY(tNow), MM(tNow), DD(tNow), hh(tNow), mm(tNow) );
		strData += szLine +	STR_LF;
		strData += GetHistoryHead();
		strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
		strData += strLine + STR_LF;

		uint nTotal         = 0;
		uint nTotalInitFail = 0;
		for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
		{
			if ( ! pLot->S[iSlot].IS_VALID() ) continue;

//			int nSlotTotal = pLot->S[iSlot].nInQty;
//			for(uint iDut=0; iDut<m_pOd->m_nMaxDut[iSlot]; ++iDut)
//			{
//				if( ! pLot->S[iSlot].sdut[iDut] ) continue;
//
//				if( m_pOd->m_pInitFail[iSlot][iDut] ) nSlotTotal--;
//			}
			uint nSlotTotal         = 0;
			uint nSlotTotalInitFail = 0;
			for( uint iDut=0; iDut<m_pOd->m_nMaxDut[iSlot]; iDut++ )
			{
				if( ! pLot->S[iSlot].sdut[iDut]     ) continue;

				nSlotTotal++;

				if( m_pOd->m_pInitFail[iSlot][iDut] ) nSlotTotalInitFail++;
			}
			nTotal += nSlotTotal;
			nTotalInitFail += nSlotTotalInitFail;
		}

		uint rSlot = pLot->m_uRS;
		uint rCpu  = pLot->m_uRC;
		uint nStepCount = m_pOd->m_nMaxStep[rSlot] +1;	//add Init Item

		std::vector<CStepData*> &vStepData = m_pOd->m_vecStepData[rSlot][rCpu];
		if( vStepData.size() < nStepCount ) nStepCount = vStepData.size();
		for(uint i=0; i<nStepCount; ++i )
		{
			time_t tStepE = vStepData[i]->tStepEnd;
			sprintf(szLine, "%3d %02d/%02d %02d:%02d ", vStepData[i]->iTsNo, MM(tStepE), DD(tStepE), hh(tStepE), mm(tStepE) );	strData += szLine;
			int iBiTime = vStepData[i]->tStepEnd - vStepData[i]->tStepBegin;
			if (iBiTime < 48000 * 3) iBiTime = 0;

			sprintf(szLine, "%s  %3d %6.1f %7.1f "    , (i==0?"ITEST":"FTEST"), vStepData[i]->iTsNo, vStepData[i]->dTemp,  iBiTime/3600.0 );	strData += szLine;
//			sprintf(szLine, "%s  %3d %6.1f %7.1f "    , (i==0?"ITEST":"FTEST"), vStepData[i]->iTsNo, vStepData[i]->dTemp,  0.0 /*m_vecStepData[rSlot][i]->dBiTime*/  );	strData += szLine;
			sprintf(szLine, "%-20s %-20s %5d %5d "    , vStepData[i]->szStepName, vStepData[i]->szPc, nTotal, nTotal );	strData += szLine;
			uint   iStepFailCnt = 0;
			double dFailRate    = 0.0;
			if ( i>0 )
			{
				for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
				{
					if ( ! pLot->S[iSlot].IS_VALID() ) continue;

					iStepFailCnt += StepFailCount( pLot, iSlot, i-1 );
				}
				dFailRate = (iStepFailCnt * 100.0) / nTotal;
			}
			else
			{
				//INIT TEST
//				iStepFailCnt = nTotalInitFail;
				iStepFailCnt = 0;
				dFailRate = (iStepFailCnt * 100.0) / nTotal;
			}
			sprintf(szLine, "%5d %7.2f  %3.1f", iStepFailCnt, dFailRate, m_pOd->m_vecStepData[rSlot][rCpu][i]->dVcc );	strData += szLine + STR_LF;
		}
		strData += STR_LF;

		m_sTestHistoryReport = strData;
	}

	uint strSize = m_sTestHistoryReport.length();
	ssize_t written = write( m_fd, m_sTestHistoryReport.c_str(), strSize );
	if( written != strSize )
	{
		printf("[ ERR]file(%s) %s write error", m_sFile.c_str(), __FUNCTION__ );
		bMake = false;
		return bMake;
	}
	bMake = true;
	return bMake;
}

uint CFls::StepFailCount(T_LOT* pLot, int iSlot, uint nStep)
{
	ssize_t nStepFailSizePerStep = m_pOd->m_nMaxDut[iSlot];	//* sizeof(char)
	uint    nOffset              = nStepFailSizePerStep*nStep;

	uint iFailCnt = 0;
	for(uint iDut=0; iDut<m_pOd->m_nMaxDut[iSlot]; ++iDut)
	{
		if( ! pLot->S[iSlot].sdut[iDut]     ) continue;
		if( m_pOd->m_pInitFail[iSlot][iDut] ) continue;

		if ( m_pOd->m_pStepFail[iSlot][nOffset + iDut] ) ++iFailCnt;
	}

	return iFailCnt;
}

bool CFls::IsStepFail(int iSlot, uint iDut, uint iStep)
{
	if ( m_pOd->m_pStepFail[iSlot] == nullptr ) return false;
	ssize_t nStepFailSizePerStep = m_pOd->m_nMaxDut[iSlot] * iStep;	//* sizeof(char)

	return static_cast<bool>( m_pOd->m_pStepFail[iSlot][ nStepFailSizePerStep + iDut ] );
}

bool CFls::IsAllStepPass(int iSlot, uint iDut, uint nStep)
{
	if ( m_pOd->m_pStepFail[iSlot]==nullptr ) return true;

	int nStepFailSizePerStep = m_pOd->m_nMaxDut[iSlot];	//* sizeof(char)

	for(uint iStep=0; iStep<nStep; ++iStep)
	{
		if ( m_pOd->m_pStepFail[iSlot][nStepFailSizePerStep*iStep + iDut] )
			return false;
	}

	return true;
}

bool CFls::MakeTestBlockCountReport(uint iFileType, T_LOT* pLot, bool& bMake)
{
	if( !bMake ) return false;

	if ( iFileType==0 )
	{
		std::string strData = "";
		std::string strLine = "";
		char szLine[PATH_MAX] = {0x0,};
		time_t tNow;

		sprintf(szLine, "%-100s", "<<TEST Block Count HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
		tNow = time(NULL);
		sprintf(szLine, "%04d/%02d/%02d %02d:%02d", YYYY(tNow), MM(tNow), DD(tNow), hh(tNow), mm(tNow) );
		strData += szLine +	STR_LF;
		strData += GetHistoryHead();

		strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
		strData += strLine + STR_LF + STR_LF + STR_LF + STR_LF;

		m_sTestBlockCountReport = strData;
	}

	uint strSize = m_sTestBlockCountReport.length();
	ssize_t written = write( m_fd, m_sTestBlockCountReport.c_str(), strSize );
	if( written != strSize )
	{
		printf("[ ERR]file(%s) %s write error", m_sFile.c_str(), __FUNCTION__ );
		bMake = false;
		return bMake;
	}
	bMake = true;
	return bMake;
}

bool CFls::MakeFailSocketReport(uint iFileType, T_LOT* pLot, bool& bMake)
{
	if( !bMake ) return false;

	if ( iFileType==0 )
	{
		std::string strData = "";
		std::string strLine = "";
		char szLine[PATH_MAX] = {0x0,};
		time_t tNow;

		sprintf(szLine, "%-100s", "<<FAIL SOCKET HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
		tNow = time(NULL);
		sprintf(szLine, "%04d/%02d/%02d %02d:%02d", YYYY(tNow), MM(tNow), DD(tNow), hh(tNow), mm(tNow) );
		strData += szLine + STR_LF;
		strData += GetHistoryHead();

		for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
		{
			if ( ! pLot->S[iSlot].IS_VALID() ) continue;

			sprintf(szLine, "BNO:%-3d BID 0%s Z%d-S%d", iSlot+1, pLot->S[iSlot].BIB_ID, 1, iSlot+1 );
			strData += szLine + STR_LF;
			strLine = "                                                                                                         1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    5    5    5    5    5    5    5";	strData += strLine + STR_LF;
			strLine = "               1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3";	strData += strLine + STR_LF;
			strLine = "          5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0";	strData += strLine + STR_LF;
			strLine = "-----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----" ;	strData += strLine + STR_LF;

			for ( uint c=0; c<m_pOd->m_nBibY[iSlot]; c++ )
			{
				for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
				{
					int dut = c*m_pOd->m_nBibX[iSlot] + r;

						 if ( m_pOd->m_pInitFail[iSlot][dut]                      ) continue;
					else if ( IsAllStepPass(iSlot, dut, m_pOd->m_nMaxStep[iSlot]) ) continue;

					sprintf(szLine, "%02d-%02d|", c+1, r+1 );	strData += szLine;
					strLine.clear();
					uint nStepCount = m_pOd->m_nMaxStep[iSlot] +1;	//add Init Item
					if( m_pOd->m_vecStepData[iSlot][CPU00].size() < nStepCount ) nStepCount = m_pOd->m_vecStepData[iSlot][CPU00].size();
					for(uint i=0; i<nStepCount; ++i )
					{
						if ( i==0 ) { strLine += "m"; continue; }

						if ( IsStepFail( iSlot, dut, i-1) ) strLine += "X";
						else                                strLine += "o";
					}
					strData += strLine + STR_LF;
				}
			}
			strData += STR_LF;
		}

		m_sFailSocketReport = strData;
	}

	uint strSize = m_sFailSocketReport.length();
	ssize_t written = write( m_fd, m_sFailSocketReport.c_str(), strSize );
	if( written != strSize )
	{
		printf("[ ERR]file(%s) %s write error", m_sFile.c_str(), __FUNCTION__ );
		bMake = false;
		return bMake;
	}
	bMake = true;
	return bMake;
}

int CFls::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CFls::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CFls::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CFls::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CFls::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CFls::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}

//CFls::CFls(CFileMap* pMgr) : m_pDataMgr(pMgr)
//{
//	m_bCpu2X = true;
//	for( int s=0;s<MAX_SLOT;s++ )
//	{
//		m_pSortBin    [s] = nullptr;
//		m_pInitFail   [s] = nullptr;
//		m_pStepFail   [s] = nullptr;
//		m_pStepInInfo [s][CPU00] = nullptr;
//		m_pStepOutInfo[s][CPU00] = nullptr;
//		m_pStepInInfo [s][CPU01] = nullptr;
//		m_pStepOutInfo[s][CPU01] = nullptr;
//		m_vecStepData [s][CPU00].clear();
//		m_vecStepData [s][CPU01].clear();
//	}
//}
//
//std::string CFls::GetHostname()
//{
//	return UTIL::TCPIP::GetHostName();
//}
//
//std::string CFls::GetChambername()
//{
//	return std::string("CHAMBER01");
//}
//
//bool CFls::GenFls(const char* szReportPath)
//{
//	snprintf( m_szReportPath, PATH_MAX, "%s", szReportPath );
//
//	uint uRefSlot = 0;
//	uint uRefCpu  = CPU00;
//	if( !m_pDataMgr->GET_LAST_CYCLE_END_SLOT_CPU( uRefSlot, uRefCpu) )
//	{
//		fprintf( stdout, "[ ERR] %s : No slots were tested or not lot set\n", __FUNCTION__ );
//		return false;
//	}
//	fprintf( stdout, "[INFO] %s Last Test Slot/Cpu : S#%02d-C#%02d\n", __FUNCTION__, uRefSlot, uRefCpu );
//
//	m_nMaxCpu = 1;
//	m_bCpu2X  = m_pDataMgr->GET_CPU2X(uRefSlot, uRefCpu);
//	if( m_bCpu2X ) m_nMaxCpu = 2;
//	uint step_count = m_pDataMgr->GET_MAX_STEP(uRefSlot, uRefCpu);	//STEPOUT
//	if( step_count < 1 )
//	{
//		fprintf( stdout, "[ ERR] %s S#%02d Step cout is less than 1\n", __FUNCTION__, uRefSlot+1 );
//		return false;
//	}
//
//	bool bMake = false;
//	if( MakePreFlsData(uRefSlot, uRefCpu) )
//	{
//		//iFileType==0 : fls, iFileType==1 : flsh
//		for(uint iFileType=0; iFileType<2; ++iFileType)
//		{
//			if( GenFlsFileName( iFileType, uRefSlot ) )
//			{
//				if( MakeSortingHead(iFileType, uRefSlot, uRefCpu) && MakeHistoryHead(iFileType, uRefSlot, uRefCpu) )
//				{
//					bMake = true;
//					MakeSortingDataSummary  ( iFileType, uRefSlot         , bMake );
//					MakeSortingDataBoardMap ( iFileType, uRefSlot         , bMake );
//					MakeTestHistoryReport   ( iFileType, uRefSlot, uRefCpu, bMake );
//					MakeTestBlockCountReport( iFileType, uRefSlot         , bMake );
//					MakeFailSocketReport    ( iFileType, uRefSlot, uRefCpu, bMake );
//				}
//				close(m_FlsFd);
//			}
//		}
//	}
//	else
//	{
//		printf("[ ERR] MakePreFlsData error!\n" );
//		bMake = false;
//	}
//	printf("[INFO] FLS MAKE %s...\n", bMake?"OK":"NG" );
//
//	for( int s=0;s<MAX_SLOT;s++ )
//	{
//		if( m_pSortBin [s] != nullptr ) { delete[] m_pSortBin [s];  m_pSortBin[s] = nullptr; }
//		if( m_pInitFail[s] != nullptr ) { delete[] m_pInitFail[s]; m_pInitFail[s] = nullptr; }
//		if( m_pStepFail[s] != nullptr ) { delete[] m_pStepFail[s]; m_pStepFail[s] = nullptr; }
//		for( uint cpu=0;cpu<2;cpu++ )
//		{
//			if( m_pStepInInfo [s][cpu] != nullptr ) { delete[] m_pStepInInfo [s][cpu]; m_pStepInInfo [s][cpu] = nullptr; }
//			if( m_pStepOutInfo[s][cpu] != nullptr ) { delete[] m_pStepOutInfo[s][cpu]; m_pStepOutInfo[s][cpu] = nullptr; }
//		}
//		for(uint i=0; i<m_vecStepData[s][CPU00].size(); ++i )
//			delete m_vecStepData[s][CPU00][i];
//		m_vecStepData[s][CPU00].clear();
//		for(uint i=0; i<m_vecStepData[s][CPU01].size(); ++i )
//			delete m_vecStepData[s][CPU01][i];
//		m_vecStepData[s][CPU01].clear();
//	}
//
//	return bMake;
//}
//
//bool CFls::GenFlsFileName(uint iFileType, int slot)
//{
//	char szFileName[PATH_MAX] = {0x0,};
//	char szExt  [2][PATH_MAX] = { FLS_EXT, FLSH_EXT };
//
//	snprintf( szFileName, PATH_MAX, "%s/%s-%03d-%03d-%s%s",
//			  m_szReportPath, m_pDataMgr->GET_LOTNO(), 0x0, 0x0, m_pDataMgr->GET_DEVICE(slot,CPU00), szExt[iFileType]);
//	m_FlsFd = open( szFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
//	if( m_FlsFd == -1 )
//	{
//		printf("[ ERR] %s open error!\n", szFileName );
//		return false;
//	}
//	//printf("[INFO] %s start...\n", szFileName );
//	 = szFileName;
//	return true;
//}
//
//bool CFls::MakeSortingHead( uint iFileType, uint rslot, uint rcpu )
//{
//	if ( iFileType >0 ) return true;
//
//	const string strCr    = "\n";
//	std::string strData   = "";
//	char szLine[PATH_MAX] = {0x0,};
//	struct tm* t;
//
//	strData.clear();
//	m_sHeadSorting.clear();
//
//	t = localtime( m_pDataMgr->GET_CYCLE_BEGIN_TIME( rslot, rcpu) );
//	sprintf(szLine, "START-TIME %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//	strData += szLine;
//
//	t = localtime( m_pDataMgr->GET_CYCLE_END_TIME( rslot, rcpu) );
//	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d ", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//	strData += szLine;
//
//	sprintf(szLine, "PARTS %-17s "     , m_pDataMgr->GET_PARTNO()             ); strData += szLine;
//	sprintf(szLine, "LOTNO %-10s "     , m_pDataMgr->GET_LOTNO()              ); strData += szLine;
//	sprintf(szLine, "SUBLOT %2d "      , 0                                    ); strData += szLine;
//	sprintf(szLine, "TP %-20s"         , m_pDataMgr->GET_DEVICE(rslot, rcpu)  ); strData += szLine + strCr;
//	sprintf(szLine, "STATION %s "      , m_pDataMgr->GET_STATION()            ); strData += szLine;
//	sprintf(szLine, "CHAMBER%1d "      , m_pDataMgr->GET_ZONE()               ); strData += szLine;
//	sprintf(szLine, "SUBLOT-STATUS %s ", "CLOSE"                              ); strData += szLine;
//	sprintf(szLine, "REMARK %s "       , m_pDataMgr->GET_REMARK(rslot, rcpu)  ); strData += szLine;
//	sprintf(szLine, "OPER %s "          , m_pDataMgr->GET_OPRID()             ); strData += szLine;
//	sprintf(szLine, "MPG_FLAG:%s"      , "N"                                  ); strData += szLine + strCr;
//	sprintf(szLine, "OID EIS"                                                 ); strData += szLine + strCr;
//	m_sHeadSorting = strData;
//	return true;
//}
//
//bool CFls::MakeHistoryHead( uint iFileType, uint rslot, uint rcpu )
//{
//	if ( iFileType >0 ) return true;
//
//	const string strCr    = "\n";
//	std::string strData   = "";
//	char szLine[PATH_MAX] = {0x0,};
//	struct tm* t;
//
//	strData.clear();
//	m_sHeadHistory.clear();
//
//	sprintf(szLine, "PARTS %-17s "     , m_pDataMgr->GET_PARTNO() ); strData += szLine;
//	sprintf(szLine, "LOTNO %-10s "     , m_pDataMgr->GET_LOTNO()  ); strData += szLine;
//	sprintf(szLine, "BATCH %04d    "   , 0                        ); strData += szLine;
//	sprintf(szLine, "TP %s   "         , " "                      ); strData += szLine;
//	sprintf(szLine, "STATION %s      " , m_pDataMgr->GET_STATION()); strData += szLine;
//	sprintf(szLine, "CHAMBER%1d    "   , m_pDataMgr->GET_ZONE()   ); strData += szLine;
//	t = localtime( m_pDataMgr->GET_CYCLE_END_TIME( rslot, rcpu ) );
//	sprintf(szLine, "END-TIME  %04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//	strData += szLine + strCr;
//	m_sHeadHistory = strData;
//	return true;
//}
//
//bool CFls::MakePreFlsData(uint rSlot,uint rCpu)
//{
//	m_nBibX = m_pDataMgr->GET_BIBX(rSlot,rCpu);
//	m_nBibY = m_pDataMgr->GET_BIBY(rSlot,rCpu);
//	m_nMaxDutPerCpu = m_pDataMgr->GET_MAXDUTPERCPU( rSlot, rCpu );
//	m_nMaxDut = m_nBibX * m_nBibY;
//
//	if( 0 )
//	{
//		printf( " ->PRE_FLS BIB X:%02d\n", m_nBibX );
//		printf( " ->PRE_FLS BIB Y:%02d\n", m_nBibY );
//		printf( " ->PRE_FLS DUT PER CPU:%d\n", m_nMaxDutPerCpu );
//		printf( " ->PRE_FLS DUT MAX:%d\n", m_nMaxDut );
//	}
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/// INIT FAIL
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	for( int slot=0;slot<MAX_SLOT;slot++ )
//	{
//		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//			continue;
//
//		stInitTestHead initHead[m_nMaxCpu];
//		m_pInitFail[slot] = new char[ m_nMaxDut ];
//		if ( m_pInitFail[slot] == nullptr )
//		{
//			printf( "[ ERR] S#%02d FLS Memory(Init Fail) alloc error\n", slot );
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_INITTEST_DATA( slot, CPU00, initHead[CPU00], m_pInitFail[slot], sizeof(char)*m_nMaxDutPerCpu) )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			delete[] m_pInitFail[slot];
//			m_pInitFail[slot] = nullptr;
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_INITTEST_DATA( slot, CPU01, initHead[CPU01], m_pInitFail[slot]+m_nMaxDutPerCpu, sizeof(char)*m_nMaxDutPerCpu) )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			delete[] m_pInitFail[slot];
//			m_pInitFail[slot] = nullptr;
//			return false;
//		}
//		m_initTestHead[slot][CPU00].nStepNo = initHead[CPU00].nStepNo;
//		m_initTestHead[slot][CPU01].nStepNo = initHead[CPU01].nStepNo;
//		sprintf( m_initTestHead[slot][CPU00].szStepName, "%s", initHead[CPU00].szStepName );
//		sprintf( m_initTestHead[slot][CPU01].szStepName, "%s", initHead[CPU01].szStepName );
//
//		m_initTestHead[slot][CPU00].tInitBegin = initHead[CPU00].tInitBegin;
//		m_initTestHead[slot][CPU01].tInitBegin = initHead[CPU01].tInitBegin;
//
//		m_initTestHead[slot][CPU00].tInitEnd   = initHead[CPU00].tInitEnd;
//		m_initTestHead[slot][CPU01].tInitEnd   = initHead[CPU01].tInitEnd;
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// SORT BIN
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//printf( " ->PRE_FLS SORT BIN DATA\n" );
//		m_pSortBin[slot]  = new uint [m_nMaxDut];
//		if ( m_pSortBin[slot] == nullptr )
//		{
//			printf( "[ ERR] S#%02d-C#%02d FLS Memory(Sort Bin) alloc error\n", slot, CPU01 );
//			return false;
//		}
//		bool bSortRead = true;
//		bSortRead = m_pDataMgr->GET_SORTBIN_DATA(slot, CPU00, m_pSortBin[slot]                , sizeof(int)*m_nMaxDutPerCpu );
//		if( !bSortRead )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			delete[] m_pSortBin[slot];
//			m_pSortBin[slot] = nullptr;
//			return false;
//
//		}
//		bSortRead = m_pDataMgr->GET_SORTBIN_DATA(slot, CPU01, m_pSortBin[slot]+m_nMaxDutPerCpu, sizeof(int)*m_nMaxDutPerCpu );
//		if( !bSortRead )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			delete[] m_pSortBin[slot];
//			m_pSortBin[slot] = nullptr;
//			return false;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// BINMAP DATA
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		if( !m_pDataMgr->GET_BINMAP_DATA(slot, CPU00, m_Binmap[slot], sizeof(uint)*16) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		memset( m_nSortBinCnt [slot], 0x0, sizeof(m_nSortBinCnt [slot]) );
//		memset( m_nSortBinHCnt[slot], 0x0, sizeof(m_nSortBinHCnt[slot]) );
//		for( uint iDut=0;iDut<m_nMaxDut;iDut++ )
//		{
//			if( m_pInitFail[slot][iDut] ) continue;
//			if( m_pSortBin [slot][iDut] > 16 ) continue;
//			m_nSortBinCnt [slot][ m_Binmap[slot][m_pSortBin[slot][iDut]] ]++;	//fls
//			m_nSortBinHCnt[slot][                m_pSortBin[slot][iDut]  ]++;	//flsh
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// STEP INFO(COUNT/BEGIN/END/TEMP/BITIME...)
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//printf( " ->PRE_FLS STEP IN/OUT DATA\n" );
//		int nMaxStep0 = m_pDataMgr->GET_MAX_STEP( slot, CPU00 );
//		int nMaxStep1 = m_pDataMgr->GET_MAX_STEP( slot, CPU01 );
//		if( nMaxStep0 != nMaxStep1 )
//		{
//			printf("[ ERR] S#%02d MaxStep[%d], CPU1 MaxStep[%d] is different\n",
//				   slot, nMaxStep0, nMaxStep1 );
//			return false;
//		}
//		m_nMaxStep = nMaxStep0;
//		//printf( "   ->MAX STEP : %d\n", m_nMaxStep );
//		if( m_nMaxStep < 1 )
//		{
//			printf("[ ERR] %s S#%02d MaxStep is less than 1\n",  __FUNCTION__, slot );
//			return false;
//		}
//		m_pStepInInfo [slot][CPU00] = new stStepInInfo [m_nMaxStep];
//		m_pStepInInfo [slot][CPU01] = new stStepInInfo [m_nMaxStep];
//		m_pStepOutInfo[slot][CPU00] = new stStepOutInfo[m_nMaxStep];
//		m_pStepOutInfo[slot][CPU01] = new stStepOutInfo[m_nMaxStep];
//
//		if( !m_pDataMgr->GET_STEPIN_INFO(slot, CPU00, m_pStepInInfo[slot][CPU00], m_nMaxStep ) )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_STEPIN_INFO(slot, CPU01, m_pStepInInfo[slot][CPU01], m_nMaxStep ) )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_STEPOUT_INFO(slot, CPU00, m_pStepOutInfo[slot][CPU00], m_nMaxStep ) )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_STEPOUT_INFO(slot, CPU01, m_pStepOutInfo[slot][CPU01], m_nMaxStep ) )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		for(uint i=0; i<m_vecStepData[slot][CPU00].size(); ++i )
//			delete m_vecStepData[slot][CPU00][i];
//		m_vecStepData[slot][CPU00].clear();
//
//		for(uint i=0; i<m_vecStepData[slot][CPU01].size(); ++i )
//			delete m_vecStepData[slot][CPU01][i];
//		m_vecStepData[slot][CPU01].clear();
//
////		m_vecStepData[slot][CPU00].clear();
////		m_vecStepData[slot][CPU01].clear();
//
//		uint nStartTs = 1;
//		for( uint c=0;c<MAX_SYS_CPU;c++ )
//		{
//			CStepData* pStepInit =new CStepData;
//			pStepInit->iTsNo = nStartTs;
//			pStepInit->tStepBegin = m_initTestHead[slot][c].tInitBegin;
//			pStepInit->tStepEnd   = m_initTestHead[slot][c].tInitEnd;
//			sprintf( pStepInit->szStepName,"%s" ,m_initTestHead[slot][c].szStepName );
//			sprintf( pStepInit->szPc      ,"%s" ,m_initTestHead[slot][c].szStepName );
//			m_vecStepData[slot][c].push_back( pStepInit );
//		}
//		nStartTs++;
//
//		for( uint step=0;step<m_nMaxStep;step++ )
//		{
//			for( uint c=0;c<MAX_SYS_CPU;c++ )
//			{
//				CStepData* pStepData = new CStepData;
//				pStepData->iTsNo = nStartTs;
//				pStepData->tStepBegin = m_pStepInInfo[slot][c][step].tStepBegin;
//				pStepData->tStepEnd   = m_pStepOutInfo[slot][c][step].tStepEnd;
//				pStepData->dTemp      = m_pStepOutInfo[slot][c][step].dTemp;
//				pStepData->dBiTime    = m_pStepOutInfo[slot][c][step].dBiTime;
//				pStepData->dVcc       = m_pStepOutInfo[slot][c][step].dVcc;
//				sprintf( pStepData->szStepName, "%s", m_pStepOutInfo[slot][c][step].szStepName );
//				sprintf( pStepData->szPc, "%s", m_pStepOutInfo[slot][c][step].szPc );
//
//				if( 0 )
//				{
//					struct tm* tBegin;
//					struct tm* tEnd;
//					tBegin = localtime( &pStepData->tStepBegin );
//					tEnd   = localtime( &pStepData->tStepEnd );
//					printf( "   ->STEP.%03d : %s\n", pStepData->iTsNo, pStepData->szStepName );
//					printf( "   ->STEP.%03d BEGIN: %04d/%02d/%02d %02d:%02d:%02d\n",
//							pStepData->iTsNo, tBegin->tm_year+1900, tBegin->tm_mon+1, tBegin->tm_mday, tBegin->tm_hour, tBegin->tm_min, tBegin->tm_sec );
//					printf( "   ->STEP.%03d END  : %04d/%02d/%02d %02d:%02d:%02d\n",
//							pStepData->iTsNo, tEnd->tm_year+1900, tEnd->tm_mon+1, tEnd->tm_mday, tEnd->tm_hour, tEnd->tm_min, tEnd->tm_sec );
//				}
//				m_vecStepData[slot][c].push_back( pStepData );
//			}
//			nStartTs++;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// STEP FAIL
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//printf( " ->PRE_FLS STEP FAIL DATA\n" );
//		uint nEachFailSize = sizeof(char) * m_nMaxDutPerCpu;
//		m_pStepFail[slot] = new char[m_nMaxDut*m_nMaxStep];
//		bool bStepRead = true;
//		bStepRead = m_pDataMgr->GET_STEPFAIL_DATA( slot, CPU00, m_pStepFail[slot], m_nMaxStep, nEachFailSize );
//		if( !bStepRead )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			delete[] m_pStepFail[slot];
//			m_pStepFail[slot] = nullptr;
//			return false;
//		}
//		//uint nCpu2Offset = m_nMaxStep * nEachFailSize;
//		bStepRead = m_pDataMgr->GET_STEPFAIL_DATA( slot, CPU01, m_pStepFail[slot], m_nMaxStep, nEachFailSize );
//		if( !bStepRead )
//		{
//			printf("[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			delete[] m_pStepFail[slot];
//			m_pStepFail[slot] = nullptr;
//			return false;
//		}
//	}
//	return true;
//}
//
//bool CFls::MakeSortingDataSummary(uint iFileType, uint rSlot, bool& bMake)
//{
//	if( !bMake ) return false;
//
//	char szLine[PATH_MAX] = {0x0,};
//	const string strCr = "\n";
//	string strLine = "";
//	string strData = "";
//	time_t tNow;
//	struct tm* t;
//
//	strData.clear();
//	sprintf(szLine, "%-100s", "<< SORTING DATA SUMMARY >>"); strData += szLine;
//	tNow = time(NULL); 	t = localtime( &tNow );
//	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//	strData += szLine + strCr;
//	strData += GetSortHead();
//
//	sprintf(szLine, "%38s"             , "SORTING-LINE"       );	strData += szLine + strCr;
//	strLine = "  BID      TOTAL    0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16"; strData += strLine + strCr;
//	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
//
//	uint nTotal = 0;
//	uint nSortBinSum[17]={0,};
//	memset( nSortBinSum, 0x0, sizeof(nSortBinSum) );
//	for( int slot=0;slot<MAX_SLOT;slot++ )
//	{
//		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//			continue;
//		uint nSlotTotal = m_nMaxDut;
//		for( uint dut=0;dut<m_nMaxDut;dut++ )
//		{
//			if( m_pInitFail[slot][dut] ) nSlotTotal--;
//		}
//		sprintf(szLine, "%09s ", m_pDataMgr->GET_BIBID(slot) );	strData += szLine;
//		sprintf(szLine, "%6d"  , nSlotTotal );	strData += szLine;
//		for(int iSortBin=0; iSortBin<17; ++iSortBin )
//		{
//			uint nBinCnt = 0;
//				 if ( iFileType==0 ) nBinCnt = m_nSortBinCnt [slot][iSortBin]; //fls
//			else if ( iFileType==1 ) nBinCnt = m_nSortBinHCnt[slot][iSortBin]; //flsh
//
//			sprintf(szLine, " %4d", nBinCnt );	strData += szLine;
//			nSortBinSum[iSortBin] += nBinCnt;
//		}
//		nTotal += nSlotTotal;
//		strData += strCr;
//	}
//	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
//	sprintf(szLine, "%-9s ", "TOTAL" );	strData += szLine;
//	sprintf(szLine, "%6d"  , nTotal );	strData += szLine;
//	for(int iSortBin=0; iSortBin<17; ++iSortBin )
//	{
//		sprintf(szLine, " %4d", nSortBinSum[iSortBin] );	strData += szLine;
//	}
//	strData += strCr;
//	strLine = "--------- ------ ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----"; strData += strLine + strCr;
//	sprintf(szLine, "%-16s", "RATE (%)" );	strData += szLine;
//	for(int iSortBin=0; iSortBin<17; ++iSortBin )
//	{
//		sprintf(szLine, " %4.1f", nSortBinSum[iSortBin]*100.0/nTotal );	strData += szLine;
//	}
//	strData += strCr + strCr;
//	uint strSize = strData.length();
//	ssize_t written = write( m_FlsFd, strData.c_str(), strSize );
//	if( written != strSize )
//	{
//		printf("[ ERR]file(%s) %s write error", m_FlsFile.c_str(), __FUNCTION__ );
//		bMake = false;
//		return bMake;
//	}
//	bMake = true;
//	return bMake;
//}
//
//bool CFls::MakeSortingDataBoardMap(uint iFileType, uint rSlot, bool& bMake)
//{
//	if( !bMake ) return false;
//
//	char szLine[PATH_MAX] = {0x0,};
//	string strData = "";
//	time_t tNow;
//	struct tm* t;
//	sprintf(szLine, "%-100s", "<< SORTING DATA BOARD MAP >>"); strData += szLine;
//	tNow = time(NULL); 	t = localtime( &tNow );
//	sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//	strData += szLine + STR_LF;
//	strData += GetSortHead();
//
//	for( int slot=0;slot<MAX_SLOT;slot++ )
//	{
//		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//			continue;
//		uint nTotal = m_nMaxDut;
//		for( uint dut=0;dut<m_nMaxDut;dut++ )
//		{
//			if( m_pInitFail[slot][dut] ) nTotal--;
//		}
//
//		sprintf(szLine, "BID %9s "      , m_pDataMgr->GET_BIBID(slot)    ); strData += szLine;
//		sprintf(szLine, "Z1S%02d IOC=0 BOC=0 MLB_CODE=%s", slot+1, "SLB" ); strData += szLine + STR_LF;
//		sprintf(szLine, "Total %d"           , nTotal                    ); strData += szLine + STR_LF;
//
//		strData += "   ";
//		for ( uint r=0; r<m_nBibX; r++ )
//		{
//			sprintf( szLine, " %2d", r+1 ); strData += szLine;
//		}
//		strData += STR_LF;
//
//		for ( int c=m_nBibY-1; c>=0; c-- )
//		{
//			sprintf( szLine, " %2d", c+1 ); strData += szLine;
//			for ( uint r=0; r<m_nBibX; r++ )
//			{
//				int dut = c*m_nBibX + r;
//
//				if (  m_pInitFail[slot][dut] )
//					sprintf( szLine, "  ."                 );
//				else
//				{
//					uint nBin = m_pSortBin[slot][dut];									//iFileType==1, flsh
//					if ( iFileType==0 ) nBin = m_Binmap[slot][ m_pSortBin[slot][dut] ];	//iFileType==0, fls
//
//					sprintf( szLine, " %2d", nBin );
//				}
//				strData += szLine;
//			}
//			strData += STR_LF;
//		}
//		strData += STR_LF;
//	}
//	uint strSize = strData.length();
//	ssize_t written = write( m_FlsFd, strData.c_str(), strSize );
//	if( written != strSize )
//	{
//		printf("[ ERR]file(%s) %s write error", m_FlsFile.c_str(), __FUNCTION__ );
//		bMake = false;
//		return bMake;
//	}
//	bMake = true;
//	return bMake;
//}
//
//bool CFls::MakeTestHistoryReport(uint iFileType, uint rSlot, uint rCpu, bool& bMake)
//{
//	if( !bMake ) return false;
//
//	if ( iFileType==0 )
//	{
//		std::string strData = "";
//		std::string strLine = "";
//		char szLine[PATH_MAX] = {0x0,};
//		time_t tNow;
//		struct tm* t;
//
//		sprintf(szLine, "%-100s", "<<TEST HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
//		tNow = time(NULL); 	t = localtime( &tNow );
//		sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//		strData += szLine +	STR_LF;
//		strData += GetHistoryHead();
//		strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
//		strData += strLine + STR_LF;
//
//		uint nTotal = 0;
//		for( int slot=0;slot<MAX_SLOT;slot++ )
//		{
//			if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//				continue;
//			int nSlotTotal = m_nMaxDut;
//			for(uint iDut=0; iDut<m_nMaxDut; ++iDut)
//			{
//				if ( m_pInitFail[slot][iDut] )
//					--nSlotTotal;
//			}
//			nTotal += nSlotTotal;
//		}
//
//		uint nStepCount = m_nMaxStep;
//		if( m_vecStepData[rSlot][rCpu].size() < nStepCount ) nStepCount = m_vecStepData[rSlot][rCpu].size();
//		for(uint i=0; i<nStepCount; ++i )
//		{
//			t = localtime( &m_vecStepData[rSlot][rCpu][i]->tStepEnd );
//			sprintf(szLine, "%3d %02d/%02d %02d:%02d ", m_vecStepData[rSlot][rCpu][i]->iTsNo, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min   );	strData += szLine;
//			sprintf(szLine, "%s  %3d %6.1f %7.1f "   , (i==0?"ITEST":"FTEST"), m_vecStepData[rSlot][rCpu][i]->iTsNo, m_vecStepData[rSlot][rCpu][i]->dTemp,  0.0 /*m_vecStepData[rSlot][i]->dBiTime*/  );	strData += szLine;
//			sprintf(szLine, "%-20s %-20s %5d %5d "  , m_vecStepData[rSlot][rCpu][i]->szStepName, m_vecStepData[rSlot][rCpu][i]->szPc, nTotal, nTotal );	strData += szLine;
//			uint   iStepFailCnt = 0;
//			double dFailRate    = 0.0;
//			if ( i>0 )
//			{
//				for( int slot=0;slot<MAX_SLOT;slot++ )
//				{
//					if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//						continue;
//					iStepFailCnt += StepFailCount( slot, i-1 );
//				}
//				dFailRate = (iStepFailCnt * 100.0) / nTotal;
//			}
//			sprintf(szLine, "%5d %7.2f %4.1f", iStepFailCnt, dFailRate, m_vecStepData[rSlot][rCpu][i]->dVcc );	strData += szLine + STR_LF;
//		}
//		strData += STR_LF;
//
//		m_sTestHistoryReport = strData;
//	}
//
//	uint strSize = m_sTestHistoryReport.length();
//	ssize_t written = write( m_FlsFd, m_sTestHistoryReport.c_str(), strSize );
//	if( written != strSize )
//	{
//		printf("[ ERR]file(%s) %s write error", m_FlsFile.c_str(), __FUNCTION__ );
//		bMake = false;
//		return bMake;
//	}
//	bMake = true;
//	return bMake;
//}
//
//uint CFls::StepFailCount(int slot, uint nStep)
//{
//	if ( m_pStepFail[slot] == nullptr ) return 0;
//	if ( m_pInitFail[slot] == nullptr ) return 0;
//
//	ssize_t nStepFailSizePerStep = m_nMaxDut;	//* sizeof(char)
//	uint    nOffset              = nStepFailSizePerStep*nStep;
//
//	uint iFailCnt = 0;
//	for(uint i=0; i<m_nMaxDut; ++i)
//	{
//		if ( m_pInitFail[slot] [          i] ) continue;
//		if ( m_pStepFail[slot] [nOffset + i] ) ++iFailCnt;
//	}
//
//	return iFailCnt;
//}
//
//bool CFls::IsStepFail(int slot, uint dut, uint step)
//{
//	if ( m_pStepFail[slot] == nullptr ) return false;
//	ssize_t nStepFailSizePerStep = m_nMaxDut * step;	//* sizeof(char)
//
//	return static_cast<bool>( m_pStepFail[slot][ nStepFailSizePerStep + dut ] );
//}
//
//bool CFls::IsAllStepPass(int slot, uint dut, uint steps)
//{
//	if ( m_pStepFail[slot]==nullptr ) return true;
//
//	int nStepFailSizePerStep = m_nMaxDut;	//* sizeof(char)
//
//	for(uint iStep=0; iStep<steps; ++iStep)
//	{
//		if ( m_pStepFail[slot][nStepFailSizePerStep*iStep + dut] )
//			return false;
//	}
//
//	return true;
//}
//
//bool CFls::MakeTestBlockCountReport(uint iFileType, uint rSlot, bool& bMake)
//{
//	if( !bMake ) return false;
//
//	if ( iFileType==0 )
//	{
//		std::string strData = "";
//		std::string strLine = "";
//		char szLine[PATH_MAX] = {0x0,};
//		time_t tNow;
//		struct tm* t;
//		uint nTotal = m_nMaxDut;
//
//		sprintf(szLine, "%-100s", "<<TEST Block Count HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
//		tNow = time(NULL); 	t = localtime( &tNow );
//		sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//		strData += szLine +	STR_LF;
//		strData += GetHistoryHead();
//
//		strLine = "TS#   END-TIME  TNAME  TNO   TEMP BI-TIME   TCS      PTS   TOTAL  TEST  FAIL FRATE(%)  VCC";
//		strData += strLine + STR_LF + STR_LF + STR_LF + STR_LF;
//
//		m_sTestBlockCountReport = strData;
//	}
//
//	uint strSize = m_sTestBlockCountReport.length();
//	ssize_t written = write( m_FlsFd, m_sTestBlockCountReport.c_str(), strSize );
//	if( written != strSize )
//	{
//		printf("[ ERR]file(%s) %s write error", m_FlsFile.c_str(), __FUNCTION__ );
//		bMake = false;
//		return bMake;
//	}
//	bMake = true;
//	return bMake;
//}
//
//bool CFls::MakeFailSocketReport(uint iFileType, uint rSlot, uint rCpu, bool& bMake)
//{
//	if( !bMake ) return false;
//
//	if ( iFileType==0 )
//	{
//		std::string strData = "";
//		std::string strLine = "";
//		char szLine[PATH_MAX] = {0x0,};
//		time_t tNow;
//		struct tm* t;
//
//		sprintf(szLine, "%-100s", "<<FAIL SOCKET HISTORY FOR PASS/FAIL REPORT>>"); strData += szLine;
//		tNow = time(NULL); 	t = localtime( &tNow );
//		sprintf(szLine, "%04d/%02d/%02d %02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//		strData += szLine + STR_LF;
//		strData += GetHistoryHead();
//
//		for( int slot=0;slot<MAX_SLOT;slot++ )
//		{
//			if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//				continue;
//
//			sprintf(szLine, "BNO:%-3d BID %s Z%d-S%d", slot+1, m_pDataMgr->GET_BIBID(slot), 1, slot+1 );
//			strData += szLine + STR_LF;
//			strLine = "                                                                                                         1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    1    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    2    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    3    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    4    5    5    5    5    5    5    5";	strData += strLine + STR_LF;
//			strLine = "               1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3    3    4    4    5    5    6    6    7    7    8    8    9    9    0    0    1    1    2    2    3";	strData += strLine + STR_LF;
//			strLine = "          5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0    5    0";	strData += strLine + STR_LF;
//			strLine = "-----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----" ;	strData += strLine + STR_LF;
//
//			for ( uint c=0; c<m_nBibY; c++ )
//			{
//				for ( uint r=0; r<m_nBibX; r++ )
//				{
//					int dut = c*m_nBibX + r;
//
//						 if ( m_pInitFail[slot][dut]                      ) continue;
//					else if ( IsAllStepPass( slot, dut, m_nMaxStep) ) continue;
//
//					sprintf(szLine, "%02d-%02d|", c+1, r+1 );	strData += szLine;
//					strLine.clear();
//					uint nStepCount = m_nMaxStep;
//					//2023.09.04 - KWMOON(m_vecStepData[slot][slot] -> m_vecStepData[slot][CPU00] bug fix )
//					if( m_vecStepData[slot][CPU00].size() > nStepCount ) nStepCount = m_vecStepData[slot][CPU00].size();
//					for(uint i=0; i<nStepCount; ++i )
//					{
//						if ( i==0 ) { strLine += "m"; continue; }
//
//						if ( IsStepFail( slot, dut, i-1) ) strLine += "X";
//						else                               strLine += "o";
//					}
//					strData += strLine + STR_LF;
//				}
//			}
//			strData += STR_LF;
//		}
//
//		m_sFailSocketReport = strData;
//	}
//
//	uint strSize = m_sFailSocketReport.length();
//	ssize_t written = write( m_FlsFd, m_sFailSocketReport.c_str(), strSize );
//	if( written != strSize )
//	{
//		printf("[ ERR]file(%s) %s write error", m_FlsFile.c_str(), __FUNCTION__ );
//		bMake = false;
//		return bMake;
//	}
//	bMake = true;
//	return bMake;
//}
//

