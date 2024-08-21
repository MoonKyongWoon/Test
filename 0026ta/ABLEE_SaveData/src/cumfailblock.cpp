#include "cumfailblock.hpp"

CCumFailBlock::CCumFailBlock(COrganizedData* pOd) : m_pOd(pOd)
{
	m_strFile = "";
}

bool CCumFailBlock::GenCumFailBlock()
{
	bool bMake = false;
	for ( LOT_LIST::iterator itTotLot = m_pOd->m_Lot.begin(); itTotLot != m_pOd->m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		if( GenFileName( pLot ) )
		{
			bMake = true;
			MakeHeader     ( pLot, bMake);
			MakeChipCumData( pLot, bMake );
			MakeStepData   ( pLot, bMake );
		}
		close(m_fd);
		printf("%s  gerneration complet.\n", m_sFile.c_str() );

		if ( ! m_strFile.empty() && m_pOd->m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("TDBCUM") )
				UTIL::FTP::PUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFile.c_str(), "ascii" );
			m_strFile.clear();
		}
	}
	printf("[INFO] CUMBLOCKFAIL MAKE %s...\n", bMake?"OK":"NG" );
	return true;
}

bool CCumFailBlock::GenFileName(T_LOT* pLot)
{
	char szDir   [PATH_MAX];
	char szFile  [PATH_MAX];
	char szPath  [PATH_MAX];

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%d",
			  m_pOd->m_szReportRoot,
			  pLot->PART_NO        ,
			  pLot->LOT_NO         ,
			  pLot->SNO             );

	snprintf( szFile, PATH_MAX, "%s-%05d-%s-%s%s",
			  pLot->LOT_NO         ,
			  pLot->SNO            ,
			  pLot->PART_NO        ,
			  pLot->TEST_PGM_NO    ,
			  CUM_EXT               );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );

	m_fd = open( szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if( m_fd == -1 )
	{
		printf("[ ERR] %s open error!\n", szPath );
		return false;
	}
	//printf("[INFO] %s start...\n", szPath );
	m_strDir = szDir ; m_strFile = szFile;
	m_sFile  = szPath;
	return true;
}

bool CCumFailBlock::MakeHeader(T_LOT* pLot, bool& rMake )
{
	string strLine;
	string strData;
	char   szLine[1024];

	CFileMap* pDataMgr  = m_pOd->m_pDataMgr;
	time_t    tStart    = pLot->START_TIME;
	time_t    tEnd      = pLot->END_TIME  ;

	strData.clear();
	strLine =       "REV            :  1"                                     ; strData += strLine + STR_LF;
	sprintf(szLine, "PART NAME      :  %s", pLot->PART_NO                    ); strData += szLine  + STR_LF;
	sprintf(szLine, "LOT NO         :  %s", pLot->LOT_NO                     ); strData += szLine  + STR_LF;
	sprintf(szLine, "PGM NAME       :  %s", pLot->TEST_PGM_NO                ); strData += szLine  + STR_LF;
	sprintf(szLine, "FW_REV         :  %s", ""                               ); strData += szLine  + STR_LF;
	sprintf(szLine, "CAT_REV        :  %s", ""                               ); strData += szLine  + STR_LF;
	sprintf(szLine, "REMARK         :  %s", m_pOd->m_szVersion[pLot->m_uRS]  ); strData += szLine  + STR_LF;
	sprintf(szLine, "C_Vendor       :  %s", ""                               ); strData += szLine  + STR_LF;
	sprintf(szLine, "SYSTEM_NAME    :  %s", pDataMgr->GET_STATION()          ); strData += szLine  + STR_LF;
	strLine =       "START TIME     :  "                                      ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) );
		strData += szLine + STR_LF;
	}

	strLine =       "END TIME       :  "                                      ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tEnd  ), MM(tEnd  ), DD(tEnd  ), hh(tEnd  ), mm(tEnd  ) );
		strData += szLine + STR_LF;
	}
	sprintf(szLine, "OPER           :  %s", pLot->OPER                       ); strData += szLine + STR_LF;
	sprintf(szLine, "DEVICE         :  %s", pLot->DEVICE_NO                  ); strData += szLine + STR_LF;
	sprintf(szLine, "FACTORY        :  %s", pLot->FACTORY                    ); strData += szLine + STR_LF;
	sprintf(szLine, "Fab            :  %s", pLot->FAB                        ); strData += szLine + STR_LF;
	sprintf(szLine, "Owner          :  %s", pLot->OWNER                      ); strData += szLine + STR_LF;
	sprintf(szLine, "Grade          :  %s", pLot->GRADE                      ); strData += szLine + STR_LF;
	sprintf(szLine, "MPG_FLAG       :  %c", pLot->MPG_FLAG                   ); strData += szLine + STR_LF;
	sprintf(szLine, "CHIP_PER_CE    :  %d", m_pOd->m_nChipPerCe[pLot->m_uRS] ); strData += szLine + STR_LF;

	uint nLen = strData.length();
	ssize_t written = write( m_fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		rMake = false;
		return rMake;
	}

	return rMake;
}

bool CCumFailBlock::MakeChipCumData( T_LOT* pLot, bool& rMake )
{
	if( !rMake ) return false;

	string strLine = "";
	string strData = "";
	char   szLine[1024];

	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

		uint nCePerDut   = m_pOd->m_stDevPgmVar[iSlot][CPU00].nCe;
		uint nChipPerCe  = m_pOd->m_nChipPerCe [iSlot];
		uint nChipPerDut = m_pOd->m_nChipPerDut[iSlot];

		for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
		{
			for ( uint c=0; c<m_pOd->m_nBibY[iSlot]; c++ )
			{
				uint dut = c*m_pOd->m_nBibX[iSlot] + r;
				if ( IsPreFail( iSlot, dut )    ) continue;
				if ( ! pLot->S[iSlot].sdut[dut] ) continue;

				for(uint iCe=0; iCe<nCePerDut; ++iCe)
				{
					for(uint iChip=0; iChip<nChipPerCe; ++iChip)
					{
						strData.clear();
						sprintf( szLine, "Slot%02d.(0%s)R%02d, C%02d, CE%02d, CHIP%02d",
								 iSlot+1, pLot->S[iSlot].BIB_ID, r+1, c+1, iCe+1, iChip+1 );
						strData += szLine;

						if ( m_pOd->m_pstChipId[iSlot] != nullptr )
						{
							stChipId* pstChipId = &(m_pOd->m_pstChipId[iSlot][dut*nChipPerDut + iCe*nChipPerCe + iChip]);
							sprintf( szLine, "     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL",
										pstChipId->szchipId,
										pstChipId->uWaferId,
										pstChipId->uXPos   ,
										pstChipId->uYPos
								   );
							strData += szLine;
						}
						strData += STR_LF;

						sprintf( szLine, "Cum(%s/%s)                     ",
								 (IsChipFail(m_pOd->m_pCumFailChip[iSlot], iSlot, dut            )?"FAIL":"PASS"),
								 (IsChipFail(m_pOd->m_pCumFailChip[iSlot], iSlot, dut, iCe, iChip)?"FAIL":"PASS")  );
						strData += szLine;
						std::vector<uint> vBlkAddr;
						GetFailBlockAddress(vBlkAddr, m_pOd->m_pCumFailBlock[iSlot], iSlot, dut, iCe, iChip );
						sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
						strData += szLine;
						int nFailBlk = (int)vBlkAddr.size();
						if ( nFailBlk > 254 ) nFailBlk = 254;
						for(int i=0; i<nFailBlk; ++i)
						{
							sprintf( szLine, " %x ", vBlkAddr[i] );
							strData += szLine;
						}
						strData += STR_LF;

						//step loop
						std::vector<uint> vCumBlkAddr;
						vCumBlkAddr.clear();
						//printf("0.MaxStep:%d\n",m_nMaxStep);
						for(uint iStep=1; iStep<=m_pOd->m_nMaxStep[iSlot]; ++iStep)
						{
							uint iPos = iStep-1;
							char szStepName[MAX_TEXT_VALUE]={0x0,};
							sprintf( szStepName, "%02d.%s%s", iStep, m_pOd->m_vecStepData[iSlot][CPU00][iStep]->szStepName, "(N)" );
							sprintf( szLine, "%-35s", szStepName );
							strData += szLine;

							vBlkAddr.clear();
							GetFailBlockAddress(vBlkAddr, m_pOd->m_pStepFailBlock[iSlot], iSlot, dut, iCe, iChip, iPos);

							if ( false && iSlot==0 && r==12 && c==9 && iChip==0 && iPos==25 )
							{
								bool bF = IsChipFail(m_pOd->m_pStepFailChip[iSlot], iSlot, dut, iCe, iChip, iPos);
								printf("vBlkAddr.size()=%d  StepChipFail=%s   UseMode=%x\n", vBlkAddr.size(), (bF?"T":"F"), m_pOd->m_pStepUseMode[iSlot][iPos]  );
							}

							if ( vBlkAddr.size()<1 && IsChipFail(m_pOd->m_pStepFailChip[iSlot], iSlot, dut, iCe, iChip, iPos) )
							{
								if ( (m_pOd->m_pStepUseMode[iSlot][iPos])&0x2 )
								{
									sprintf( szLine, "%dEA", 0 );
									strData += szLine + STR_LF;
								}
								else
								{
									sprintf( szLine, "%dEA FAIL ", 1 );
									strData += szLine + STR_LF;
								}

//								if ( iStep<18 || iStep>327)
//								{
//									sprintf( szLine, "%dEA FAIL ", 1 );
//									strData += szLine + STR_LF;
//								}
//								else
//								{
//									if ( iStep>20 && iStep<77)
//									{
//										sprintf( szLine, "%dEA FAIL ", 1 );
//										strData += szLine + STR_LF;
//									}
//									else
//									{
//										sprintf( szLine, "%dEA", 0 );
//										strData += szLine + STR_LF;
//									}
//								}
							}
							else
							{
								sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
								strData += szLine;
								nFailBlk = (int)vBlkAddr.size();
								if ( nFailBlk > 253 ) nFailBlk = 253;
								for(int i=0; i<nFailBlk; ++i)
								{
									sprintf( szLine, " %x%c", vBlkAddr[i], !IsIn(vCumBlkAddr, vBlkAddr[i])?'!':' ' );
									strData += szLine;
								}
								if ( vBlkAddr.size() > 253 ) strData += " ...";
								strData += STR_LF;
							}
							mergeFailBlockInfo(vCumBlkAddr, vBlkAddr);
						}
						uint strSize = strData.length();
						ssize_t written = write( m_fd, strData.c_str(), strSize );
						if ( written != strSize )
						{
							printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
							rMake = false;
							return rMake;
						}
					} //End of iChip
				} //End of iCe
			} //End of c
		} //End of r
	} //End of iSlot

	return rMake;
}

bool CCumFailBlock::MakeStepData( T_LOT* pLot, bool& rMake )
{
	if( !rMake ) return false;

	string strData = "";
	char   szLine[1024];

	strData.clear();
	char szBegin[ 32] = {0x0,};
	char szEnd  [ 32] = {0x0,};
	char szTmp  [256] = {0x0,};

	uint rSlot = pLot->m_uRS;
	uint rCpu  = pLot->m_uRC;
	uint nStepCount = m_pOd->m_nMaxStep[rSlot];

	std::vector<CStepData*> &vStepData = m_pOd->m_vecStepData[rSlot][rCpu];
	if( vStepData.size() < nStepCount ) nStepCount = vStepData.size();

	for(uint iStep=1; iStep<=nStepCount; ++iStep)
	{
		uint iPos = iStep;

		time_t tStepS = vStepData[iPos]->tStepBegin;
		time_t tStepE = vStepData[iPos]->tStepEnd;

		sprintf( szBegin, "%04d/%02d/%02d %02d:%02d:%02d", YYYY(tStepS), MM(tStepS), DD(tStepS), hh(tStepS), mm(tStepS), ss(tStepS) );
		sprintf( szEnd  , "%04d/%02d/%02d %02d:%02d:%02d", YYYY(tStepE), MM(tStepE), DD(tStepE), hh(tStepE), mm(tStepE), ss(tStepE) );
		sprintf( szTmp  , "%02d.%-s"     , iStep, vStepData[iPos]->szStepName );
		sprintf( szLine , "%-35s:%s - %s", szTmp, szBegin, szEnd );
		strData += szLine + STR_LF;
	}//End of iStep

	uint strSize = strData.length();
	ssize_t written = write( m_fd, strData.c_str(), strSize );
	if ( written != strSize )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		rMake = false;
		return rMake;
	}

	return rMake;
}

bool CCumFailBlock::IsChipFail( char* pFailChip, uint slot, uint dut, uint ce, uint chip, uint step )
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nChipPerDutBl[slot];	//* sizeof(char)

	int chip_index = ce * m_pOd->m_nChipPerCe[slot] + chip;
	int m = nFailChipSizePerStep*step + dut * m_pOd->m_nChipPerDutBl[slot] + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
	int n = chip_index & 0x7;																	//chip_index % 8;

	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
}

bool CCumFailBlock::IsChipFail(char* pFailChip, uint slot, uint dut, uint step)
{
	if ( pFailChip==nullptr ) return false;

	ssize_t nFailChipSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nChipPerDutBl[slot];

	bool bFail = false;
	for(uint iChip=0; iChip<m_pOd->m_nCePerDut[slot]*m_pOd->m_nChipPerCe[slot]; iChip+=8)
	{
		int chip_index = iChip;
		int m = nFailChipSizePerStep*step + dut * m_pOd->m_nChipPerDutBl[slot] + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
		if ( pFailChip[m] )
		{
			bFail = true;
			break;
		}
	}
	return bFail;
}

bool CCumFailBlock::GetFailBlockAddress( std::vector<uint>& vBlkAddr, char* pFailBlock, uint slot, uint dut, uint ce, uint chip, uint step/*=0*/ )
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

bool CCumFailBlock::IsPreFail(uint iSlot, uint iDut)
{
	if ( m_pOd->m_pInitFail[iSlot]==nullptr ) return false;

	return static_cast<bool>( m_pOd->m_pInitFail[iSlot][iDut] );
}

void CCumFailBlock::mergeFailBlockInfo(std::vector<uint> &vCumBlkAddr, std::vector<uint> &vBlkAddr)
{
	std::vector<uint> vNewBlkAddr;
	for(int i=0; i<(int)vBlkAddr.size(); ++i)
	{
		if ( ! IsIn(vCumBlkAddr, vBlkAddr[i]) )
			vNewBlkAddr.push_back( vBlkAddr[i] );
	}

	for(int i=0; i<(int)vNewBlkAddr.size(); ++i)
		vCumBlkAddr.push_back( vNewBlkAddr[i] );
}

bool CCumFailBlock::IsIn(std::vector<uint> &vCumBlkAddr, uint iBlkAddr)
{
	for(int i=0; i<(int)vCumBlkAddr.size(); ++i)
	{
		if ( vCumBlkAddr[i]==iBlkAddr ) return true;
	}

	return false;
}

int CCumFailBlock::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CCumFailBlock::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CCumFailBlock::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CCumFailBlock::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CCumFailBlock::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CCumFailBlock::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}
/////////////////////////////////////////////////////////////////////////////////////////

//CCumFailBlock::CCumFailBlock(CFileMap* p):m_pDataMgr(p)
//{
//	m_bCpu2X = true;
//	for( int s=0;s<MAX_SLOT;s++ )
//	{
//		m_pstChipId     [s] = nullptr;
//		m_pCumFailChip  [s] = nullptr;
//		m_pStepFailChip [s] = nullptr;
//		m_pCumFailBlock [s] = nullptr;
//		m_pStepFailBlock[s] = nullptr;
//
//		m_pStepInInfo [s][CPU00] = nullptr;
//		m_pStepOutInfo[s][CPU00] = nullptr;
//		m_pStepInInfo [s][CPU01] = nullptr;
//		m_pStepOutInfo[s][CPU01] = nullptr;
//		m_vecStepData [s][CPU00].clear();
//		m_vecStepData [s][CPU01].clear();
//	}
//}
//
//bool CCumFailBlock::GenCumFileName(uint rSlot)
//{
//	char szFileName[PATH_MAX]={0x0,};
//
//	snprintf( szFileName, PATH_MAX, "%s/%s-%05d-%s-%s%s",
//			  m_szReportPath, m_pDataMgr->GET_LOTNO(), 0x0, m_pDataMgr->GET_PARTNO(), m_pDataMgr->GET_DEVICE(rSlot,CPU00), CUM_EXT);
//	m_CumFd = open( szFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
//	if( m_CumFd == -1 )
//	{
//		printf("[ ERR] %s open error!\n", szFileName );
//		return false;
//	}
//	//printf("[INFO] %s start...\n", szFileName );
//	m_CumFile = szFileName;
//	return true;
//}
//
//bool CCumFailBlock::MakePreCumData(uint rSlot)
//{
//	m_nBibX = m_pDataMgr->GET_BIBX(rSlot,CPU00);
//	m_nBibY = m_pDataMgr->GET_BIBY(rSlot,CPU00);
//	m_nMaxDutPerCpu = m_pDataMgr->GET_MAXDUTPERCPU( rSlot, CPU00 );
//	m_nMaxChipPerDut= m_pDataMgr->GET_CHIPPERDUT( rSlot, CPU00 );
//	m_nCePerDut     = m_pDataMgr->GET_CE_COUNT( rSlot, CPU00 );
//	m_nChipPerCe    = m_pDataMgr->GET_CHIP_COUNT( rSlot, CPU00 );
//	m_nChipPerDutBl = m_pDataMgr->GET_CHIPPERDUTBL( rSlot, CPU00 );
//	m_nBlockPerDut  = m_pDataMgr->GET_BLOCKPERDUT( rSlot, CPU00 );
//	m_nBlockPerDutBl= m_pDataMgr->GET_BLOCKPERDUTBL( rSlot, CPU00 );
//	m_nBlockPerCe   = m_pDataMgr->GET_BLOCKPERCE( rSlot, CPU00 );
//	m_nBlockPerChip = m_pDataMgr->GET_BLOCKPERCHIP( rSlot, CPU00 );
//
//	m_nChipPerDut   = m_nCePerDut * m_nChipPerCe;
//	m_nMaxDut = m_nBibX * m_nBibY;
//
//	if( 0 )
//	{
//		printf( " SLOT#%02d\n", rSlot+1 );
//		printf( " ->PRE_CUM BIB X:%02d\n", m_nBibX );
//		printf( " ->PRE_CUM BIB Y:%02d\n", m_nBibY );
//		printf( " ->PRE_CUM DUT PER CPU:%d\n", m_nMaxDutPerCpu );
//		printf( " ->PRE_CUM DUT MAX:%d\n", m_nMaxDut );
//		printf( " ->PRE_CUM BLOCK PER DUT:%d\n", m_nBlockPerDut );
//		printf( " ->PRE_CUM BLOCK PER DUTBL:%d\n", m_nBlockPerDutBl );
//		printf( " ->PRE_CUM CE PER DUT:%d\n", m_nCePerDut );
//		printf( " ->PRE_CUM CHIP PER CE:%d\n", m_nChipPerCe );
//		printf( " ->PRE_CUM CHIP PER DUT:%d\n", m_nChipPerDut );
//		printf( " ->PRE_CUM CHIP PER DUT BL:%d\n", m_nChipPerDutBl );
//		printf( " ->PRE_CUM BLOCK PER CE:%d\n", m_nBlockPerCe );
//		printf( " ->PRE_CUM BLOCK PER CHIP:%d\n", m_nBlockPerChip );
//	}
//
//	for( int slot=0;slot<MAX_SLOT;slot++ )
//	{
//		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//			continue;
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// INIT FAIL
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		stInitTestHead initHead[m_nMaxCpu];
//		m_pInitFail[slot] = new char[ m_nMaxDut ];
//		if ( m_pInitFail[slot] == nullptr )
//		{
//			printf( "[ ERR] S#%02d FLS Memory(Init Fail) alloc error\n", slot+1 );
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
//		//if( 0 )
//		//{
//		//	struct tm* tBegin;
//		//	struct tm* tEnd;
//		//	tBegin = localtime( m_pDataMgr->GET_CYCLE_BEGIN_TIME() );
//		//	tEnd   = localtime( m_pDataMgr->GET_CYCLE_END_TIME() );
//		//	printf( "   ->BEGIN: %04d/%02d/%02d %02d:%02d:%02d\n", tBegin->tm_year+1900, tBegin->tm_mon+1, tBegin->tm_mday, tBegin->tm_hour, tBegin->tm_min, tBegin->tm_sec );
//		//	printf( "   ->END  : %04d/%02d/%02d %02d:%02d:%02d\n", tEnd->tm_year+1900, tEnd->tm_mon+1, tEnd->tm_mday, tEnd->tm_hour, tEnd->tm_min, tEnd->tm_sec );
//		//	printf( "   ->STEP NO : %d\n", m_initTestHead[slot][CPU00].nStepNo );
//		//	printf( "   ->STEP NAME: %s\n", m_initTestHead[slot][CPU00].szStepName );
//		//}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// STEP INFO(COUNT/BEGIN/END/TEMP/BITIME...)
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		int nMaxStep0 = m_pDataMgr->GET_MAX_STEP( slot, CPU00 );
//		int nMaxStep1 = m_pDataMgr->GET_MAX_STEP( slot, CPU01 );
//		if( nMaxStep0 != nMaxStep1 )
//		{
//			fprintf( stdout, "[ ERR] S#%02d CPU0 MaxStep[%d], CPU1 MaxStep[%d] is different\n",
//				   slot, nMaxStep0, nMaxStep1 );
//			return false;
//		}
//		m_nMaxStep = nMaxStep0;
//		//printf( "   ->MAX STEP : %d\n", m_nMaxStep );
//		if( m_nMaxStep < 1 )
//		{
//			fprintf( stdout, "[ ERR] S#%02d MaxStep is less than 1\n", slot );
//			return false;
//		}
//		m_pStepInInfo [slot][CPU00] = new stStepInInfo [m_nMaxStep];
//		m_pStepInInfo [slot][CPU01] = new stStepInInfo [m_nMaxStep];
//		m_pStepOutInfo[slot][CPU00] = new stStepOutInfo[m_nMaxStep];
//		m_pStepOutInfo[slot][CPU01] = new stStepOutInfo[m_nMaxStep];
//
//		if( !m_pDataMgr->GET_STEPIN_INFO(slot, CPU00, m_pStepInInfo[slot][CPU00], m_nMaxStep ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_STEPIN_INFO(slot, CPU01, m_pStepInInfo[slot][CPU01], m_nMaxStep ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_STEPOUT_INFO(slot, CPU00, m_pStepOutInfo[slot][CPU00], m_nMaxStep ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		if( !m_pDataMgr->GET_STEPOUT_INFO(slot, CPU01, m_pStepOutInfo[slot][CPU01], m_nMaxStep ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//		m_vecStepData[slot][CPU00].clear();
//		m_vecStepData[slot][CPU01].clear();
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
//					fprintf( stdout, "   ->STEP.%03d : %s\n", pStepData->iTsNo, pStepData->szStepName );
//					fprintf( stdout, "   ->STEP.%03d BEGIN: %04d/%02d/%02d %02d:%02d:%02d\n",
//							pStepData->iTsNo, tBegin->tm_year+1900, tBegin->tm_mon+1, tBegin->tm_mday, tBegin->tm_hour, tBegin->tm_min, tBegin->tm_sec );
//					fprintf( stdout, "   ->STEP.%03d END  : %04d/%02d/%02d %02d:%02d:%02d\n",
//							pStepData->iTsNo, tEnd->tm_year+1900, tEnd->tm_mon+1, tEnd->tm_mday, tEnd->tm_hour, tEnd->tm_min, tEnd->tm_sec );
//				}
//				m_vecStepData[slot][c].push_back( pStepData );
//			}
//			nStartTs++;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// CHIP ID DATA
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		uint nChipDataSize  = m_nMaxDut * m_nChipPerDut;
//		uint nDutPerCpuSize = m_nMaxDutPerCpu * m_nChipPerDut;
//		uint nChipIdSize    = sizeof(stChipId);
//		m_pstChipId[slot] = new (std::nothrow)stChipId[ nChipDataSize ];
//		if( !m_pDataMgr->GET_CHIPID_DATA(slot, CPU00, (char*)m_pstChipId[slot], nDutPerCpuSize, nChipIdSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//		if( !m_pDataMgr->GET_CHIPID_DATA(slot, CPU01, (char*)(m_pstChipId[slot]+nDutPerCpuSize), nDutPerCpuSize, nChipIdSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// CUM FAIL CHIP DATA
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		nChipDataSize  = m_nMaxDut * m_nChipPerDutBl;
//		nDutPerCpuSize = m_nMaxDutPerCpu * m_nChipPerDutBl;
//		m_pCumFailChip[slot] = new (std::nothrow)char[ nChipDataSize ];
//		if( !m_pDataMgr->GET_CUMFAIL_CHIP_DATA(slot, CPU00, m_pCumFailChip[slot], nDutPerCpuSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//		if( !m_pDataMgr->GET_CUMFAIL_CHIP_DATA(slot, CPU01, m_pCumFailChip[slot]+nDutPerCpuSize, nDutPerCpuSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// STEP FAIL CHIP DATA
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		uint nStepChipDataSize  = m_nMaxDut * m_nChipPerDutBl * m_nMaxStep;
//		nChipDataSize  = m_nMaxDutPerCpu * m_nChipPerDutBl;
//		m_pStepFailChip[slot] = new (std::nothrow)char[ nStepChipDataSize ];
//		if( !m_pDataMgr->GET_STEPFAIL_CHIP_DATA(slot, CPU00, m_pStepFailChip[slot], m_nMaxStep, nChipDataSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//		if( !m_pDataMgr->GET_STEPFAIL_CHIP_DATA(slot, CPU01, m_pStepFailChip[slot], m_nMaxStep, nChipDataSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// CUM FAIL BLOCK DATA
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		uint nCumBlockSize       = m_nMaxDut * m_nBlockPerDutBl;
//		uint nCumBlockSizePerCpu = m_nMaxDutPerCpu * m_nBlockPerDutBl;
//		m_pCumFailBlock[slot] = new (std::nothrow)char[ nCumBlockSize ];
//		if( !m_pDataMgr->GET_CUMFAIL_BLOCK_DATA(slot, CPU00, m_pCumFailBlock[slot], nCumBlockSizePerCpu ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//		if( !m_pDataMgr->GET_CUMFAIL_BLOCK_DATA(slot, CPU01, m_pCumFailBlock[slot]+nCumBlockSizePerCpu, nCumBlockSizePerCpu ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/// FAIL BLOCK DATA
//		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		uint nMaxStepBlockSize       = m_nMaxDut * m_nBlockPerDutBl * m_nMaxStep;
//		nCumBlockSize                = m_nMaxDutPerCpu * m_nBlockPerDutBl;
//		m_pStepFailBlock[slot] = new (std::nothrow)char[ nMaxStepBlockSize ];
//		if( !m_pDataMgr->GET_STEPFAIL_BLOCK_DATA(slot, CPU00, m_pStepFailBlock[slot], m_nMaxStep, nCumBlockSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU00, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//		if( !m_pDataMgr->GET_STEPFAIL_BLOCK_DATA(slot, CPU01, m_pStepFailBlock[slot], m_nMaxStep, nCumBlockSize ) )
//		{
//			fprintf( stdout, "[ ERR] S#%02d-C#%02d : %s\n", slot, CPU01, m_pDataMgr->GET_ERRMSG() );
//			return false;
//		}
//	}
//	return true;
//}
//
//bool CCumFailBlock::GenCumFailBlock(const char* szReportPath)
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
//	fprintf( stdout, "[INFO] %s Last Test End Slot/Cpu : S#%02d-C#%02d\n", __FUNCTION__, uRefSlot, uRefCpu );
//
//	m_nMaxCpu = 1;
//	m_bCpu2X  = m_pDataMgr->GET_CPU2X(uRefSlot,uRefCpu);
//	if( m_bCpu2X ) m_nMaxCpu = 2;
//
//	uint step_count = m_pDataMgr->GET_MAX_STEP(uRefSlot, uRefCpu);	//STEPOUT
//	if( step_count < 1 )
//	{
//		fprintf( stdout, "[ ERR] S#%02d-C#%02d %s Step cout is less than 1", uRefSlot+1, CPU00+1, __FUNCTION__ );
//		return false;
//	}
//
//	bool bMake = false;
//	if( GenCumFileName( uRefSlot ) )
//	{
//		if( MakePreCumData( uRefSlot ) )
//		{
//			bMake = true;
//			MakeHeader( uRefSlot, uRefCpu, bMake);
//			MakeChipCumData ( uRefSlot, uRefCpu, bMake );
//			MakeStepData( uRefSlot, bMake );
//		}
//		close(m_CumFd);
//		for( int s=0;s<MAX_SLOT;s++ )
//		{
//			if( m_pstChipId     [s] != nullptr ) { delete[] m_pstChipId     [s]; m_pstChipId      [s] = nullptr; }
//			if( m_pInitFail     [s] != nullptr ) { delete[] m_pInitFail     [s]; m_pInitFail      [s] = nullptr; }
//			if( m_pStepFailChip [s] != nullptr ) { delete[] m_pStepFailChip [s]; m_pStepFailChip  [s] = nullptr; }
//			if( m_pCumFailChip  [s] != nullptr ) { delete[] m_pCumFailChip  [s]; m_pCumFailChip   [s] = nullptr; }
//			if( m_pCumFailBlock [s] != nullptr ) { delete[] m_pCumFailBlock [s]; m_pCumFailBlock  [s] = nullptr; }
//			if( m_pStepFailBlock[s] != nullptr ) { delete[] m_pStepFailBlock[s]; m_pStepFailBlock [s] = nullptr; }
//			for( uint cpu=0;cpu<2;cpu++ )
//			{
//				if( m_pStepInInfo [s][cpu] != nullptr ) { delete[] m_pStepInInfo [s][cpu]; m_pStepInInfo [s][cpu] = nullptr; }
//				if( m_pStepOutInfo[s][cpu] != nullptr ) { delete[] m_pStepOutInfo[s][cpu]; m_pStepOutInfo[s][cpu] = nullptr; }
//			}
//			for(uint i=0; i<m_vecStepData[s][CPU00].size(); ++i )
//				delete m_vecStepData[s][CPU00][i];
//			m_vecStepData[s][CPU00].clear();
//			for(uint i=0; i<m_vecStepData[s][CPU01].size(); ++i )
//				delete m_vecStepData[s][CPU01][i];
//			m_vecStepData[s][CPU01].clear();
//		}
//
//	}
//	printf("[INFO] CUMBLOCKFAIL MAKE %s...\n", bMake?"OK":"NG" );
//	return true;
//}
//
//bool CCumFailBlock::GetFailBlockAddress( std::vector<uint>& vBlkAddr, char* pFailBlock, uint dut, uint ce, uint chip, uint step )
//{
//	ssize_t nFailBlockSizePerStep = m_nMaxDut * m_nBlockPerDutBl;
//	uint iOffset = nFailBlockSizePerStep*step + dut*m_nBlockPerDutBl;
//	uint minaddr = ce * m_nBlockPerCe + chip * m_nBlockPerChip;
//	uint maxaddr = ce * m_nBlockPerCe + chip * m_nBlockPerChip + m_nBlockPerChip - 1;
//
//	for(uint iBlock=minaddr; iBlock<=maxaddr; ++iBlock)
//	{
//		int m = iOffset + (iBlock>>3);
//		if ( ! pFailBlock[m] )
//		{
////			printf("Bf iBlock=%x     7-(iBlock%%8)=%d   ", iBlock, 7-(iBlock%8) );
//			iBlock += 7 - (iBlock%8);
////			printf("Af iBlock=%x\n", iBlock );
//			continue;
//		}
//
//		int n = iBlock&0x7;
//		bool bFail = static_cast<bool>( (pFailBlock[m] >> n) & 1);
//		if ( bFail )
//		{
//			//printf("fail=%x\n", iBlock );
//			vBlkAddr.push_back( iBlock );
//		}
////		else
////			printf("pass=%x\n", iBlock );
//	}
//	return true;
//}
//
///////////////////////////////////////////////////////////////////////////
//
//bool CCumFailBlock::IsChipFail( char* pFailChip, uint dut, uint ce, uint chip, uint step )
//{
//	if ( pFailChip==nullptr ) return false;
//
//	ssize_t nFailChipSizePerStep = m_nMaxDut * m_nChipPerDutBl;	//* sizeof(char)
//
//	int chip_index = ce * m_nChipPerCe + chip;
//	int m = nFailChipSizePerStep*step + dut * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
//	int n = chip_index & 0x7;														//chip_index % 8;
//
//	return static_cast<bool>( (pFailChip[m] >> n) & 1 );
//}
//
//bool CCumFailBlock::IsChipFail(char* pFailChip, uint dut, uint step)
//{
//	if ( pFailChip==nullptr ) return false;
//
//	ssize_t nFailChipSizePerStep = m_nMaxDut * m_nChipPerDutBl;
//
//	bool bFail = false;
//	for(uint iChip=0; iChip<m_nCePerDut*m_nChipPerCe; iChip+=8)
//	{
//		int chip_index = iChip;
//		int m = nFailChipSizePerStep*step + dut * m_nChipPerDutBl + (chip_index>>3);	//dut * m_nChipPerDutBl + chip_index / 8;
//		if ( pFailChip[m] )
//		{
//			bFail = true;
//			break;
//		}
//	}
//	return bFail;
//}


//bool CCumFailBlock::IsPreFail( uint slot, uint dut)
//{
//	if ( m_pInitFail[slot]==nullptr ) return false;
//
//	return static_cast<bool>( m_pInitFail[slot][dut] );
//
//}
//
//bool CCumFailBlock::MakeHeader(uint slot, uint cpu, bool& rMake )
//{
//	string strLine;
//	string strData;
//	char   szLine[1024];
//	struct tm* t;
//
////	m_sCumHead.clear();
//	strData.clear();
//	strLine =       "REV            :  "                                           ; strData += strLine + STR_LF;
//	sprintf(szLine, "PART NAME      :  %-17s ", m_pDataMgr->GET_PARTNO()          ); strData += szLine  + STR_LF;
//	sprintf(szLine, "LOT NO         :  %-17s ", m_pDataMgr->GET_LOTNO()           ); strData += szLine  + STR_LF;
//	sprintf(szLine, "PGM NAME       :  %-17s ", m_pDataMgr->GET_DEVICE(slot,cpu)  ); strData += szLine  + STR_LF;
//	strLine =       "FW_REV         :  "                                           ; strData += strLine + STR_LF;
//	strLine =       "CAT_REV        :  "                                           ; strData += strLine + STR_LF;
//////	strLine =       "REMARK         :  "; strData += strLine + m_pDataMgr->GET_DEVICE(slot,cpu) + STR_LF;
//	sprintf(szLine, "REMARK         :  %-17s ", m_pDataMgr->GET_REMARK(slot,cpu)  ); strData += szLine  + STR_LF;
//	strLine =       "C_Vendor       :  "                                           ; strData += strLine + STR_LF;
//	sprintf(szLine, "SYSTEM_NAME    :  %-17s ", UTIL::TCPIP::GetHostName().c_str()); strData += szLine  + STR_LF;
//	strLine =       "START TIME     :  "                                           ; strData += strLine;
//	{
//		t = localtime( m_pDataMgr->GET_CYCLE_BEGIN_TIME(slot, cpu) );
//		sprintf(szLine, "%04d%02d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//		strData += szLine + STR_LF;
//	}
//
//	strLine =       "END TIME       :  "                                           ; strData += strLine;
//	{
//		t = localtime( m_pDataMgr->GET_CYCLE_END_TIME(slot, cpu) );
//		sprintf(szLine, "%04d%02d%02d%02d%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min );
//		strData += szLine + STR_LF;
//	}
//
//	strLine =       "OPER           :  "                                           ; strData += strLine + STR_LF;
//	sprintf(szLine, "DEVICE         :  %-17s ", m_pDataMgr->GET_DEVICE(slot,cpu)  ); strData += szLine  + STR_LF;
//	strLine =       "FACTORY        :  "                                           ; strData += strLine + STR_LF;
//	strLine =       "Fab            :  "                                           ; strData += strLine + STR_LF;
//	strLine =       "Owner          :  "                                           ; strData += strLine + STR_LF;
//	strLine =       "Grade          :  "                                           ; strData += strLine + STR_LF;
//	sprintf(szLine, "MPG_FLAG       :  %c", 'N'                                   ); strData += szLine  + STR_LF;
//
//	uint nLen = strData.length();
//	ssize_t written = write( m_CumFd, strData.c_str(), nLen );
//	if ( written != nLen )
//	{
//		printf("%s file(%s) write error", __FUNCTION__, m_CumFile.c_str() );
//		rMake = false;
//		return rMake;
//	}
//
//	return rMake;
//}
//
//bool CCumFailBlock::MakeChipCumData( uint rSlot, uint rCpu, bool& rMake )
//{
//	if( !rMake ) return false;
//
//	uint flag_slot = rSlot;
//	string strLine = "";
//	string strData = "";
//	char   szLine[1024];
//	struct tm* t;
//
//	for( uint slot=0; slot<MAX_SLOT; slot++ )
//	{
//		if( !m_pDataMgr->IS_EXIST_DEVPGM_VAR(slot) )
//			continue;
//		for ( uint r=0; r<m_nBibX; r++ )
//		{
//			for ( uint c=0; c<m_nBibY; c++ )
//			{
//				uint dut = c*m_nBibX + r;
//				if ( IsPreFail( slot, dut ) ) continue;
//				for(uint iCe=0; iCe<m_nCePerDut; ++iCe)
//				{
//					for(uint iChip=0; iChip<m_nChipPerCe; ++iChip)
//					{
//						strData.clear();
//						sprintf( szLine, "Slot%02d.(%s)R%02d, C%02d, CE%02d, CHIP%02d",
//								 slot+1, m_pDataMgr->GET_BIBID(slot), r+1, c+1, iCe+1, iChip+1 );
//						strData += szLine;
//
//						if ( m_pstChipId[slot] != nullptr )
//						{
//							sprintf( szLine, "     %4s       %02x       %02d       %02d      NUL      NUL      NUL      NUL",
//										m_pstChipId[slot][dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].szchipId,
//										m_pstChipId[slot][dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uWaferId,
//										m_pstChipId[slot][dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uXPos   ,
//										m_pstChipId[slot][dut*m_nChipPerDut + iCe*m_nChipPerCe + iChip].uYPos
//								   );
//							strData += szLine;
//						}
//						strData += STR_LF;
//
//						sprintf( szLine, "Cum(%s/%s)                     ", (IsChipFail(m_pCumFailChip[slot], dut)?"FAIL":"PASS"), (IsChipFail(m_pCumFailChip[slot], dut, iCe, iChip)?"FAIL":"PASS") );
//						strData += szLine;
//						std::vector<uint> vBlkAddr;
//						GetFailBlockAddress(vBlkAddr, m_pCumFailBlock[slot], dut, iCe, iChip );
//						sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
//						strData += szLine;
//						int nFailBlk = (int)vBlkAddr.size();
//						if ( nFailBlk > 254 ) nFailBlk = 254;
//						for(int i=0; i<nFailBlk; ++i)
//						{
//							sprintf( szLine, " %x ", vBlkAddr[i] );
//							strData += szLine;
//						}
//						strData += STR_LF;
//
//						//step loop
//						std::vector<uint> vCumBlkAddr;
//						vCumBlkAddr.clear();
//						//printf("0.MaxStep:%d\n",m_nMaxStep);
//						for(uint iStep=1; iStep<=m_nMaxStep; ++iStep)
//						{
//							uint iPos = iStep-1;
//							char szStepName[MAX_TEXT_VALUE]={0x0,};
//							sprintf( szStepName, "%02d.%s%s", iStep, m_vecStepData[slot][rCpu][iStep]->szStepName, "(N)" );
//							sprintf( szLine, "%-35s", szStepName );
//							strData += szLine;
//
//							vBlkAddr.clear();
//							GetFailBlockAddress(vBlkAddr, m_pStepFailBlock[slot], dut, iCe, iChip, iPos);
//
//							if ( vBlkAddr.size()<1 && IsChipFail(m_pStepFailChip[slot], dut, iCe, iChip, iPos) )
//							{
//								if ( iStep<18 || iStep>327)
//								{
//									sprintf( szLine, "%dEA FAIL ", 1 );
//									strData += szLine + STR_LF;
//								}
//								else
//								{
//									if ( iStep>20 && iStep<77)
//									{
//										sprintf( szLine, "%dEA FAIL ", 1 );
//										strData += szLine + STR_LF;
//									}
//									else
//									{
//										sprintf( szLine, "%dEA", 0 );
//										strData += szLine + STR_LF;
//									}
//								}
//							}
//							else
//							{
//								sprintf( szLine, "%dEA", (int)vBlkAddr.size() );
//								strData += szLine;
//								nFailBlk = (int)vBlkAddr.size();
//								if ( nFailBlk > 253 ) nFailBlk = 253;
//								for(int i=0; i<nFailBlk; ++i)
//								{
//									sprintf( szLine, " %x%c", vBlkAddr[i], !IsIn(vCumBlkAddr, vBlkAddr[i])?'!':' ' );
//									strData += szLine;
//								}
//								if ( vBlkAddr.size() > 253 ) strData += " ...";
//								strData += STR_LF;
//							}
//							mergeFailBlockInfo(vCumBlkAddr, vBlkAddr);
//						}
//						uint strSize = strData.length();
//						ssize_t written = write( m_CumFd, strData.c_str(), strSize );
//						if ( written != strSize )
//						{
//							printf("%s file(%s) write error", __FUNCTION__, m_CumFile.c_str() );
//							rMake = false;
//							return rMake;
//						}
//					}
//				}
//			}
//		}
//	}
//	strData.clear();
//	char szBegin[32] = {0x0,};
//	char szEnd  [32] = {0x0,};
//	char szTmp  [256] = {0x0,};
//
//	uint nStepCount = m_nMaxStep;
//	uint last_slot   = rSlot;
//	uint last_cpu    = rCpu;
//
//	for(uint iStep=1; iStep<=m_nMaxStep; ++iStep)
//	{
//		uint iPos = iStep;
//		t = localtime( &m_vecStepData[last_slot][last_cpu][iPos]->tStepBegin );
//		sprintf( szBegin, "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
//		t = localtime( &m_vecStepData[last_slot][last_cpu][iPos]->tStepEnd );
//		sprintf( szEnd  , "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec );
//		sprintf( szTmp, "%02d.%-s"      , iStep, m_vecStepData[last_slot][last_cpu][iPos]->szStepName );
//		sprintf( szLine, "%-35s:%s - %s", szTmp, szBegin, szEnd );
//		strData += szLine + STR_LF;
//	}//End of iStep
//
//	uint strSize = strData.length();
//	ssize_t written = write( m_CumFd, strData.c_str(), strSize );
//	if ( written != strSize )
//	{
//		printf("%s file(%s) write error", __FUNCTION__, m_CumFile.c_str() );
//		rMake = false;
//		return rMake;
//	}
//
//	return rMake;
//}
//
//bool CCumFailBlock::MakeStepData( uint slot, bool& rMake )
//{
//	if( !rMake ) return false;
//	return rMake;
//}

