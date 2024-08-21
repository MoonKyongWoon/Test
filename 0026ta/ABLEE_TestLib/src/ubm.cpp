#include "bitchart.hpp"
#include "ubm.hpp"
#include "io.hpp"
#include "pg.hpp"
#include "sys.hpp"

extern CPcie g_pcie;
extern CPg   g_pg;
extern CSys  g_sys;


CUbm::CUbm()
{

}

CUbm::~CUbm()
{

}

void CUbm::SetUdata0x00()
{
	for(int m=0;m<UBM_DEPTH;m++ )
	{
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b31_b0  + (m*4), 0x0 );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b47_b32 + (m*4), 0x0 );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b31_b0  + (m*4), 0x0 );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b47_b32 + (m*4), 0x0 );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b31_b0  + (m*4), 0x0 );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b47_b32 + (m*4), 0x0 );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b31_b0  + (m*4), 0x0 );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b47_b32 + (m*4), 0x0 );
	}
}

void CUbm::SetUdata0xFF()
{
	for(int m=0;m<UBM_DEPTH;m++ )
	{
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b31_b0  + (m*4), 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b47_b32 + (m*4), 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b31_b0  + (m*4), 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b47_b32 + (m*4), 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b31_b0  + (m*4), 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b47_b32 + (m*4), 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b31_b0  + (m*4), 0xFFFFFFFF );
		g_pcie.write_user( BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b47_b32 + (m*4), 0xFFFFFFFF );
	}
}

void CUbm::SetUdata32( unsigned int fc, int index, int index2, unsigned int data )
{
	int main_addres = index * 4;
	int sub_address = index2;
	int ioa_wdata   = (data>> 0)&0xFFFF;
	int iob_wdata   = (data>>16)&0xFFFF;
	int ioa_address = 0x0;
	int iob_address = 0x0;
	int mask = 0x0;
	int shift = 0;
	if( fc == 0 )
	{
		if( sub_address < 2 )
		{
			mask  = (~0xffff);
			shift = 0x0;
			if( sub_address == 1 ) { mask = (~0xffff0000); shift = 16; }
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b31_b0 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b31_b0 + main_addres;
		}
		else
		{
			mask  = (~0xffff);
			shift = 0;
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b47_b32 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b47_b32 + main_addres;
		}
	}
	else
	{
		if( sub_address < 2 )
		{
			mask = (~0xffff);
			shift = 0;
			if( sub_address == 1 ) { mask = (~0xffff0000); shift = 16; }
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b31_b0 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b31_b0 + main_addres;
		}
		else
		{
			mask = (~0xffff);
			shift = 0;
			if( sub_address == 1 ) mask = (~0xffff0000);
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b47_b32 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b47_b32 + main_addres;
		}
	}
	int ioa_data = g_pcie.read_user( ioa_address ); if( 0 ) { fprintf( stderr, "%s ioa a:0x%08X  read:0x%08X\n", __FUNCTION__, ioa_address, ioa_data ); }
	int iob_data = g_pcie.read_user( iob_address ); if( 0 ) { fprintf( stderr, "%s iob a:0x%08X  read:0x%08X\n", __FUNCTION__, iob_address, iob_data ); }

	ioa_data = ioa_data & mask;
	iob_data = iob_data & mask;
	ioa_data |= (ioa_wdata<<shift);
	iob_data |= (iob_wdata<<shift);

	g_pcie.write_user( ioa_address, ioa_data );	 if( 0 ) { fprintf( stderr, "%s ioa a:0x%08X write:0x%08X\n", __FUNCTION__, ioa_address, ioa_data ); }
	g_pcie.write_user( iob_address, iob_data );	 if( 0 ) { fprintf( stderr, "%s iob a:0x%08X write:0x%08X\n", __FUNCTION__, iob_address, iob_data ); }

}

void CUbm::SetUdata( unsigned int fc, unsigned int ioab, unsigned int index, unsigned long long int data )
{
	int main_addres = index * 4;
	int ioh_wdata   = static_cast<int>( (data>>32)&0xFFFF );
	int iol_wdata   = static_cast<int>( (data>> 0)&0xFFFFFFFF );
	int ioh_address = 0x0;
	int iol_address = 0x0;
	if( fc == 0 )
	{
		if( ioab == IOAA )
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b47_b32 + main_addres;
		}
		else
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b47_b32 + main_addres;
		}
	}
	else
	{
		if( ioab == 0 )
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b47_b32 + main_addres;
		}
		else
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b47_b32 + main_addres;
		}
	}
	g_pcie.write_user( iol_address, iol_wdata );
	g_pcie.write_user( ioh_address, ioh_wdata );
	if( 0 ) { fprintf( stderr, "%s io%c L a:0x%08X write:0x%08X\n", __FUNCTION__, (ioab==0)?'a':'b', iol_address, iol_wdata ); }
	if( 0 ) { fprintf( stderr, "%s io%c H a:0x%08X write:0x%08X\n", __FUNCTION__, (ioab==0)?'a':'b', ioh_address, ioh_wdata ); }
}

void CUbm::SetUdataOr( unsigned int fc, unsigned int ioab, int index, unsigned long long int data )
{
	int main_addres = index * 4;
	int ioh_wdata   = static_cast<int>( (data>>32)&0xFFFF );
	int iol_wdata   = static_cast<int>( (data>> 0)&0xFFFFFFFF );
	int ioh_address = 0x0;
	int iol_address = 0x0;
	if( fc == 0 )
	{
		if( ioab == 0 )
		{
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b31_b0  + main_addres;
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b47_b32 + main_addres;
		}
		else
		{
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b31_b0  + main_addres;
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b47_b32 + main_addres;
		}
	}
	else
	{
		if( ioab == 0 )
		{
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b31_b0  + main_addres;
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b47_b32 + main_addres;
		}
		else
		{
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b31_b0  + main_addres;
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b47_b32 + main_addres;
		}
	}
	int iol_rdata = g_pcie.read_user( iol_address );
	int ioh_rdata = g_pcie.read_user( ioh_address );
	int iol_or    = iol_wdata |= iol_rdata;
	int ioh_or    = ioh_wdata |= ioh_rdata;
	//iol_wdata |= iol_rdata;
	//ioh_wdata |= ioh_rdata;
	g_pcie.write_user( iol_address, iol_or );
	g_pcie.write_user( ioh_address, ioh_or );
	if( 0 ) { fprintf( stderr, "%s io%c L a:0x%08X read:0x%08X data:0x%08X or:0x%08X\n", __FUNCTION__, (ioab==0)?'a':'b', iol_address, iol_rdata, iol_wdata, iol_or ); }
	if( 0 ) { fprintf( stderr, "%s io%c H a:0x%08X read:0x%08X data:0x%08X or:0x%08X\n", __FUNCTION__, (ioab==0)?'a':'b', ioh_address, ioh_rdata, ioh_wdata, ioh_or ); }
}

//device program
//void CUbm::SetUdataDut( unsigned int dut_index, int real_scan, unsigned int failth, unsigned int data, int jump )
//{
//
//}

unsigned int CUbm::GetUdata32( unsigned int fc, int index, int index2 )
{
	int main_addres = index * 4;
	int sub_address = index2;
	int ioa_address = 0x0;
	int iob_address = 0x0;
	int shift = 0;
	if( fc == 0 )
	{
		if( sub_address < 2 )
		{
			shift = 0x0;
			if( sub_address == 1 ) shift = 16;
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b31_b0 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b31_b0 + main_addres;
		}
		else
		{
			shift = 0;
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b47_b32 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b47_b32 + main_addres;
		}
	}
	else
	{
		if( sub_address < 2 )
		{
			shift = 0;
			if( sub_address == 1 ) shift = 16;
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b31_b0 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b31_b0 + main_addres;
		}
		else
		{
			shift = 0;
			ioa_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b47_b32 + main_addres;
			iob_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b47_b32 + main_addres;
		}
	}
	unsigned int ioa_data = ( g_pcie.read_user( ioa_address ) >> shift ) & 0xFFFF;
	unsigned int iob_data = ( g_pcie.read_user( iob_address ) >> shift ) & 0xFFFF;
	unsigned int ioab_data = (iob_data<<16) | (ioa_data&0xFFFF);
	if( 0 )
	{
		fprintf( stderr, "%s ioa  a:0x%08X  read:0x%08X\n", __FUNCTION__, ioa_address, ioa_data  );
		fprintf( stderr, "%s iob  a:0x%08X  read:0x%08X\n", __FUNCTION__, iob_address, iob_data  );
		fprintf( stderr, "%s ioab           data:0x%08X\n", __FUNCTION__, ioab_data );
	}
	return ioab_data;
}

unsigned long long CUbm::GetUdata( unsigned int fc, unsigned int ioab, int index )
{
	unsigned long long iohl_data = 0x0LL;
	int main_addres = index * 4;
	int ioh_address = 0x0;
	int iol_address = 0x0;
	int ioh_data    = 0x0;
	int iol_data    = 0x0;

	if( fc == 0 )
	{
		if( ioab == 0 )
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOA_b47_b32 + main_addres;
		}
		else
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC1_IOB_b47_b32 + main_addres;
		}
	}
	else
	{
		if( ioab == 0 )
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOA_b47_b32 + main_addres;
		}
		else
		{
			iol_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b31_b0  + main_addres;
			ioh_address = BITCHART::ALPG::UBM_REG::UBM_FC2_IOB_b47_b32 + main_addres;
		}
	}
	iol_data = g_pcie.read_user( iol_address );
	ioh_data = g_pcie.read_user( ioh_address );
	iohl_data = ((static_cast<unsigned long long>(ioh_data)<<32)&0xffffffff00000000 ) | (static_cast<unsigned long long>(iol_data)&0xffffffff);
	//printf("[UBM][DBG] READ(0x%08X : 0x%08X)(0x%08X : 0x%08X) --> 0x%llX, 0x%llX, 0x%llX\n", 
	//		iol_address, iol_data, ioh_address, ioh_data, iohl_data, static_cast<unsigned long long>(ioh_data)<<32, static_cast<unsigned long long>(iol_data), iohl_data );
	return iohl_data;
}

void CUbm::PrintfUbm( int fc, int size )
{

}
