#include "chipid.hpp"

CChipId::CChipId(COrganizedData* pOd) : m_pOd(pOd)
{
	m_strFiles = "";
}

bool CChipId::GenChipId()
{
	bool bMake = true;
	if ( m_pOd->m_Lot.empty() ) bMake = false;

	for ( LOT_LIST::iterator itTotLot = m_pOd->m_Lot.begin(); itTotLot != m_pOd->m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		uint nTotChip = m_pOd->m_nCePerDut[pLot->m_uRS] * m_pOd->m_nChipPerCe[pLot->m_uRS];
		for(uint iChip=0; iChip<nTotChip; ++iChip)
		{
			//iFileType==0 : cat, iFileType==1 : tbd, iFileType==2 : tbdh,
			for(uint iFileType=0; iFileType<3; ++iFileType)
			{
				if ( ! CreateChipIdFile(iFileType, pLot, iChip, nTotChip) )
					bMake = false;;
			}
		}

		if ( ! m_strFiles.empty() && m_pOd->m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("TDB") )
				UTIL::FTP::MPUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFiles.c_str(), "ascii" );
			m_strFiles.clear();
		}
	}

	printf("[INFO] CHIP_ID MAKE %s...\n", bMake?"OK":"NG" );
	return true;
}

bool CChipId::IsPreFail(uint iSlot, uint iDut)
{
	if ( m_pOd->m_pInitFail[iSlot]==nullptr ) return false;

	return static_cast<bool>( m_pOd->m_pInitFail[iSlot][iDut] );
}

bool CChipId::IsChipFail( char* pFailChip, uint slot, uint dut, uint ce, uint chip, uint step/*=0*/ )
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nChipPerDutBl[slot];	//* sizeof(char)

	int chip_index = ce * m_pOd->m_nChipPerCe[slot] + chip;
	int m = nFailChipSizePerStep*step + dut * m_pOd->m_nChipPerDutBl[slot] + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;																	//chip_index % 8;

	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
}

int CChipId::GetFailBlockCnt(char* pFailBlock, uint iSlot, uint dut, uint ce, uint chip, uint step/*=0*/ )
{
	ssize_t nFailBlockSizePerStep = m_pOd->m_nMaxDut[iSlot] * m_pOd->m_nBlockPerDutBl[iSlot];
	uint iOffset = nFailBlockSizePerStep*step + dut*m_pOd->m_nBlockPerDutBl[iSlot];
	uint minaddr = ce * m_pOd->m_nBlockPerCe[iSlot] + chip * m_pOd->m_nBlockPerChip[iSlot];
	uint maxaddr = ce * m_pOd->m_nBlockPerCe[iSlot] + chip * m_pOd->m_nBlockPerChip[iSlot] + m_pOd->m_nBlockPerChip[iSlot] - 1;
	uint iFailBlkCnt = 0;

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
			iFailBlkCnt ++;
			//printf("fail=%x\n", iBlock );
//			vBlkAddr.push_back( iBlock );
		}
//		else
//			printf("pass=%x\n", iBlock );
	}
	return iFailBlkCnt;
}

bool CChipId::GetFCate(uint* pStepFailCate, uint slot, uint dut, std::vector<uint>& vFCate, bool bWithStep/*=true*/)
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

bool CChipId::GetChipFailStep(char* pStepFailCate, uint slot, uint dut, uint ce, uint chip, std::vector<uint>& vStep )
{
	vStep.clear();
	for(uint iStep=1; iStep<=m_pOd->m_nMaxStep[slot]; ++iStep)
	{
		uint uStep = iStep-1;
		if ( IsChipFail( pStepFailCate, slot, dut, ce, chip, uStep) )
		{
			vStep.push_back( iStep );
		}
	} //End of iStep

	if ( vStep.empty() ) return false;

	return true;
}

bool CChipId::CreateChipIdFile(uint iFileType, T_LOT* pLot, uint iChip, uint nTotChip)
{
	int fd = 0;
	char szDir    [PATH_MAX];
	char szFile   [PATH_MAX];
	char szPath   [PATH_MAX];
	char szExt [3][PATH_MAX] = { CHIP_EXT0, CHIP_EXT1, CHIP_EXT2 };

	CFileMap* pDataMgr = m_pOd->m_pDataMgr;
	time_t    tStart   = pLot->START_TIME;
	time_t    tEnd     = pLot->END_TIME  ;

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%d",
			  m_pOd->m_szReportRoot   ,
			  pLot->PART_NO           ,
			  pLot->LOT_NO            ,
			  pLot->SNO                );

	snprintf( szFile, PATH_MAX, "%s_1_%04d%02d%02d%02d%02d_%s%s",
			  pDataMgr->GET_STATION() ,
			  YYYY(tEnd  )            ,
			  MM  (tEnd  )            ,
			  DD  (tEnd  )            ,
			  hh  (tStart)            ,
			  mm  (tStart) + iChip + 1,
			  pLot->LOT_NO            ,
			  szExt[iFileType]         );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );

	m_strDir = szDir ;
	m_sFile  = szPath;

	fd = open(szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
		printf("[ ERR] %s open error!\n", szPath );

	if ( ! MakeHeader(iFileType, pLot, iChip, nTotChip, fd) )
	{
		close(fd);
		return false;
	}

	if ( ! AppendChipId(iFileType, pLot, iChip, nTotChip, fd) )
	{
		close(fd);
		return false;
	}

	close(fd);
	printf("%s  gerneration complet.\n", m_sFile.c_str() );
	m_strFiles += szFile + string(" ");

	return true;
}

bool CChipId::MakeHeader(uint iFileType, T_LOT* pLot, uint iChip, uint nTotChip, int fd)
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

	sprintf(szLine, "OPER           :  %s", pLot->OPER                       ); strData += szLine  + STR_LF;
	sprintf(szLine, "STACK_NO       :  %d / %d", iChip+1, nTotChip           ); strData += szLine  + STR_LF;
	sprintf(szLine, "DEVICE         :  %s", pLot->DEVICE_NO                  ); strData += szLine  + STR_LF;
	sprintf(szLine, "FACTORY        :  %s", pLot->FACTORY                    ); strData += szLine + STR_LF;
	sprintf(szLine, "Fab            :  %s", pLot->FAB                        ); strData += szLine + STR_LF;
	sprintf(szLine, "Owner          :  %s", pLot->OWNER                      ); strData += szLine + STR_LF;
	sprintf(szLine, "Grade          :  %s", pLot->GRADE                      ); strData += szLine + STR_LF;

	if ( iFileType==1 || iFileType==2 )	//tbd, tbdh
	{
		sprintf(szLine, "Rwk Count      :  %d", pLot->RWK_COUNT              ); strData += szLine  + STR_LF;
		sprintf(szLine, "Rwk Code       :  %s", pLot->RWK_CODE               ); strData += szLine  + STR_LF;
		sprintf(szLine, "MPG_FLAG       :  %c", pLot->MPG_FLAG               ); strData += szLine  + STR_LF;
	}

	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		return false;
	}

	return true;
}

bool CChipId::AppendChipId(uint iFileType, T_LOT* pLot, uint iChip, uint nTotChip, int fd)
{
	if ( fd<0 ) return false;

	string strData;
	char   szLine[1024];


	strData.clear();
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

		sprintf(szLine, "Slot No        :  %d"   , iSlot+1               ); strData += szLine  + STR_LF;
		sprintf(szLine, "BIB ID         :  0%-8s", pLot->S[iSlot].BIB_ID ); strData += szLine  + STR_LF;
		for ( uint c=0; c<m_pOd->m_nBibY[iSlot]; c++ )
		{
			for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
			{
				uint dut = c*m_pOd->m_nBibX[iSlot] + r;
				if ( ! pLot->S[iSlot].sdut[dut] ) continue;
				if (   IsPreFail( iSlot, dut )  ) continue;

				sprintf(szLine, " %2d - %2d - %2d  :  ", iSlot+1, c+1, r+1   ); strData += szLine;
				if ( m_pOd->m_pstChipId[iSlot] != nullptr )
				{
					stChipId* pstChipId = &(m_pOd->m_pstChipId[iSlot][dut*m_pOd->m_nChipPerDut[iSlot] + iChip]);

					sprintf( szLine, "%4s    %02x    %02d    %02d    %4s   %4s   %4s   %4s",
							 pstChipId->szchipId,
							 pstChipId->uWaferId,
							 pstChipId->uXPos   ,
							 pstChipId->uYPos   ,
							 "NUL", "NUL", "NUL", "NUL"
						   );
					strData += szLine;
				}

				bool bChipFail  = IsChipFail     (m_pOd->m_pCumFailChip[iSlot], iSlot, dut, iChip/m_pOd->m_nChipPerCe[iSlot], iChip%m_pOd->m_nChipPerCe[iSlot]);
				int iFailBlkCnt = GetFailBlockCnt(m_pOd->m_pCumFailChip[iSlot], iSlot, dut, iChip/m_pOd->m_nChipPerCe[iSlot], iChip%m_pOd->m_nChipPerCe[iSlot]);

				uint nBin = m_pOd->m_pSortBin[iSlot][dut]; //( iFileType==0 || iFileType==2 )
				if ( iFileType==1 ) nBin = m_pOd->m_Binmap[iSlot][ m_pOd->m_pSortBin[iSlot][dut] ];

				sprintf(szLine, " %4c %4x %4d  ", (bChipFail ? 'F' : 'P'), nBin, iFailBlkCnt ); strData += szLine;

				if ( iFileType==0 )							//cat
				{
					std::vector<uint> vFCate;
					if ( GetFCate(m_pOd->m_pStepFCate[iSlot], iSlot, dut, vFCate) )
					{
						for( uint i=0; i<vFCate.size(); ++i )
						{
							uint fcate = vFCate[i];
							sprintf(szLine, "%3d(%3d)  ", fcate>>16, fcate&0xffff ); strData += szLine;
						}
						vFCate.clear();
					}
				}
				else if ( iFileType==1 || iFileType==2 )	//tbd, tbdh
				{
					std::vector<uint> vStep;
					if ( GetChipFailStep(m_pOd->m_pStepFailChip[iSlot], iSlot, dut, iChip/m_pOd->m_nChipPerCe[iSlot], iChip%m_pOd->m_nChipPerCe[iSlot], vStep) )
					{
						for( uint i=0; i<vStep.size(); ++i )
						{
							uint iStep = vStep[i];
							sprintf(szLine, "%3d  ", iStep ); strData += szLine;
						}
						vStep.clear();
					}
				}
				strData += STR_LF;
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

int CChipId::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CChipId::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CChipId::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CChipId::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CChipId::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CChipId::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}

