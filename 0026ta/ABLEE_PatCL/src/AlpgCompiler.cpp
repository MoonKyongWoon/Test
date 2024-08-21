#include "AlpgCompiler.h"

CAlpgCompiler::CAlpgCompiler()
{
	create_syntax();
}

CAlpgCompiler::~CAlpgCompiler()
{
}

void CAlpgCompiler::create_syntax()
{
///////////////////////////////////////
// Stack Size 변경(Default: 1MB) -> 2MB
// 1024 * 1024 * 2 => 2097152 Byte
//#pragma comment(linker, "/STACK:2097152")
///////////////////////////////////////
	#include "PatternSyntax.h"

	//CURRENT ADDRESS
	#include "CurrAddrXSyntax.h"
	#include "CurrAddrYSyntax.h"

	burst_map[ "pretest_result"    ] = OUT_MODE::pretest_result   ;
	burst_map[ "bitest_result"     ] = OUT_MODE::bitest_result    ;
	burst_map[ "latch"             ] = OUT_MODE::latch            ;
	burst_map[ "pre_result"        ] = OUT_MODE::pre_result       ;
	burst_map[ "fm_block"          ] = OUT_MODE::fm_block         ;
	burst_map[ "extfm_result"      ] = OUT_MODE::extfm_result     ;
	burst_map[ "L1_result"         ] = OUT_MODE::L1_result        ;
	burst_map[ "latch_result"      ] = OUT_MODE::latch_result     ;
	burst_map[ "init_result"       ] = OUT_MODE::init_result      ;
	burst_map[ "fm_read_data"      ] = OUT_MODE::fm_read_data     ;
	burst_map[ "bist_io_latch"     ] = OUT_MODE::bist_io_latch    ;
	burst_map[ "bist_result"       ] = OUT_MODE::bist_result      ;
	burst_map[ "bist2_result"      ] = OUT_MODE::bist2_result     ;
	burst_map[ "emmc_io_latch"     ] = OUT_MODE::emmc_io_latch    ;
	burst_map[ "emmc_read_drv"     ] = OUT_MODE::emmc_read_drv    ;
	burst_map[ "emmc_init"         ] = OUT_MODE::emmc_init        ;
	burst_map[ "emmc_result"       ] = OUT_MODE::emmc_result      ;
	burst_map[ "emmc_block_result" ] = OUT_MODE::emmc_block_result;
	burst_map[ "emmc_chip_id"      ] = OUT_MODE::emmc_chip_id     ;
	burst_map[ "emmc_read_data"    ] = OUT_MODE::emmc_read_data   ;
}

void CAlpgCompiler::Reset()
{
	Init();
}

bool CAlpgCompiler::SetDefine( atstring funcname, vector<pat_line_data> lines )
{
	atstring strFunc = funcname;
	macro_map.clear();

	for(size_t i=0; i<lines.size(); ++i )
	{
		lines[i].data.Replace("\t", " ");

		vector<atstring> vSdefs;
		lines[i].data.Split(vSdefs, " ");

		string strKey;
		string strData;
		if ( vSdefs.size()<2 )
		{
			add_err(strFunc, lines[i].line, "SET_DEFINE Syntax error", (char *)lines[i].data);
			continue;
		}
		else
		{
			if ( vSdefs[0].compare("SDEF") ) add_err(strFunc, lines[i].line, "SET_DEFINE Syntax error", (char *)lines[i].data );

			strKey = vSdefs[1];
			if ( vSdefs.size()<3 )
			{
				strData = "";
			}
			else
			{
				for(size_t iSdefs=2; iSdefs<vSdefs.size(); ++iSdefs )
				{
					strData += vSdefs[iSdefs] + " ";					
				}
				strData.erase( strData.length()-1, 1 );//remove last " "	//fatric 2022-11-01
			}
		}
		macro_map[strKey] = strData;
	}
/////////////////////////////////////////////////////////////////////////
// Append By fatric 2022.11.17
/////////////////////////////////////////////////////////////////////////
	if ( macro_map.find("R") != macro_map.end() )
	{
		string strData = macro_map["R"];
		strData += " C41";
		macro_map["R"] = strData;
	}
/////////////////////////////////////////////////////////////////////////

	return true;
}

bool CAlpgCompiler::PreprocIFE( atstring &subname, vector<pat_line_data> &lines )
{
	bool bDebug = false;

if ( bDebug )
{
	printf("BF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}
	bool bErr = false;
	int iNested = -1;
	vector<bool> vecIEF;
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
	{
		bool bPreProc = false;
		if ( lines[iLine].data.TrimLeft().compare(0, 4, "%IFE")==0 )
		{
			bPreProc = true;
			int iIFE = ParseIFE( lines[iLine].data.substr( lines[iLine].data.find("%IFE") ) );
			if (iIFE<0 )
			{
				add_err("%IFE", lines[iLine].line, "Syntax error", lines[iLine].data );
				bErr = true;
				break;
			}
			else if ( iIFE == 0 ) vecIEF.push_back( false );
			else                  vecIEF.push_back( true  );
			++iNested;
		}
		bool bAlive = true;
		if ( iNested >= 0 )
		{
			for( int ii=0; ii<=iNested; ++ii )
				bAlive &= vecIEF[ii];

//			printf("%c-%d [%d]{%s}\n", (bAlive ? 'T' : 'F'), iNested, lines[iLine].line, (char *)lines[iLine].data);
		}

		if ( lines[iLine].data.TrimLeft().compare(0, 5, "%ENDC")==0 )
		{
			bPreProc = true;
			--iNested;
			if ( iNested<-1 )
			{
				add_err("%ENDC", lines[iLine].line, "%IFE - %ENDC Pair error", lines[iLine].data );
				bErr = true;
				break;
			}
			vecIEF.pop_back();
		}
		if ( ! bAlive || bPreProc ) lines[iLine].data.clear();
	}

if ( bDebug )
{
	printf("AF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}

	if ( bErr ) return false;

	return true;
}

bool CAlpgCompiler::PreprocSDEF( atstring &subname, vector<pat_line_data> &lines )
{
	bool bDebug = false;

if ( bDebug )
{
	printf("BF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}

	for(size_t iLine=0; iLine<lines.size(); ++iLine )
	{
		lines[iLine].data.Replace( ";", " ; " );		//fatric 2022-11-01
		for ( Str2StrMap::iterator it=macro_map.begin(); it!=macro_map.end(); ++it)
		{
			replace_ds( lines[iLine].data, it->first.c_str(), it->second.c_str() );
		}
	}

if ( bDebug )
{
	printf("AF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}

	return true;
}

bool CAlpgCompiler::PreprocDsetReg( atstring &subname, vector<pat_line_data> &lines )
{
	bool bDebug = false;

if ( bDebug )
{
	printf("BF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
	{
		replace_PrimeReg( lines[iLine].data, "D1", "D1A" );
		replace_PrimeReg( lines[iLine].data, "D2", "D2A" );
		replace_PrimeReg( lines[iLine].data, "ND", "ND1" );
		replace_PrimeReg( lines[iLine].data, "BD", "BD1" );

		lines[iLine].data.Replace( "^", "\"" );
		lines[iLine].data.Replace( "|", "'" );

		//??
		replace_ds( lines[iLine].data, ".NOT.", " ! "  );
		replace_ds( lines[iLine].data, ".AND.", " && " );
		replace_ds( lines[iLine].data, ".OR." , " || " );
		replace_ds( lines[iLine].data, ".XOR.", " ^ "  );

		replace_DsetReg( lines[iLine].data, "XH<"    , "XH< "    );
		replace_DsetReg( lines[iLine].data, "D1A<"   , "D1A< "   );
		replace_DsetReg( lines[iLine].data, "D3B<"   , "D3B< "   );
		replace_DsetReg( lines[iLine].data, "NH<"    , "NH< "    );
		replace_DsetReg( lines[iLine].data, "XOS<"   , "XOS< "   );
		replace_DsetReg( lines[iLine].data, "XT<"    , "XT< "    );
		replace_DsetReg( lines[iLine].data, "ZD<"    , "ZD< "    );
		replace_DsetReg( lines[iLine].data, "ND1<"   , "ND1< "   );
		replace_DsetReg( lines[iLine].data, "D1B<"   , "D1B< "   );
		replace_DsetReg( lines[iLine].data, "D1C<"   , "D1C< "   );
		replace_DsetReg( lines[iLine].data, "D1D<"   , "D1D< "   );
		replace_DsetReg( lines[iLine].data, "D1E<"   , "D1E< "   );
		replace_DsetReg( lines[iLine].data, "D1F<"   , "D1F< "   );
		replace_DsetReg( lines[iLine].data, "D1G<"   , "D1G< "   );
		replace_DsetReg( lines[iLine].data, "D1H<"   , "D1H< "   );

		replace_DsetReg( lines[iLine].data, "YH<"    , "YH< "    );
		replace_DsetReg( lines[iLine].data, "D2A<"   , "D2A< "   );
		replace_DsetReg( lines[iLine].data, "D4B<"   , "D4B< "   );
		replace_DsetReg( lines[iLine].data, "BH<"    , "BH< "    );
		replace_DsetReg( lines[iLine].data, "ZH<"    , "ZH< "    );
		replace_DsetReg( lines[iLine].data, "YOS<"   , "YOS< "   );
		replace_DsetReg( lines[iLine].data, "YT<"    , "YT< "    );
		replace_DsetReg( lines[iLine].data, "Z<"     , "Z< "     );
		replace_DsetReg( lines[iLine].data, "BD1<"   , "BD1< "   );

		replace_DsetReg( lines[iLine].data, "TP<"    , "TP< "    );
		replace_DsetReg( lines[iLine].data, "TP1<"   , "TP1< "   );
		replace_DsetReg( lines[iLine].data, "TPH<"   , "TPH< "   );
		replace_DsetReg( lines[iLine].data, "TPH1<"  , "TPH1< "  );
		replace_DsetReg( lines[iLine].data, "D5<"    , "D5< "    );
		replace_DsetReg( lines[iLine].data, "DCMR<"  , "DCMR< "  );
		replace_DsetReg( lines[iLine].data, "DCMR1<" , "DCMR1< " );
		replace_DsetReg( lines[iLine].data, "XMASK<" , "XMASK< " );
		replace_DsetReg( lines[iLine].data, "CBMA<"  , "CBMA< "  );
		replace_DsetReg( lines[iLine].data, "RCD<"   , "RCD< "   );
		replace_DsetReg( lines[iLine].data, "DSD<"   , "DSD< "   );
		replace_DsetReg( lines[iLine].data, "DBMAR<" , "DBMAR< " );

		replace_DsetReg( lines[iLine].data, "TP2<"   , "TP2< "   );
		replace_DsetReg( lines[iLine].data, "TPH2<"  , "TPH2< "  );
		replace_DsetReg( lines[iLine].data, "D6<"    , "D6< "    );
		replace_DsetReg( lines[iLine].data, "DCMR2<" , "DCMR2< " );
		replace_DsetReg( lines[iLine].data, "YMASK<" , "YMASK< " );
		replace_DsetReg( lines[iLine].data, "DSC<"   , "DSC< "   );
		replace_DsetReg( lines[iLine].data, "CCD<"   , "CCD< "   );

		replace_Dbmap( lines[iLine].data, "DBMAP<"      , "DBMAP< "       );
		replace_Dbmap( lines[iLine].data, "DBMAP<DBMAP+", "DBMAP<DBMAP+ " );
		replace_Dbmap( lines[iLine].data, "DBMAP<DBMAP-", "DBMAP<DBMAP- " );

		replace_Addr( lines[iLine].data, "XB<"         , "XB< "    );
		replace_Addr( lines[iLine].data, "XB<XB+"      , "XB<XB+ " );
		replace_Addr( lines[iLine].data, "XB<XB-"      , "XB<XB- " );
		replace_Addr( lines[iLine].data, "YB<"         , "YB< "    );
		replace_Addr( lines[iLine].data, "YB<YB+"      , "YB<YB+ " );
		replace_Addr( lines[iLine].data, "YB<YB-"      , "YB<YB- " );

		replace_Addr( lines[iLine].data, "XC<"    , "XC< "     );
		replace_Addr( lines[iLine].data, "XC<XB+" , "XC<XB+ "  );
		replace_Addr( lines[iLine].data, "XC<XC+" , "XC<XC+ "  );
		replace_Addr( lines[iLine].data, "XC<XS+" , "XC<XS+ "  );
		replace_Addr( lines[iLine].data, "XC<XK+" , "XC<XK+ "  );
		replace_Addr( lines[iLine].data, "XC<XA+" , "XC<XA+ "  );
		replace_Addr( lines[iLine].data, "XC<XD+" , "XC<XD+ "  );
		replace_Addr( lines[iLine].data, "XC<XE+" , "XC<XE+ "  );
		replace_Addr( lines[iLine].data, "XC<XF+" , "XC<XF+ "  );
		replace_Addr( lines[iLine].data, "XC<XB-" , "XC<XB- "  );
		replace_Addr( lines[iLine].data, "XC<XC-" , "XC<XC- "  );
		replace_Addr( lines[iLine].data, "XC<XS-" , "XC<XS- "  );
		replace_Addr( lines[iLine].data, "XC<XK-" , "XC<XK- "  );
		replace_Addr( lines[iLine].data, "XC<XA-" , "XC<XA- "  );
		replace_Addr( lines[iLine].data, "XC<XD-" , "XC<XD- "  );
		replace_Addr( lines[iLine].data, "XC<XE-" , "XC<XE- "  );
		replace_Addr( lines[iLine].data, "XC<XF-" , "XC<XF- "  );
		replace_Addr( lines[iLine].data, "XC<XB&" , "XC<XB& "  );
		replace_Addr( lines[iLine].data, "XC<XC&" , "XC<XC& "  );
		replace_Addr( lines[iLine].data, "XC<XS&" , "XC<XS& "  );
		replace_Addr( lines[iLine].data, "XC<XK&" , "XC<XK& "  );
		replace_Addr( lines[iLine].data, "XC<XA&" , "XC<XA& "  );
		replace_Addr( lines[iLine].data, "XC<XD&" , "XC<XD& "  );
		replace_Addr( lines[iLine].data, "XC<XE&" , "XC<XE& "  );
		replace_Addr( lines[iLine].data, "XC<XF&" , "XC<XF& "  );
		replace_Addr( lines[iLine].data, "XC<XB\"", "XC<XB\" " );
		replace_Addr( lines[iLine].data, "XC<XC\"", "XC<XC\" " );
		replace_Addr( lines[iLine].data, "XC<XS\"", "XC<XS\" " );
		replace_Addr( lines[iLine].data, "XC<XK\"", "XC<XK\" " );
		replace_Addr( lines[iLine].data, "XC<XA\"", "XC<XA\" " );
		replace_Addr( lines[iLine].data, "XC<XD\"", "XC<XD\" " );
		replace_Addr( lines[iLine].data, "XC<XE\"", "XC<XE\" " );
		replace_Addr( lines[iLine].data, "XC<XF\"", "XC<XF\" " );
		replace_Addr( lines[iLine].data, "XC<XB'" , "XC<XB' "  );
		replace_Addr( lines[iLine].data, "XC<XC'" , "XC<XC' "  );
		replace_Addr( lines[iLine].data, "XC<XS'" , "XC<XS' "  );
		replace_Addr( lines[iLine].data, "XC<XK'" , "XC<XK' "  );
		replace_Addr( lines[iLine].data, "XC<XA'" , "XC<XA' "  );
		replace_Addr( lines[iLine].data, "XC<XD'" , "XC<XD' "  );
		replace_Addr( lines[iLine].data, "XC<XE'" , "XC<XE' "  );
		replace_Addr( lines[iLine].data, "XC<XF'" , "XC<XF' "  );
		replace_Addr( lines[iLine].data, "XS<"    , "XS< "     );
		replace_Addr( lines[iLine].data, "XS<XB+" , "XS<XB+ "  );
		replace_Addr( lines[iLine].data, "XS<XC+" , "XS<XC+ "  );
		replace_Addr( lines[iLine].data, "XS<XS+" , "XS<XS+ "  );
		replace_Addr( lines[iLine].data, "XS<XK+" , "XS<XK+ "  );
		replace_Addr( lines[iLine].data, "XS<XA+" , "XS<XA+ "  );
		replace_Addr( lines[iLine].data, "XS<XD+" , "XS<XD+ "  );
		replace_Addr( lines[iLine].data, "XS<XE+" , "XS<XE+ "  );
		replace_Addr( lines[iLine].data, "XS<XF+" , "XS<XF+ "  );
		replace_Addr( lines[iLine].data, "XS<XB-" , "XS<XB- "  );
		replace_Addr( lines[iLine].data, "XS<XC-" , "XS<XC- "  );
		replace_Addr( lines[iLine].data, "XS<XS-" , "XS<XS- "  );
		replace_Addr( lines[iLine].data, "XS<XK-" , "XS<XK- "  );
		replace_Addr( lines[iLine].data, "XS<XA-" , "XS<XA- "  );
		replace_Addr( lines[iLine].data, "XS<XD-" , "XS<XD- "  );
		replace_Addr( lines[iLine].data, "XS<XE-" , "XS<XE- "  );
		replace_Addr( lines[iLine].data, "XS<XF-" , "XS<XF- "  );
		replace_Addr( lines[iLine].data, "XS<XB&" , "XS<XB& "  );
		replace_Addr( lines[iLine].data, "XS<XC&" , "XS<XC& "  );
		replace_Addr( lines[iLine].data, "XS<XS&" , "XS<XS& "  );
		replace_Addr( lines[iLine].data, "XS<XK&" , "XS<XK& "  );
		replace_Addr( lines[iLine].data, "XS<XA&" , "XS<XA& "  );
		replace_Addr( lines[iLine].data, "XS<XD&" , "XS<XD& "  );
		replace_Addr( lines[iLine].data, "XS<XE&" , "XS<XE& "  );
		replace_Addr( lines[iLine].data, "XS<XF&" , "XS<XF& "  );
		replace_Addr( lines[iLine].data, "XS<XB\"", "XS<XB\" " );
		replace_Addr( lines[iLine].data, "XS<XC\"", "XS<XC\" " );
		replace_Addr( lines[iLine].data, "XS<XS\"", "XS<XS\" " );
		replace_Addr( lines[iLine].data, "XS<XK\"", "XS<XK\" " );
		replace_Addr( lines[iLine].data, "XS<XA\"", "XS<XA\" " );
		replace_Addr( lines[iLine].data, "XS<XD\"", "XS<XD\" " );
		replace_Addr( lines[iLine].data, "XS<XE\"", "XS<XE\" " );
		replace_Addr( lines[iLine].data, "XS<XF\"", "XS<XF\" " );
		replace_Addr( lines[iLine].data, "XS<XB'" , "XS<XB' "  );
		replace_Addr( lines[iLine].data, "XS<XC'" , "XS<XC' "  );
		replace_Addr( lines[iLine].data, "XS<XS'" , "XS<XS' "  );
		replace_Addr( lines[iLine].data, "XS<XK'" , "XS<XK' "  );
		replace_Addr( lines[iLine].data, "XS<XA'" , "XS<XA' "  );
		replace_Addr( lines[iLine].data, "XS<XD'" , "XS<XD' "  );
		replace_Addr( lines[iLine].data, "XS<XE'" , "XS<XE' "  );
		replace_Addr( lines[iLine].data, "XS<XF'" , "XS<XF' "  );
		replace_Addr( lines[iLine].data, "XK<"    , "XK< "     );
		replace_Addr( lines[iLine].data, "XK<XB+" , "XK<XB+ "  );
		replace_Addr( lines[iLine].data, "XK<XC+" , "XK<XC+ "  );
		replace_Addr( lines[iLine].data, "XK<XS+" , "XK<XS+ "  );
		replace_Addr( lines[iLine].data, "XK<XK+" , "XK<XK+ "  );
		replace_Addr( lines[iLine].data, "XK<XA+" , "XK<XA+ "  );
		replace_Addr( lines[iLine].data, "XK<XD+" , "XK<XD+ "  );
		replace_Addr( lines[iLine].data, "XK<XE+" , "XK<XE+ "  );
		replace_Addr( lines[iLine].data, "XK<XF+" , "XK<XF+ "  );
		replace_Addr( lines[iLine].data, "XK<XB-" , "XK<XB- "  );
		replace_Addr( lines[iLine].data, "XK<XC-" , "XK<XC- "  );
		replace_Addr( lines[iLine].data, "XK<XS-" , "XK<XS- "  );
		replace_Addr( lines[iLine].data, "XK<XK-" , "XK<XK- "  );
		replace_Addr( lines[iLine].data, "XK<XA-" , "XK<XA- "  );
		replace_Addr( lines[iLine].data, "XK<XD-" , "XK<XD- "  );
		replace_Addr( lines[iLine].data, "XK<XE-" , "XK<XE- "  );
		replace_Addr( lines[iLine].data, "XK<XF-" , "XK<XF- "  );
		replace_Addr( lines[iLine].data, "XK<XB&" , "XK<XB& "  );
		replace_Addr( lines[iLine].data, "XK<XC&" , "XK<XC& "  );
		replace_Addr( lines[iLine].data, "XK<XS&" , "XK<XS& "  );
		replace_Addr( lines[iLine].data, "XK<XK&" , "XK<XK& "  );
		replace_Addr( lines[iLine].data, "XK<XA&" , "XK<XA& "  );
		replace_Addr( lines[iLine].data, "XK<XD&" , "XK<XD& "  );
		replace_Addr( lines[iLine].data, "XK<XE&" , "XK<XE& "  );
		replace_Addr( lines[iLine].data, "XK<XF&" , "XK<XF& "  );
		replace_Addr( lines[iLine].data, "XK<XB\"", "XK<XB\" " );
		replace_Addr( lines[iLine].data, "XK<XC\"", "XK<XC\" " );
		replace_Addr( lines[iLine].data, "XK<XS\"", "XK<XS\" " );
		replace_Addr( lines[iLine].data, "XK<XK\"", "XK<XK\" " );
		replace_Addr( lines[iLine].data, "XK<XA\"", "XK<XA\" " );
		replace_Addr( lines[iLine].data, "XK<XD\"", "XK<XD\" " );
		replace_Addr( lines[iLine].data, "XK<XE\"", "XK<XE\" " );
		replace_Addr( lines[iLine].data, "XK<XF\"", "XK<XF\" " );
		replace_Addr( lines[iLine].data, "XK<XB'" , "XK<XB' "  );
		replace_Addr( lines[iLine].data, "XK<XC'" , "XK<XC' "  );
		replace_Addr( lines[iLine].data, "XK<XS'" , "XK<XS' "  );
		replace_Addr( lines[iLine].data, "XK<XK'" , "XK<XK' "  );
		replace_Addr( lines[iLine].data, "XK<XA'" , "XK<XA' "  );
		replace_Addr( lines[iLine].data, "XK<XD'" , "XK<XD' "  );
		replace_Addr( lines[iLine].data, "XK<XE'" , "XK<XE' "  );
		replace_Addr( lines[iLine].data, "XK<XF'" , "XK<XF' "  );
		replace_Addr( lines[iLine].data, "XA<"    , "XA< "     );
		replace_Addr( lines[iLine].data, "XA<XB+" , "XA<XB+ "  );
		replace_Addr( lines[iLine].data, "XA<XC+" , "XA<XC+ "  );
		replace_Addr( lines[iLine].data, "XA<XS+" , "XA<XS+ "  );
		replace_Addr( lines[iLine].data, "XA<XK+" , "XA<XK+ "  );
		replace_Addr( lines[iLine].data, "XA<XA+" , "XA<XA+ "  );
		replace_Addr( lines[iLine].data, "XA<XD+" , "XA<XD+ "  );
		replace_Addr( lines[iLine].data, "XA<XE+" , "XA<XE+ "  );
		replace_Addr( lines[iLine].data, "XA<XF+" , "XA<XF+ "  );
		replace_Addr( lines[iLine].data, "XA<XB-" , "XA<XB- "  );
		replace_Addr( lines[iLine].data, "XA<XC-" , "XA<XC- "  );
		replace_Addr( lines[iLine].data, "XA<XS-" , "XA<XS- "  );
		replace_Addr( lines[iLine].data, "XA<XK-" , "XA<XK- "  );
		replace_Addr( lines[iLine].data, "XA<XA-" , "XA<XA- "  );
		replace_Addr( lines[iLine].data, "XA<XD-" , "XA<XD- "  );
		replace_Addr( lines[iLine].data, "XA<XE-" , "XA<XE- "  );
		replace_Addr( lines[iLine].data, "XA<XF-" , "XA<XF- "  );
		replace_Addr( lines[iLine].data, "XA<XB&" , "XA<XB& "  );
		replace_Addr( lines[iLine].data, "XA<XC&" , "XA<XC& "  );
		replace_Addr( lines[iLine].data, "XA<XS&" , "XA<XS& "  );
		replace_Addr( lines[iLine].data, "XA<XK&" , "XA<XK& "  );
		replace_Addr( lines[iLine].data, "XA<XA&" , "XA<XA& "  );
		replace_Addr( lines[iLine].data, "XA<XD&" , "XA<XD& "  );
		replace_Addr( lines[iLine].data, "XA<XE&" , "XA<XE& "  );
		replace_Addr( lines[iLine].data, "XA<XF&" , "XA<XF& "  );
		replace_Addr( lines[iLine].data, "XA<XB\"", "XA<XB\" " );
		replace_Addr( lines[iLine].data, "XA<XC\"", "XA<XC\" " );
		replace_Addr( lines[iLine].data, "XA<XS\"", "XA<XS\" " );
		replace_Addr( lines[iLine].data, "XA<XK\"", "XA<XK\" " );
		replace_Addr( lines[iLine].data, "XA<XA\"", "XA<XA\" " );
		replace_Addr( lines[iLine].data, "XA<XD\"", "XA<XD\" " );
		replace_Addr( lines[iLine].data, "XA<XE\"", "XA<XE\" " );
		replace_Addr( lines[iLine].data, "XA<XF\"", "XA<XF\" " );
		replace_Addr( lines[iLine].data, "XA<XB'" , "XA<XB' "  );
		replace_Addr( lines[iLine].data, "XA<XC'" , "XA<XC' "  );
		replace_Addr( lines[iLine].data, "XA<XS'" , "XA<XS' "  );
		replace_Addr( lines[iLine].data, "XA<XK'" , "XA<XK' "  );
		replace_Addr( lines[iLine].data, "XA<XA'" , "XA<XA' "  );
		replace_Addr( lines[iLine].data, "XA<XD'" , "XA<XD' "  );
		replace_Addr( lines[iLine].data, "XA<XE'" , "XA<XE' "  );
		replace_Addr( lines[iLine].data, "XA<XF'" , "XA<XF' "  );
		replace_Addr( lines[iLine].data, "XD<"    , "XD< "     );
		replace_Addr( lines[iLine].data, "XD<XB+" , "XD<XB+ "  );
		replace_Addr( lines[iLine].data, "XD<XC+" , "XD<XC+ "  );
		replace_Addr( lines[iLine].data, "XD<XS+" , "XD<XS+ "  );
		replace_Addr( lines[iLine].data, "XD<XK+" , "XD<XK+ "  );
		replace_Addr( lines[iLine].data, "XD<XA+" , "XD<XA+ "  );
		replace_Addr( lines[iLine].data, "XD<XD+" , "XD<XD+ "  );
		replace_Addr( lines[iLine].data, "XD<XE+" , "XD<XE+ "  );
		replace_Addr( lines[iLine].data, "XD<XF+" , "XD<XF+ "  );
		replace_Addr( lines[iLine].data, "XD<XB-" , "XD<XB- "  );
		replace_Addr( lines[iLine].data, "XD<XC-" , "XD<XC- "  );
		replace_Addr( lines[iLine].data, "XD<XS-" , "XD<XS- "  );
		replace_Addr( lines[iLine].data, "XD<XK-" , "XD<XK- "  );
		replace_Addr( lines[iLine].data, "XD<XA-" , "XD<XA- "  );
		replace_Addr( lines[iLine].data, "XD<XD-" , "XD<XD- "  );
		replace_Addr( lines[iLine].data, "XD<XE-" , "XD<XE- "  );
		replace_Addr( lines[iLine].data, "XD<XF-" , "XD<XF- "  );
		replace_Addr( lines[iLine].data, "XD<XB&" , "XD<XB& "  );
		replace_Addr( lines[iLine].data, "XD<XC&" , "XD<XC& "  );
		replace_Addr( lines[iLine].data, "XD<XS&" , "XD<XS& "  );
		replace_Addr( lines[iLine].data, "XD<XK&" , "XD<XK& "  );
		replace_Addr( lines[iLine].data, "XD<XA&" , "XD<XA& "  );
		replace_Addr( lines[iLine].data, "XD<XD&" , "XD<XD& "  );
		replace_Addr( lines[iLine].data, "XD<XE&" , "XD<XE& "  );
		replace_Addr( lines[iLine].data, "XD<XF&" , "XD<XF& "  );
		replace_Addr( lines[iLine].data, "XD<XB\"", "XD<XB\" " );
		replace_Addr( lines[iLine].data, "XD<XC\"", "XD<XC\" " );
		replace_Addr( lines[iLine].data, "XD<XS\"", "XD<XS\" " );
		replace_Addr( lines[iLine].data, "XD<XK\"", "XD<XK\" " );
		replace_Addr( lines[iLine].data, "XD<XA\"", "XD<XA\" " );
		replace_Addr( lines[iLine].data, "XD<XD\"", "XD<XD\" " );
		replace_Addr( lines[iLine].data, "XD<XE\"", "XD<XE\" " );
		replace_Addr( lines[iLine].data, "XD<XF\"", "XD<XF\" " );
		replace_Addr( lines[iLine].data, "XD<XB'" , "XD<XB' "  );
		replace_Addr( lines[iLine].data, "XD<XC'" , "XD<XC' "  );
		replace_Addr( lines[iLine].data, "XD<XS'" , "XD<XS' "  );
		replace_Addr( lines[iLine].data, "XD<XK'" , "XD<XK' "  );
		replace_Addr( lines[iLine].data, "XD<XA'" , "XD<XA' "  );
		replace_Addr( lines[iLine].data, "XD<XD'" , "XD<XD' "  );
		replace_Addr( lines[iLine].data, "XD<XE'" , "XD<XE' "  );
		replace_Addr( lines[iLine].data, "XD<XF'" , "XD<XF' "  );
		replace_Addr( lines[iLine].data, "XE<"    , "XE< "     );
		replace_Addr( lines[iLine].data, "XE<XB+" , "XE<XB+ "  );
		replace_Addr( lines[iLine].data, "XE<XC+" , "XE<XC+ "  );
		replace_Addr( lines[iLine].data, "XE<XS+" , "XE<XS+ "  );
		replace_Addr( lines[iLine].data, "XE<XK+" , "XE<XK+ "  );
		replace_Addr( lines[iLine].data, "XE<XA+" , "XE<XA+ "  );
		replace_Addr( lines[iLine].data, "XE<XD+" , "XE<XD+ "  );
		replace_Addr( lines[iLine].data, "XE<XE+" , "XE<XE+ "  );
		replace_Addr( lines[iLine].data, "XE<XF+" , "XE<XF+ "  );
		replace_Addr( lines[iLine].data, "XE<XB-" , "XE<XB- "  );
		replace_Addr( lines[iLine].data, "XE<XC-" , "XE<XC- "  );
		replace_Addr( lines[iLine].data, "XE<XS-" , "XE<XS- "  );
		replace_Addr( lines[iLine].data, "XE<XK-" , "XE<XK- "  );
		replace_Addr( lines[iLine].data, "XE<XA-" , "XE<XA- "  );
		replace_Addr( lines[iLine].data, "XE<XD-" , "XE<XD- "  );
		replace_Addr( lines[iLine].data, "XE<XE-" , "XE<XE- "  );
		replace_Addr( lines[iLine].data, "XE<XF-" , "XE<XF- "  );
		replace_Addr( lines[iLine].data, "XE<XB&" , "XE<XB& "  );
		replace_Addr( lines[iLine].data, "XE<XC&" , "XE<XC& "  );
		replace_Addr( lines[iLine].data, "XE<XS&" , "XE<XS& "  );
		replace_Addr( lines[iLine].data, "XE<XK&" , "XE<XK& "  );
		replace_Addr( lines[iLine].data, "XE<XA&" , "XE<XA& "  );
		replace_Addr( lines[iLine].data, "XE<XD&" , "XE<XD& "  );
		replace_Addr( lines[iLine].data, "XE<XE&" , "XE<XE& "  );
		replace_Addr( lines[iLine].data, "XE<XF&" , "XE<XF& "  );
		replace_Addr( lines[iLine].data, "XE<XB\"", "XE<XB\" " );
		replace_Addr( lines[iLine].data, "XE<XC\"", "XE<XC\" " );
		replace_Addr( lines[iLine].data, "XE<XS\"", "XE<XS\" " );
		replace_Addr( lines[iLine].data, "XE<XK\"", "XE<XK\" " );
		replace_Addr( lines[iLine].data, "XE<XA\"", "XE<XA\" " );
		replace_Addr( lines[iLine].data, "XE<XD\"", "XE<XD\" " );
		replace_Addr( lines[iLine].data, "XE<XE\"", "XE<XE\" " );
		replace_Addr( lines[iLine].data, "XE<XF\"", "XE<XF\" " );
		replace_Addr( lines[iLine].data, "XE<XB'" , "XE<XB' "  );
		replace_Addr( lines[iLine].data, "XE<XC'" , "XE<XC' "  );
		replace_Addr( lines[iLine].data, "XE<XS'" , "XE<XS' "  );
		replace_Addr( lines[iLine].data, "XE<XK'" , "XE<XK' "  );
		replace_Addr( lines[iLine].data, "XE<XA'" , "XE<XA' "  );
		replace_Addr( lines[iLine].data, "XE<XD'" , "XE<XD' "  );
		replace_Addr( lines[iLine].data, "XE<XE'" , "XE<XE' "  );
		replace_Addr( lines[iLine].data, "XE<XF'" , "XE<XF' "  );
		replace_Addr( lines[iLine].data, "XF<"    , "XF< "     );
		replace_Addr( lines[iLine].data, "XF<XB+" , "XF<XB+ "  );
		replace_Addr( lines[iLine].data, "XF<XC+" , "XF<XC+ "  );
		replace_Addr( lines[iLine].data, "XF<XS+" , "XF<XS+ "  );
		replace_Addr( lines[iLine].data, "XF<XK+" , "XF<XK+ "  );
		replace_Addr( lines[iLine].data, "XF<XA+" , "XF<XA+ "  );
		replace_Addr( lines[iLine].data, "XF<XD+" , "XF<XD+ "  );
		replace_Addr( lines[iLine].data, "XF<XE+" , "XF<XE+ "  );
		replace_Addr( lines[iLine].data, "XF<XF+" , "XF<XF+ "  );
		replace_Addr( lines[iLine].data, "XF<XB-" , "XF<XB- "  );
		replace_Addr( lines[iLine].data, "XF<XC-" , "XF<XC- "  );
		replace_Addr( lines[iLine].data, "XF<XS-" , "XF<XS- "  );
		replace_Addr( lines[iLine].data, "XF<XK-" , "XF<XK- "  );
		replace_Addr( lines[iLine].data, "XF<XA-" , "XF<XA- "  );
		replace_Addr( lines[iLine].data, "XF<XD-" , "XF<XD- "  );
		replace_Addr( lines[iLine].data, "XF<XE-" , "XF<XE- "  );
		replace_Addr( lines[iLine].data, "XF<XF-" , "XF<XF- "  );
		replace_Addr( lines[iLine].data, "XF<XB&" , "XF<XB& "  );
		replace_Addr( lines[iLine].data, "XF<XC&" , "XF<XC& "  );
		replace_Addr( lines[iLine].data, "XF<XS&" , "XF<XS& "  );
		replace_Addr( lines[iLine].data, "XF<XK&" , "XF<XK& "  );
		replace_Addr( lines[iLine].data, "XF<XA&" , "XF<XA& "  );
		replace_Addr( lines[iLine].data, "XF<XD&" , "XF<XD& "  );
		replace_Addr( lines[iLine].data, "XF<XE&" , "XF<XE& "  );
		replace_Addr( lines[iLine].data, "XF<XF&" , "XF<XF& "  );
		replace_Addr( lines[iLine].data, "XF<XB\"", "XF<XB\" " );
		replace_Addr( lines[iLine].data, "XF<XC\"", "XF<XC\" " );
		replace_Addr( lines[iLine].data, "XF<XS\"", "XF<XS\" " );
		replace_Addr( lines[iLine].data, "XF<XK\"", "XF<XK\" " );
		replace_Addr( lines[iLine].data, "XF<XA\"", "XF<XA\" " );
		replace_Addr( lines[iLine].data, "XF<XD\"", "XF<XD\" " );
		replace_Addr( lines[iLine].data, "XF<XE\"", "XF<XE\" " );
		replace_Addr( lines[iLine].data, "XF<XF\"", "XF<XF\" " );
		replace_Addr( lines[iLine].data, "XF<XB'" , "XF<XB' "  );
		replace_Addr( lines[iLine].data, "XF<XC'" , "XF<XC' "  );
		replace_Addr( lines[iLine].data, "XF<XS'" , "XF<XS' "  );
		replace_Addr( lines[iLine].data, "XF<XK'" , "XF<XK' "  );
		replace_Addr( lines[iLine].data, "XF<XA'" , "XF<XA' "  );
		replace_Addr( lines[iLine].data, "XF<XD'" , "XF<XD' "  );
		replace_Addr( lines[iLine].data, "XF<XE'" , "XF<XE' "  );
		replace_Addr( lines[iLine].data, "XF<XF'" , "XF<XF' "  );

		replace_Addr( lines[iLine].data, "YC<"    , "YC< "     );
		replace_Addr( lines[iLine].data, "YC<YB+" , "YC<YB+ "  );
		replace_Addr( lines[iLine].data, "YC<YC+" , "YC<YC+ "  );
		replace_Addr( lines[iLine].data, "YC<YS+" , "YC<YS+ "  );
		replace_Addr( lines[iLine].data, "YC<YK+" , "YC<YK+ "  );
		replace_Addr( lines[iLine].data, "YC<YA+" , "YC<YA+ "  );
		replace_Addr( lines[iLine].data, "YC<YD+" , "YC<YD+ "  );
		replace_Addr( lines[iLine].data, "YC<YE+" , "YC<YE+ "  );
		replace_Addr( lines[iLine].data, "YC<YF+" , "YC<YF+ "  );
		replace_Addr( lines[iLine].data, "YC<YB-" , "YC<YB- "  );
		replace_Addr( lines[iLine].data, "YC<YC-" , "YC<YC- "  );
		replace_Addr( lines[iLine].data, "YC<YS-" , "YC<YS- "  );
		replace_Addr( lines[iLine].data, "YC<YK-" , "YC<YK- "  );
		replace_Addr( lines[iLine].data, "YC<YA-" , "YC<YA- "  );
		replace_Addr( lines[iLine].data, "YC<YD-" , "YC<YD- "  );
		replace_Addr( lines[iLine].data, "YC<YE-" , "YC<YE- "  );
		replace_Addr( lines[iLine].data, "YC<YF-" , "YC<YF- "  );
		replace_Addr( lines[iLine].data, "YC<YB&" , "YC<YB& "  );
		replace_Addr( lines[iLine].data, "YC<YC&" , "YC<YC& "  );
		replace_Addr( lines[iLine].data, "YC<YS&" , "YC<YS& "  );
		replace_Addr( lines[iLine].data, "YC<YK&" , "YC<YK& "  );
		replace_Addr( lines[iLine].data, "YC<YA&" , "YC<YA& "  );
		replace_Addr( lines[iLine].data, "YC<YD&" , "YC<YD& "  );
		replace_Addr( lines[iLine].data, "YC<YE&" , "YC<YE& "  );
		replace_Addr( lines[iLine].data, "YC<YF&" , "YC<YF& "  );
		replace_Addr( lines[iLine].data, "YC<YB\"", "YC<YB\" " );
		replace_Addr( lines[iLine].data, "YC<YC\"", "YC<YC\" " );
		replace_Addr( lines[iLine].data, "YC<YS\"", "YC<YS\" " );
		replace_Addr( lines[iLine].data, "YC<YK\"", "YC<YK\" " );
		replace_Addr( lines[iLine].data, "YC<YA\"", "YC<YA\" " );
		replace_Addr( lines[iLine].data, "YC<YD\"", "YC<YD\" " );
		replace_Addr( lines[iLine].data, "YC<YE\"", "YC<YE\" " );
		replace_Addr( lines[iLine].data, "YC<YF\"", "YC<YF\" " );
		replace_Addr( lines[iLine].data, "YC<YB'" , "YC<YB' "  );
		replace_Addr( lines[iLine].data, "YC<YC'" , "YC<YC' "  );
		replace_Addr( lines[iLine].data, "YC<YS'" , "YC<YS' "  );
		replace_Addr( lines[iLine].data, "YC<YK'" , "YC<YK' "  );
		replace_Addr( lines[iLine].data, "YC<YA'" , "YC<YA' "  );
		replace_Addr( lines[iLine].data, "YC<YD'" , "YC<YD' "  );
		replace_Addr( lines[iLine].data, "YC<YE'" , "YC<YE' "  );
		replace_Addr( lines[iLine].data, "YC<YF'" , "YC<YF' "  );
		replace_Addr( lines[iLine].data, "YS<"    , "YS< "     );
		replace_Addr( lines[iLine].data, "YS<YB+" , "YS<YB+ "  );
		replace_Addr( lines[iLine].data, "YS<YC+" , "YS<YC+ "  );
		replace_Addr( lines[iLine].data, "YS<YS+" , "YS<YS+ "  );
		replace_Addr( lines[iLine].data, "YS<YK+" , "YS<YK+ "  );
		replace_Addr( lines[iLine].data, "YS<YA+" , "YS<YA+ "  );
		replace_Addr( lines[iLine].data, "YS<YD+" , "YS<YD+ "  );
		replace_Addr( lines[iLine].data, "YS<YE+" , "YS<YE+ "  );
		replace_Addr( lines[iLine].data, "YS<YF+" , "YS<YF+ "  );
		replace_Addr( lines[iLine].data, "YS<YB-" , "YS<YB- "  );
		replace_Addr( lines[iLine].data, "YS<YC-" , "YS<YC- "  );
		replace_Addr( lines[iLine].data, "YS<YS-" , "YS<YS- "  );
		replace_Addr( lines[iLine].data, "YS<YK-" , "YS<YK- "  );
		replace_Addr( lines[iLine].data, "YS<YA-" , "YS<YA- "  );
		replace_Addr( lines[iLine].data, "YS<YD-" , "YS<YD- "  );
		replace_Addr( lines[iLine].data, "YS<YE-" , "YS<YE- "  );
		replace_Addr( lines[iLine].data, "YS<YF-" , "YS<YF- "  );
		replace_Addr( lines[iLine].data, "YS<YB&" , "YS<YB& "  );
		replace_Addr( lines[iLine].data, "YS<YC&" , "YS<YC& "  );
		replace_Addr( lines[iLine].data, "YS<YS&" , "YS<YS& "  );
		replace_Addr( lines[iLine].data, "YS<YK&" , "YS<YK& "  );
		replace_Addr( lines[iLine].data, "YS<YA&" , "YS<YA& "  );
		replace_Addr( lines[iLine].data, "YS<YD&" , "YS<YD& "  );
		replace_Addr( lines[iLine].data, "YS<YE&" , "YS<YE& "  );
		replace_Addr( lines[iLine].data, "YS<YF&" , "YS<YF& "  );
		replace_Addr( lines[iLine].data, "YS<YB\"", "YS<YB\" " );
		replace_Addr( lines[iLine].data, "YS<YC\"", "YS<YC\" " );
		replace_Addr( lines[iLine].data, "YS<YS\"", "YS<YS\" " );
		replace_Addr( lines[iLine].data, "YS<YK\"", "YS<YK\" " );
		replace_Addr( lines[iLine].data, "YS<YA\"", "YS<YA\" " );
		replace_Addr( lines[iLine].data, "YS<YD\"", "YS<YD\" " );
		replace_Addr( lines[iLine].data, "YS<YE\"", "YS<YE\" " );
		replace_Addr( lines[iLine].data, "YS<YF\"", "YS<YF\" " );
		replace_Addr( lines[iLine].data, "YS<YB'" , "YS<YB' "  );
		replace_Addr( lines[iLine].data, "YS<YC'" , "YS<YC' "  );
		replace_Addr( lines[iLine].data, "YS<YS'" , "YS<YS' "  );
		replace_Addr( lines[iLine].data, "YS<YK'" , "YS<YK' "  );
		replace_Addr( lines[iLine].data, "YS<YA'" , "YS<YA' "  );
		replace_Addr( lines[iLine].data, "YS<YD'" , "YS<YD' "  );
		replace_Addr( lines[iLine].data, "YS<YE'" , "YS<YE' "  );
		replace_Addr( lines[iLine].data, "YS<YF'" , "YS<YF' "  );
		replace_Addr( lines[iLine].data, "YK<"    , "YK< "     );
		replace_Addr( lines[iLine].data, "YK<YB+" , "YK<YB+ "  );
		replace_Addr( lines[iLine].data, "YK<YC+" , "YK<YC+ "  );
		replace_Addr( lines[iLine].data, "YK<YS+" , "YK<YS+ "  );
		replace_Addr( lines[iLine].data, "YK<YK+" , "YK<YK+ "  );
		replace_Addr( lines[iLine].data, "YK<YA+" , "YK<YA+ "  );
		replace_Addr( lines[iLine].data, "YK<YD+" , "YK<YD+ "  );
		replace_Addr( lines[iLine].data, "YK<YE+" , "YK<YE+ "  );
		replace_Addr( lines[iLine].data, "YK<YF+" , "YK<YF+ "  );
		replace_Addr( lines[iLine].data, "YK<YB-" , "YK<YB- "  );
		replace_Addr( lines[iLine].data, "YK<YC-" , "YK<YC- "  );
		replace_Addr( lines[iLine].data, "YK<YS-" , "YK<YS- "  );
		replace_Addr( lines[iLine].data, "YK<YK-" , "YK<YK- "  );
		replace_Addr( lines[iLine].data, "YK<YA-" , "YK<YA- "  );
		replace_Addr( lines[iLine].data, "YK<YD-" , "YK<YD- "  );
		replace_Addr( lines[iLine].data, "YK<YE-" , "YK<YE- "  );
		replace_Addr( lines[iLine].data, "YK<YF-" , "YK<YF- "  );
		replace_Addr( lines[iLine].data, "YK<YB&" , "YK<YB& "  );
		replace_Addr( lines[iLine].data, "YK<YC&" , "YK<YC& "  );
		replace_Addr( lines[iLine].data, "YK<YS&" , "YK<YS& "  );
		replace_Addr( lines[iLine].data, "YK<YK&" , "YK<YK& "  );
		replace_Addr( lines[iLine].data, "YK<YA&" , "YK<YA& "  );
		replace_Addr( lines[iLine].data, "YK<YD&" , "YK<YD& "  );
		replace_Addr( lines[iLine].data, "YK<YE&" , "YK<YE& "  );
		replace_Addr( lines[iLine].data, "YK<YF&" , "YK<YF& "  );
		replace_Addr( lines[iLine].data, "YK<YB\"", "YK<YB\" " );
		replace_Addr( lines[iLine].data, "YK<YC\"", "YK<YC\" " );
		replace_Addr( lines[iLine].data, "YK<YS\"", "YK<YS\" " );
		replace_Addr( lines[iLine].data, "YK<YK\"", "YK<YK\" " );
		replace_Addr( lines[iLine].data, "YK<YA\"", "YK<YA\" " );
		replace_Addr( lines[iLine].data, "YK<YD\"", "YK<YD\" " );
		replace_Addr( lines[iLine].data, "YK<YE\"", "YK<YE\" " );
		replace_Addr( lines[iLine].data, "YK<YF\"", "YK<YF\" " );
		replace_Addr( lines[iLine].data, "YK<YB'" , "YK<YB' "  );
		replace_Addr( lines[iLine].data, "YK<YC'" , "YK<YC' "  );
		replace_Addr( lines[iLine].data, "YK<YS'" , "YK<YS' "  );
		replace_Addr( lines[iLine].data, "YK<YK'" , "YK<YK' "  );
		replace_Addr( lines[iLine].data, "YK<YA'" , "YK<YA' "  );
		replace_Addr( lines[iLine].data, "YK<YD'" , "YK<YD' "  );
		replace_Addr( lines[iLine].data, "YK<YE'" , "YK<YE' "  );
		replace_Addr( lines[iLine].data, "YK<YF'" , "YK<YF' "  );
		replace_Addr( lines[iLine].data, "YA<"    , "YA< "     );
		replace_Addr( lines[iLine].data, "YA<YB+" , "YA<YB+ "  );
		replace_Addr( lines[iLine].data, "YA<YC+" , "YA<YC+ "  );
		replace_Addr( lines[iLine].data, "YA<YS+" , "YA<YS+ "  );
		replace_Addr( lines[iLine].data, "YA<YK+" , "YA<YK+ "  );
		replace_Addr( lines[iLine].data, "YA<YA+" , "YA<YA+ "  );
		replace_Addr( lines[iLine].data, "YA<YD+" , "YA<YD+ "  );
		replace_Addr( lines[iLine].data, "YA<YE+" , "YA<YE+ "  );
		replace_Addr( lines[iLine].data, "YA<YF+" , "YA<YF+ "  );
		replace_Addr( lines[iLine].data, "YA<YB-" , "YA<YB- "  );
		replace_Addr( lines[iLine].data, "YA<YC-" , "YA<YC- "  );
		replace_Addr( lines[iLine].data, "YA<YS-" , "YA<YS- "  );
		replace_Addr( lines[iLine].data, "YA<YK-" , "YA<YK- "  );
		replace_Addr( lines[iLine].data, "YA<YA-" , "YA<YA- "  );
		replace_Addr( lines[iLine].data, "YA<YD-" , "YA<YD- "  );
		replace_Addr( lines[iLine].data, "YA<YE-" , "YA<YE- "  );
		replace_Addr( lines[iLine].data, "YA<YF-" , "YA<YF- "  );
		replace_Addr( lines[iLine].data, "YA<YB&" , "YA<YB& "  );
		replace_Addr( lines[iLine].data, "YA<YC&" , "YA<YC& "  );
		replace_Addr( lines[iLine].data, "YA<YS&" , "YA<YS& "  );
		replace_Addr( lines[iLine].data, "YA<YK&" , "YA<YK& "  );
		replace_Addr( lines[iLine].data, "YA<YA&" , "YA<YA& "  );
		replace_Addr( lines[iLine].data, "YA<YD&" , "YA<YD& "  );
		replace_Addr( lines[iLine].data, "YA<YE&" , "YA<YE& "  );
		replace_Addr( lines[iLine].data, "YA<YF&" , "YA<YF& "  );
		replace_Addr( lines[iLine].data, "YA<YB\"", "YA<YB\" " );
		replace_Addr( lines[iLine].data, "YA<YC\"", "YA<YC\" " );
		replace_Addr( lines[iLine].data, "YA<YS\"", "YA<YS\" " );
		replace_Addr( lines[iLine].data, "YA<YK\"", "YA<YK\" " );
		replace_Addr( lines[iLine].data, "YA<YA\"", "YA<YA\" " );
		replace_Addr( lines[iLine].data, "YA<YD\"", "YA<YD\" " );
		replace_Addr( lines[iLine].data, "YA<YE\"", "YA<YE\" " );
		replace_Addr( lines[iLine].data, "YA<YF\"", "YA<YF\" " );
		replace_Addr( lines[iLine].data, "YA<YB'" , "YA<YB' "  );
		replace_Addr( lines[iLine].data, "YA<YC'" , "YA<YC' "  );
		replace_Addr( lines[iLine].data, "YA<YS'" , "YA<YS' "  );
		replace_Addr( lines[iLine].data, "YA<YK'" , "YA<YK' "  );
		replace_Addr( lines[iLine].data, "YA<YA'" , "YA<YA' "  );
		replace_Addr( lines[iLine].data, "YA<YD'" , "YA<YD' "  );
		replace_Addr( lines[iLine].data, "YA<YE'" , "YA<YE' "  );
		replace_Addr( lines[iLine].data, "YA<YF'" , "YA<YF' "  );
		replace_Addr( lines[iLine].data, "YD<"    , "YD< "     );
		replace_Addr( lines[iLine].data, "YD<YB+" , "YD<YB+ "  );
		replace_Addr( lines[iLine].data, "YD<YC+" , "YD<YC+ "  );
		replace_Addr( lines[iLine].data, "YD<YS+" , "YD<YS+ "  );
		replace_Addr( lines[iLine].data, "YD<YK+" , "YD<YK+ "  );
		replace_Addr( lines[iLine].data, "YD<YA+" , "YD<YA+ "  );
		replace_Addr( lines[iLine].data, "YD<YD+" , "YD<YD+ "  );
		replace_Addr( lines[iLine].data, "YD<YE+" , "YD<YE+ "  );
		replace_Addr( lines[iLine].data, "YD<YF+" , "YD<YF+ "  );
		replace_Addr( lines[iLine].data, "YD<YB-" , "YD<YB- "  );
		replace_Addr( lines[iLine].data, "YD<YC-" , "YD<YC- "  );
		replace_Addr( lines[iLine].data, "YD<YS-" , "YD<YS- "  );
		replace_Addr( lines[iLine].data, "YD<YK-" , "YD<YK- "  );
		replace_Addr( lines[iLine].data, "YD<YA-" , "YD<YA- "  );
		replace_Addr( lines[iLine].data, "YD<YD-" , "YD<YD- "  );
		replace_Addr( lines[iLine].data, "YD<YE-" , "YD<YE- "  );
		replace_Addr( lines[iLine].data, "YD<YF-" , "YD<YF- "  );
		replace_Addr( lines[iLine].data, "YD<YB&" , "YD<YB& "  );
		replace_Addr( lines[iLine].data, "YD<YC&" , "YD<YC& "  );
		replace_Addr( lines[iLine].data, "YD<YS&" , "YD<YS& "  );
		replace_Addr( lines[iLine].data, "YD<YK&" , "YD<YK& "  );
		replace_Addr( lines[iLine].data, "YD<YA&" , "YD<YA& "  );
		replace_Addr( lines[iLine].data, "YD<YD&" , "YD<YD& "  );
		replace_Addr( lines[iLine].data, "YD<YE&" , "YD<YE& "  );
		replace_Addr( lines[iLine].data, "YD<YF&" , "YD<YF& "  );
		replace_Addr( lines[iLine].data, "YD<YB\"", "YD<YB\" " );
		replace_Addr( lines[iLine].data, "YD<YC\"", "YD<YC\" " );
		replace_Addr( lines[iLine].data, "YD<YS\"", "YD<YS\" " );
		replace_Addr( lines[iLine].data, "YD<YK\"", "YD<YK\" " );
		replace_Addr( lines[iLine].data, "YD<YA\"", "YD<YA\" " );
		replace_Addr( lines[iLine].data, "YD<YD\"", "YD<YD\" " );
		replace_Addr( lines[iLine].data, "YD<YE\"", "YD<YE\" " );
		replace_Addr( lines[iLine].data, "YD<YF\"", "YD<YF\" " );
		replace_Addr( lines[iLine].data, "YD<YB'" , "YD<YB' "  );
		replace_Addr( lines[iLine].data, "YD<YC'" , "YD<YC' "  );
		replace_Addr( lines[iLine].data, "YD<YS'" , "YD<YS' "  );
		replace_Addr( lines[iLine].data, "YD<YK'" , "YD<YK' "  );
		replace_Addr( lines[iLine].data, "YD<YA'" , "YD<YA' "  );
		replace_Addr( lines[iLine].data, "YD<YD'" , "YD<YD' "  );
		replace_Addr( lines[iLine].data, "YD<YE'" , "YD<YE' "  );
		replace_Addr( lines[iLine].data, "YD<YF'" , "YD<YF' "  );
		replace_Addr( lines[iLine].data, "YE<"    , "YE< "     );
		replace_Addr( lines[iLine].data, "YE<YB+" , "YE<YB+ "  );
		replace_Addr( lines[iLine].data, "YE<YC+" , "YE<YC+ "  );
		replace_Addr( lines[iLine].data, "YE<YS+" , "YE<YS+ "  );
		replace_Addr( lines[iLine].data, "YE<YK+" , "YE<YK+ "  );
		replace_Addr( lines[iLine].data, "YE<YA+" , "YE<YA+ "  );
		replace_Addr( lines[iLine].data, "YE<YD+" , "YE<YD+ "  );
		replace_Addr( lines[iLine].data, "YE<YE+" , "YE<YE+ "  );
		replace_Addr( lines[iLine].data, "YE<YF+" , "YE<YF+ "  );
		replace_Addr( lines[iLine].data, "YE<YB-" , "YE<YB- "  );
		replace_Addr( lines[iLine].data, "YE<YC-" , "YE<YC- "  );
		replace_Addr( lines[iLine].data, "YE<YS-" , "YE<YS- "  );
		replace_Addr( lines[iLine].data, "YE<YK-" , "YE<YK- "  );
		replace_Addr( lines[iLine].data, "YE<YA-" , "YE<YA- "  );
		replace_Addr( lines[iLine].data, "YE<YD-" , "YE<YD- "  );
		replace_Addr( lines[iLine].data, "YE<YE-" , "YE<YE- "  );
		replace_Addr( lines[iLine].data, "YE<YF-" , "YE<YF- "  );
		replace_Addr( lines[iLine].data, "YE<YB&" , "YE<YB& "  );
		replace_Addr( lines[iLine].data, "YE<YC&" , "YE<YC& "  );
		replace_Addr( lines[iLine].data, "YE<YS&" , "YE<YS& "  );
		replace_Addr( lines[iLine].data, "YE<YK&" , "YE<YK& "  );
		replace_Addr( lines[iLine].data, "YE<YA&" , "YE<YA& "  );
		replace_Addr( lines[iLine].data, "YE<YD&" , "YE<YD& "  );
		replace_Addr( lines[iLine].data, "YE<YE&" , "YE<YE& "  );
		replace_Addr( lines[iLine].data, "YE<YF&" , "YE<YF& "  );
		replace_Addr( lines[iLine].data, "YE<YB\"", "YE<YB\" " );
		replace_Addr( lines[iLine].data, "YE<YC\"", "YE<YC\" " );
		replace_Addr( lines[iLine].data, "YE<YS\"", "YE<YS\" " );
		replace_Addr( lines[iLine].data, "YE<YK\"", "YE<YK\" " );
		replace_Addr( lines[iLine].data, "YE<YA\"", "YE<YA\" " );
		replace_Addr( lines[iLine].data, "YE<YD\"", "YE<YD\" " );
		replace_Addr( lines[iLine].data, "YE<YE\"", "YE<YE\" " );
		replace_Addr( lines[iLine].data, "YE<YF\"", "YE<YF\" " );
		replace_Addr( lines[iLine].data, "YE<YB'" , "YE<YB' "  );
		replace_Addr( lines[iLine].data, "YE<YC'" , "YE<YC' "  );
		replace_Addr( lines[iLine].data, "YE<YS'" , "YE<YS' "  );
		replace_Addr( lines[iLine].data, "YE<YK'" , "YE<YK' "  );
		replace_Addr( lines[iLine].data, "YE<YA'" , "YE<YA' "  );
		replace_Addr( lines[iLine].data, "YE<YD'" , "YE<YD' "  );
		replace_Addr( lines[iLine].data, "YE<YE'" , "YE<YE' "  );
		replace_Addr( lines[iLine].data, "YE<YF'" , "YE<YF' "  );
		replace_Addr( lines[iLine].data, "YF<"    , "YF< "     );
		replace_Addr( lines[iLine].data, "YF<YB+" , "YF<YB+ "  );
		replace_Addr( lines[iLine].data, "YF<YC+" , "YF<YC+ "  );
		replace_Addr( lines[iLine].data, "YF<YS+" , "YF<YS+ "  );
		replace_Addr( lines[iLine].data, "YF<YK+" , "YF<YK+ "  );
		replace_Addr( lines[iLine].data, "YF<YA+" , "YF<YA+ "  );
		replace_Addr( lines[iLine].data, "YF<YD+" , "YF<YD+ "  );
		replace_Addr( lines[iLine].data, "YF<YE+" , "YF<YE+ "  );
		replace_Addr( lines[iLine].data, "YF<YF+" , "YF<YF+ "  );
		replace_Addr( lines[iLine].data, "YF<YB-" , "YF<YB- "  );
		replace_Addr( lines[iLine].data, "YF<YC-" , "YF<YC- "  );
		replace_Addr( lines[iLine].data, "YF<YS-" , "YF<YS- "  );
		replace_Addr( lines[iLine].data, "YF<YK-" , "YF<YK- "  );
		replace_Addr( lines[iLine].data, "YF<YA-" , "YF<YA- "  );
		replace_Addr( lines[iLine].data, "YF<YD-" , "YF<YD- "  );
		replace_Addr( lines[iLine].data, "YF<YE-" , "YF<YE- "  );
		replace_Addr( lines[iLine].data, "YF<YF-" , "YF<YF- "  );
		replace_Addr( lines[iLine].data, "YF<YB&" , "YF<YB& "  );
		replace_Addr( lines[iLine].data, "YF<YC&" , "YF<YC& "  );
		replace_Addr( lines[iLine].data, "YF<YS&" , "YF<YS& "  );
		replace_Addr( lines[iLine].data, "YF<YK&" , "YF<YK& "  );
		replace_Addr( lines[iLine].data, "YF<YA&" , "YF<YA& "  );
		replace_Addr( lines[iLine].data, "YF<YD&" , "YF<YD& "  );
		replace_Addr( lines[iLine].data, "YF<YE&" , "YF<YE& "  );
		replace_Addr( lines[iLine].data, "YF<YF&" , "YF<YF& "  );
		replace_Addr( lines[iLine].data, "YF<YB\"", "YF<YB\" " );
		replace_Addr( lines[iLine].data, "YF<YC\"", "YF<YC\" " );
		replace_Addr( lines[iLine].data, "YF<YS\"", "YF<YS\" " );
		replace_Addr( lines[iLine].data, "YF<YK\"", "YF<YK\" " );
		replace_Addr( lines[iLine].data, "YF<YA\"", "YF<YA\" " );
		replace_Addr( lines[iLine].data, "YF<YD\"", "YF<YD\" " );
		replace_Addr( lines[iLine].data, "YF<YE\"", "YF<YE\" " );
		replace_Addr( lines[iLine].data, "YF<YF\"", "YF<YF\" " );
		replace_Addr( lines[iLine].data, "YF<YB'" , "YF<YB' "  );
		replace_Addr( lines[iLine].data, "YF<YC'" , "YF<YC' "  );
		replace_Addr( lines[iLine].data, "YF<YS'" , "YF<YS' "  );
		replace_Addr( lines[iLine].data, "YF<YK'" , "YF<YK' "  );
		replace_Addr( lines[iLine].data, "YF<YA'" , "YF<YA' "  );
		replace_Addr( lines[iLine].data, "YF<YD'" , "YF<YD' "  );
		replace_Addr( lines[iLine].data, "YF<YE'" , "YF<YE' "  );
		replace_Addr( lines[iLine].data, "YF<YF'" , "YF<YF' "  );

//		lines[iLine].data.Replace( "0x", " 0x" );
		replace_Hex( lines[iLine].data, "0x" , " 0x"  );

		lines[iLine].data.Replace( "#", " 0x" );

		lines[iLine].data.Replace( "ADDRESS DEFINE", "ADDRESS_DEFINE" );

//		lines[iLine].data.Replace( ";", " ; " );	//fatric 2022-11-01
		lines[iLine].data.Replace( "=", " " );
		lines[iLine].data.Replace( "\t", " " );
		lines[iLine].data.Replace( "\r", " " );
		lines[iLine].data.Replace( "\n", " " );
	}

if ( bDebug )
{
	printf("AF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}

	return true;
}

bool CAlpgCompiler::PreprocOutMode( atstring &subname, vector<pat_line_data> &lines )
{
	bool bDebug = false;

if ( bDebug )
{
	printf("BF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}

	char szHex[32];
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
	{
		for ( Str2WordMap::iterator it=burst_map.begin(); it!=burst_map.end(); ++it)
		{
			sprintf(szHex, "0x%X", it->second);
			replace_ds( lines[iLine].data, it->first.c_str(), szHex );
		}
	}

if ( bDebug )
{
	printf("AF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}

	return true;
}

bool CAlpgCompiler::SetSubCompile( atstring subname, vector<pat_line_data> lines )
{
//	if subname==HEX then {ALPG_MODULE_BEGIN, process, ALPG_MODULE_END }
//	else { subname<- flobal_label, sub_routine }
	bool bDebug = false;
	if( bDebug )
	{
		printf("BF--------------------\n");
		for(size_t iLine=0; iLine<lines.size(); ++iLine )
			printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
	}

	//1. %IFE Preprocess
	if ( ! PreprocIFE( subname, lines) ) return false;

	//2. SDEF Preprocess
	if ( ! PreprocSDEF( subname, lines) ) return false;

	//3. DSET Reg Preprocess
	if ( ! PreprocDsetReg( subname, lines) ) return false;

	//4. OUT MODE Preprocess
	if ( ! PreprocOutMode( subname, lines) ) return false;

	//5. Parse Pattern
	if ( ! ParsePattern( subname, lines) ) return false;

	if( bDebug )
	{
		printf("AF--------------------\n");
		for(size_t iLine=0; iLine<lines.size(); ++iLine )
			printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
	}
	return true;
}

void CAlpgCompiler::Init()
{
	CCompiler::Init();
	pattern.Init();
}

int CAlpgCompiler::GetIdxCompCond( atstring& strCond )
{
	char szConds[][2] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
						  "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
	for(size_t i=0; i<( sizeof(szConds)/sizeof(szConds[0]) ); ++i )
	{
		if ( ! strCond.compare(szConds[i]) )
			return i;
	}

	return -1;
}

int CAlpgCompiler::ParseIFE( atstring strExpr )
{
	strExpr.Replace("%IFE" , ""       );
	strExpr.Replace("("    , " ( "    );
	strExpr.Replace(")"    , " ) "    );
	strExpr.Replace(".AND.", " .AND. ");	//1
	strExpr.Replace(".OR." , " .OR. " );	//2
	strExpr.Replace(".XOR.", " .XOR. ");	//3
	strExpr.Replace("\t"   , " "      );
	strExpr.Replace(";"    , " ; "    );

	vector<atstring> v;
	strExpr.Split(v, " ");

	bool bEnd   = false;
	bool bError = false;
	bool bparentheses = false;
	int iGrade = 0;
	int iOpr   = 0;
	IFE_GREADE_LIST ife_grade_list;
	IFE_GRADE* pIfeGrade = NULL;
	for(vector<atstring>::size_type i=0; i<v.size(); ++i )
	{
		if ( i==0 ) pIfeGrade = new IFE_GRADE( iGrade++ );

		if ( bEnd ) { bError = true; break; }

		if ( ! v[i].compare("(") )
		{
			if ( bparentheses ) { bError = true; break; }

			if ( i > 0 )
			{
				ife_grade_list.push_back( pIfeGrade );

				pIfeGrade = new IFE_GRADE( iGrade++ );
				pIfeGrade->opr = iOpr;

				iOpr = 0;
			}

			bparentheses = true;
		}
		else if ( ! v[i].compare(")") )
		{
			if ( iOpr != 0 || ! bparentheses ) { bError = true; break; }

			ife_grade_list.push_back( pIfeGrade );
			bparentheses = false;

			pIfeGrade = new IFE_GRADE( iGrade++ );
		}
		else if( ! v[i].compare(".AND.") )
		{
			if ( iOpr != 0 ) { bError = true; break; }

			iOpr = 1;
		}
		else if( ! v[i].compare(".OR.") )
		{
			if ( iOpr != 0 ) { bError = true; break; }

			iOpr = 2;
		}
		else if( ! v[i].compare(".XOR.") )
		{
			if ( iOpr != 0 ) { bError = true; break; }

			iOpr = 3;
		}
		else if( ! v[i].compare(";") )
		{
			if ( iOpr != 0 ) { bError = true; break; }

			if ( pIfeGrade->ife_list.size() > 0 )
			{
				ife_grade_list.push_back( pIfeGrade );
				pIfeGrade = NULL;
			}

			bEnd = true;
		}
		else
		{
			int iCond = GetIdxCompCond( v[i] );
			if ( iCond < 0 ) { bError = true; break; }
			else
			{
				pIfeGrade->ife_list.push_back( new IFE_EMT(iOpr, iCond) );
				iOpr = 0;
			}
		}
	}

	if ( pIfeGrade ) { delete pIfeGrade; pIfeGrade = NULL; }

	if ( iOpr != 0 || bparentheses || ! bEnd ) bError = true;

	if ( bError )
	{
		for( IFE_GREADE_LIST::iterator it=ife_grade_list.begin(); it!=ife_grade_list.end(); ++it)
			 delete *it;

		ife_grade_list.clear();
		return -1;
	}

///////////////////////////////////////////////////////////////////////////
	int iResult = 0;
	for( IFE_GREADE_LIST::iterator it=ife_grade_list.begin(); it!=ife_grade_list.end(); ++it)
	{
		pIfeGrade = *it;
		int iiResult = iResult;
		for( IFE_LIST::iterator iit=pIfeGrade->ife_list.begin(); iit!=pIfeGrade->ife_list.end(); ++iit )
		{
			IFE_EMT* pEmt = *iit;

			switch ( pEmt->opr )
			{
				case 0:		//NO OPR
					iiResult  = (m_iCompOpt >> pEmt->r)&1;
					break;
				case 1:		// .AND.
					iiResult &= (m_iCompOpt >> pEmt->r)&1;
					break;
				case 2:		// .OR.
					iiResult |= (m_iCompOpt >> pEmt->r)&1;
					break;
				case 3:		// .XOR.
					iiResult ^= (m_iCompOpt >> pEmt->r)&1;
					break;
				default:
					return -1;
					break;
			}
		}

		switch ( pIfeGrade->opr )
		{
			case 0:		//NO OPR
				iResult = iiResult;
				break;
			case 1:		// .AND.
				iResult &= iiResult;
				break;
			case 2:		// .OR.
				iResult |= iiResult;
				break;
			case 3:		// .XOR.
				iResult ^= iiResult;
				break;
			default:
				return -1;
				break;
		}
	}

	for( IFE_GREADE_LIST::iterator it=ife_grade_list.begin(); it!=ife_grade_list.end(); ++it)
	{
		 delete *it;
	}
	ife_grade_list.clear();

	return iResult;
}

//Left Space + from + Right Space => to
inline void CAlpgCompiler::replace_ds( string& src, const char *from , const char *to )
{
	string str ;
	char *s = (char*)src.c_str();
	char *s1 ;
	int from_len = (int)strlen(from);

	if( ! (s1=strstr( s , from )) )
	{
		//printf( "[replace2] %s Not Found\n", from );
		return;
	}
	//printf( "[replace2] from : %s, to : %s\n", from, to );

	while( (s1=strstr( s , from )) )
	{
		if( isspace(*(s1-1)) && isspace(*(s1+from_len)) )
		{
			*s1=0;			//NULL Terminate: str에 추가 목적
			str += s ;		//찾은 지점 이전까지 str에 복사
			*s1 = *from ;	//NULL로 변환된 값을 원복
			str += to ;		//찾은 지점을 대상 스트링으로 치환
		}
		else
		{
			*s1=0;			//NULL Terminate: str에 추가 목적
			str += s ;		//찾은 지점 이전까지 str에 복사
			*s1 = *from ;	//NULL로 변환된 값을 원복
			str += from;
		}
		s = s1 + from_len ;
	}
	str += s ;
	src = str;
	//printf( "[replace2] end\n" );
	//*this = (const char *)str ;
	//return *this ;
}

bool CAlpgCompiler::isPreValid ( char pre )
{
//	char invalidCode[] = {'$','%','(',')',',','.','0','1','2','3','4','5','6','7','8','9',
	char invalidCode[] = {'#','$','%','(',')',',','.','0','1','2','3','4','5','6','7','8','9',
		':',';','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T',
		'U','V','W','X','Y','Z','[','\\',']','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}' };

// 	if( isdigit(pre) )	return false;		//D1 의 CHAR 가 숫자인 경우   - X
// 	if( isalpha(pre) )	return false;		//D1 의 CHAR 가 알파벹인 경우 - X

	for( size_t i=0; i<sizeof(invalidCode); ++i )
		if( pre == invalidCode[i] ) return false;

	return true;
}

bool CAlpgCompiler::isPostValid ( char post )
{
//	char invalidCode[] = {'$','%','(',')',',','.','0','1','2','3','4','5','6','7','8','9',
	char invalidCode[] = {'#','$','%','(',')',',','.','0','1','2','3','4','5','6','7','8','9',
		':',';','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T',
		'U','V','W','X','Y','Z','[','\\',']','_','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}' };

	//if( isdigit(post) )	return false;		//D1 의 CHAR 가 숫자인 경우   - X
	//if( isalpha(post) )	return false;		//D1 의 CHAR 가 알파벹인 경우 - X

	for( size_t i=0; i<sizeof(invalidCode); ++i )
		if( post == invalidCode[i] ) return false;

	return true;
}

inline void CAlpgCompiler::replace_PrimeReg(string &src, const char *from, const char *to)
{
	string str;
	char *s = (char*)src.c_str();
	char *s1;
	int from_len = (int)strlen(from);

	if( ! (s1=strstr( s , from )) )
	{
		//printf( "[replace3] %s Not Found\n", from );
		return;
	}

	while( (s1=strstr( s , from )) )
	{
		*s1=0;
		str += s ;
		*s1 = *from ;
		char cPre  = *(s1-1);
		char cPost = *(s1+strlen(from));


		//if( ! isalpha(*(s1+strlen(from))) && ! isalpha(*(s1-1)) ) {
		if( isPreValid(*(s1-1)) && isPostValid(*(s1+strlen(from))) )
			str += to ;
		else str += from ;

		s = s1 + from_len ;
	}
	str += s ;
	src = str;
}

inline void CAlpgCompiler::replace_Hex(string &src, const char *from, const char *to)
{
	string str;
	char *s = (char*)src.c_str();
	char *s1;
	int from_len = (int)strlen(from);

	if( ! (s1=strstr( s , from )) )
	{
		//printf( "[replace3] %s Not Found\n", from );
		return;
	}

	while( (s1=strstr( s , from )) )
	{
		*s1=0;
		str += s ;
		*s1 = *from ;
		char cPre  = *(s1-1);
		char cPost = *(s1+strlen(from));


		//if( ! isalpha(*(s1+strlen(from))) && ! isalpha(*(s1-1)) ) {
		if( isPreValid(*(s1-1)) )
			str += to ;
		else str += from ;

		s = s1 + from_len ;
	}
	str += s ;
	src = str;
}

inline void CAlpgCompiler::replace_DsetReg(string &src, const char *from, const char *to)
{
	string str ;
	char *s = (char*)src.c_str();
	char *s1 ;

	int from_len = (int)strlen(from);

	if( ! (s1=strstr( s , from )) )
	{
		//printf( "[replace] %s Not Found\n", from );
		return;
	}

	while( (s1=strstr( s , from )) )
	{
		*s1=0;
		str += s;
		*s1 = *from;

		if( isblank(*(s1-1))&&isdigit(*(s1+strlen(from))) )
		{
				if ( strlen(from)==2 && *(s1)=='Z'                                 && *(s1+strlen(from))=='0' )	//Z<0
			   str += from;
		   else if ( strlen(from)==3 && *(s1)=='T' && *(s1+1)=='P'                 && *(s1+strlen(from))=='0' )	//TP<0
			   str += from;
		   else if ( strlen(from)==4 && *(s1)=='T' && *(s1+1)=='P' && *(s1+2)=='1' && *(s1+strlen(from))=='0' )	//TP1<0
			   str += from;
		   else if ( strlen(from)==4 && *(s1)=='T' && *(s1+1)=='P' && *(s1+2)=='2' && *(s1+strlen(from))=='0' )	//TP2<0
			   str += from;
		   else
			   str += to;
		}
		else
			str += from;

		s = s1 + from_len;
	}
	str += s ;
	src = str;
}

inline void CAlpgCompiler::replace_Dbmap(string &src, const char *from, const char *to)
{
	string str ;
	char *s = (char*)src.c_str();
	char *s1 ;

	int from_len = (int)strlen(from);

	if( ! (s1=strstr( s , from )) )
	{
		//printf( "[replace] %s Not Found\n", from );
		return;
	}

	while( (s1=strstr( s , from )) )
	{
		*s1=0;
		str += s;
		*s1 = *from;

		if( isblank(*(s1-1))&&isdigit(*(s1+strlen(from))) )
		{
				 if ( strlen(from)== 6 && *(s1+strlen(from))=='0' && isblank(*(s1+strlen(from)+1)) )	//DBMAP<0
			   str += from;
			else if ( strlen(from)==12 && *(s1+strlen(from))=='1' && isblank(*(s1+strlen(from)+1)) )	//DBMAP<DBMAP+1, DBMAP<DBMAP-1
			   str += from;
			else
			   str += to;
		}
		else
			str += from;

		s = s1 + from_len;
	}
	str += s ;
	src = str;
}

inline void CAlpgCompiler::replace_Addr(string &src, const char *from, const char *to)
{
	string str ;
	char *s = (char*)src.c_str();
	char *s1 ;

	int from_len = (int)strlen(from);

	if( ! (s1=strstr( s , from )) )
	{
		//printf( "[replace] %s Not Found\n", from );
		return;
	}

	while( (s1=strstr( s , from )) )
	{
		*s1=0;
		str += s;
		*s1 = *from;

		if( isblank(*(s1-1))&&isdigit(*(s1+strlen(from))) )
		{
				 if ( strlen(from)== 3 && *(s1+strlen(from))=='0' && isblank(*(s1+strlen(from)+1)) )	//XB<0, XC<0,
			   str += from;
			else if ( strlen(from)== 6 && *(s1+strlen(from))=='1' && isblank(*(s1+strlen(from)+1)) )	//XB<XB+1, XB<XB-1, XC,...
			   str += from;
			else
			   str += to;
		}
		else
			str += from;

		s = s1 + from_len;
	}
	str += s ;
	src = str;
}

#define add_error(no, msg, syntax)		add_err(subname, lines[iLine].line, msg, syntax)

//current_pc -> iCurrPc
//strcpy_s   -> strncpy
bool CAlpgCompiler::ParsePattern( atstring &subname, vector<pat_line_data> &lines )
{
	bool bDebug = false;

if ( bDebug )
{
	printf("BF--------------------\n");
	for(size_t iLine=0; iLine<lines.size(); ++iLine )
		printf("[%d]{%s}\n", lines[iLine].line, (char *)lines[iLine].data);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NoLabelList		noLabel;
	NoLabelList		noLabelJSR;
	bool            bRegArea = false;
	PGDATA&			patData =  pattern;
	ALPG*			alpg    = &patData.alpg;
	unsigned int	dw_value;
	char			*endptr;
	unsigned int    iCurrPc = 0;
	int				iWay = 0;

//	if ( subname.Trim().Left(1).compare("#")==0 )
	{
		alpg = new ALPG;
		alpg->pc  = m_iStartPc + iCurrPc;
//		alpg->sta = dw_value;
		sprintf( alpg->name, "%s", subname.c_str() );

		patData.module.push_back( alpg );

//		char szTmp[32];
//		sprintf(szTmp, "0x%s", (char *)subname.Trim().Mid(1) );
//		dw_value = strtoul( szTmp, &endptr, 0);
//
//		if ( *endptr )
//		{
//			add_err( subname, lines[0].line, "SUB name error", lines[0].data );
//			return false;
//		}
//		else
//		{
//			alpg = new ALPG;
//			alpg->pc  = m_iStartPc + iCurrPc;
//			alpg->sta = dw_value;
//			sprintf( alpg->name, "%s", (char *)subname);
//
//			patData.module.push_back( alpg );
//		}
	}

	if ( sub_map.find(subname) != sub_map.end() )
	{
		add_err( subname, lines[0].line, "SUB redefined", lines[0].data );
		return false;
	}
	else
	{
		sub_map[ subname ] = m_iStartPc + iCurrPc;
	}

	//patData.pcs = new PcList;
	PCS*	ram = new PCS;

	Str2WordMap	labelMap;

	for(size_t iLine=0; iLine<lines.size(); ++iLine )
	{
//		printf("\n(%ld) ", iLine);
		if ( lines[iLine].data.empty() ) continue;

		//2023.08.28 - KWMOON( LABEL + OPCODE NO SPACE - LBEL:JSR CLEAR)
		atstring sline = lines[iLine].data;
		atstring sdata = "";
		std::string::size_type pos = sline.find(":");
		if( pos != std::string::npos )
		{
			sdata = sline.Left(pos+1);
			sdata += " ";
			sdata += sline.Mid(pos+1);
		}
		else
		{
			sdata = sline;
		}

		char *in_src = new char [ sdata.length()+1 ];
		//char *in_src = new char [ lines[iLine].data.length()+1 ];
		strncpy(in_src, sdata.c_str(), sdata.length());
		in_src[ sdata.length() ] = '\0';
//		printf("{%s} ", in_src );

		char *word = strtok( in_src , " " );
		while( word )
		{
//			printf("[%s] ", word );
			PGCMD cmd;

			Str2WordMap::iterator it = syntax_map.find(word);
			if(it == syntax_map.end() )
			{
				if ( word[strlen(word)-1]==':') //ALPG_LABEL
				{
					word[strlen(word)-1] = '\0';
					if ( labelMap.find(word) != labelMap.end() )
						add_error( 103 , "label redefined" , word );
					else
						labelMap[ word ] = iCurrPc;	//module label
				}
				else
				{
					add_error( 101 , "unknown syntax" , word );
				}

				word = macro( NULL );
				continue;
			}
			else
			{
				cmd.value = it->second;
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			switch( cmd.command )
			{
				case ALPG_REG :
				case ALPG_MPAT :
					if ( cmd.opt == 1 )
					{
						word = macro( NULL );
						strncpy( patData.name, word, sizeof(patData.name) );
					}
					break;
				case ALPG_OPLINE :
					if ( ram->op_use )
					{
/////////////////////////////////////////////////////////////////////////
						bool bR   = false;
						bool bJNI = false;
						bool bRMW = false;

						if ( (ram->way[iWay].mut_ctl&0x280000)!=0x280000 && ram->way[iWay].mut_ctl&0x200000 ) bR   = true;	//R(C21)
						if ( (ram->opcode&0xffff) == 0x0101   ) bJNI = true;	//JNIn
						if ( bR && bJNI )
						{
//							unsigned int iTargetPc = ram->operand - m_iStartPc + 1;	//Label PC + 1
							unsigned int iTargetPc = ram->operand + 1;	//Label PC + 1
							if ( ram->operand>=m_iStartPc && iTargetPc < patData.pcs->size() )
							{
								PCS *pTarget = patData.GetPCS(iTargetPc);

								if ( pTarget )
								{
									if( 0 ) fprintf( stdout, "operand:0x%04X m_iStartPc:0x%05X\n", ram->operand, m_iStartPc );
									if( 0 ) fprintf( stdout, "TARGET PC:%04d[0x%04X] OPCODE:0x%05X ", iTargetPc, iTargetPc, pTarget->opcode );
									if( 0 ) fprintf( stdout, "MUT:0x%06X ", pTarget->way[0].mut_ctl );
									if( 0 ) fprintf( stdout, "TS:0x%02X\n", pTarget->way[0].ts );

									if ( (pTarget->way[0].mut_ctl&0x280000)!=0x280000 && pTarget->way[0].mut_ctl&0x80000 )	//W(C19)
										bRMW = true;
								}
							}
						}
/////////////////////////////////////////////////////////////////////////
						bool bW   = false;
						PCS *pTarget1 = NULL;
						if ( (ram->way[iWay].mut_ctl&0x280000)!=0x280000 && ram->way[iWay].mut_ctl&0x80000 ) bW = true;	//W(C19)
//						if ( bW )
//							fprintf( stdout, "iCurrPc:%d m_iStartPc:%d\n", iCurrPc, m_iStartPc );

						if ( bW && iCurrPc >= 2 )
						{
//							fprintf( stdout, "iCurrPc:%d m_iStartPc:%d\n", iCurrPc, m_iStartPc );
								 pTarget1 = patData.GetPCS(m_iStartPc+iCurrPc -1 );
							PCS *pTarget2 = patData.GetPCS(m_iStartPc+iCurrPc -2 );

							if ( pTarget1 && pTarget2 )
							{
								if( 0 ) fprintf( stdout, "pTarget1 TS:0x%02X\n", pTarget1->way[0].ts );
								if( 0 ) fprintf( stdout, "pTarget2 TS:0x%02X\n", pTarget2->way[0].ts );
								if ( pTarget1->opcode==0xa && pTarget2->opcode==0xa )
								{
									 if ( (pTarget2->way[0].mut_ctl&0x280000)!=0x280000 && pTarget2->way[0].mut_ctl&0x200000 )	//R(C21)
										bRMW = true;
								}
							}
						}
/////////////////////////////////////////////////////////////////////////

						if ( bRMW )
						{
							if ( bR )
							{
								PCS *pDummy = new PCS;
								pDummy->op_use     = ram->op_use;
								pDummy->opcode     = ram->opcode;
								pDummy->operand    = ram->operand;
								pDummy->way[0].cyp = ram->way[0].cyp;
								pDummy->way[0].ts  = ram->way[0].ts;

								ram->opcode  = 0xa;	ram->operand = 0x0;		//JNIn -> NOP

								patData.pcs->push_back( ram    );
								patData.pcs->push_back( pDummy );
								iCurrPc += 2;
							}
							else	//bW
							{
								PCS *pDummy = new PCS;
								pDummy->op_use     = pTarget1->op_use;
								pDummy->opcode     = pTarget1->opcode;
//								pDummy->operand    = pTarget1->operand;
								pDummy->way[0].cyp = pTarget1->way[0].cyp;
								pDummy->way[0].ts  = pTarget1->way[0].ts;

								patData.pcs->push_back( pDummy );
								patData.pcs->push_back( ram    );
								iCurrPc += 2;
							}
						}
						else
						{
/////////////////////////////////////////////////////////////////////////
							patData.pcs->push_back( ram );
							++iCurrPc;
						}

						ram = new PCS;
					}
					break;
				case ALPG_TEMP :
					if( cmd.opt ) word = macro( NULL );

					if (cmd.bit==REGISTER ) bRegArea = true; //for REGISTER area process
					break ;
				case SEQ_MODE :		//PAUSE, REFREAHA, REFRESHB
					alpg->seq_mode  |= cmd.bit; //SEQ_MODE
					break;
				case ALPG_MODE :	//TPM18, TPM36
					alpg->alpg_mode |= cmd.bit; //ALPG_MODE
					break ;
				case ALPG_REG0 : {
					//PORT
					word = macro( NULL );
					dw_value = strtoul( word, &endptr,0) ;
					if( *endptr)
					{
						add_error( 124 , "syntax error" , word );
						break ;
					}
					REGI *reg = new REGI;
					reg->addr = dw_value;

					word = macro( NULL );
					dw_value = strtoul( word, &endptr,0) ;
					if( *endptr) {
						delete reg ;
						add_error( 125 , "syntax error" , word );
						break ;
					}
					reg->i2 = dw_value ;
					alpg->reg_list.push_back( reg ) ;
					break ; }
				case ALPG_REG2 :
				case ALPG_REG4 :
					if ( cmd.opt == 0  )
					{
						word = macro( NULL );
						REGI *reg=0 ;
						if ( cmd.command == ALPG_REG4 )	//IDX1..16, MLOOP
						{
							dw_value = strtoul( word, &endptr,0) ;
							if ( *endptr==0)
							{
								reg = new REGI;
								reg->addr = cmd.bit + BIT_ALPG::REG;
								reg->i2   = dw_value;
								alpg->reg_list.push_back( reg );
							}
							else add_error( 107 , "syntax error", word );
						}
						else if( cmd.command == ALPG_REG2 )
						{
	//						STA, ISP, BAR, CFLG, TIMER, TREFRALPG_TEMPESH, TPAUSE, TJET, REFRESH/PAUSE MODE
							double dTime;
							//TIMER, TREFRESH, TPAUSE, TJET(GAIA Only)
							if ( cmd.bit == BIT_ALPG::BIT_TIMER    ||
								 cmd.bit == BIT_ALPG::BIT_TREFRESH ||
								 cmd.bit == BIT_ALPG::BIT_TPAUSE   ||
								 cmd.bit == BIT_ALPG::BIT_TJET        )
								dTime = strtod( word, &endptr);
							else
								dw_value = strtoul( word, &endptr, 0);

							if( isdigit(word[0]) && (*endptr=='P' || *endptr=='N' || *endptr=='U' || *endptr=='M' || *endptr=='S') )
							{
								unsigned int uTime;

								//100PS Resolution
								//PS UNIT Conversion
									 if ( *endptr=='P') dTime /=         100.0;
								else if ( *endptr=='N') dTime *=          10.0;
								else if ( *endptr=='U') dTime *=       10000.0;
								else if ( *endptr=='M') dTime *=    10000000.0;
								else if ( *endptr=='S') dTime *= 10000000000.0;

								//MAX: 429.4967295 MS
								uTime = (unsigned int)(dTime); //100PS Resolution
	////////////////////////////////////////////////////////////////////////////
	//							if( ! bDone )
								{
									//add_error( 108 , "timer set error" , word );
									reg = new REGI;
									reg->addr = cmd.bit | BIT_ALPG::REG;
									reg->i2   = uTime;

									// MODE REFRESHA/B, MODE PAUSE 에서 처리
	//								if ( reg->addr == ALPG::TREFRESH ) alpg->seq_mode |= 0x10; //MODE REFRESHA/B
	//								if ( reg->addr == ALPG::TPAUSE   ) alpg->seq_mode |= 0x20; //MODE PAUSE

									alpg->reg_list.push_back( reg ) ;
								}
							}
							else	//CFLG, ISP, BAR
							{
								reg = new REGI ;
								reg->addr = cmd.bit | BIT_ALPG::REG;

								if( *endptr==0)
								{
									reg->i2 = dw_value;
									alpg->reg_list.push_back( reg );
									//printf( "[ALPG_REG4_1] ADDR =0x%X, i2 = 0x%X\n",reg->addr, reg->i2 );
								}
								else if( *endptr )
								{
									//ISP
									NOLABEL *nolabel = new NOLABEL;
									nolabel->pc  = m_iStartPc + iCurrPc;
//									nolabel->reg = (unsigned int *)(int)&reg->i2 ;
									nolabel->reg = (unsigned int *)(unsigned long long)&reg->i2 ;
									strcpy( nolabel->label, word );
//									strcpy_s( nolabel->label, sizeof(nolabel->label), word );
									noLabel.push_back( nolabel ) ;
									alpg->reg_list.push_back( reg ) ;
									//printf( "[ALPG_REG4_1] ADDR =0x%X, i2 = 0x%X\n",reg->addr, reg->i2 );
								}
							}
						}
						else add_error( 109, "syntax error", word );
					}
					else if( cmd.opt == 2 )	// DRE1..3, CPE1..8
					{
						word = macro( NULL );

						PGCMD cm;
						Str2WordMap::iterator it = syntax_map.find(word);
	//					if(! syntax_map.Lookup(word , cm.value) )
						if( it == syntax_map.end() )
						{
							add_error( 101, "syntax error", word );
							word = macro( NULL );
							continue;
						}
						else
							cm.value = it->second;

						if(  cm.command != ALPG_MUT ) add_error( 110, "register mut set error", word );
						else
						{
	//////////////////////////////////////////////////////////////////////////////////////
							if ( cm.opt!=0 && cm.opt!=3)	//W,R, M1, M2 error
							{
								add_error( 110, "register mut set error", word );
								word = macro( NULL );
								continue;
							}
							else							//C0..C23, FIXH, FIXL
							{
								REGI *reg = new REGI;
								reg->addr = BIT_ALPG::DRE + cmd.bit*4;
								reg->i2   = cm.bit;
								alpg->reg_list.push_back( reg );
							}
						}
					}
					else if( cmd.opt == 1 )
					{
	//					D1B..H , D2B..D, LMAX, HMAX, XMAX, YMAX, ZMAX
	//					ND2..32, BD2..32
	//					L1A..H, ~ L4A..H
	//					S1A, S_TEMP, V1A, V_TEMP
						word = macro( NULL );
						dw_value = strtoul( word, &endptr, 0);
						if( *endptr==0 )
						{
							REGI *regM = new REGI;
							regM->addr = cmd.bit | BIT_ALPG::REG_D; //Main
							regM->i2 = dw_value;
							alpg->reg_list.push_back( regM ) ;

	//						if ( ilmode>1 ) // 4WAY, MC
	//						{
	//							REGI *regS = new REGI ;
	//							regS->addr = cmd.bit + BIT_ALPG::REG_D_SUB; //Sub
	//							regS->i2 = dw_value;
	//							alpg->reg_list.push_back( regS ) ;
	//						}
						}
						else add_error( 111 , "syntax error" , word );
					}
					else if( cmd.opt == 3 )
					{
	////					START
	//					word = macro( NULL );
	//					dw_value = strtoul( word, &endptr,0) ;
	//					if( cmd.bit ==1) alpg->sta = dw_value ;

						bRegArea = false; //for REGISTER area process
					}
					break ;
				case APLG_ADDRESS :
				{
					word = macro( NULL );
					while( word[0] == '\n'  || word[0] == ';' ) {
						word = macro( NULL );
//						total_line++ ;
					}

					int st=0,end=0,y=0,z=0 ;
					int zst=0,zend=0 ;
					do
					{
						zst=zend=z=0;

							 if( sscanf( word , "X%d-%d" , &st , &end ) == 2 ) y=1 ;
						else if( sscanf( word , "Y%d-%d" , &st , &end ) == 2 ) y=2 ;
						else if( sscanf( word , "X%d" , &st ) == 1 ) y=1 , end=st ;
						else if( sscanf( word , "Y%d" , &st ) == 1 ) y=2 , end=st;
						else
							add_error(112 , "Address define syntax error" , word );

						if ( y ) word = macro( NULL );
						if ( !word ) break;

						while( word[0] == '\n'  || word[0] == ';' ) {
							word = macro( NULL );
							if ( !word ) break;
//							total_line++ ;
						}
						if ( !word ) break;

							 if( sscanf( word , "X%d-%d" , &zst , &zend ) == 2 ) z=BIT_ALPG::ADDR_DEF_Z_OFF;
						else if( sscanf( word , "Y%d-%d" , &zst , &zend ) == 2 ) z=BIT_ALPG::ADDR_DEF_Z_OFF;
						else if( sscanf( word , "Z%d-%d" , &zst , &zend ) == 2 ) z=BIT_ALPG::ADDR_DEF_Z_OFF;
						else if( sscanf( word , "N%d-%d" , &zst , &zend ) == 2 ) z=BIT_ALPG::ADDR_DEF_N_OFF;
						else if( sscanf( word , "B%d-%d" , &zst , &zend ) == 2 ) z=BIT_ALPG::ADDR_DEF_B_OFF;
						else if( sscanf( word , "X%d"    , &zst         ) == 1 ) z=BIT_ALPG::ADDR_DEF_Z_OFF, zend=zst;
						else if( sscanf( word , "Y%d"    , &zst         ) == 1 ) z=BIT_ALPG::ADDR_DEF_Z_OFF, zend=zst;
						else if( sscanf( word , "N%d"    , &zst         ) == 1 ) z=BIT_ALPG::ADDR_DEF_N_OFF, zend=zst;
						else if( sscanf( word , "B%d"    , &zst         ) == 1 ) z=BIT_ALPG::ADDR_DEF_B_OFF, zend=zst;
						else if( strcmp( word , "0"                     ) == 0 ) z=-1;
						else if( strcmp( word , "X"                     ) == 0 ) z=-1;
						else if( strcmp( word , "Y"                     ) == 0 ) z=-1;

						if( z<0 )
						{
							if( y == 1 ) for( ;st<=end;st++ ) alpg->addr.x[st] = 0;
							if( y == 2 ) for( ;st<=end;st++ ) alpg->addr.y[st] = 0;
						}
						else {
							if( y == 1 ) for( ;zst<=zend;zst++ ) alpg->addr.x[st++] = (1+zst + z);
							if( y == 2 ) for( ;zst<=zend;zst++ ) alpg->addr.y[st++] = (1+zst + z);
						}

						word = macro( NULL );
						if ( !word ) break;
						while( word[0] == '\n'  || word[0] == ';' )
						{
							word = macro( NULL );
							if ( !word ) break;
//							total_line++ ;
						}
						if ( !word ) break;

						if ( strcmp(word, "MODULE_BEGIN")==0 || strcmp(word, "REGISTER")==0 || strcmp(word, "START")==0 ) break;
					}
					while ( !(word[0]=='X'&& word[1]=='Y') ) ;

					word = macro( NULL );
					if ( !word ) break;

						 if( (strcmp( word, "ZN"      ) == 0) || (strcmp( word, "ZNB"      ) == 0) ) alpg->addr.mode |=   0x1 ;
					else if( (strcmp( word, "XY.OR.ZN") == 0) || (strcmp( word, "XY.OR.ZNB") == 0) ) alpg->addr.mode &= ~(0x1);
	//				else add_error(112 , "Address define syntax error(XY=??)" , word );

					REGI *reg ;
					for( z=0;z<24;z+=4)
					{
						reg = new REGI ;
						reg->addr  = BIT_ALPG::ADDR_DEF_X + (z/4)*4 ; // Address define X
						reg->i2    =  alpg->addr.x[z  ];
						reg->i2   |= (alpg->addr.x[z+1]<< 8);
						reg->i2   |= (alpg->addr.x[z+2]<<16);
						reg->i2   |= (alpg->addr.x[z+3]<<24);
						alpg->reg_list.push_back( reg );
						reg = new REGI ;
						reg->addr  = BIT_ALPG::ADDR_DEF_Y + (z/4)*4 ; // Address define Y
						reg->i2    =  alpg->addr.y[z  ];
						reg->i2   |= (alpg->addr.y[z+1]<< 8);
						reg->i2   |= (alpg->addr.y[z+2]<<16);
						reg->i2   |= (alpg->addr.y[z+3]<<24);
						alpg->reg_list.push_back( reg );
					}
					break ;
				}
				case ALPG_OPCODE:
				{
					if( ram->op_use)
					{
						add_error(200 , "opcode redefinition" , word );
						break;
					}

					//if ( cmd.bit == 0x0e00 && cmd.opt==1 ) //OUT
					//{
					//	//RPT 8 TS0; //Insert pattern
					//	ram->op_use       = 1;
					//	ram->opcode       = 0x90102;	//RPT
					//	ram->operand      = 9;			// 9
					//	ram->way[iWay].ts = 1;			// TS1
					//
					//	//ALPG_OPLINE
					//	patData.pcs->push_back( ram );
					//	++iCurrPc;
					//
					//	ram = new PCS;
					//}

					int m = (cmd.bit>>12)&0x0f;
					int n =  cmd.bit     &0xff;
					int O = (cmd.opt>>6) &0x03;

					ram->op_use = 1;
//					ram->opcode =                   (cmd.bit>>8)&0xf  | (m<<8) | (n<<16);
//					ram->opcode = (cmd.opt&0x10) | ((cmd.bit>>8)&0xf) | (m<<8) | (n<<16);
					ram->opcode = (cmd.opt&0x10) | ((cmd.bit>>8)&0xf) | (m<<8) | (n<<16) | (O<<12);

//					printf("OPCODE=%x cmd.opt=%x cmd.bit=%x\n" ,ram->opcode, cmd.opt, cmd.bit );
					//printf("OPCODE=%d %x - %x\n" ,cmd.bit>>8 ,  opcode[cmd.bit>>8] ,ram->opcode );


					if( cmd.opt == 1 || cmd.opt == 3 ) //IDXI, LDI, STI, OUT
					{ //
						word = macro( NULL );
						dw_value = strtoul( word, &endptr, 0) ;
						if( *endptr ) add_error(201, "syntax error", word );
						else          ram->operand = dw_value;
					}
					if( cmd.opt == 2 || cmd.opt == 4 || cmd.opt == 8 || cmd.opt == 0x20 || cmd.opt == 0x40 || cmd.opt == 0x80 )	//JNI, JNInI, JNInD, JSR, JNC, JZD, JET, JMP, STISP, STBAR
					{
						word = macro( NULL );
						if( word[0] == '.' )
						{
							if( word[1] == '+' || word[1] == '-' )
							{
								dw_value = strtol( word+1, &endptr,0);
								if( *endptr ) add_error(201, "syntax error", word );
								ram->operand = m_iStartPc + iCurrPc + dw_value;
							}
							else ram->operand = m_iStartPc + iCurrPc;
//							printf("OPCODE[%s] ram->operand=%x \n", word, ram->operand);
						}
						else
						{
							dw_value = strtoul( word, &endptr, 0);
							if( *endptr == 0 ) ram->operand = dw_value;
							else
							{
//								printf("OPCODE: [%s]\n", word);
								if( labelMap.find(word) != labelMap.end() )				//LABEL PC(module label)
								{
									ram->operand = m_iStartPc + (labelMap.find(word))->second;
//									printf("labelMap operand: %d\n", ram->operand);
								}
								else if( sub_map.find(word) != sub_map.end() )		//LABEL PC(global label)
								{
//									ram->operand = start_pc + (labelMap_global.find(word))->second;
									ram->operand = (sub_map.find(word))->second;
//									printf("sub_map operand: %d\n", ram->operand);
								}
								else
								{
									NOLABEL *nolabel = new NOLABEL;
									nolabel->pc  = m_iStartPc + iCurrPc;
									nolabel->reg = &ram->operand;
									strcpy( nolabel->label, word );

									if ( cmd.bit==0x0500 )	//JSR
										noLabelJSR.push_back( nolabel );
									else
										noLabel.push_back( nolabel );
//									printf("nolabel pc: %d\n", nolabel->pc);
								}
							}
						}
					}
				}
					break;
				case ALPG_CONTROL:
					//TM1, TM2, TM3, T, I
					if ( cmd.opt==0 ) ram->opcode |= cmd.bit<<24;
					break;
				case ALPG_MUT :
						 if( cmd.opt == 0 )	ram->way[iWay].mut_ctl |= ( 1<<(cmd.bit&0x1f) ); //C0~C31
					else if( cmd.opt == 1 )	ram->way[iWay].mut_rwm |= ( 1<<(cmd.bit&0x1f) ); //W, R, M1, M2
					else																					 //FIXL, FIXH error
						add_error( 398, "MUT syntax error", word );
					break ;
				case ALPG_TIMESET :
					if ( ram->way[iWay].ts!=0x1 && ram->way[iWay].ts )
						add_error( 399, "TS syntax error", word );
					else
						ram->way[iWay].ts = cmd.bit ;
					break ;
				case ALPG_CYP :
					//2022.10.13 - df overwrite
					//if ( ram->way[iWay].cyp )
					//	add_error( 346, "CYP syntax error", word );
					//else
						ram->way[iWay].cyp = cmd.bit;

					break;
				case ALPG_TP :
					if( cmd.opt == 0 )
					{
						if ( ram->way[iWay].tp1 )
							add_error( 341, "TP syntax error", word );
						else
							ram->way[iWay].tp1 = cmd.bit;
					}
					else if( cmd.opt == 1 )
					{
						if ( ram->way[iWay].tp2 )
							add_error( 341, "TP2 syntax error", word );
						else
							ram->way[iWay].tp2 = cmd.bit;
					}
					break ;
				case ALPG_DBMAP :
					if ( ram->way[iWay].dbmap_op )
						add_error( 342, "DBMAP syntax error", word );
					else
					{
						ram->way[iWay].dbmap_op = cmd.bit;
						if ( cmd.opt==1 )	//digit3
						{
							if( (ram->way[iWay].set_use>>2)&1 )
							{
								add_error(124 , "DSET redefined" , word );
								break ;
							}
							ram->way[iWay].set_use |= (1<<2);


							unsigned long long ll_value   = 0LL;

							word = macro( NULL );

							ll_value = strtoull( word, &endptr, 0);
							ram->way[iWay].group_c   = 0xA;
							ram->way[iWay].c_operand = ll_value;
						}
					}
					break ;
				case ALPG_D :
					if( cmd.opt == 0 )		// /D
					{
						if ( ram->way[iWay].tpd_inv_d )
							add_error( 346, "/D syntax error", word );
						else
							ram->way[iWay].tpd_inv_d = cmd.bit;
					}
					else if( cmd.opt == 1 )	// /D2
					{
						if ( ram->way[iWay].tpd_inv_d2 )
							add_error( 346, "/D2 syntax error", word );
						else
							ram->way[iWay].tpd_inv_d2 = cmd.bit;
					}
					else if( cmd.opt == 2 )	//D<TP1, TP2, TPXOR
					{
						if ( ram->way[iWay].tpd_src_data )
							add_error( 346, "D< syntax error", word );
						else
							ram->way[iWay].tpd_src_data = cmd.bit;
					}
					else if( cmd.opt == 3 )	//PD
					{
						if ( ram->way[iWay].tpd_data_hold )
							add_error( 346, "PD syntax error", word );
						else
							ram->way[iWay].tpd_data_hold = cmd.bit;
					}
					break;
				case ALPG_SET :
				{
					word = macro( NULL );

					unsigned long long ll_value   = 0LL;
					unsigned long      ul_value_h = 0;

					char  szHigh[132];
					char  szLow [132];
					char  szTmp [128];
					bool  bOneshot = true;

					if ( strlen(word) > 20 ) //MAX: 0xffffffffffffffffff
					{
						add_error( 120, "syntax error", word );
						continue;
					}
					if ( strlen(word) > 18 )
						bOneshot = false;

					if ( bOneshot )
					{
						ll_value = strtoull( word, &endptr, 0) ;
					}
					else
					{
						if ( ! strncmp(word, "0x", strlen("0x")) )
						{
							strncpy( szTmp, word+strlen(word)-16, 16 );
							szTmp[16] = '\0';
							sprintf(szLow, "0x%s", szTmp);
							ll_value = strtoull( szLow, &endptr, 0);
							if( *endptr!=0)
							{
								add_error( 120, "syntax error", word );
								continue;
							}

							strncpy( szTmp, word+strlen("0x"), strlen(word)-16-strlen("0x") );
							szTmp[strlen(word)-16-strlen("0x")] = '\0';
							sprintf(szHigh, "0x%s", szTmp);
							ul_value_h = strtoul( szHigh, &endptr, 0);
							if( *endptr!=0)
							{
								add_error( 120, "syntax error", word );
								continue;
							}
						}
						else
						{
							add_error( 120, "syntax error", word );
							continue;
						}
					}

					if ( bRegArea )
					{
						//REGISTER Section(COMMON, MODULE)
						int iReg;
						for(iReg=0; iReg<16; ++iReg )
						{
							if( alpg->set_reg[iReg].groupReg(cmd.opt) )
							{
								if( (alpg->set_reg[iReg].groupReg(cmd.opt))==cmd.bit )
									break;
								else
									continue;
							}
							else
								break ;
						}
						alpg->set_reg[iReg].reg |= cmd.bit<<(cmd.opt*4);

							 if( cmd.opt == 0 )   alpg->set_reg[iReg].a = static_cast<unsigned int>(ll_value);
						else if( cmd.opt == 1 )   alpg->set_reg[iReg].b = static_cast<unsigned int>(ll_value);
						else if( cmd.opt == 2 ) { alpg->set_reg[iReg].c = ll_value; /*alpg->set_reg[iReg].c_h = ul_value_h;*/ }
						else if( cmd.opt == 3 ) { alpg->set_reg[iReg].d = ll_value; /*alpg->set_reg[iReg].d_h = ul_value_h;*/ }

//						Processing at write_register_module
						//TP/TP2 REGISTER TP<TP, TP2<TP2 for DSET
//						if ( cmd.opt == 2 && cmd.bit == 1 )	//TP1<
//							ram->way[iWay].tp1 = 0x1;
//						if ( cmd.opt == 3 && cmd.bit == 1 )	//TP2<
//							ram->way[iWay].tp2 = 0x1;

						break;
					}

					if( (ram->way[iWay].set_use>>cmd.opt)&1 )
					{
						add_error(124 , "DSET redefined" , word );
						break ;
					}
					ram->way[iWay].set_use |= (1<<cmd.opt);

						 if( cmd.opt == 0 ) ram->way[iWay].group_a = cmd.bit ;
					else if( cmd.opt == 1 ) ram->way[iWay].group_b = cmd.bit ;
					else if( cmd.opt == 2 ) ram->way[iWay].group_c = cmd.bit ;
					else if( cmd.opt == 3 ) ram->way[iWay].group_d = cmd.bit ;

						 if( cmd.opt == 0 )   ram->way[iWay].a_operand = static_cast<unsigned int>(ll_value);
					else if( cmd.opt == 1 )   ram->way[iWay].b_operand = static_cast<unsigned int>(ll_value);
					else if( cmd.opt == 2 ) { ram->way[iWay].c_operand = ll_value; /*ram->way[iWay].c_operand_h = ul_value_h;*/ }
					else if( cmd.opt == 3 )	{ ram->way[iWay].d_operand = ll_value; /*ram->way[iWay].d_operand_h = ul_value_h;*/ }

					//TP/TP2 REGISTER TP<TP, TP2<TP2 for DSET
					if ( cmd.opt == 2 && cmd.bit == 1 )	//TP1<
						ram->way[iWay].tp1 = 0x1;
					if ( cmd.opt == 3 && cmd.bit == 1 )	//TP2<
						ram->way[iWay].tp2 = 0x1;

					//DBMAP REGISTER opertaion for DSET
					//2022.11.03 - KWMOON(COMMENT)
					//if ( cmd.opt == 2 && cmd.bit == 9 )	//DBMAP<
					//	ram->way[iWay].dbmap_op = 0x5;
				}
					break;
				case ALPG_ETC :
					if( cmd.opt == 14 ) ram->way[iWay].dle = 1;	//DIAG LATCH ENABLE
					break;
				case ALPG_ADDR_BASE:
				{
					//if opt==0 then XB
					//if opt==1 then YB
					//if opt==2 then Z

					 //XB
					if ( (cmd.opt&0x3)==0 )			//XB
					{
						if ( ram->way[iWay].x_base )
							add_error( 302, "XB syntax error", word );
						else
						{
							int base_alu =  cmd.bit    &0x00f;
							int base_d   = (cmd.bit>>4)&0xfff;

							ram->way[iWay].x_base_alu = base_alu;
							ram->way[iWay].x_base_d   = base_d;
						}
					}
					else if ( (cmd.opt&0x3)==1 )	//YB
					{
						if ( ram->way[iWay].y_base )
							add_error( 303, "YB syntax error", word );
						else
						{
							int base_alu =  cmd.bit    &0x00f;
							int base_d   = (cmd.bit>>4)&0xfff;

							ram->way[iWay].y_base_alu = base_alu;
							ram->way[iWay].y_base_d   = base_d;
						}
					}
					else if ( (cmd.opt&0x3)==2 )	//Z
					{
						if ( ram->way[iWay].z_base )
							add_error( 304, "Z syntax error", word );
						else
						{
							int base_alu =  cmd.bit    &0xff;
	//						int base_d   = (cmd.bit>>8)&0x0f;

							ram->way[iWay].z_base_alu = base_alu;
						}
					}

					if ( cmd.opt&0x4 )
					{
						int iGrp = cmd.opt&0x3;

						if( (ram->way[iWay].set_use>>iGrp)&1 )
						{
							add_error(124 , "DSET redefined" , word );
							break ;
						}
						ram->way[iWay].set_use |= (1<<iGrp);


						unsigned long long ll_value   = 0LL;

						word = macro( NULL );

						ll_value = strtoull( word, &endptr, 0);
						if ( iGrp==0 )
						{
							ram->way[iWay].group_a   = 0xA;
							ram->way[iWay].a_operand = ll_value;
						}
						else if ( iGrp==1 )
						{
							ram->way[iWay].group_b   = 0xA;
							ram->way[iWay].b_operand = ll_value;
						}
					}
				} // End of ALPG_ADDR_BASE
					break;
				case ALPG_ADDR_CURR_X:
				case ALPG_ADDR_CURR_Y:
				{
					// if cmd.command==ALPG_ADDR_CURR_X then XC, XS, XK, XCS, XCK, XCSK
					// if cmd.command==ALPG_ADDR_CURR_Y then YC, YS, YK, YCS, YCK, YCSK
					int curr_dest =  cmd.bit    &0xff;
					int curr_alu  = (cmd.bit>>8)&0xff;
					int curr_src  =  cmd.opt    &0x0f;
					int curr_d    = (cmd.opt>>4)&0x0f;

					if ( cmd.command==ALPG_ADDR_CURR_X )		//X CURRENT
					{
						if ( ram->way[iWay].x_curr )
							add_error( 305, "X CURRENT ADRESS syntax error", word );
						else
						{
							ram->way[iWay].x_curr_dest = curr_dest;
							ram->way[iWay].x_curr_alu  = curr_alu ;
							ram->way[iWay].x_curr_src  = curr_src ;
							ram->way[iWay].x_curr_d    = curr_d   ;
						}
					}
					else if ( cmd.command==ALPG_ADDR_CURR_Y )	//Y CURRENT
					{
						if ( ram->way[iWay].y_curr )
							add_error( 306, "Y CURRENT ADRESS syntax error", word );
						else
						{
							ram->way[iWay].y_curr_dest = curr_dest;
							ram->way[iWay].y_curr_alu  = curr_alu ;
							ram->way[iWay].y_curr_src  = curr_src ;
							ram->way[iWay].y_curr_d    = curr_d   ;
						}
					}

					if ( (cmd.opt&0xf0)==0xf0 ||  (cmd.opt&0xf0)==0xe0 )	//digit1, digit2
					{

						int iGrp = 0;

						if ( (cmd.opt&0xf0)==0xf0 ) iGrp = 1;

						if( (ram->way[iWay].set_use>>iGrp)&1 )
						{
							add_error(124 , "DSET redefined" , word );
							break ;
						}
						ram->way[iWay].set_use |= (1<<iGrp);

						unsigned long long ll_value   = 0LL;

						word = macro( NULL );

						ll_value = strtoull( word, &endptr, 0);

						if ( iGrp==0 )
						{
							ram->way[iWay].group_a   = 0xA;
							ram->way[iWay].a_operand = ll_value;
						}
						else if ( iGrp==1 )
						{
							ram->way[iWay].group_b   = 0xA;
							ram->way[iWay].b_operand = ll_value;
						}
					}
				} //End of ALPG_ADDR_CURR_X, ALPG_ADDR_CURR_Y
					break;
				case ALPG_SCAN_ADDR:
					if ( cmd.opt==0 )
					{
						int dest  =  cmd.bit     &0xf;
						int alu   = (cmd.bit>> 4)&0xf;
						int src_a = (cmd.bit>> 8)&0xf;
						int src_b = (cmd.bit>>12)&0xf;

						ram->way[iWay].scan_addr_dest  = dest;
						ram->way[iWay].scan_addr_alu   = alu;
						ram->way[iWay].scan_addr_src_a = src_a;
						ram->way[iWay].scan_addr_src_b = src_b;
					}
					else if ( cmd.opt==1 )
					{
						ram->way[iWay].scan_addr_out = cmd.bit&0x1;
					}
					break;
				case ALPG_VECTOR_ADDR:
					if ( cmd.opt==0 )
					{
						int dest  =  cmd.bit     &0xf;
						int alu   = (cmd.bit>> 4)&0xf;
						int src_a = (cmd.bit>> 8)&0xf;
						int src_b = (cmd.bit>>12)&0xf;

						ram->way[iWay].vector_addr_dest  = dest;
						ram->way[iWay].vector_addr_alu   = alu;
						ram->way[iWay].vector_addr_src_a = src_a;
						ram->way[iWay].vector_addr_src_b = src_b;
					}
					else if ( cmd.opt==1 )
					{
						ram->way[iWay].vector_addr_out = cmd.bit&0x1;
					}
					break;
				case ALPG_NB_REG:
				{
					//if opt==0 then N
					//if opt==1 then B
					int nb_alu =  cmd.bit    &0xf;
					int nb_d   = (cmd.bit>>4)&0xf;

					if ( cmd.opt==0 )		//N
					{
						if ( ram->way[iWay].nb_reg_n_alu )
							add_error( 307, "N REGISTER syntax error", word );
						else
						{
							ram->way[iWay].nb_reg_n_alu = nb_alu;
							ram->way[iWay].nb_reg_n_d   = nb_d  ;
						}
					}
					else if ( cmd.opt==1 )	//B
					{
						if ( ram->way[iWay].nb_reg_b_alu )
							add_error( 307, "B REGISTER syntax error", word );
						else
						{
							ram->way[iWay].nb_reg_b_alu = nb_alu;
							ram->way[iWay].nb_reg_b_d   = nb_d  ;
						}
					}


				} //End of ALPG_NB_REG
					break;
				case ALPG_D3D4_REG:
				{
					//if opt==0 then D3
					//if opt==1 then D4
					int d3d4_d = cmd.bit&0xf;
					if ( cmd.opt==0 )		//D3
					{
						if ( ram->way[iWay].d3d4_reg_d3 )
							add_error( 308, "D3 REGISTER syntax error", word );
						else
							ram->way[iWay].d3d4_reg_d3 = d3d4_d;
					}
					else if ( cmd.opt==1 )	//D4
					{
						if ( ram->way[iWay].d3d4_reg_d4 )
							add_error( 308, "D4 REGISTER syntax error", word );
						else
							ram->way[iWay].d3d4_reg_d4 = d3d4_d;
					}
				} //End of ALPG_D3D4_REG
					break;
				case ALPG_ADDR_SRC:
				{
					//////////////////////////////////////////////////////////////////////////////////////
					//DSET XT, YT
					if ( bRegArea && ( cmd.opt==5 || cmd.opt==6 ) )	//XT, YT
					{
						//XT, YT - ALPG_SET
						int				iGrp ;
						unsigned int	index;

						unsigned long long ll_value   = 0LL;
						unsigned long      ul_value_h = 0;

							 if ( cmd.opt==5 ) { iGrp = 0; index = 7; } //XT
						else if ( cmd.opt==6 ) { iGrp = 1; index = 7; } //YT

						word = macro( NULL );

						char  szHigh[132];
						char  szLow [132];
						char  szTmp [128];
						bool  bOneshot = true;

						if ( strlen(word) > 20 ) //MAX: 0xffffffffffffffffff
						{
							add_error( 301, "syntax error", word );
							continue;
						}
						if ( strlen(word) > 18 )
							bOneshot = false;

						if ( bOneshot )
						{
							ll_value = strtoull( word, &endptr, 0) ;
						}
						else
						{
							if ( ! strncmp(word, "0x", strlen("0x")) )
							{
								strncpy( szTmp, word+strlen(word)-16, 16 );
								szTmp[16] = '\0';
								sprintf(szLow, "0x%s", szTmp);
								ll_value = strtoull( szLow, &endptr, 0);
								if( *endptr!=0)
								{
									add_error( 301, "syntax error", word );
									continue;
								}

								strncpy( szTmp, word+strlen("0x"), strlen(word)-16-strlen("0x") );
								szTmp[strlen(word)-16-strlen("0x")] = '\0';
								sprintf(szHigh, "0x%s", szTmp);
								ul_value_h = strtoul( szHigh, &endptr, 0);
								if( *endptr!=0)
								{
									add_error( 301, "syntax error", word );
									continue;
								}
							}
							else
							{
								add_error( 301, "syntax error", word );
								continue;
							}
						}

						//REGISTER Section(COMMON, MODULE)
						int iReg;
						for(iReg=0; iReg<16; ++iReg )
						{
							if( alpg->set_reg[iReg].groupReg(iGrp) )
							{
								if( (alpg->set_reg[iReg].groupReg(iGrp))==index )
									break;
								else
									continue;
							}
							else
								break ;
						}
						alpg->set_reg[iReg].reg |= index<<(iGrp*4);

							 if( iGrp == 0 )   alpg->set_reg[iReg].a = static_cast<unsigned int>(ll_value);
						else if( iGrp == 1 )   alpg->set_reg[iReg].b = static_cast<unsigned int>(ll_value);
						break;
					}
					//DSET XT, YT
					//////////////////////////////////////////////////////////////////////////////////////
					//ALPG_ADDR_SRC
					//if opt==0 then X OUT
					//if opt==1 then Y OUT
					//if opt==2 then /X
					//if opt==3 then /Y
					//if opt==4 then /Z
					//if opt==5 then XT
					//if opt==6 then YT
					//if opt==7 then SWAP
					//if opt==8 then RF
					if ( cmd.opt==0 )		//X<
					{
						if ( ram->way[iWay].b_src_addr_x_out )
							add_error( 309, "X< syntax error", word );
						else
						{
							ram->way[iWay].src_addr_x_out   = cmd.bit;
							ram->way[iWay].b_src_addr_x_out = true;
						}
					}
					else if ( cmd.opt==1 )	//Y<
					{
						if ( ram->way[iWay].b_src_addr_y_out )
							add_error( 309, "Y< syntax error", word );
						else
						{
							ram->way[iWay].src_addr_y_out   = cmd.bit;
							ram->way[iWay].b_src_addr_y_out = true;
						}
					}
					else if ( cmd.opt==2 )	// /X
					{
						if ( ram->way[iWay].src_addr_x_inv )
							add_error( 309, "/X syntax error", word );	// /X
						else
							ram->way[iWay].src_addr_x_inv = cmd.bit;
					}
					else if ( cmd.opt==3 )	// /Y
					{
						if ( ram->way[iWay].src_addr_y_inv )
							add_error( 309, "/Y syntax error", word );	// /Y
						else
							ram->way[iWay].src_addr_y_inv = cmd.bit;
					}
					else if ( cmd.opt==4 )	// /Z
					{
						if ( ram->way[iWay].src_addr_z_inv )
							add_error( 309, "/Z syntax error", word );	// /Z
						else
							ram->way[iWay].src_addr_z_inv = cmd.bit;
					}
					else if ( cmd.opt==5 )	//XT
					{
						if ( ram->way[iWay].src_addr_xt )
							add_error( 309, "XT syntax error", word );	//XT
						else
							ram->way[iWay].src_addr_xt = cmd.bit;
					}
					else if ( cmd.opt==6 )	//YT
					{
						if ( ram->way[iWay].src_addr_yt )
							add_error( 309, "YT syntax error", word );	//YT
						else
							ram->way[iWay].src_addr_yt = cmd.bit;
					}
					else if ( cmd.opt==7 )	//LMAX<>HMAX
					{
						if ( ram->way[iWay].src_addr_swap )
							add_error( 309, "LMAX<>HMAX syntax error", word );
						else
							ram->way[iWay].src_addr_swap = cmd.bit;
					}
					else if ( cmd.opt==8 )	//RF<RF+1
					{
						if ( ram->way[iWay].src_addr_rf )
							add_error( 309, "RF<RF+1 syntax error", word );
						else
							ram->way[iWay].src_addr_rf = cmd.bit;
					}
				} //End of ALPG_ADDR_SRC
					break;
//case-------------------------------------------------------------------------------------------------------------
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			word = macro( NULL );
		}
		if ( in_src ) { delete [] in_src; in_src = NULL; }
	}
	//proc MODULE_END, SUB_END - nolabel
	for( NoLabelList::iterator nLit=noLabel.begin(); nLit!=noLabel.end(); ++nLit)
	{
		NOLABEL *nolabel = *nLit;

		Str2WordMap::iterator it=labelMap.find(nolabel->label);
		if( it != labelMap.end() )
		{
			*(nolabel->reg) = it->second + m_iStartPc;
//				printf("LABEL=%s operand=%x\n" , nolabel->label, it->second + m_iStartPc );
		}
		else
		{
			add_err(subname, lines[lines.size()-1].line, "label undefined", nolabel->label );
		}
	}
	for( NoLabelList::iterator nLit=noLabel.begin(); nLit!=noLabel.end(); ++nLit)
	{
		NOLABEL *nolabel = *nLit;
		delete nolabel;
		nolabel = NULL;
	}
	noLabel.clear();

	for( NoLabelList::iterator nLit=noLabelJSR.begin(); nLit!=noLabelJSR.end(); ++nLit)
	{
		NOLABEL *nolabel = *nLit;

		Str2WordMap::iterator it=labelMap.find(nolabel->label);
		if( it != labelMap.end() )
		{
			*(nolabel->reg) = it->second + m_iStartPc;
//				printf("LABEL=%s operand=%x\n" , nolabel->label, it->second + m_iStartPc );
			delete nolabel;
			nolabel = NULL;
		}
		else
		{
			Str2WordMap::iterator itt=sub_map.find(nolabel->label);
			if( itt != sub_map.end() )
			{
				*(nolabel->reg) = itt->second;

				delete nolabel;
				nolabel = NULL;
			}
			else
			{
				m_noLabelRemain.push_back( nolabel );
			}
		}
	}
	noLabelJSR.clear();
	labelMap.clear();

	if ( ram )
	{
		if ( ram->op_use )
			add_err(subname, lines[lines.size()-1].line, "semicolon is omitted" , (char *)lines[lines.size()-1].data );

		delete ram; ram = NULL;
	}

	if( 0 ) printf("m_iStartPc=%d   iCurrPc=%d PC Count:%ld\n", m_iStartPc, iCurrPc, patData.pcs->size() );
	m_iStartPc += iCurrPc;

	return true;
}

bool CAlpgCompiler::ProcNoLabelRemain()
{
	bool bRtn = true;

	for( NoLabelList::iterator nLit=m_noLabelRemain.begin(); nLit!=m_noLabelRemain.end(); ++nLit)
	{
		NOLABEL *nolabel = *nLit;

		Str2WordMap::iterator itt=sub_map.find(nolabel->label);
		if( itt != sub_map.end() )
		{
			*(nolabel->reg) = itt->second;
		}
		else
		{
			add_err("JSR", 0, "SUB undefined" , nolabel->label );
			bRtn = false;
		}
	}
	for( NoLabelList::iterator nLit=m_noLabelRemain.begin(); nLit!=m_noLabelRemain.end(); ++nLit)
	{
		NOLABEL *nolabel = *nLit;
		delete nolabel;
		nolabel = NULL;
	}
	m_noLabelRemain.clear();

	return bRtn;
}
