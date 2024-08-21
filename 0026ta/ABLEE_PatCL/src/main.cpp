#include <iostream>
#include "patcl.hpp"

bool makeSdef( std::vector<pat_line_data>& sdef )
{
	sdef.clear();
	sdef.push_back( pat_line_data{  1, "SDEF STROBE	C21 C17"} );
	sdef.push_back( pat_line_data{  2, "SDEF DRE	C19"} );
	sdef.push_back( pat_line_data{  3, "SDEF CmdIn	C2 	C0 C19"} );
	sdef.push_back( pat_line_data{  4, "SDEF CLE	C2"} );
	sdef.push_back( pat_line_data{  5, "SDEF ALE	C3"} );
	sdef.push_back( pat_line_data{  6, "SDEF XYCHD    X<YC Y<YC"} );
	sdef.push_back( pat_line_data{  7, "SDEF XXCHD    X<XC Y<XC"} );
	sdef.push_back( pat_line_data{  8, "SDEF YYSHD    Y<YS X<YS"} );
	sdef.push_back( pat_line_data{  9, "SDEF COUT     X<XC Y<YC"} );
	sdef.push_back( pat_line_data{ 10, "SDEF YCINC    YC<YC+1"  } );
	sdef.push_back( pat_line_data{ 11, "SDEF INC_SCAN	SA<SA+1"  } );
	sdef.push_back( pat_line_data{ 12, "SDEF M1	CYP2"  } );
	sdef.push_back( pat_line_data{ 13, "SDEF M2	CYP5"  } );
	sdef.push_back( pat_line_data{ 14, "SDEF R	C1 C21 C17"  } );
	sdef.push_back( pat_line_data{ 15, "SDEF W	C0 C19"  } );

	return true;
}

void makesub_0( std::vector<pat_line_data>& sub )
{
	sub.clear();
	sub.push_back( pat_line_data{  1, "RPT	100	TS16	CYP1				            ;"} );
	sub.push_back( pat_line_data{  2, "NOP		TS1		XB<0	YB<0	XC<0	YC<0	;"} );

	sub.push_back( pat_line_data{  2, "%IFE (A.OR.B);"} );
	sub.push_back( pat_line_data{  2, "NOP		TS1		XB<0	YB<0	XC<0	YC<0	;"} );
	sub.push_back( pat_line_data{  2, "%IFE (A.AND.B).OR.C.AND.(A.OR.B);"} );
//	sub.push_back( pat_line_data{  2, "%IFE (A.AND.B);"} );
	sub.push_back( pat_line_data{  2, "NOP		TS2		XB<0	YB<0	XC<0	YC<0	;"} );
	sub.push_back( pat_line_data{  2, "%IFE (A.AND.B);"} );
	sub.push_back( pat_line_data{  2, "NOP		TS4		XB<0	YB<0	XC<0	YC<0	;"} );
	sub.push_back( pat_line_data{  2, "%ENDC;"} );
	sub.push_back( pat_line_data{  2, "NOP		TS4		XB<0	YB<0	XC<0	YC<0	;"} );
	sub.push_back( pat_line_data{  2, "%ENDC;"} );

	sub.push_back( pat_line_data{  2, "%IFE (A.AND.B);"} );
	sub.push_back( pat_line_data{  2, "NOP		TS4		XB<0	YB<0	XC<0	YC<0	;"} );
	sub.push_back( pat_line_data{  2, "%ENDC;"} );
	sub.push_back( pat_line_data{  2, "NOP		TS5		XB<0	YB<0	XC<0	YC<0	;"} );
	sub.push_back( pat_line_data{  2, "%ENDC;"} );

	sub.push_back( pat_line_data{  3, "NOP		TS1		D3<D3B	D4<D4B	XS<0	YS<0	;"} );
	sub.push_back( pat_line_data{  4, "NOP		TS1		TP<0	TP2<0	XK<0	YK<0	;"} );
	sub.push_back( pat_line_data{  5, "NOP		TS1		SA<0	D5<0	XA<0	YA<0	;"} );
	sub.push_back( pat_line_data{  6, "NOP		TS1		CA<0		XD<0	YD<0	    ;"} );
	sub.push_back( pat_line_data{  7, "NOP		TS1		VA<0		XE<0	YE<0	    ;"} );
	sub.push_back( pat_line_data{  8, "NOP		TS1		DBMAR<0		XF<0	YF<0	    ;"} );
	sub.push_back( pat_line_data{  9, "NOP		TS1		DCMR<0				            ;"} );
	sub.push_back( pat_line_data{ 10, "RTN		TS1						                ;"} );

	sub.push_back( pat_line_data{ 99, "NOP		D1<#4D132     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XH<#5352     TS1;"} );

	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<0     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<0x5352     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<5352     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<#5352     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<DBMAP+5352     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<DBMAP-5352     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<DBMAP+1     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<DBMAP-1     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<DBMAP+123     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		DBMAP<DBMAP-123     TS1;"} );

	sub.push_back( pat_line_data{ 99, "NOP		XB<0     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB+1     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB-1     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB+0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB+#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB+1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB-0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB-#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XB<XB-1234     TS1;"} );

	sub.push_back( pat_line_data{ 99, "NOP		XC<0     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC+1     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC-1     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC+0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC+#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC+1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC-0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC-#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC-1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC\"0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC\"#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC\"1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC'0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC'#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC'1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC&0x1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC&#1234     TS1;"} );
	sub.push_back( pat_line_data{ 99, "NOP		XC<XC&1234     TS1;"} );

//	sub.push_back( pat_line_data{ 99, "NOP		XC<XC*0x1234     TS1;"} );
//	sub.push_back( pat_line_data{ 99, "NOP		XC<XC*#1234     TS1;"} );
//	sub.push_back( pat_line_data{ 99, "NOP		XC<XC*1234     TS1;"} );

	sub.push_back( pat_line_data{ 99, "NOP		XC<XC*2     TS1;"} );
	sub.push_back( pat_line_data{ 99, "OUT  init_result				      		TS5  ;"} );
}

void makesub_1( std::vector<pat_line_data>& sub )
{
	sub.clear();
	sub.push_back( pat_line_data{  1, "NOP                         	M1                      	    TS5  ;"} );
	sub.push_back( pat_line_data{  2, "NOP                         	M1                     TP<0xEE  TS5  ;"} );
	sub.push_back( pat_line_data{  3, "NOP                         	M1    W CYP1 CLE 		        TS1  ;"} );
	sub.push_back( pat_line_data{  4, "NOP      XXCHD              	M1    W      ALE 		        TS2  ;"} );
	sub.push_back( pat_line_data{  5, "NOP                         	M1                        	    TS12 ;"} );
	sub.push_back( pat_line_data{  6, "NOP                         	M1                     TP<TPH   TS5  ;"} );
	sub.push_back( pat_line_data{  7, "NOP      COUT YCINC         	M1    R CYP1           TP<0x00  TS5  ;"} );
	sub.push_back( pat_line_data{  8, "NOP      COUT YCINC         	M1    R CYP1           TP<D5    TS5  ;"} );
	sub.push_back( pat_line_data{  9, "NOP      COUT YCINC         	M1    R CYP1           TP<0x00  TS5  ;"} );
	sub.push_back( pat_line_data{ 10, "NOP      COUT               	M1    R CYP1            	    TS5  ;"} );
//	sub.push_back( pat_line_data{ 11, "RTN" } );
	sub.push_back( pat_line_data{ 11, "RTN                                                          TS5  ;" } );
}

void makesub_2( std::vector<pat_line_data>& sub )
{
	sub.clear();
	sub.push_back( pat_line_data{  1, "NOP                         	M1            	       TP<0x80  TS15  ; "} );
	sub.push_back( pat_line_data{  2, "NOP                         	M1    W CYP1 CLE 		         TS1  ;	"} );
	sub.push_back( pat_line_data{  3, "NOP      XYCHD              	M1    W      ALE 		         TS2  ;	"} );
	sub.push_back( pat_line_data{  4, "NOP      XYCHD              	M1 M2 W      ALE 		         TS2  ;	"} );
	sub.push_back( pat_line_data{  5, "NOP      XXCHD              	M1    W      ALE 		         TS2  ;	"} );
	sub.push_back( pat_line_data{  6, "NOP      XXCHD              	M1 M2 W      ALE 		         TS2  ;	"} );
	sub.push_back( pat_line_data{  7, "NOP      YYSHD              	M1    W      ALE 		         TS2  ;	"} );
	sub.push_back( pat_line_data{  8, "RTN                         	M1                    		     TS5  ;	"} );
}

void makesub_3( std::vector<pat_line_data>& sub )
{
	sub.clear();
	sub.push_back( pat_line_data{  1, "	REGISTER;                                                               "} );
	sub.push_back( pat_line_data{  1, "	LMAX=0x3ffff;                                                           "} );
	sub.push_back( pat_line_data{  1, "	HMAX=0x3ffff;                                                           "} );
	sub.push_back( pat_line_data{  1, "	ZMAX=0x3ffff;                                                           "} );
	sub.push_back( pat_line_data{  1, "	XMAX=0x3ffff;                                                           "} );
	sub.push_back( pat_line_data{  1, "	YMAX=0x3ffff;                                                           "} );
	sub.push_back( pat_line_data{  1, "                                                                         "} );
	sub.push_back( pat_line_data{  1, "	START;                                                                  "} );
	sub.push_back( pat_line_data{  1, "        NOP               	M1                        	TS5  ;  "} );
	sub.push_back( pat_line_data{  1, "        JSR REG_CLEAR               	M1                        	TS5  ;  "} );
	sub.push_back( pat_line_data{  1, "ST01:   JSR REG_CLEAR_NO_SCAN       	M1                        	TS5  ;  "} );
	sub.push_back( pat_line_data{  1, "	JSR CAMRESET			M1				TS5  ;                          "} );
	sub.push_back( pat_line_data{  1, "XXX:NOP M1 TS5;                                                          "} );
//	sub.push_back( pat_line_data{  1, "	JSR FTMADD		    	M1				TS5  ;                          "} );
//	sub.push_back( pat_line_data{  1, "	JSR GET_FEATURE_DEFULT      	M1                        	TS5  ;      "} );
//	sub.push_back( pat_line_data{  1, "	JSR SET_GET_FEATURE_ALL     	M1                        	TS5  ;      "} );
//	sub.push_back( pat_line_data{  1, "                                                                         "} );
	sub.push_back( pat_line_data{  1, "	OUT  init_result			              		TS5  ;                  "} );
	sub.push_back( pat_line_data{  1, "	JNI0 ST01 			                      		TS5  INC_SCAN ;         "} );
	sub.push_back( pat_line_data{  1, "	STPS						      		TS5  SA<0 ;                     "} );
}

void makesub_4( std::vector<pat_line_data>& sub )
{

	sub.clear();
	sub.push_back( pat_line_data{  1, "RPT	100	TS16						;                             "} );
	sub.push_back( pat_line_data{  2, "NOP		TS1		XB<0	YB<0	XC<0	YC<0	;                 "} );
	sub.push_back( pat_line_data{  3, "NOP		TS1		D3<D3B	D4<D4B	XS<0	YS<0	;                 "} );
	sub.push_back( pat_line_data{  4, "NOP		TS1		TP<0	TP2<0	XK<0	YK<0	;                 "} );
	sub.push_back( pat_line_data{  5, "NOP		TS1			D5<0	XA<0	YA<0	;                     "} );
	sub.push_back( pat_line_data{  6, "NOP		TS1		CA<0		XD<0	YD<0	;                     "} );
	sub.push_back( pat_line_data{  7, "NOP		TS1		VA<0		XE<0	YE<0	;                     "} );
	sub.push_back( pat_line_data{  8, "NOP		TS1		DBMAR<0		XF<0	YF<0	;                     "} );
	sub.push_back( pat_line_data{  9, "NOP		TS1		DCMR<0				;                             "} );
	sub.push_back( pat_line_data{ 10, "RTN		TS1						;                                 "} );
}

void makesub_5( std::vector<pat_line_data>& sub )
{

	sub.clear();
	sub.push_back( pat_line_data{  1, "NOP				M1				TS5  ;                     "} );
	sub.push_back( pat_line_data{  2, "    NOP				M1		       TP<0xFF  TS5  ;         "} );
	sub.push_back( pat_line_data{  3, "    NOP				M1    W CYP1 C2			TS11 ;         "} );
	sub.push_back( pat_line_data{  4, "    JNI7 .				M1                       	TS6  ; "} );
	sub.push_back( pat_line_data{  5, "RTN				M1                       	TS5  ;         "} );
}

void makesub_6( std::vector<pat_line_data>& sub )
{

	sub.clear();
	sub.push_back( pat_line_data{  1, "		NOP				TS9 ; "} );
	sub.push_back( pat_line_data{  2, "		NOP				TS9 ; "} );
	sub.push_back( pat_line_data{  3, "ST1:	NOP		TP<0x05	TS1 ; "} );
	sub.push_back( pat_line_data{  4, "		NOP			W	TS2 ; "} );
	sub.push_back( pat_line_data{  5, "		NOP				TS3 ; "} );
	sub.push_back( pat_line_data{  6, "		JNI3 ST1	R	TS4 ; "} );
	sub.push_back( pat_line_data{  7, "		NOP				TS5 ; "} );
	sub.push_back( pat_line_data{  8, "		NOP	XC<XC^D1A			TS6 ; "} );
}

void makesub_7( std::vector<pat_line_data>& sub )
{

	sub.clear();
	sub.push_back( pat_line_data{  1, "		NOP	M1			TS9 ; "} );
	sub.push_back( pat_line_data{  2, "		NOP	M1			TS9 ; "} );
	sub.push_back( pat_line_data{  3, "  	NOP	M1	TP<0x05	TS1 ; "} );
	sub.push_back( pat_line_data{  4, "		NOP	M1		R	TS2 ; "} );
	sub.push_back( pat_line_data{  5, "		NOP	M1			TS3 ; "} );
	sub.push_back( pat_line_data{  6, "		NOP M1 CYP1	W	TS4 ; "} );
	sub.push_back( pat_line_data{  7, "		NOP	M1			TS5 ; "} );
}

bool makesub( int nsub, std::vector<pat_line_data>& sub )
{
		 if( nsub == 0 ) makesub_0( sub );
	else if( nsub == 1 ) makesub_1( sub );
	else if( nsub == 2 ) makesub_2( sub );
	else if( nsub == 3 ) makesub_3( sub );
	else if( nsub == 4 ) makesub_4( sub );
	else if( nsub == 5 ) makesub_5( sub );
	else if( nsub == 6 ) makesub_6( sub );
	else if( nsub == 7 ) makesub_7( sub );
	return true;
}

int main()
{
	CPatCl patcl;
	std::vector<pat_line_data> sdef;
	std::vector<pat_line_data> sub;

	fprintf( stdout, "make sdef\n");
	makeSdef( sdef );
	patcl.Reset();
	patcl.SetDefine( "device_default", sdef );

	patcl.SetCompOpt( 0x35 );

	fprintf( stdout, "make sub\n");
	makesub( 1, sub );
	patcl.SetSubCompile( "REG_CLEAR1", sub );
	fprintf( stdout, "1----GetMaxPc:%d\n", patcl.GetMaxPc() );

	makesub( 0, sub );
	patcl.SetSubCompile( "REG_CLEAR", sub );
	fprintf( stdout, "0----GetMaxPc:%d\n", patcl.GetMaxPc() );

	makesub( 2, sub );
	patcl.SetSubCompile( "DIESEL", sub );
	fprintf( stdout, "2----GetMaxPc:%d\n", patcl.GetMaxPc() );

	makesub( 3, sub );
	patcl.SetSubCompile( "#000", sub );
	fprintf( stdout, "3----GetMaxPc:%d\n", patcl.GetMaxPc() );

	makesub( 4, sub );
	patcl.SetSubCompile( "REG_CLEAR_NO_SCAN", sub );
	fprintf( stdout, "4----GetMaxPc:%d\n", patcl.GetMaxPc() );

	makesub( 5, sub );
	patcl.SetSubCompile( "CAMRESET", sub );
	fprintf( stdout, "5----GetMaxPc:%d\n", patcl.GetMaxPc() );

	makesub( 6, sub );
	patcl.SetSubCompile( "RMW", sub );
	fprintf( stdout, "6----GetMaxPc:%d\n", patcl.GetMaxPc() );

	makesub( 7, sub );
	patcl.SetSubCompile( "RMW7", sub );
	fprintf( stdout, "7----GetMaxPc:%d\n", patcl.GetMaxPc() );

	patcl.ProcNoLabelRemain();

	fprintf( stdout, "ErrCnt = %u\n", patcl.GetErrCnt() );

	PGDATA* pgdata = patcl.GetPatternData();
	fprintf( stdout, "PatCnt = %ld\n", pgdata->pcs->size() );
///////////////////////////////////////////////////////////////////////
	if( 1 && pgdata->pcs )
	{
		int pc = 0, waypc = 0, i = 0;
		if( 1 ) printf("Write Pattern Size : %ld\n", pgdata->pcs->size() );
		for( PcList::iterator pit=pgdata->pcs->begin(); pit!=pgdata->pcs->end(); ++pit)
		{
			PCS* pcs = (*pit);
			if( 1 ) fprintf( stdout, "PC:%04d[0x%04X] OPCODE:0x%05X ", i, i, pcs->opcode );
			if( 1 ) fprintf( stdout, "OPR:0x%08X ", pcs->operand );
			for( int way=0;way<1;way++ )
			{
				if( 1 ) fprintf( stdout, "MUT:0x%06X ", pcs->way[way].mut_ctl );
//				if( 1 ) fprintf( stdout, "RWM:0x%04X ", pcs->way[way].mut_rwm );
				if( 1 ) fprintf( stdout, "CYP:0x%02X ", pcs->way[way].cyp );
				if( 1 ) fprintf( stdout, "TS:0x%02X \n", pcs->way[way].ts );
			}
			pc    += 4;
			waypc += 4;
			i++;
		}
	}
///////////////////////////////////////////////////////////////////////
	std::vector<atstring> vecErr;
	vecErr = patcl.GetErrors();
	for (uint i=0; i<vecErr.size(); ++i )
		fprintf( stdout, "[ERR%3d]%s\n", i+1, (char *)vecErr[i] );

	return 0;
}
