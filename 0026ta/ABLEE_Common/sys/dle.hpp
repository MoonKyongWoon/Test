#ifndef CDLE_HPP
#define CDLE_HPP

#include "testlib_global.h"
#include "bitchart.hpp"
#include "io.hpp"

extern CPcie g_pcie;

class CDle
{
public:
	int* PC;
	int* DRE;
	int* CPE;
	int* PD_H;
	int* PD_L;	
	int* UCLK_H_0;
	int* UCLK_H_1;
	int* UCLK_H_2;
	int* UCLK_L_0;
	int* UCLK_L_1;
	int* UCLK_L_2;
	long long int* P_H;
	long long int* P_L;
	int* CYP;
	int* TS;

	CDle()
	{
		PC       = nullptr;
		DRE      = nullptr;
		CPE      = nullptr;
		PD_H     = nullptr;
		PD_L     = nullptr;
		P_H      = nullptr;
		P_L      = nullptr;
		UCLK_H_0 = nullptr;
		UCLK_H_1 = nullptr;
		UCLK_H_2 = nullptr;
		UCLK_L_0 = nullptr;
		UCLK_L_1 = nullptr;
		UCLK_L_2 = nullptr;
		CYP      = nullptr;
		TS       = nullptr;
	}

	~CDle()
	{
		Delete();
	}

	int Count() { return nCnt; }
	void Get()
	{
		New();

		long long MUT_L, MUT_H;
		//fprintf( stdout, "\n[ALPG Diagnostic Memory]\n");
		for ( int i=0; i<nCnt; ++i )
		{
			int High = 0 , Low = 0;
			PC      [i] = g_pcie.read_user( BITCHART::ALPG::_WCS_ADDR      + i*4 );
			DRE     [i] = g_pcie.read_user( BITCHART::ALPG::_DIAG_DRE      + i*4 );
			CPE     [i] = g_pcie.read_user( BITCHART::ALPG::_DIAG_CPE      + i*4 );
			PD_H    [i] = g_pcie.read_user( BITCHART::ALPG::_H_EDGE_PD     + i*4 );
			PD_L    [i] = g_pcie.read_user( BITCHART::ALPG::_L_EDGE_PD     + i*4 );
			Low         = g_pcie.read_user( BITCHART::ALPG::_H_EDGE_P_0    + i*4 );
			High        = g_pcie.read_user( BITCHART::ALPG::_H_EDGE_P_1    + i*4 );
			P_H[i]      = (static_cast<long long int>(High)<<32 ) | ( static_cast<long long int>(Low) & 0xFFFFFFFF );
			Low         = g_pcie.read_user( BITCHART::ALPG::_L_EDGE_P_0    + i*4 );
			High        = g_pcie.read_user( BITCHART::ALPG::_L_EDGE_P_1    + i*4 );
			P_L[i]      = (static_cast<long long int>(High)<<32 ) | ( static_cast<long long int>(Low) & 0xFFFFFFFF );
			UCLK_H_0[i] = g_pcie.read_user( BITCHART::ALPG::_H_EDGE_UCLK_0 + i*4 );
			UCLK_H_1[i] = g_pcie.read_user( BITCHART::ALPG::_H_EDGE_UCLK_1 + i*4 );
			UCLK_H_2[i] = g_pcie.read_user( BITCHART::ALPG::_H_EDGE_UCLK_2 + i*4 );
			UCLK_L_0[i] = g_pcie.read_user( BITCHART::ALPG::_L_EDGE_UCLK_0 + i*4 );
			UCLK_L_1[i] = g_pcie.read_user( BITCHART::ALPG::_L_EDGE_UCLK_1 + i*4 );
			UCLK_L_2[i] = g_pcie.read_user( BITCHART::ALPG::_L_EDGE_UCLK_2 + i*4 );
			CYP     [i] = g_pcie.read_user( BITCHART::ALPG::_DIAG_CYP      + i*4 );
			TS      [i] = g_pcie.read_user( BITCHART::ALPG::_DIAG_TS       + i*4 );
			fprintf( stdout, "PC[%04d] TS%02d CYP%02d DRE:%02X LCPE:%02X HCPE:%02X\n", PC[i], TS[i], CYP[i], DRE[i], (CPE[i]>>4)&0xF, CPE[i]&0xF );
			fprintf( stdout, "%8s PD_H:0x%08X PD_L:0x%08X\n"              , "", PD_H[i], PD_L[i] );
			fprintf( stdout, "%8s P_H:0x%llX P_L:0x%llX\n"                , "", P_H[i] , P_L[i]  );
			fprintf( stdout, "%8s UCLK_H:0x%02X:%08X:%08X UCLK_L:0x%02X:%08X:%08X\n", "", UCLK_H_2[i], UCLK_H_1[i], UCLK_H_0[i], UCLK_L_2[i], UCLK_L_1[i], UCLK_L_0[i] );
		}
	}

private:
	int nCnt;

	void Delete()
	{
		if ( PC )
		{
			delete PC      ; PC       = nullptr;
			delete DRE     ; DRE      = nullptr;
			delete CPE     ; CPE      = nullptr;
			delete PD_H    ; PD_H     = nullptr;
			delete PD_L    ; PD_L     = nullptr;
			delete P_H     ; P_H      = nullptr;
			delete P_L     ; P_L      = nullptr;
			delete UCLK_H_0; UCLK_H_0 = nullptr;
			delete UCLK_H_1; UCLK_H_1 = nullptr;
			delete UCLK_H_2; UCLK_H_2 = nullptr;
			delete UCLK_L_0; UCLK_L_0 = nullptr;
			delete UCLK_L_1; UCLK_L_1 = nullptr;
			delete UCLK_L_2; UCLK_L_2 = nullptr;
			delete CYP     ; CYP      = nullptr;
			delete TS      ; TS       = nullptr;
		}
	}

	void New()
	{
		nCnt = g_pcie.read_user( BITCHART::GLOBAL_MODE_REG::_DIAG_COUNT);
		if( nCnt < 1 ) nCnt = 1;

		PC       = new int[nCnt];
		DRE      = new int[nCnt];
		CPE      = new int[nCnt];
		PD_H     = new int[nCnt];
		PD_L     = new int[nCnt];
		P_H      = new long long[nCnt];
		P_L      = new long long[nCnt];
		UCLK_H_0 = new int[nCnt];
		UCLK_H_1 = new int[nCnt];
		UCLK_H_2 = new int[nCnt];
		UCLK_L_0 = new int[nCnt];
		UCLK_L_1 = new int[nCnt];
		UCLK_L_2 = new int[nCnt];
		CYP      = new int[nCnt];
		TS       = new int[nCnt];
	}
};

#endif
