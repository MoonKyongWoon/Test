#include "itemcum.hpp"

CItemCum::CItemCum(COrganizedData* pOd) : m_pOd(pOd)
{
}

bool CItemCum::GenItemCum()
{
	bool bMake = true;
	if ( m_pOd->m_Lot.empty() ) bMake = false;

	for ( LOT_LIST::iterator itTotLot = m_pOd->m_Lot.begin(); itTotLot != m_pOd->m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		if ( ! CreateItemCumFile(pLot) )
			bMake = false;

		if ( ! m_strFile.empty() && m_pOd->m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("ITEMCUM") )
				UTIL::FTP::PUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFile.c_str(), "ascii" );
			m_strFile.clear();
		}
	}

	printf("[INFO] ITEM_CUM MAKE %s...\n", bMake?"OK":"NG" );
	return true;
}

bool CItemCum::GetFailBlockAddress( std::vector<uint>& vBlkAddr, char* pFailBlock, uint slot, uint dut, uint ce, uint chip, uint step )
{
	ssize_t nFailBlockSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nBlockPerDutBl[slot];
	uint iOffset = nFailBlockSizePerStep*step + dut*m_pOd->m_nBlockPerDutBl[slot];
	uint minaddr = ce * m_pOd->m_nBlockPerCe[slot] + chip * m_pOd->m_nBlockPerChip[slot];
	uint maxaddr = ce * m_pOd->m_nBlockPerCe[slot] + chip * m_pOd->m_nBlockPerChip[slot] + m_pOd->m_nBlockPerChip[slot] - 1;

	for(uint iBlock=minaddr; iBlock<=maxaddr; ++iBlock)
	{
		int m = iOffset + (iBlock>>3);
		if ( ! pFailBlock[m] )
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
			//printf("fail=%x\n", iBlock );
			vBlkAddr.push_back( iBlock );
		}
//		else
//			printf("pass=%x\n", iBlock );
	}
	return true;
}

bool CItemCum::IsBlockFail( char* pFailBlock, uint slot, uint dut, uint ce, uint chip, uint iBlkAddr, uint step )
{
	ssize_t nFailBlockSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nBlockPerDutBl[slot];
	uint iOffset = nFailBlockSizePerStep*step + dut*m_pOd->m_nBlockPerDutBl[slot];
	uint minaddr = ce * m_pOd->m_nBlockPerCe[slot] + chip * m_pOd->m_nBlockPerChip[slot];

	int m = iOffset + ((minaddr + iBlkAddr)>>3);
	int n = iBlkAddr&0x7;
	bool bFail = static_cast<bool>( (pFailBlock[m] >> n) & 1);

	return bFail;
}

bool CItemCum::IsStepFail(char* pStepFail, uint slot, uint dut, uint step )
{
	return static_cast<bool>( pStepFail[m_pOd->m_nMaxDut[slot]*step + dut] );
}

bool CItemCum::IsPreFail(uint iSlot, uint iDut)
{
	if ( m_pOd->m_pInitFail[iSlot]==nullptr ) return false;

	return static_cast<bool>( m_pOd->m_pInitFail[iSlot][iDut] );
}


bool CItemCum::IsChipFail( char* pFailChip, uint slot, uint dut, uint ce, uint chip, uint step/*=0*/ )
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nChipPerDutBl[slot];	//* sizeof(char)

	int chip_index = ce * m_pOd->m_nChipPerCe[slot] + chip;
	int m = nFailChipSizePerStep*step + dut * m_pOd->m_nChipPerDutBl[slot] + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;																	//chip_index % 8;

	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
}

int  CItemCum::GetStepFailMode(int* pStepFailMode, uint step )
{
	return pStepFailMode[step];
}

bool CItemCum::CreateItemCumFile(T_LOT* pLot)
{
	int fd = 0;
	char szDir [PATH_MAX];
	char szFile[PATH_MAX];
	char szPath[PATH_MAX];

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%d",
			  m_pOd->m_szReportRoot,
			  pLot->PART_NO        ,
			  pLot->LOT_NO         ,
			  pLot->SNO             );

	snprintf( szFile, PATH_MAX, "%s-%05d-%s_%s%s",
			  pLot->LOT_NO         ,
			  pLot->SNO            ,
			  pLot->PART_NO        ,
			  pLot->TEST_PGM_NO    ,
			  ITEMCUM_EXT           );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );

	m_sFile  = szPath;

	fd = open(szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
		printf("[ ERR] %s open error!\n", szPath );

	if ( ! MakeHeader(pLot, fd) )
	{
		close(fd);
		return false;
	}

	if ( ! AppendItemCum(pLot, fd) )
	{
		close(fd);
		return false;
	}

	close(fd);
	printf("%s  gerneration complet.\n", m_sFile.c_str() );

	m_strDir = szDir ; m_strFile = szFile;

	return true;
}

bool CItemCum::MakeHeader(T_LOT* pLot, int fd)
{
	if ( fd<0 ) return false;

	string strLine;
	string strData;
	char   szLine[1024];

	CFileMap* pDataMgr  = m_pOd->m_pDataMgr;
	time_t    tStart    = pLot->START_TIME;
	time_t    tEnd      = pLot->END_TIME  ;

	strData.clear();
	sprintf(szLine, "REV            :  %d", 1                               ); strData += szLine + STR_LF;
	sprintf(szLine, "PART NAME      :  %s", pLot->PART_NO                   ); strData += szLine + STR_LF;
	sprintf(szLine, "LOT NO         :  %s", pLot->LOT_NO                    ); strData += szLine + STR_LF;
	sprintf(szLine, "PGM NAME       :  %s", pLot->TEST_PGM_NO               ); strData += szLine + STR_LF;
	sprintf(szLine, "FW_REV         :  %s", ""                              ); strData += szLine + STR_LF;
	sprintf(szLine, "CAT_REV        :  %s", ""                              ); strData += szLine + STR_LF;
	sprintf(szLine, "REMARK         :  %s", m_pOd->m_szVersion[pLot->m_uRS] ); strData += szLine + STR_LF;
	sprintf(szLine, "C_Vendor       :  %s", ""                              ); strData += szLine + STR_LF;
	sprintf(szLine, "SYSTEM_NAME    :  %s", pDataMgr->GET_STATION()         ); strData += szLine + STR_LF;
	strLine =       "START TIME     :  "                                     ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) );
		strData += szLine + STR_LF;
	}

	strLine =       "END TIME       :  "                                     ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tEnd  ), MM(tEnd  ), DD(tEnd  ), hh(tEnd  ), mm(tEnd  ) );
		strData += szLine + STR_LF;
	}

	sprintf(szLine, "OPER           :  ", pLot->OPER                        ); strData += szLine + STR_LF;

	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		return false;
	}

	return true;
}

bool CItemCum::AppendItemCum(T_LOT* pLot, int fd)
{
	if ( fd<0 ) return false;

	string strData;
	string strLine;
	char   szLine[1024];

	strData.clear();
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

		sprintf(szLine, "Slot%02d.(0%s)" , iSlot+1, pLot->S[iSlot].BIB_ID                        ); strData += szLine  + STR_LF;
		strLine =       " Dut      CE_n   Chip_n"                                                 ; strData += strLine;
//		strLine =       "   Run_No Wafer_No    X-Pos    Y-Pos  CRun_No CWafer_No   CX-Pos  CY-Pos"; strData += strLine;
		if ( m_pOd->m_pstChipId[iSlot] != nullptr )
		{
			strLine =       "   Run_No Wafer_No    X-Pos    Y-Pos  CRun_No CWafer_No   CX-Pos  CY-Pos"; strData += strLine;
		}
		else
		{
			strLine =       "      "                                                                  ; strData += strLine;
		}
		strLine =       " Pass/Fail Cum "                                                             ; strData += strLine;

		for(uint iStep=1; iStep<=m_pOd->m_nMaxStep[iSlot]; ++iStep)
		{
			sprintf( szLine, " %s", m_pOd->m_vecStepData[iSlot][CPU00][iStep]->szStepName );
			strData += szLine;
		}
		strData += STR_LF;

		for ( uint c=0; c<m_pOd->m_nBibY[iSlot]; c++ )
		{
			for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
			{
				uint dut  = c*m_pOd->m_nBibX[iSlot] + r;
				if ( ! pLot->S[iSlot].sdut[dut] ) continue;
				if (   IsPreFail( iSlot, dut )  ) continue;

				for(uint iCe=0; iCe<m_pOd->m_nCePerDut[iSlot]; ++iCe)
				{
					for(uint iChip=0; iChip<m_pOd->m_nChipPerCe[iSlot]; ++iChip)
					{
						if ( m_pOd->m_pstChipId[iSlot] != nullptr )
						{
							stChipId* pstChipId = &(m_pOd->m_pstChipId[iSlot][dut*m_pOd->m_nChipPerDut[iSlot] + iChip]);

							sprintf(szLine, "%02d-%02d     %4d     %4d", r+1, c+1, iCe, iChip); strData += szLine;
							sprintf( szLine, " %8s       %02x       %02d       %02d %8s %8s %8s %8s",
										pstChipId->szchipId,
										pstChipId->uWaferId,
										pstChipId->uXPos   ,
										pstChipId->uYPos   ,
										 "NUL", "NUL", "NUL", "NUL"
								   );
							strData += szLine;
						}

						bool bChipFail = IsChipFail(m_pOd->m_pCumFailChip[iSlot], iSlot, dut, iCe, iChip);

						std::vector<uint> vBlkAddr;
						GetFailBlockAddress(vBlkAddr, m_pOd->m_pCumFailBlock[iSlot], iSlot, dut, iCe, iChip );
						int nFailBlk = (int)vBlkAddr.size();
						sprintf(szLine, " %8s %4d ", (bChipFail ? "FAIL" : (nFailBlk==0 ? "Prime" : "PASS ")), nFailBlk ); strData += szLine;

						uint iFailBlkCntCum = 0;
						for(uint iStep=1; iStep<=m_pOd->m_nMaxStep[iSlot]; ++iStep)
						{
							uint uStep = iStep-1;

							uint nStepNmLen = strlen( m_pOd->m_vecStepData[iSlot][CPU00][iStep]->szStepName ) + 1;
							bool bStepFail  = IsStepFail( m_pOd->m_pStepFail[iSlot], iSlot, dut, uStep );
							if ( bStepFail)
							{
								if ( GetStepFailMode( m_pOd->m_pStepFailMode[iSlot], uStep)==GRADE_TEST )
								{
									sprintf(szLine, "%*s", nStepNmLen, "GRF1" ); strData += szLine;
								}
								else
								{
									sprintf(szLine, "%*s", nStepNmLen, "FAIL" ); strData += szLine;
								}
								++iFailBlkCntCum;
								continue;
							}
							else
							{
								for(int iBlk=0; iBlk<nFailBlk; ++iBlk)
								{
									if ( IsBlockFail(m_pOd->m_pStepFailBlock[iSlot], iSlot, dut, iCe, iChip, vBlkAddr[iBlk], uStep) )
									{
										++iFailBlkCntCum;
										vBlkAddr.erase( vBlkAddr.begin() + iBlk );
										--iBlk;
										--nFailBlk;
	//									continue;
									}
								} //End of iBlock
							}
							sprintf(szLine, "%*d", nStepNmLen, iFailBlkCntCum ); strData += szLine;
						} //End of iStep

						strData += STR_LF;
					} //End of iChip
				} // End of iCe
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

int CItemCum::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CItemCum::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CItemCum::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CItemCum::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CItemCum::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CItemCum::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}
