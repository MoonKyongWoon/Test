#include "chdr.hpp"

CChdr::CChdr(COrganizedData* pOd) : m_pOd(pOd)
{
	m_strFiles = "";
}

bool CChdr::GenChdr()
{
	bool bMake = true;
	if ( m_pOd->m_Lot.empty() ) bMake = false;

	for ( LOT_LIST::iterator itTotLot = m_pOd->m_Lot.begin(); itTotLot != m_pOd->m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		uint nTotChip = m_pOd->m_nCePerDut[pLot->m_uRS] * m_pOd->m_nChipPerCe[pLot->m_uRS];
		for(uint iChip=0; iChip<nTotChip; ++iChip)
		{
			if ( ! CreateChdrFile( pLot, iChip, nTotChip ) )
				bMake = false;
		}

		if ( ! m_strFiles.empty() && m_pOd->m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("CHDR") )
				UTIL::FTP::MPUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFiles.c_str(), "ascii" );
			m_strFiles.clear();
		}
	}

	printf("[INFO] CHDR MAKE %s...\n", bMake?"OK":"NG" );
	return true;
}

uint CChdr::GetPara(T_LOT* pLot)
{
	uint nSlot=16;	//Total slot count

//	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
//	{
//		if ( ! pLot->S[iSlot].IS_VALID() ) continue;
//
//		nSlot++;
//	}

	return m_pOd->m_nBibX[pLot->m_uRS] * m_pOd->m_nBibY[pLot->m_uRS] * nSlot;
}

const char* CChdr::LotProc()
{
	static const char* szLotProc = "T3C1";

	return szLotProc;
}

bool CChdr::IsPreFail(uint iSlot, uint iDut)
{
	if ( m_pOd->m_pInitFail[iSlot]==nullptr ) return false;

	return static_cast<bool>( m_pOd->m_pInitFail[iSlot][iDut] );
}

bool CChdr::IsChipFail( char* pFailChip, uint slot, uint dut, uint ce, uint chip, uint step/*=0*/ )
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nChipPerDutBl[slot];	//* sizeof(char)

	int chip_index = ce * m_pOd->m_nChipPerCe[slot] + chip;
	int m = nFailChipSizePerStep*step + dut * m_pOd->m_nChipPerDutBl[slot] + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;																	//chip_index % 8;

	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
}

int CChdr::GetFailBlockCnt(char* pFailBlock, uint iSlot, uint dut, uint ce, uint chip, uint step/*=0*/ )
{
	ssize_t nFailBlockSizePerStep = m_pOd->m_nMaxDut[iSlot] * m_pOd->m_nBlockPerDutBl[iSlot];
	uint iOffset = nFailBlockSizePerStep*step + dut*m_pOd->m_nBlockPerDutBl[iSlot];
	uint minaddr = ce * m_pOd->m_nBlockPerCe[iSlot] + chip * m_pOd->m_nBlockPerChip[iSlot];
	uint maxaddr = ce * m_pOd->m_nBlockPerCe[iSlot] + chip * m_pOd->m_nBlockPerChip[iSlot] + m_pOd->m_nBlockPerChip[iSlot] - 1;
	uint iFailBlkCnt = 0;

	for(uint iBlock=minaddr; iBlock<=maxaddr; ++iBlock)
	{
		int m = iOffset + (iBlock>>3);		if ( ! pFailBlock[m] )
		{
//			printf("Bf iBlock=%x     7-(iBlock%%8)=%d   ", iBlock, 7-(iBlock%8) );
			iBlock += 7 - (iBlock%8);
//			printf("Af iBlock=%x\n", iBlock );
			continue;
		}

		int n = iBlock&0x7;
		bool bFail = static_cast<bool>( (pFailBlock[m] >> n) & 1);
		if ( bFail )
		{
			iFailBlkCnt ++;
			//printf("fail=%x\n", iBlock );
//			vBlkAddr.push_back( iBlock );
		}
//		else
//			printf("pass=%x\n", iBlock );
	}
	return iFailBlkCnt;
}

bool CChdr::GetFCate(uint* pStepFailCate, uint slot, uint dut, std::vector<uint>& vFCate, bool bWithStep/*=true*/)
{
	vFCate.clear();
	for(uint iStep=1; iStep<=m_pOd->m_nMaxStep[slot]; ++iStep)
	{
		uint uStep = iStep-1;
		uint fcate = pStepFailCate[m_pOd->m_nMaxDut[slot]*uStep + dut];
		if ( fcate > 0 )
		{
			if ( bWithStep )
				vFCate.push_back( (iStep<<16) | fcate );
			else
				vFCate.push_back(               fcate );
		}
	} //End of iStep

	if ( vFCate.empty() ) return false;

	return true;
}

bool CChdr::CreateChdrFile(T_LOT* pLot, uint iChip, uint nTotChip)
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

	snprintf( szFile, PATH_MAX, "%s_1_%04d%02d%02d%02d%02d_%s_DIE%d_%d%s",
			  pDataMgr->GET_STATION() ,
			  YYYY(tEnd  )            ,
			  MM  (tEnd  )            ,
			  DD  (tEnd  )            ,
			  hh  (tStart)            ,
			  mm  (tStart) + iChip + 1,
			  pLot->LOT_NO            ,
			  iChip+1, nTotChip       ,
			  CHDR_EXT                  );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );

	fd = open(szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
		printf("[ ERR] %s open error!\n", szPath );

	m_strDir = szDir ; m_strFiles += szFile + string(" ");
	m_sFile = szPath;

	if ( ! MakeHeader(pLot, iChip, nTotChip, fd) )
	{
		close(fd);
		return false;
	}

	if ( ! AppendChdr(pLot, iChip, nTotChip, fd) )
	{
		close(fd);
		return false;
	}

	close(fd);
	printf("%s  gerneration complet.\n", m_sFile.c_str() );

	return true;
}

bool CChdr::MakeHeader(T_LOT* pLot, uint iChip, uint nTotChip, int fd)
{
	if ( fd<0 ) return false;

	string strLine;
	string strData;
	char   szLine[1024];

	CFileMap* pDataMgr  = m_pOd->m_pDataMgr;
	time_t    tStart    = pLot->START_TIME;
	time_t    tEnd      = pLot->END_TIME  ;

	strData.clear();
	strLine =       "Revision            =  3"                                                  ; strData += strLine + STR_LF;
	sprintf(szLine, "Machine ID          =  %s", pDataMgr->GET_STATION()            ); strData += szLine  + STR_LF;
	strLine =       "EQP Start Time      =  "                                                   ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) );
		strData += szLine + STR_LF;
	}
	strLine =       "Board Start Time    =  "                                                   ; strData += strLine;
	{
//		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) );
		strData += szLine + STR_LF;
	}
	strLine =       "Board End Time      =  "                                                   ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tEnd  ), MM(tEnd  ), DD(tEnd  ), hh(tEnd  ), mm(tEnd  ) );
		strData += szLine + STR_LF;
	}
	strLine =       "ST1 Start Info      =  START"                                              ; strData += strLine + STR_LF;
	sprintf(szLine, "ST1 Lot ID CE1      =  %s", pLot->LOT_NO                                  ); strData += szLine  + STR_LF;
	sprintf(szLine, "ST1 Operator        =  %s", pLot->OP_ID                                   ); strData += szLine  + STR_LF;
	sprintf(szLine, "ST1 Lot Process     =  %s", LotProc()                                     ); strData += szLine  + STR_LF;
	sprintf(szLine, "ST1 Device Name     =  %s", pLot->PART_NO                                 ); strData += szLine  + STR_LF;
	sprintf(szLine, "ST1 Program Name    =  %s", pLot->TEST_PGM_NO                             ); strData += szLine  + STR_LF;
	sprintf(szLine, "ST1 Para            =  %d", GetPara( pLot )                               ); strData += szLine  + STR_LF;
	strLine =       "ST1 Status          =  NORMAL"                                             ; strData += strLine + STR_LF;
	sprintf(szLine, "ST1 Pgm Rev         =  %s", m_pOd->m_szVersion[pLot->m_uRS]               ); strData += szLine  + STR_LF;
	sprintf(szLine, "ST1 Comment         =  %s", m_pOd->m_szVersion[pLot->m_uRS]               ); strData += szLine  + STR_LF;
	sprintf(szLine, "ST1 Stack_No        =  %d/%d", iChip+1, nTotChip                          ); strData += szLine  + STR_LF;
	strLine =       "ST1 FW_Rev          =  "                                                   ; strData += strLine + STR_LF;
	strLine =       "ST1 C_Vender        =  "                                                   ; strData += strLine + STR_LF;
	strLine =       "DUT NO   NR          LOTID        N_ID N_WF N_X N_Y    C_ID C_WF C_X C_Y ICC1    ICC2    ICC3    ICC4B   ICC5B   ICC4A   ICC5A   NUL NUL NUL NUL NUL NUL NUL NUL NUL NUL GBB RealP/F BC    BIN CAT\n"; strData += strLine + STR_LF;
	strLine =       "=== ==== == ===================== ==== ==== === === ======= ==== === === ======= ======= ======= ======= ======= ======= ======= === === === === === === === === === === === ======= ===== === ===\n"; strData += strLine + STR_LF;

	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		return false;
	}

	return true;
}

bool CChdr::AppendChdr(T_LOT* pLot, uint iChip, uint nTotChip, int fd)
{
	if ( fd<0 ) return false;

	string strData;
	char   szLine[1024];


	strData.clear();
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
//		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

		uint nPara = m_pOd->m_nBibX[iSlot] * m_pOd->m_nBibY[iSlot];
		for ( uint c=0; c<m_pOd->m_nBibY[iSlot]; c++ )
		{
			for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
			{
				uint dut  = c*m_pOd->m_nBibX[iSlot] + r;
				uint nDut = iSlot*nPara + dut + 1;
//				if ( IsPreFail(iSlot, dut) || ! pLot->S[iSlot].sdut[dut] )
				if ( ! pLot->S[iSlot].IS_VALID() || IsPreFail(iSlot, dut) || ! pLot->S[iSlot].sdut[dut] )
				{
					sprintf(szLine, "DUT %4d NO %-21.21s %4s %4d %3d %3d     NUL  NUL NUL NUL NUL     NUL     NUL     NUL     NUL     NUL     NUL     NUL NUL NUL NUL NUL NUL NUL NUL NUL NUL %3d %c       %5d %3d %s",
							nDut, "LOTID", "0000", 0, 0, 0, 0, 'F', 0, 0, "");
					strData += szLine + STR_LF;
					continue;
				}

				bool bChipFail  = IsChipFail     (m_pOd->m_pCumFailChip[iSlot], iSlot, dut, iChip/m_pOd->m_nChipPerCe[iSlot], iChip%m_pOd->m_nChipPerCe[iSlot]);
				int iFailBlkCnt = GetFailBlockCnt(m_pOd->m_pCumFailChip[iSlot], iSlot, dut, iChip/m_pOd->m_nChipPerCe[iSlot], iChip%m_pOd->m_nChipPerCe[iSlot]);
				uint nBin = m_pOd->m_pSortBin[iSlot][dut]; //nBin = m_pOd->m_Binmap[iSlot][ m_pOd->m_pSortBin[iSlot][dut]%16 ];
				string strFcate;
				{
					strFcate.clear();
					std::vector<uint> vFCate;
					if ( GetFCate(m_pOd->m_pStepFCate[iSlot], iSlot, dut, vFCate, false) )
					{
						for( uint i=0; i<vFCate.size(); ++i )
						{
							uint fcate = vFCate[i];
							sprintf(szLine, "%3d  ", fcate ); strFcate += szLine;
						}
						vFCate.clear();
					}
				}

				stChipId* pstChipId = &(m_pOd->m_pstChipId[iSlot][dut*m_pOd->m_nChipPerDut[iSlot] + iChip]);

				//"              DUT NO  NR LOTID  N_ID N_WF N_X N_Y    C_ID C_WF C_X C_Y ICC1    ICC2    ICC3    ICC4B   ICC5B   ICC4A   ICC5A   NUL NUL NUL NUL NUL NUL NUL NUL NUL NUL GBB RealP/F BC BIN CAT"
				sprintf(szLine, "DUT %4d NO %-21.21s %4s %4x %3d %3d     NUL  NUL NUL NUL NUL     NUL     NUL     NUL     NUL     NUL     NUL     NUL NUL NUL NUL NUL NUL NUL NUL NUL NUL %3d %c       %5d %3d %s",
						nDut, pLot->LOT_NO,
						pstChipId->szchipId,
						pstChipId->uWaferId,
						pstChipId->uXPos   ,
						pstChipId->uYPos   ,
						iFailBlkCnt, (bChipFail ? 'F' : 'P'), iFailBlkCnt, nBin, strFcate.c_str());
				strData += szLine + STR_LF;
			} //End of r
		} //End of c
	} //End of slot

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

int CChdr::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CChdr::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CChdr::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CChdr::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CChdr::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CChdr::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}
