#include "mrd.hpp"

CMrd::CMrd(COrganizedData* pOd) : m_pOd(pOd)
{
	m_strFiles = "";
}

bool CMrd::GenMrd()
{
	bool bMake = false;
	for ( LOT_LIST::iterator itTotLot = m_pOd->m_Lot.begin(); itTotLot != m_pOd->m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		bMake = true;
/////////////////////////////////////////////////////////////////////////
		for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
		{
			if ( ! pLot->S[iSlot].IS_VALID() ) continue;

			for ( uint r=0; r<m_pOd->m_nBibX[iSlot]; r++ )
			{
				for ( uint c=0; c<m_pOd->m_nBibY[iSlot]; c++ )
				{
					uint dut = c*m_pOd->m_nBibX[iSlot] + r;
					if ( ! pLot->S[iSlot].sdut[dut] ) continue;
					if (   IsPreFail( iSlot, dut )  ) continue;

					for(uint iCe=0; iCe<m_pOd->m_nCePerDut[iSlot]; ++iCe)
					{
						for(uint iChip=0; iChip<m_pOd->m_nChipPerCe[iSlot]; ++iChip)
						{
							int fd = CreateMrdFile(pLot, iSlot, r, c, iCe, iChip );
							if ( fd < 0 ) continue;

							if ( ! AppendMrd(iSlot, r, c, iCe, iChip, fd) ) continue;

							close( fd );
							printf("%s  gerneration complet.\n", m_sFile.c_str() );
						}//End of iChp
					}//End of iCe
				}//End of c
			}//End of c
		}//End of slot

		if( ! m_strFiles.empty() && m_pOd->m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("MRD") )
				UTIL::FTP::MPUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFiles.c_str(), "ascii" );
			m_strFiles.clear();
		}
		m_vecMrdFiles.clear();;
	}

/////////////////////////////////////////////////////////////////////////
///
	printf("[INFO] MRD MAKE %s...\n", bMake?"OK":"NG" );
	return true;
}

bool CMrd::GetFailBlockAddress( std::vector<uint>& vBlkAddr, char* pFailBlock, uint slot, uint dut, uint ce, uint chip, uint step )
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
//			vBlkAddr.push_back( iBlock );
			vBlkAddr.push_back( iBlock%m_pOd->m_nBlockPerCe[slot] );
		}
//		else
//			printf("pass=%x\n", iBlock );
	}
	return true;
}

bool CMrd::IsPreFail(uint iSlot, uint iDut)
{
	if ( m_pOd->m_pInitFail[iSlot]==nullptr ) return false;

	return static_cast<bool>( m_pOd->m_pInitFail[iSlot][iDut] );

}

bool CMrd::IsValidChipId(uint slot, uint x, uint y, uint iCe, uint iChip)
{
	if ( slot  >= MAX_SLOT     ) return false;
	if ( x     >= m_pOd->m_nBibX     [slot] ) return false;
	if ( y     >= m_pOd->m_nBibY     [slot] ) return false;
	if ( iCe   >= m_pOd->m_nCePerDut [slot] ) return false;
	if ( iChip >= m_pOd->m_nChipPerCe[slot] ) return false;

	if ( m_pOd->m_pstChipId[slot] == nullptr ) return false;

	uint dut = y*m_pOd->m_nBibX[slot] + x;
	char* pszChipId = m_pOd->m_pstChipId[slot][dut*m_pOd->m_nChipPerDut[slot] + iCe*m_pOd->m_nChipPerCe[slot] + iChip].szchipId;
	for(int i=0; i<4; ++i )
	{
		if ( !isdigit(pszChipId[i]) && !isalpha(pszChipId[i]) )
			return false;
	}

	uint uWaferId = m_pOd->m_pstChipId[slot][dut*m_pOd->m_nChipPerDut[slot] + iCe*m_pOd->m_nChipPerCe[slot] + iChip].uWaferId;
	if ( uWaferId > 99 ) return false;

	return true;
}

char* CMrd::GetChipId(uint slot, uint x, uint y, uint iCe, uint iChip)
{
//	if ( ! IsValidChipId(slot, x, y, iCe, iChip) ) return NULL;
	uint dut = y*m_pOd->m_nBibX[slot] + x;

	return m_pOd->m_pstChipId[slot][dut*m_pOd->m_nChipPerDut[slot] + iCe*m_pOd->m_nChipPerCe[slot] + iChip].szchipId;
}

unsigned short CMrd::GetWaferId(uint slot, uint x, uint y, uint iCe, uint iChip)
{
//	if ( ! IsValidChipId(slot, x, y, iCe, iChip) ) return 0;
	uint dut = y*m_pOd->m_nBibX[slot] + x;

	return m_pOd->m_pstChipId[slot][dut*m_pOd->m_nChipPerDut[slot] + iCe*m_pOd->m_nChipPerCe[slot] + iChip].uWaferId;
}

unsigned short CMrd::GetPosX(uint slot, uint x, uint y, uint iCe, uint iChip)
{
//	if ( ! IsValidChipId(slot, x, y, iCe, iChip) ) return 0;
	uint dut = y*m_pOd->m_nBibX[slot] + x;

	return m_pOd->m_pstChipId[slot][dut*m_pOd->m_nChipPerDut[slot] + iCe*m_pOd->m_nChipPerCe[slot] + iChip].uXPos;
}

unsigned short CMrd::GetPosY(uint slot, uint x, uint y, uint iCe, uint iChip)
{
//	if ( ! IsValidChipId(slot, x, y, iCe, iChip) ) return 0;
	uint dut = y*m_pOd->m_nBibX[slot] + x;

	return m_pOd->m_pstChipId[slot][dut*m_pOd->m_nChipPerDut[slot] + iCe*m_pOd->m_nChipPerCe[slot] + iChip].uYPos;
}

char* CMrd::GetFailBlkStepName(uint slot, uint dut, uint iBlock)
{
	char* pFailBlock = m_pOd->m_pStepFailBlock[slot];
	ssize_t nFailBlockSizePerStep = m_pOd->m_nMaxDut[slot] * m_pOd->m_nBlockPerDutBl[slot];

	for(uint iStep=1; iStep<=m_pOd->m_nMaxStep[slot]; ++iStep)
	{
		uint iPos = iStep-1;
		uint iOffset = nFailBlockSizePerStep*iPos + dut*m_pOd->m_nBlockPerDutBl[slot];
		int m = iOffset + (iBlock>>3);
		int n = iBlock&0x7;
		bool bFail = static_cast<bool>( (pFailBlock[m] >> n) & 1);
		if ( bFail )
			return m_pOd->m_vecStepData[slot][CPU00][iStep]->szStepName;
	}
	return NULL;
}

int CMrd::CreateMrdFile(T_LOT* pLot, uint slot, uint x, uint y, uint iCe, uint iChip)
{
	if ( ! IsValidChipId(slot, x, y, iCe, iChip) ) return -1;

	int fd = 0;
	char szDir [PATH_MAX];
	char szFile[PATH_MAX];
	char szPath[PATH_MAX];
	char szEXT [PATH_MAX];

	if ( ! strcmp(pLot->OPER, "4030") ) sprintf(szEXT, "%s", MRD_EXT1 );
	else                                sprintf(szEXT, "%s", MRD_EXT0 );

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%d",
			  m_pOd->m_szReportRoot,
			  pLot->PART_NO        ,
			  pLot->LOT_NO         ,
			  pLot->SNO             );

	char szLot[PATH_MAX];
	snprintf( szLot, PATH_MAX, "%s", pLot->LOT_NO );
	if ( strlen(szLot)>3 ) szLot[3] = '\0';
	snprintf( szFile, PATH_MAX, "%s%4sP%02d%s" ,
			  szLot                               ,
			  GetChipId ( slot, x, y, iCe, iChip ),
			  GetWaferId( slot, x, y, iCe, iChip ),
			  szEXT                                );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );

	m_strDir = szDir ;
	m_sFile  = szPath;

	if ( find(m_vecMrdFiles.begin(), m_vecMrdFiles.end(), szPath) == m_vecMrdFiles.end() )
	{
		fd = open(szPath, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
		if ( fd == -1 )
			printf("[ ERR] %s open error!\n", szPath );
		m_vecMrdFiles.push_back( m_sFile );
		m_strFiles += szFile + string(" ");
	}
	else
	{
		fd = open(szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
		if ( fd == -1 )
			printf("[ ERR] %s open error!\n", szPath );
		if ( ! MakeHeader(pLot, slot, x, y, iCe, iChip, fd) )
		{
			close(fd);
			return -1;
		}
	}

	return fd;
}

bool CMrd::MakeHeader(T_LOT* pLot, uint slot, uint x, uint y, uint iCe, uint iChip, int fd)
{
	if ( fd<0 ) return false;

	string strLine;
	string strData;
	char   szLine[1024];

	CFileMap* pDataMgr  = m_pOd->m_pDataMgr;
	time_t    tStart    = pLot->START_TIME;
	time_t    tEnd      = pLot->END_TIME  ;

	strData.clear();
	sprintf(szLine, "DEVICE         :  %s", pLot->DEVICE_NO                 ); strData += szLine + STR_LF;
	sprintf(szLine, "FACTORY        :  %s", pLot->FACTORY                   ); strData += szLine + STR_LF;
	sprintf(szLine, "LOT NO         :  %s", pLot->LOT_NO                    ); strData += szLine + STR_LF;
	sprintf(szLine, "Fab            :  %s", pLot->FAB                       ); strData += szLine + STR_LF;
	sprintf(szLine, "OPER           :  %s", pLot->OPER                      ); strData += szLine + STR_LF;
	sprintf(szLine, "PART NAME      :  %s", pLot->PART_NO                   ); strData += szLine + STR_LF;
	sprintf(szLine, "PGM NAME       :  %s", pLot->TEST_PGM_NO               ); strData += szLine + STR_LF;
	sprintf(szLine, "REMARK         :  %s", m_pOd->m_szVersion[pLot->m_uRS] ); strData += szLine + STR_LF;
	sprintf(szLine, "SYSTEM_NAME    :  %s", pDataMgr->GET_STATION()         ); strData += szLine + STR_LF;
	strLine =       "START TIME     :  "                                     ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) );
		strData += szLine + STR_LF;
	}

	strLine = "END TIME       :  "                                                                  ; strData += strLine;
	{
		sprintf(szLine, "%04d%02d%02d%02d%02d", YYYY(tEnd  ), MM(tEnd  ), DD(tEnd  ), hh(tEnd  ), mm(tEnd  ) );
		strData += szLine + STR_LF;
	}

	sprintf(szLine, "Owner          :  %s", pLot->OWNER                     ); strData += szLine + STR_LF;
	sprintf(szLine, "Grade          :  %s", pLot->GRADE                     ); strData += szLine + STR_LF;

	sprintf(szLine, "Product LOT# %4s WAF# %2d System#Slot",
			GetChipId ( slot, x, y, iCe, iChip ),
			GetWaferId( slot, x, y, iCe, iChip )                                                   ); strData += szLine  + STR_LF;

	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, m_sFile.c_str() );
		return false;
	}

	return true;
}

bool CMrd::AppendMrd(uint slot, uint x, uint y, uint iCe, uint iChip, int fd)
{
	if ( fd<0 ) return false;

	string strData;
	char   szLine[1024];

	strData.clear();
	sprintf(szLine, "die %02d %02d",
			GetPosX( slot, x, y, iCe, iChip ),
			GetPosY( slot, x, y, iCe, iChip ) ); strData += szLine  + STR_LF;

/////////////////////////////////////////////////////////////////////////////////
	uint dut = y*m_pOd->m_nBibX[slot] + x;
	std::vector<uint> vBlkAddr;
	GetFailBlockAddress(vBlkAddr, m_pOd->m_pCumFailBlock[slot], slot, dut, iCe, iChip );
	int nFailBlk = (int)vBlkAddr.size();
	if ( nFailBlk > 254 ) nFailBlk = 254;
	for(int i=0; i<nFailBlk; ++i)
	{
		char* pszStepName = GetFailBlkStepName(slot, dut, vBlkAddr[i]);
		sprintf( szLine, "Z%c%04d %8.8s", 'A' + vBlkAddr[i]%4, vBlkAddr[i], (pszStepName ? pszStepName : " ") );
		strData += szLine;
		strData += STR_LF;
	}
/////////////////////////////////////////////////////////////////////////////////

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

int CMrd::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CMrd::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CMrd::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CMrd::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CMrd::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CMrd::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}
