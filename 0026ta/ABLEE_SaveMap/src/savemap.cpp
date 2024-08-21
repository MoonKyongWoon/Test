#include "savemap.hpp"

namespace fs = std::filesystem;

CSaveMap::CSaveMap()
{
	m_iCnt = 0;
}

CSaveMap::~CSaveMap()
{
}

int CSaveMap::parseArgument( int argc, char* argv[] )
{
	if ( argc!=3 && argc!=2 )
	{
		ShowUsage();
		return 0;
	}

	uint bSend     = false;
	uint iFileType = 0;
	uint iCnt = 1;

		 if ( !strcmp(argv[1], "-set" ) ) iFileType = 0;
	else if ( !strcmp(argv[1], "-pre" ) ) iFileType = 1;
	else if ( !strcmp(argv[1], "-send") ) bSend     = true;

	if ( argc >= 3 )
	{
		iCnt = atoi(argv[2]);
		if( (iCnt<1) || (iCnt>10) )
		{
			ShowUsage();
			return 0;
		}
	}
	m_iCnt = iCnt;

	bool bSavemap = true;
	if ( ! m_OD.ParseRCmd         (bSavemap ) ) return 0;
	if ( ! m_OD.MkReportDir       (bSavemap ) ) return 0;
	if ( ! m_OD.GatherInitFailData(iFileType) ) return 0;

	if ( bSend ) return SendMap();

	return GenSaveMap(iFileType);
}

void CSaveMap::ShowUsage()
{
	printf("atsavemap -set n(1~n)\n");
	printf("atsavemap -pre n(1~n)\n");
	printf("atsavemap -send\n"      );
	printf("  -set n          save set map\n");
	printf("  -pre n          save pre map\n");
	printf("  -send           tranfer map data\n");
	return;
}

bool CSaveMap::SendMap()
{
	bool bMake = true;
	if ( m_OD.m_Lot.empty() ) bMake = false;

	for ( LOT_LIST::iterator itTotLot = m_OD.m_Lot.begin(); itTotLot != m_OD.m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		if ( ! AddSendList(pLot) ) bMake = false;

		if ( ! m_strFiles.empty() && m_OD.m_pDataMgr->GET_UIAUTOMODE() )
		{
			CFtpConf svr;
			if ( svr.Get("DATA") )
				UTIL::FTP::MPUT( svr.IP, svr.USER, svr.PASS, svr.DIR, m_strDir.c_str(), m_strFiles.c_str(), "ascii" );
			m_strFiles.clear();
		}
	}

	printf("[INFO] SAVEMAP SEND %s...\n", bMake?"OK":"NG" );
	return true;
}

bool CSaveMap::GenSaveMap(uint iFileType)
{
	//iFileType==0 : set, iFileType==1 : pre
	if ( iFileType!=0 && iFileType!=1  ) return false;

	bool bMake = true;
	if ( m_OD.m_Lot.empty() ) bMake = false;

	for ( LOT_LIST::iterator itTotLot = m_OD.m_Lot.begin(); itTotLot != m_OD.m_Lot.end(); ++ itTotLot )
	{
		T_LOT* pLot = *itTotLot;

		if ( ! GenMap(iFileType, pLot) ) bMake = false;
	}

	printf("[INFO] SAVEMAP MAKE %s...\n", bMake?"OK":"NG" );
	return true;
}

bool CSaveMap::GenMap(uint iFileType, T_LOT* pLot)
{
	int  fd = 0;
	char szDir [PATH_MAX];
	char szFile[PATH_MAX];
	char szPath[PATH_MAX];

	m_tStart = time(0);	//for set map
///////////////////////////////////////////////////////////////////////
	CFileMap* pDataMgr = m_OD.m_pDataMgr;
	time_t    tNow     = time(0);

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%s",
			  m_OD.m_szReportRoot,
			  pLot->PART_NO      ,
			  pLot->LOT_NO       ,
			  DIR_SAVEMAP_PATH    );

	snprintf( szFile , PATH_MAX, "%s_%04d%02d%02d%02d%02d_%s_%s%03d%s",
			  pDataMgr->GET_STATION()        ,
			  YYYY(tNow)                     ,
			  MM  (tNow)                     ,
			  DD  (tNow)                     ,
			  hh  (tNow)                     ,
			  mm  (tNow)                     ,
			  pLot->LOT_NO                   ,
			  (iFileType==1 ? "pre" : "set"),
			  m_iCnt                        ,
			  MAP_EXT                        );

	snprintf( szPath, PATH_MAX, "%s/%s", szDir, szFile );
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
	int iSetMapCnt = 1;
	int iPreMapCnt = 1;

	char   szIniFile[PATH_MAX];
	char   szKey[PATH_MAX];
	QString sValue;

	sprintf(szIniFile, "%s/%s.ini", szDir, pLot->LOT_NO );	// "/home/ablee/atdb/prod/PART/LOT/SaveMap/LOT.ini"

	fs::path pr( szIniFile );
	if( fs::exists(pr) )
	{
		sprintf(szKey, "MAP_COUNT/SET_MAP");
		if ( iFileType==1 && UTIL::INI::readSettings(szIniFile, szKey, sValue) )
		{
			bool bOk = false;
			int iCnt = sValue.toInt(&bOk, 10);
			if ( bOk ) iSetMapCnt = iCnt;
		}

		sprintf(szKey, "MAP_COUNT/PRE_MAP");
		if ( iFileType==0 && UTIL::INI::readSettings(szIniFile, szKey, sValue) )
		{
			bool bOk = false;
			int iCnt = sValue.toInt(&bOk, 10);
			if ( bOk ) iPreMapCnt = iCnt;
		}
	}

		 if( iFileType==0 ) iSetMapCnt = m_iCnt;
	else if( iFileType==1 ) iPreMapCnt = m_iCnt;

	if ( iFileType==0 && iSetMapCnt==1 )
	{
		iPreMapCnt = 1;

		char szCmd[PATH_MAX];
		sprintf(szCmd, "rm -f %s/*", szDir);
		system( szCmd );
	}

	if ( ! SaveMapCnt(szIniFile, iSetMapCnt, iPreMapCnt) )
		return false;

///////////////////////////////////////////////////////////////////////

	m_strBody.clear();
	if ( ! MakeBody(iFileType, pLot) )
		return false;

	m_tEnd = time(0);	//for set map
	m_strHead.clear();
	if ( ! MakeHead(iFileType, pLot) )
		return false;

	fd = open(szPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		printf("[ ERR] %s open error!\n", szPath );
		return false;
	}

	string strData = m_strHead + m_strBody;
	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, szPath );
		return false;
	}
	close(fd);

	{
		tNow = time(0);

		char szFileT[PATH_MAX];
		snprintf( szFileT, PATH_MAX, "%s_%04d%02d%02d%02d%02d_%s_%s%s",
				  pDataMgr->GET_STATION()        ,
				  YYYY(tNow)                     ,
				  MM  (tNow)                     ,
				  DD  (tNow)                     ,
				  hh  (tNow)                     ,
				  mm  (tNow)                     ,
				  pLot->LOT_NO                   ,
				  (iFileType==1 ? "pre" : "set"),
				  MAP_EXT                        );


		char szCmd[PATH_MAX];
		sprintf(szCmd, "rm -f %s/*%s%s", szDir, (iFileType==1 ? "pre" : "set"), MAP_EXT );
		system( szCmd );

		sprintf(szCmd, "cp -f %s/%s %s/%s", szDir, szFile, szDir, szFileT );
		system( szCmd );
	}

	m_strDir = szDir ;
	m_strFiles += szFile + string(" ");

	return true;
}

bool CSaveMap::AddSendList(T_LOT* pLot)
{
	char szDir [PATH_MAX];

	snprintf( szDir, PATH_MAX, "%s/%s/%s/%s",
			  m_OD.m_szReportRoot,
			  pLot->PART_NO      ,
			  pLot->LOT_NO       ,
			  DIR_SAVEMAP_PATH    );

	fs::path p( szDir );	//SaveMap directory
	m_strFiles.clear();
	if( fs::is_directory(p) )
	{
		for( const auto& entry : fs::directory_iterator(p) )
		{
			if ( entry.path().extension().compare(MAP_EXT)==0 )
			{
				char szExclMap[PATH_MAX];
				sprintf(szExclMap, "set%s", MAP_EXT);
				string f    = entry.path().filename();
				uint   nLen = f.length();
				if ( nLen <=  strlen(szExclMap) ) continue;
				if ( f.compare( nLen-strlen(szExclMap), strlen(szExclMap), szExclMap)==0 )
					continue;

				sprintf(szExclMap, "pre%s", MAP_EXT);
				f    = entry.path().filename();
				nLen = f.length();
				if ( nLen <=  strlen(szExclMap) ) continue;
				if ( f.compare( nLen-strlen(szExclMap), strlen(szExclMap), szExclMap)==0 )
					continue;

				m_strFiles += entry.path().filename();
				m_strFiles += " ";
//				printf("DIR:[%s]\tFile:[%s]\n", szDir, entry.path().filename().c_str() );
			}
		}
	}
	m_strDir = szDir;

	if ( m_strFiles.empty() ) return false;

	return true;
}

bool CSaveMap::MakeHead(uint iFileType, T_LOT* pLot)
{
	string strLine;
	string strData;
	char   szLine[1024];

	CFileMap* pDataMgr  = m_OD.m_pDataMgr;

	time_t    tStart;
	time_t    tEnd  ;

	if ( iFileType==1 )				//pre
	{
		tStart = pLot->m_tInitStart;
		tEnd   = pLot->m_tInitEnd  ;
	}
	else	//if ( iFileType==0 )	//set
	{
		stSetupTimePair tSetup;
		if ( m_OD.ReadSetupTime(tSetup) )
		{
			tStart = tSetup.start_time;
			tEnd   = tSetup.end_time  ;
		}
		else
		{
			tStart = m_tStart;
			tEnd   = m_tEnd  ;
		}
	}

	strData.clear();
	sprintf(szLine, "REVISION = %d"     , 1                              ); strData += szLine + STR_LF;
	sprintf(szLine, "GUBUN = %s"   , (iFileType==0 ? "SET" : "PRE") ); strData += szLine + STR_LF;
	strLine =       "START TIME = "                                  ; strData += strLine;
	{
		sprintf(szLine, "%04d/%02d/%02d %02d:%02d:%02d",
				YYYY(tStart), MM(tStart), DD(tStart), hh(tStart), mm(tStart) , ss(tStart) );
		strData += szLine + STR_LF;
	}
	strLine =       "END TIME = "                                    ; strData += strLine;
	{
		sprintf(szLine, "%04d/%02d/%02d %02d:%02d:%02d",
				YYYY(tEnd  ), MM(tEnd  ), DD(tEnd  ), hh(tEnd  ), mm(tEnd  ) , ss(tEnd  ) );
		strData += szLine + STR_LF;
	}
	sprintf(szLine, "OPER = %s"    , pLot->OPER                     ); strData += szLine + STR_LF;
	sprintf(szLine, "LOT NO = %s"  , pLot->LOT_NO                   ); strData += szLine + STR_LF;
	sprintf(szLine, "PGM NAME = %s", pLot->TEST_PGM_NO              ); strData += szLine + STR_LF;
	sprintf(szLine, "STATION = %s" , pDataMgr->GET_STATION()        ); strData += szLine + STR_LF;

	m_strHead = strData;

	return true;
}

bool CSaveMap::MakeBody(uint iFileType, T_LOT* pLot)
{
	string strData;
	char   szLine[1024];

	strData.clear();
	for( int iSlot=0; iSlot<MAX_SLOT; ++iSlot )
	{
		if ( ! pLot->S[iSlot].IS_VALID() ) continue;

		sprintf(szLine, "BID 0%s Z1S%02d MLB_CODE=%s", pLot->S[iSlot].BIB_ID, iSlot+1, pLot->S[iSlot].MLB_CODE ); strData += szLine  + STR_LF;

		uint nX         = pLot->S[iSlot].BIB_X;
		uint nY         = pLot->S[iSlot].BIB_Y;
		uint nMaxDut    = m_OD.m_nMaxDut[iSlot];
		uint nSlotTotal = 0;

		if (false)
		{
			printf("SLOT = %d\n", iSlot+1 );
			printf("BIB_X = %d\n", nX     );
			printf("BIB_Y = %d\n", nY     );
			printf("nMaxDut = %d\n", nMaxDut     );
			printf("ldut=");
			for( uint iDut=0; iDut<nMaxDut; ++iDut )
			{
				if( pLot->S[iSlot].ldut[iDut] ) printf("1");
				else                            printf("0");
			}
			printf("\n");
			printf("mdut=");
			for( uint iDut=0; iDut<nMaxDut; ++iDut )
			{
				if( pLot->S[iSlot].mdut[iDut] ) printf("1");
				else                            printf("0");
			}
			printf("\n");
			printf("preF=");
			for( uint iDut=0; iDut<nMaxDut; ++iDut )
			{
				if( iFileType==1 && m_OD.m_pInitFail[iSlot][iDut] ) printf("1");
				else                                printf("0");
			}
			printf("\n");
		}

		for( uint iDut=0; iDut<nMaxDut; ++iDut )
		{
			//set, pre
			if( ! pLot->S[iSlot].ldut[iDut] ) continue;
			if(   pLot->S[iSlot].mdut[iDut] ) continue;

			//pre
			if ( iFileType==1 )
			{
				if( m_OD.m_pInitFail[iSlot][iDut] ) continue;
			}

			nSlotTotal++;
		}

		sprintf(szLine, "TOTAL %d", nSlotTotal ); strData += szLine + STR_LF;

		strData += "   ";
		for ( uint r=0; r<nX; r++ )
		{
			sprintf( szLine, " %2d", r+1 ); strData += szLine;
		}
		strData += STR_LF;

		for ( int c=nY-1; c>=0; c-- )
		{
			sprintf( szLine, " %2d", c+1 ); strData += szLine;
			for ( uint r=0; r<nX; r++ )
			{
				uint dut = c*nX + r;

				if ( iFileType==0 )			//set
				{
						 if(   pLot->S[iSlot].mdut[dut] ) sprintf( szLine, "  X");
					else if( ! pLot->S[iSlot].ldut[dut] ) sprintf( szLine, "  .");
					else                                  sprintf( szLine, "  m");

				}
				else if ( iFileType==1 )	//pre
				{
						 if(   pLot->S[iSlot].mdut[dut]     ) sprintf( szLine, "  X");
					else if( ! pLot->S[iSlot].ldut[dut]     ) sprintf( szLine, "  .");
					else if(   m_OD.m_pInitFail[iSlot][dut] ) sprintf( szLine, "  0");
					else                                      sprintf( szLine, "  m");
				}
				else						//error??
					sprintf( szLine, "   ");

				strData += szLine;
			} //End of r
			strData += STR_LF;
		} //End of c
		strData += STR_LF;
	} //End of slot

	m_strBody = strData;
	return true;
}

bool CSaveMap::SaveMapCnt(char* pszPath, uint iSetCnt, uint iPreCnt)
{
	string strData = "";
	char   szLine[1024];

	strData.clear();
	sprintf(szLine, "[MAP_COUNT]"         ); strData += szLine  + STR_LF;
	sprintf(szLine, "SET_MAP=%d", iSetCnt ); strData += szLine  + STR_LF;
	sprintf(szLine, "PRE_MAP=%d", iPreCnt ); strData += szLine  + STR_LF;

	int fd = open(pszPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
	if ( fd == -1 )
	{
		printf("[ ERR] %s open error!\n", pszPath );
		return false;
	}

	uint nLen = strData.length();
	ssize_t written = write( fd, strData.c_str(), nLen );
	if ( written != nLen )
	{
		printf("%s file(%s) write error", __FUNCTION__, pszPath );
		return false;
	}
	close(fd);

	return true;
}

int CSaveMap::YYYY( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_year+1900;
}

int CSaveMap::MM( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mon+1;
}

int CSaveMap::DD( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_mday;
}

int CSaveMap::hh( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_hour;
}

int CSaveMap::mm( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_min;
}

int CSaveMap::ss( time_t tTime )
{
	struct tm* t = localtime( &tTime );
	return t->tm_sec;
}

bool CSaveMap::WriteFile(int fd, const char* data, uint size )
{
	ssize_t written = write( fd, data, size );
	if( size != written ) return false;
	return true;
}
