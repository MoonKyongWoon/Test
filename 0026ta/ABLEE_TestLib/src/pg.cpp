#include "bitchart.hpp"
#include "pg.hpp"
#include "tg.hpp"
#include "io.hpp"

extern CPcie g_pcie;
extern CSys  g_sys;
extern CTg   g_tg;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PG
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPg::CPg() :
	_IDX0 (RIDX0 , BITCHART::ALPG::_IDX1 , false),
	_IDX1 (RIDX1 , BITCHART::ALPG::_IDX2 , false),
	_IDX2 (RIDX2 , BITCHART::ALPG::_IDX3 , false),
	_IDX3 (RIDX3 , BITCHART::ALPG::_IDX4 , false),
	_IDX4 (RIDX4 , BITCHART::ALPG::_IDX5 , false),
	_IDX5 (RIDX5 , BITCHART::ALPG::_IDX6 , false),
	_IDX6 (RIDX6 , BITCHART::ALPG::_IDX7 , false),
	_IDX7 (RIDX7 , BITCHART::ALPG::_IDX8 , false),
	_IDX8 (RIDX8 , BITCHART::ALPG::_IDX9 , false),
	_IDX9 (RIDX9 , BITCHART::ALPG::_IDX10, false),
	_IDX10(RIDX10, BITCHART::ALPG::_IDX11, false),
	_IDX11(RIDX11, BITCHART::ALPG::_IDX12, false),
	_IDX12(RIDX12, BITCHART::ALPG::_IDX13, false),
	_IDX13(RIDX13, BITCHART::ALPG::_IDX14, false),
	_IDX14(RIDX14, BITCHART::ALPG::_IDX15, false),
	_IDX15(RIDX15, BITCHART::ALPG::_IDX16, false),

	_CFLAG(RCFLAG, BITCHART::ALPG::_CFLAG         , false),
	_ISP  (RISP  , BITCHART::ALPG::_ISP_REGISTER  , false),
	_BAR  (RBAR  , BITCHART::ALPG::_BAR_REGISTER  , false),
	_MLOOP(RMLOOP, BITCHART::ALPG::_MLOOP_REGISTER, false),

//2023.06.13 - kwmoon(comment fatric)
//	_D1B  (RD1B  , BITCHART::ALPG::_D1B, false),
//	_D1C  (RD1C  , BITCHART::ALPG::_D1C, false),
//	_D1D  (RD1D  , BITCHART::ALPG::_D1D, false),
//	_D1E  (RD1E  , BITCHART::ALPG::_D1E, false),
//	_D1F  (RD1F  , BITCHART::ALPG::_D1F, false),
//	_D1G  (RD1G  , BITCHART::ALPG::_D1G, false),
//	_D1H  (RD1H  , BITCHART::ALPG::_D1H, false),
	_D2B  (RD2B  , BITCHART::ALPG::_D2B, false),
	_D2C  (RD2C  , BITCHART::ALPG::_D2C, false),
	_D2D  (RD2D  , BITCHART::ALPG::_D2D, false),

	_LMAX ( RLMAX, BITCHART::ALPG::_LMAX, false),
	_HMAX ( RHMAX, BITCHART::ALPG::_HMAX, false),
	_XMAX ( RXMAX, BITCHART::ALPG::_XMAX, false),
	_YMAX ( RYMAX, BITCHART::ALPG::_YMAX, false),
	_ZMAX ( RZMAX, BITCHART::ALPG::_ZMAX, false),

	_ND2  ( RND2 , BITCHART::ALPG::_ND2 , false),
	_ND3  ( RND3 , BITCHART::ALPG::_ND3 , false),
	_ND4  ( RND4 , BITCHART::ALPG::_ND4 , false),
	_ND5  ( RND5 , BITCHART::ALPG::_ND5 , false),
	_ND6  ( RND6 , BITCHART::ALPG::_ND6 , false),
	_ND7  ( RND7 , BITCHART::ALPG::_ND7 , false),
	_ND8  ( RND8 , BITCHART::ALPG::_ND8 , false),
	_ND9  ( RND9 , BITCHART::ALPG::_ND9 , false),
	_ND10 ( RND10, BITCHART::ALPG::_ND10, false),
	_ND11 ( RND11, BITCHART::ALPG::_ND11, false),
	_ND12 ( RND12, BITCHART::ALPG::_ND12, false),
	_ND13 ( RND13, BITCHART::ALPG::_ND13, false),
	_ND14 ( RND14, BITCHART::ALPG::_ND14, false),
	_ND15 ( RND15, BITCHART::ALPG::_ND15, false),
	_ND16 ( RND16, BITCHART::ALPG::_ND16, false),
	_ND17 ( RND17, BITCHART::ALPG::_ND17, false),
	_ND18 ( RND18, BITCHART::ALPG::_ND18, false),
	_ND19 ( RND19, BITCHART::ALPG::_ND19, false),
	_ND20 ( RND20, BITCHART::ALPG::_ND20, false),
	_ND21 ( RND21, BITCHART::ALPG::_ND21, false),
	_ND22 ( RND22, BITCHART::ALPG::_ND22, false),
	_ND23 ( RND23, BITCHART::ALPG::_ND23, false),
	_ND24 ( RND24, BITCHART::ALPG::_ND24, false),
	_ND25 ( RND25, BITCHART::ALPG::_ND25, false),
	_ND26 ( RND26, BITCHART::ALPG::_ND26, false),
	_ND27 ( RND27, BITCHART::ALPG::_ND27, false),
	_ND28 ( RND28, BITCHART::ALPG::_ND28, false),
	_ND29 ( RND29, BITCHART::ALPG::_ND29, false),
	_ND30 ( RND30, BITCHART::ALPG::_ND30, false),
	_ND31 ( RND31, BITCHART::ALPG::_ND31, false),
	_ND32 ( RND32, BITCHART::ALPG::_ND32, false),

	_BD2  ( RBD2 , BITCHART::ALPG::_BD2 , false),
	_BD3  ( RBD3 , BITCHART::ALPG::_BD3 , false),
	_BD4  ( RBD4 , BITCHART::ALPG::_BD4 , false),
	_BD5  ( RBD5 , BITCHART::ALPG::_BD5 , false),
	_BD6  ( RBD6 , BITCHART::ALPG::_BD6 , false),
	_BD7  ( RBD7 , BITCHART::ALPG::_BD7 , false),
	_BD8  ( RBD8 , BITCHART::ALPG::_BD8 , false),
	_BD9  ( RBD9 , BITCHART::ALPG::_BD9 , false),
	_BD10 ( RBD10, BITCHART::ALPG::_BD10, false),
	_BD11 ( RBD11, BITCHART::ALPG::_BD11, false),
	_BD12 ( RBD12, BITCHART::ALPG::_BD12, false),
	_BD13 ( RBD13, BITCHART::ALPG::_BD13, false),
	_BD14 ( RBD14, BITCHART::ALPG::_BD14, false),
	_BD15 ( RBD15, BITCHART::ALPG::_BD15, false),
	_BD16 ( RBD16, BITCHART::ALPG::_BD16, false),
	_BD17 ( RBD17, BITCHART::ALPG::_BD17, false),
	_BD18 ( RBD18, BITCHART::ALPG::_BD18, false),
	_BD19 ( RBD19, BITCHART::ALPG::_BD19, false),
	_BD20 ( RBD20, BITCHART::ALPG::_BD20, false),
	_BD21 ( RBD21, BITCHART::ALPG::_BD21, false),
	_BD22 ( RBD22, BITCHART::ALPG::_BD22, false),
	_BD23 ( RBD23, BITCHART::ALPG::_BD23, false),
	_BD24 ( RBD24, BITCHART::ALPG::_BD24, false),
	_BD25 ( RBD25, BITCHART::ALPG::_BD25, false),
	_BD26 ( RBD26, BITCHART::ALPG::_BD26, false),
	_BD27 ( RBD27, BITCHART::ALPG::_BD27, false),
	_BD28 ( RBD28, BITCHART::ALPG::_BD28, false),
	_BD29 ( RBD29, BITCHART::ALPG::_BD29, false),
	_BD30 ( RBD30, BITCHART::ALPG::_BD30, false),
	_BD31 ( RBD31, BITCHART::ALPG::_BD31, false),
	_BD32 ( RBD32, BITCHART::ALPG::_BD32, false),

	_L1A ( RL1A, BITCHART::ALPG::_L1A, false),
	_L1B ( RL1B, BITCHART::ALPG::_L1B, false),
	_L1C ( RL1C, BITCHART::ALPG::_L1C, false),
	_L1D ( RL1D, BITCHART::ALPG::_L1D, false),
	_L1E ( RL1E, BITCHART::ALPG::_L1E, false),
	_L1F ( RL1F, BITCHART::ALPG::_L1F, false),
	_L1G ( RL1G, BITCHART::ALPG::_L1G, false),
	_L1H ( RL1H, BITCHART::ALPG::_L1H, false),

	_L2A ( RL2A, BITCHART::ALPG::_L2A, false),
	_L2B ( RL2B, BITCHART::ALPG::_L2B, false),
	_L2C ( RL2C, BITCHART::ALPG::_L2C, false),
	_L2D ( RL2D, BITCHART::ALPG::_L2D, false),
	_L2E ( RL2E, BITCHART::ALPG::_L2E, false),
	_L2F ( RL2F, BITCHART::ALPG::_L2F, false),
	_L2G ( RL2G, BITCHART::ALPG::_L2G, false),
	_L2H ( RL2H, BITCHART::ALPG::_L2H, false),

	_L3A ( RL2A, BITCHART::ALPG::_L3A, false),
	_L3B ( RL2B, BITCHART::ALPG::_L3B, false),
	_L3C ( RL2C, BITCHART::ALPG::_L3C, false),
	_L3D ( RL2D, BITCHART::ALPG::_L3D, false),
	_L3E ( RL2E, BITCHART::ALPG::_L3E, false),
	_L3F ( RL2F, BITCHART::ALPG::_L3F, false),
	_L3G ( RL2G, BITCHART::ALPG::_L3G, false),
	_L3H ( RL2H, BITCHART::ALPG::_L3H, false),

	_L4A ( RL2A, BITCHART::ALPG::_L4A, false),
	_L4B ( RL2B, BITCHART::ALPG::_L4B, false),
	_L4C ( RL2C, BITCHART::ALPG::_L4C, false),
	_L4D ( RL2D, BITCHART::ALPG::_L4D, false),
	_L4E ( RL2E, BITCHART::ALPG::_L4E, false),
	_L4F ( RL2F, BITCHART::ALPG::_L4F, false),
	_L4G ( RL2G, BITCHART::ALPG::_L4G, false),
	_L4H ( RL2H, BITCHART::ALPG::_L4H, false),

	_S1A ( RS1A, BITCHART::ALPG::_S1A, false),
	_V1A ( RV1A, BITCHART::ALPG::_V1A, false),

	_XH   ( RXH  , 0, 0x1 ), _YH   ( RYH  , 1, 0x10 ), _TP1  ( RTP1  , 2, 0x100 ), _TP2  ( RTP2  , 3, 0x1000 ),
	_D1A  ( RD1A , 0, 0x2 ), _D2A  ( RD2A , 1, 0x20 ), _TPH1 ( RTPH1 , 2, 0x200 ), _TPH2 ( RTPH2 , 3, 0x2000 ),
	_D3B  ( RD3B , 0, 0x3 ), _D4B  ( RD4B , 1, 0x30 ), _D5   ( RD5   , 2, 0x300 ), _D6   ( RD6   , 3, 0x3000 ),
	_NH   ( RNH  , 0, 0x4 ), _BH   ( RBH  , 1, 0x40 ), _DCMR1( RDCMR1, 2, 0x400 ), _DCMR2( RDCMR2, 3, 0x4000 ),
//							 _ZH   ( RZH  , 1, 0x50 ), _XMASK( RXMASK, 2, 0x500 ), _YMASK( RYMASK, 3, 0x5000 ),	//2023.06.13 - kwmoon(comment fatric)
	_D1B  ( RD1B , 0, 0x5 ), _ZH   ( RZH  , 1, 0x50 ), _XMASK( RXMASK, 2, 0x500 ), _YMASK( RYMASK, 3, 0x5000 ),
	_XOS  ( RXOS , 0, 0x6 ), _YOS  ( RYOS , 1, 0x60 ), _CBMA ( RCBMA , 2, 0x600 ), _DSC  ( RDSC  , 3, 0x6000 ),
	_XT   ( RXT  , 0, 0x7 ), _YT   ( RYT  , 1, 0x70 ), _RCD  ( RRCD  , 2, 0x700 ), _CCD  ( RCCD  , 3, 0x7000 ),
	_ZD   ( RZD  , 0, 0x8 ), _Z    ( RZ   , 1, 0x80 ), _DSD  ( RDSD  , 2, 0x800 ),
//	_ND   ( RND  , 0, 0x9 ), _BD   ( RBD  , 1, 0x90 ), _DBMAR( RDBMAR, 2, 0x900 ),	//2023.06.13 - kwmoon(comment fatric)
	_D1C  ( RD1C , 0, 0x9 ), _BD   ( RBD  , 1, 0x90 ), _DBMAR( RDBMAR, 2, 0x900 ),
	_DIGI1( RDIG1, 0, 0xA ), _DIGI2( RDIG2, 1, 0xA0 ), _DIGI3( RDIG3 , 2, 0xA00 ),
	_D1D  ( RD1D , 0, 0xB ),						   _TPH1A( RTPH1A, 2, 0xB00 ), _TPH2A( RTPH2A, 3, 0xB000 ),	//2023.06.13 - kwmoon(modify fatric)
	_D1E  ( RD1E , 0, 0xC ),						   _TPH1B( RTPH1B, 2, 0xC00 ), _TPH2B( RTPH2B, 3, 0xC000 ),	//2023.06.13 - kwmoon(modify fatric)
	_D1F  ( RD1F , 0, 0xD ),						   _TPH1C( RTPH1C, 2, 0xD00 ), _TPH2C( RTPH2C, 3, 0xD000 ),	//2023.06.13 - kwmoon(modify fatric)
	_D1G  ( RD1G , 0, 0xE ),						   _TPH1D( RTPH1D, 2, 0xE00 ), _TPH2D( RTPH2D, 3, 0xE000 ),	//2023.06.13 - kwmoon(modify fatric)
	_D1H  ( RD1H , 0, 0xF ),

	_PD0 (PIN_DR,RPD0) , _PD1 (PIN_DR,RPD1) , _PD2 (PIN_DR,RPD2) , _PD3 (PIN_DR,RPD3 ), _PD4 (PIN_DR,RPD4) , _PD5 (PIN_DR,RPD5) , _PD6 (PIN_DR,RPD6) , _PD7 (PIN_DR,RPD7) ,
	_PD8 (PIN_DR,RPD8) , _PD9 (PIN_DR,RPD9) , _PD10(PIN_DR,RPD10), _PD11(PIN_DR,RPD11), _PD12(PIN_DR,RPD12), _PD13(PIN_DR,RPD13), _PD14(PIN_DR,RPD14), _PD15(PIN_DR,RPD15),
	_PD16(PIN_DR,RPD16), _PD17(PIN_DR,RPD17), _PD18(PIN_DR,RPD18), _PD19(PIN_DR,RPD19), _PD20(PIN_DR,RPD20), _PD21(PIN_DR,RPD21), _PD22(PIN_DR,RPD22), _PD23(PIN_DR,RPD23),
	_PD24(PIN_DR,RPD24), _PD25(PIN_DR,RPD25), _PD26(PIN_DR,RPD26), _PD27(PIN_DR,RPD27), _PD28(PIN_DR,RPD28), _PD29(PIN_DR,RPD29), _PD30(PIN_DR,RPD30), _PD31(PIN_DR,RPD31),
	_P0(PIN_IO,RP0), _P1(PIN_IO,RP1), _P2 (PIN_IO,RP2) , _P3 (PIN_IO,RP3) , _P4 (PIN_IO,RP4) , _P5 (PIN_IO,RP5) , _P6 (PIN_IO,RP6) , _P7 (PIN_IO,RP7) ,
	_P8(PIN_IO,RP8), _P9(PIN_IO,RP9), _P10(PIN_IO,RP10), _P11(PIN_IO,RP11), _P12(PIN_IO,RP12), _P13(PIN_IO,RP13), _P14(PIN_IO,RP14), _P15(PIN_IO,RP15),
	_P16(PIN_IO,RP16), _P17(PIN_IO,RP17), _P18(PIN_IO,RP18), _P19(PIN_IO,RP19), _P20(PIN_IO,RP20), _P21(PIN_IO,RP21), _P22(PIN_IO,RP22), _P23(PIN_IO,RP23),
	_P24(PIN_IO,RP24), _P25(PIN_IO,RP25), _P26(PIN_IO,RP26), _P27(PIN_IO,RP27), _P28(PIN_IO,RP28), _P29(PIN_IO,RP29), _P30(PIN_IO,RP30), _P31(PIN_IO,RP31),
	_P32(PIN_IO,RP32), _P33(PIN_IO,RP33), _P34(PIN_IO,RP34), _P35(PIN_IO,RP35), _P36(PIN_IO,RP36), _P37(PIN_IO,RP37), _P38(PIN_IO,RP38), _P39(PIN_IO,RP39),
	_P40(PIN_IO,RP40), _P41(PIN_IO,RP41), _P42(PIN_IO,RP42), _P43(PIN_IO,RP43), _P44(PIN_IO,RP44), _P45(PIN_IO,RP45), _P46(PIN_IO,RP46), _P47(PIN_IO,RP47),
	_UCLK0 (PIN_UCLK,RUCLK0) , _UCLK1 (PIN_UCLK,RUCLK1) , _UCLK2 (PIN_UCLK,RUCLK2) , _UCLK3 (PIN_UCLK,RUCLK3) , _UCLK4 (PIN_UCLK,RUCLK4) , _UCLK5 (PIN_UCLK,RUCLK5) , _UCLK6 (PIN_UCLK,RUCLK6) , _UCLK7 (PIN_UCLK,RUCLK7) ,
	_UCLK8 (PIN_UCLK,RUCLK8) , _UCLK9 (PIN_UCLK,RUCLK9) , _UCLK10(PIN_UCLK,RUCLK10), _UCLK11(PIN_UCLK,RUCLK11), _UCLK12(PIN_UCLK,RUCLK12), _UCLK13(PIN_UCLK,RUCLK13), _UCLK14(PIN_UCLK,RUCLK14), _UCLK15(PIN_UCLK,RUCLK15),
	_UCLK16(PIN_UCLK,RUCLK16), _UCLK17(PIN_UCLK,RUCLK17), _UCLK18(PIN_UCLK,RUCLK18), _UCLK19(PIN_UCLK,RUCLK19), _UCLK20(PIN_UCLK,RUCLK20), _UCLK21(PIN_UCLK,RUCLK21), _UCLK22(PIN_UCLK,RUCLK22), _UCLK23(PIN_UCLK,RUCLK23),
	_UCLK24(PIN_UCLK,RUCLK24), _UCLK25(PIN_UCLK,RUCLK25), _UCLK26(PIN_UCLK,RUCLK26), _UCLK27(PIN_UCLK,RUCLK27), _UCLK28(PIN_UCLK,RUCLK28), _UCLK29(PIN_UCLK,RUCLK29), _UCLK30(PIN_UCLK,RUCLK30), _UCLK31(PIN_UCLK,RUCLK31),
	_UCLK32(PIN_UCLK,RUCLK32), _UCLK33(PIN_UCLK,RUCLK33), _UCLK34(PIN_UCLK,RUCLK34), _UCLK35(PIN_UCLK,RUCLK35), _UCLK36(PIN_UCLK,RUCLK36), _UCLK37(PIN_UCLK,RUCLK37), _UCLK38(PIN_UCLK,RUCLK38), _UCLK39(PIN_UCLK,RUCLK39),
	_UCLK40(PIN_UCLK,RUCLK40), _UCLK41(PIN_UCLK,RUCLK41), _UCLK42(PIN_UCLK,RUCLK42), _UCLK43(PIN_UCLK,RUCLK43), _UCLK44(PIN_UCLK,RUCLK44), _UCLK45(PIN_UCLK,RUCLK45), _UCLK46(PIN_UCLK,RUCLK46), _UCLK47(PIN_UCLK,RUCLK47),
	_UCLK48(PIN_UCLK,RUCLK48), _UCLK49(PIN_UCLK,RUCLK49), _UCLK50(PIN_UCLK,RUCLK50), _UCLK51(PIN_UCLK,RUCLK51), _UCLK52(PIN_UCLK,RUCLK52), _UCLK53(PIN_UCLK,RUCLK53), _UCLK54(PIN_UCLK,RUCLK54), _UCLK55(PIN_UCLK,RUCLK55),
	_UCLK56(PIN_UCLK,RUCLK56), _UCLK57(PIN_UCLK,RUCLK57), _UCLK58(PIN_UCLK,RUCLK58), _UCLK59(PIN_UCLK,RUCLK59), _UCLK60(PIN_UCLK,RUCLK60), _UCLK61(PIN_UCLK,RUCLK61), _UCLK62(PIN_UCLK,RUCLK62), _UCLK63(PIN_UCLK,RUCLK63),
	_UCLK64(PIN_UCLK,RUCLK64), _UCLK65(PIN_UCLK,RUCLK65), _UCLK66(PIN_UCLK,RUCLK66), _UCLK67(PIN_UCLK,RUCLK67), _UCLK68(PIN_UCLK,RUCLK68), _UCLK69(PIN_UCLK,RUCLK69), _UCLK70(PIN_UCLK,RUCLK70), _UCLK71(PIN_UCLK,RUCLK71),
	_UCLK72(PIN_UCLK,RUCLK72), _UCLK73(PIN_UCLK,RUCLK73), _UCLK74(PIN_UCLK,RUCLK74), _UCLK75(PIN_UCLK,RUCLK75), _UCLK76(PIN_UCLK,RUCLK76), _UCLK77(PIN_UCLK,RUCLK77), _UCLK78(PIN_UCLK,RUCLK78), _UCLK79(PIN_UCLK,RUCLK79),
	_UCLK80(PIN_UCLK,RUCLK80), _UCLK81(PIN_UCLK,RUCLK81), _UCLK82(PIN_UCLK,RUCLK82), _UCLK83(PIN_UCLK,RUCLK83), _UCLK84(PIN_UCLK,RUCLK84), _UCLK85(PIN_UCLK,RUCLK85), _UCLK86(PIN_UCLK,RUCLK86), _UCLK87(PIN_UCLK,RUCLK87),
	_DREL  (PIN_DRE , RDREL ), _DRET  (PIN_DRE , RDRET ), _DRDRE (PIN_DRE ,RDRE2  ),
	_CPE0  (PIN_CPE , RCPE0 ), _CPE1  (PIN_CPE , RCPE1 ), _CPE2  (PIN_CPE , RCPE2 ), _CPE3  (PIN_CPE , RCPE3 ),
	_FMA0  (PIN_FMA , RFMA0 ), _FMA1  (PIN_FMA , RFMA1 ), _FMA2  (PIN_FMA , RFMA2 ), _FMA3  (PIN_FMA , RFMA3 ), _FMA4  (PIN_FMA , RFMA4 ), _FMA5  (PIN_FMA , RFMA5 ), _FMA6  (PIN_FMA , RFMA6 ), _FMA7  (PIN_FMA , RFMA7 ),
	_FMA8  (PIN_FMA , RFMA8 ), _FMA9  (PIN_FMA , RFMA9 ), _FMA10 (PIN_FMA , RFMA10), _FMA11 (PIN_FMA , RFMA11), _FMA12 (PIN_FMA , RFMA12), _FMA13 (PIN_FMA , RFMA13), _FMA14 (PIN_FMA , RFMA14), _FMA15 (PIN_FMA , RFMA15),
	_FMA16 (PIN_FMA , RFMA16), _FMA17 (PIN_FMA , RFMA17), _FMA18 (PIN_FMA , RFMA18), _FMA19 (PIN_FMA , RFMA19), _FMA20 (PIN_FMA , RFMA20), _FMA21 (PIN_FMA , RFMA21), _FMA22 (PIN_FMA , RFMA22), _FMA23 (PIN_FMA , RFMA23),
	_FMA24 (PIN_FMA , RFMA24), _FMA25 (PIN_FMA , RFMA25),
	_MUT0  (PIN_MUT , RMUT0 ), _MUT1  (PIN_MUT , RMUT1 ), _MUT2  (PIN_MUT , RMUT2 ), _MUT3  (PIN_MUT , RMUT3 ), _MUT4  (PIN_MUT , RMUT4 ), _MUT5  (PIN_MUT , RMUT5 ), _MUT6  (PIN_MUT , RMUT6 ), _MUT7  (PIN_MUT , RMUT7 ),
	_MUT8  (PIN_MUT , RMUT8 ), _MUT9  (PIN_MUT , RMUT9 ), _MUT10 (PIN_MUT , RMUT10), _MUT11 (PIN_MUT , RMUT11), _MUT12 (PIN_MUT , RMUT12), _MUT13 (PIN_MUT , RMUT13), _MUT14 (PIN_MUT , RMUT14), _MUT15 (PIN_MUT , RMUT15)
{
	m_pPgc = new CPatCl;
	m_rVecError.clear();
	m_bModule = false;
	m_PGDATA = nullptr;
	m_nCurrentTok = 0;
	SetIoDdrModeGlobal( SDR );	//2022.11.23 - kwmoon
	InitRegisterStr();
	InitPins();
	InitModule();
	InitCyp();
	InitPinOutVal();	//2023.04.21 - fatric
}

CPg::~CPg()
{
	if( m_pPgc )
	{
		delete m_pPgc;
		m_pPgc = nullptr;
	}
}

void CPg::InitRegisterStr()
{
	m_mapRegStr.clear();
	m_mapSetStr.clear();
//////////////////////////////////////////////////////////////////////////////
/// GLOBAL REGISTER
//////////////////////////////////////////////////////////////////////////////
	m_mapRegStr[ &_IDX0  ] = "IDX0    ";
	m_mapRegStr[ &_IDX1  ] = "IDX1    ";
	m_mapRegStr[ &_IDX2  ] = "IDX2    ";
	m_mapRegStr[ &_IDX3  ] = "IDX3    ";
	m_mapRegStr[ &_IDX4  ] = "IDX4    ";
	m_mapRegStr[ &_IDX5  ] = "IDX5    ";
	m_mapRegStr[ &_IDX6  ] = "IDX6    ";
	m_mapRegStr[ &_IDX7  ] = "IDX7    ";
	m_mapRegStr[ &_IDX8  ] = "IDX8    ";
	m_mapRegStr[ &_IDX9  ] = "IDX9    ";
	m_mapRegStr[ &_IDX10 ] = "IDX10   ";
	m_mapRegStr[ &_IDX11 ] = "IDX11   ";
	m_mapRegStr[ &_IDX12 ] = "IDX12   ";
	m_mapRegStr[ &_IDX13 ] = "IDX13   ";
	m_mapRegStr[ &_IDX14 ] = "IDX14   ";
	m_mapRegStr[ &_IDX15 ] = "IDX15   ";
	m_mapRegStr[ &_CFLAG ] = "CFLAG   ";
	m_mapRegStr[ &_ISP   ] = "ISP     ";
	m_mapRegStr[ &_BAR   ] = "BAR     ";
//2023.06.13 - kwmoon(comment fatric)	
//	m_mapRegStr[ &_D1B   ] = "D1B     ";
//	m_mapRegStr[ &_D1C   ] = "D1C     ";
//	m_mapRegStr[ &_D1D   ] = "D1D     ";
//	m_mapRegStr[ &_D1E   ] = "D1E     ";
//	m_mapRegStr[ &_D1F   ] = "D1F     ";
//	m_mapRegStr[ &_D1G   ] = "D1G     ";
//	m_mapRegStr[ &_D1H   ] = "D1H     ";
	m_mapRegStr[ &_D2B   ] = "D2B     ";
	m_mapRegStr[ &_D2C   ] = "D2C     ";
	m_mapRegStr[ &_D2D   ] = "D2D     ";

	m_mapRegStr[ &_LMAX  ] = "LMAX    ";
	m_mapRegStr[ &_HMAX  ] = "HMAX    ";
	m_mapRegStr[ &_XMAX  ] = "XMAX    ";
	m_mapRegStr[ &_YMAX  ] = "YMAX    ";
	m_mapRegStr[ &_ZMAX  ] = "ZMAX    ";

	m_mapRegStr[ &_ND2   ] = "ND2     ";
	m_mapRegStr[ &_ND3   ] = "ND3     ";
	m_mapRegStr[ &_ND4   ] = "ND4     ";
	m_mapRegStr[ &_ND5   ] = "ND5     ";
	m_mapRegStr[ &_ND6   ] = "ND6     ";
	m_mapRegStr[ &_ND7   ] = "ND7     ";
	m_mapRegStr[ &_ND8   ] = "ND8     ";
	m_mapRegStr[ &_ND9   ] = "ND9     ";
	m_mapRegStr[ &_ND10  ] = "ND10    ";
	m_mapRegStr[ &_ND11  ] = "ND11    ";
	m_mapRegStr[ &_ND12  ] = "ND12    ";
	m_mapRegStr[ &_ND13  ] = "ND13    ";
	m_mapRegStr[ &_ND14  ] = "ND14    ";
	m_mapRegStr[ &_ND15  ] = "ND15    ";
	m_mapRegStr[ &_ND16  ] = "ND16    ";
	m_mapRegStr[ &_ND17  ] = "ND17    ";
	m_mapRegStr[ &_ND18  ] = "ND18    ";
	m_mapRegStr[ &_ND19  ] = "ND19    ";
	m_mapRegStr[ &_ND20  ] = "ND20    ";
	m_mapRegStr[ &_ND21  ] = "ND21    ";
	m_mapRegStr[ &_ND22  ] = "ND22    ";
	m_mapRegStr[ &_ND23  ] = "ND23    ";
	m_mapRegStr[ &_ND24  ] = "ND24    ";
	m_mapRegStr[ &_ND25  ] = "ND25    ";
	m_mapRegStr[ &_ND26  ] = "ND26    ";
	m_mapRegStr[ &_ND27  ] = "ND27    ";
	m_mapRegStr[ &_ND28  ] = "ND28    ";
	m_mapRegStr[ &_ND29  ] = "ND29    ";
	m_mapRegStr[ &_ND30  ] = "ND30    ";
	m_mapRegStr[ &_ND31  ] = "ND31    ";
	m_mapRegStr[ &_ND32  ] = "ND32    ";

	m_mapRegStr[ &_BD2   ] = "BD2     ";
	m_mapRegStr[ &_BD3   ] = "BD3     ";
	m_mapRegStr[ &_BD4   ] = "BD4     ";
	m_mapRegStr[ &_BD5   ] = "BD5     ";
	m_mapRegStr[ &_BD6   ] = "BD6     ";
	m_mapRegStr[ &_BD7   ] = "BD7     ";
	m_mapRegStr[ &_BD8   ] = "BD8     ";
	m_mapRegStr[ &_BD9   ] = "BD9     ";
	m_mapRegStr[ &_BD10  ] = "BD10    ";
	m_mapRegStr[ &_BD11  ] = "BD11    ";
	m_mapRegStr[ &_BD12  ] = "BD12    ";
	m_mapRegStr[ &_BD13  ] = "BD13    ";
	m_mapRegStr[ &_BD14  ] = "BD14    ";
	m_mapRegStr[ &_BD15  ] = "BD15    ";
	m_mapRegStr[ &_BD16  ] = "BD16    ";
	m_mapRegStr[ &_BD17  ] = "BD17    ";
	m_mapRegStr[ &_BD18  ] = "BD18    ";
	m_mapRegStr[ &_BD19  ] = "BD19    ";
	m_mapRegStr[ &_BD20  ] = "BD20    ";
	m_mapRegStr[ &_BD21  ] = "BD21    ";
	m_mapRegStr[ &_BD22  ] = "BD22    ";
	m_mapRegStr[ &_BD23  ] = "BD23    ";
	m_mapRegStr[ &_BD24  ] = "BD24    ";
	m_mapRegStr[ &_BD25  ] = "BD25    ";
	m_mapRegStr[ &_BD26  ] = "BD26    ";
	m_mapRegStr[ &_BD27  ] = "BD27    ";
	m_mapRegStr[ &_BD28  ] = "BD28    ";
	m_mapRegStr[ &_BD29  ] = "BD29    ";
	m_mapRegStr[ &_BD30  ] = "BD30    ";
	m_mapRegStr[ &_BD31  ] = "BD31    ";
	m_mapRegStr[ &_BD32  ] = "BD32    ";

	m_mapRegStr[ &_L1A   ] = "L1A     ";
	m_mapRegStr[ &_L1B   ] = "L1B     ";
	m_mapRegStr[ &_L1C   ] = "L1C     ";
	m_mapRegStr[ &_L1D   ] = "L1D     ";
	m_mapRegStr[ &_L1E   ] = "L1E     ";
	m_mapRegStr[ &_L1F   ] = "L1F     ";
	m_mapRegStr[ &_L1G   ] = "L1G     ";
	m_mapRegStr[ &_L1H   ] = "L1H     ";

	m_mapRegStr[ &_L2A   ] = "L2A     ";
	m_mapRegStr[ &_L2B   ] = "L2B     ";
	m_mapRegStr[ &_L2C   ] = "L2C     ";
	m_mapRegStr[ &_L2D   ] = "L2D     ";
	m_mapRegStr[ &_L2E   ] = "L2E     ";
	m_mapRegStr[ &_L2F   ] = "L2F     ";
	m_mapRegStr[ &_L2G   ] = "L2G     ";
	m_mapRegStr[ &_L2H   ] = "L2H     ";
									   ;
	m_mapRegStr[ &_L3A   ] = "L3A     ";
	m_mapRegStr[ &_L3B   ] = "L3B     ";
	m_mapRegStr[ &_L3C   ] = "L3C     ";
	m_mapRegStr[ &_L3D   ] = "L3D     ";
	m_mapRegStr[ &_L3E   ] = "L3E     ";
	m_mapRegStr[ &_L3F   ] = "L3F     ";
	m_mapRegStr[ &_L3G   ] = "L3G     ";
	m_mapRegStr[ &_L3H   ] = "L3H     ";
									   ;
	m_mapRegStr[ &_L4A   ] = "L4A     ";
	m_mapRegStr[ &_L4B   ] = "L4B     ";
	m_mapRegStr[ &_L4C   ] = "L4C     ";
	m_mapRegStr[ &_L4D   ] = "L4D     ";
	m_mapRegStr[ &_L4E   ] = "L4E     ";
	m_mapRegStr[ &_L4F   ] = "L4F     ";
	m_mapRegStr[ &_L4G   ] = "L4G     ";
	m_mapRegStr[ &_L4H   ] = "L4H     ";
									   ;
	m_mapRegStr[ &_S1A   ] = "S1A     ";
	m_mapRegStr[ &_V1A   ] = "V1A     ";

	//////////////////////////////////////////////////////////////////////////////
	/// SET/DSET
	//////////////////////////////////////////////////////////////////////////////
	m_mapSetStr[ &_XH    ] = "XH      ";
	m_mapSetStr[ &_YH    ] = "YH      ";
	m_mapSetStr[ &_TP1   ] = "TP1     ";
	m_mapSetStr[ &_TP2   ] = "TP2     ";
	m_mapSetStr[ &_D1A   ] = "D1A     ";
	m_mapSetStr[ &_D2A   ] = "D2A     ";
	m_mapSetStr[ &_TPH1  ] = "TPH1    ";
	m_mapSetStr[ &_TPH2  ] = "TPH2    ";
	m_mapSetStr[ &_D3B   ] = "D3B     ";
	m_mapSetStr[ &_D4B   ] = "D4B     ";
	m_mapSetStr[ &_D5    ] = "D5      ";
	m_mapSetStr[ &_D6    ] = "D6      ";
	m_mapSetStr[ &_NH    ] = "NH      ";
	m_mapSetStr[ &_BH    ] = "BH      ";
	m_mapSetStr[ &_DCMR1 ] = "DCMR1   ";
	m_mapSetStr[ &_DCMR2 ] = "DCMR2   ";
	m_mapSetStr[ &_D1B   ] = "D1B     ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_ZH    ] = "ZH      ";
	m_mapSetStr[ &_XMASK ] = "XMASK   ";
	m_mapSetStr[ &_YMASK ] = "YMASK   ";
	m_mapSetStr[ &_XOS   ] = "XOS     ";
	m_mapSetStr[ &_YOS   ] = "YOS     ";
	m_mapSetStr[ &_CBMA  ] = "CBMA    ";
	m_mapSetStr[ &_DSC   ] = "DSC     ";
	m_mapSetStr[ &_ZD    ] = "ZD      ";
	m_mapSetStr[ &_Z     ] = "Z       ";
	m_mapSetStr[ &_DSD   ] = "DSD     ";
//	m_mapSetStr[ &_ND    ] = "ND      ";	//2023.06.13 - kwmoon(comment fatric)
	m_mapSetStr[ &_D1C   ] = "D1C     ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_BD    ] = "BD      ";
	m_mapSetStr[ &_DBMAR ] = "DBMAR   ";
	m_mapSetStr[ &_DIGI1 ] = "DIGIT1  ";
	m_mapSetStr[ &_DIGI2 ] = "DIGIT2  ";
	m_mapSetStr[ &_DIGI3 ] = "DIGIT3  ";
	m_mapSetStr[ &_D1D   ] = "D1D     ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH1A ] = "TPH1A   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH2A ] = "TPH2A   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_D1E   ] = "D1E     ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH1B ] = "TPH1B   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH2B ] = "TPH2B   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_D1F   ] = "D1F     ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH1C ] = "TPH1C   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH2C ] = "TPH2C   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_D1G   ] = "D1G     ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH1D ] = "TPH1D   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_TPH2D ] = "TPH2D   ";	//2023.06.13 - kwmoon(add fatric)
	m_mapSetStr[ &_D1H   ] = "D1H     ";	//2023.06.13 - kwmoon(add fatric)
}


void CPg::InitPins()
{
	m_vecPins.clear();
	m_vecPins.push_back( &_PD0 );	m_vecPins.push_back( &_PD1 );	m_vecPins.push_back( &_PD2 );	m_vecPins.push_back( &_PD3 );
	m_vecPins.push_back( &_PD4 );	m_vecPins.push_back( &_PD5 );	m_vecPins.push_back( &_PD6 );	m_vecPins.push_back( &_PD7 );
	m_vecPins.push_back( &_PD8 );	m_vecPins.push_back( &_PD9 );	m_vecPins.push_back( &_PD10);	m_vecPins.push_back( &_PD11);
	m_vecPins.push_back( &_PD12);	m_vecPins.push_back( &_PD13);	m_vecPins.push_back( &_PD14);	m_vecPins.push_back( &_PD15);
	m_vecPins.push_back( &_PD16);	m_vecPins.push_back( &_PD17);	m_vecPins.push_back( &_PD18);	m_vecPins.push_back( &_PD19);
	m_vecPins.push_back( &_PD20);	m_vecPins.push_back( &_PD21);	m_vecPins.push_back( &_PD22);	m_vecPins.push_back( &_PD23);
	m_vecPins.push_back( &_PD24);	m_vecPins.push_back( &_PD25);	m_vecPins.push_back( &_PD26);	m_vecPins.push_back( &_PD27);
	m_vecPins.push_back( &_PD28);	m_vecPins.push_back( &_PD29);	m_vecPins.push_back( &_PD30);	m_vecPins.push_back( &_PD31);

	m_vecPins.push_back( &_P0 );	m_vecPins.push_back( &_P1 );	m_vecPins.push_back( &_P2 );	m_vecPins.push_back( &_P3 );
	m_vecPins.push_back( &_P4 );	m_vecPins.push_back( &_P5 );	m_vecPins.push_back( &_P6 );	m_vecPins.push_back( &_P7 );
	m_vecPins.push_back( &_P8 );	m_vecPins.push_back( &_P9 );	m_vecPins.push_back( &_P10);	m_vecPins.push_back( &_P11);
	m_vecPins.push_back( &_P12);	m_vecPins.push_back( &_P13);	m_vecPins.push_back( &_P14);	m_vecPins.push_back( &_P15);
	m_vecPins.push_back( &_P16);	m_vecPins.push_back( &_P17);	m_vecPins.push_back( &_P18);	m_vecPins.push_back( &_P19);
	m_vecPins.push_back( &_P20);	m_vecPins.push_back( &_P21);	m_vecPins.push_back( &_P22);	m_vecPins.push_back( &_P23);
	m_vecPins.push_back( &_P24);	m_vecPins.push_back( &_P25);	m_vecPins.push_back( &_P26);	m_vecPins.push_back( &_P27);
	m_vecPins.push_back( &_P28);	m_vecPins.push_back( &_P29);	m_vecPins.push_back( &_P30);	m_vecPins.push_back( &_P31);
	m_vecPins.push_back( &_P32);	m_vecPins.push_back( &_P33);	m_vecPins.push_back( &_P34);	m_vecPins.push_back( &_P35);
	m_vecPins.push_back( &_P36);	m_vecPins.push_back( &_P37);	m_vecPins.push_back( &_P38);	m_vecPins.push_back( &_P39);
	m_vecPins.push_back( &_P40);	m_vecPins.push_back( &_P41);	m_vecPins.push_back( &_P42);	m_vecPins.push_back( &_P43);
	m_vecPins.push_back( &_P44);	m_vecPins.push_back( &_P45);	m_vecPins.push_back( &_P46);	m_vecPins.push_back( &_P47);
	m_vecPins.push_back( &_UCLK0 );	m_vecPins.push_back( &_UCLK1 );	m_vecPins.push_back( &_UCLK2 );	m_vecPins.push_back( &_UCLK3 );
	m_vecPins.push_back( &_UCLK4 );	m_vecPins.push_back( &_UCLK5 );	m_vecPins.push_back( &_UCLK6 );	m_vecPins.push_back( &_UCLK7 );
	m_vecPins.push_back( &_UCLK8 );	m_vecPins.push_back( &_UCLK9 );	m_vecPins.push_back( &_UCLK10);	m_vecPins.push_back( &_UCLK11);
	m_vecPins.push_back( &_UCLK12);	m_vecPins.push_back( &_UCLK13);	m_vecPins.push_back( &_UCLK14);	m_vecPins.push_back( &_UCLK15);
	m_vecPins.push_back( &_UCLK16);	m_vecPins.push_back( &_UCLK17);	m_vecPins.push_back( &_UCLK18);	m_vecPins.push_back( &_UCLK19);
	m_vecPins.push_back( &_UCLK20);	m_vecPins.push_back( &_UCLK21);	m_vecPins.push_back( &_UCLK22);	m_vecPins.push_back( &_UCLK23);
	m_vecPins.push_back( &_UCLK24);	m_vecPins.push_back( &_UCLK25);	m_vecPins.push_back( &_UCLK26);	m_vecPins.push_back( &_UCLK27);
	m_vecPins.push_back( &_UCLK28);	m_vecPins.push_back( &_UCLK29);	m_vecPins.push_back( &_UCLK30);	m_vecPins.push_back( &_UCLK31);
	m_vecPins.push_back( &_UCLK32);	m_vecPins.push_back( &_UCLK33);	m_vecPins.push_back( &_UCLK34);	m_vecPins.push_back( &_UCLK35);
	m_vecPins.push_back( &_UCLK36);	m_vecPins.push_back( &_UCLK37);	m_vecPins.push_back( &_UCLK38);	m_vecPins.push_back( &_UCLK39);
	m_vecPins.push_back( &_UCLK40);	m_vecPins.push_back( &_UCLK41);	m_vecPins.push_back( &_UCLK42);	m_vecPins.push_back( &_UCLK43);
	m_vecPins.push_back( &_UCLK44);	m_vecPins.push_back( &_UCLK45);	m_vecPins.push_back( &_UCLK46);	m_vecPins.push_back( &_UCLK47);
	m_vecPins.push_back( &_UCLK48);	m_vecPins.push_back( &_UCLK49);	m_vecPins.push_back( &_UCLK50);	m_vecPins.push_back( &_UCLK51);
	m_vecPins.push_back( &_UCLK52);	m_vecPins.push_back( &_UCLK53);	m_vecPins.push_back( &_UCLK54);	m_vecPins.push_back( &_UCLK55);
	m_vecPins.push_back( &_UCLK56);	m_vecPins.push_back( &_UCLK57);	m_vecPins.push_back( &_UCLK58);	m_vecPins.push_back( &_UCLK59);
	m_vecPins.push_back( &_UCLK60);	m_vecPins.push_back( &_UCLK61);	m_vecPins.push_back( &_UCLK62);	m_vecPins.push_back( &_UCLK63);
	m_vecPins.push_back( &_UCLK64);	m_vecPins.push_back( &_UCLK65);	m_vecPins.push_back( &_UCLK66);	m_vecPins.push_back( &_UCLK67);
	m_vecPins.push_back( &_UCLK68);	m_vecPins.push_back( &_UCLK69);	m_vecPins.push_back( &_UCLK70);	m_vecPins.push_back( &_UCLK71);
	m_vecPins.push_back( &_UCLK72);	m_vecPins.push_back( &_UCLK73);	m_vecPins.push_back( &_UCLK74);	m_vecPins.push_back( &_UCLK75);
	m_vecPins.push_back( &_UCLK76);	m_vecPins.push_back( &_UCLK77);	m_vecPins.push_back( &_UCLK78);	m_vecPins.push_back( &_UCLK79);
	m_vecPins.push_back( &_UCLK80);	m_vecPins.push_back( &_UCLK81);	m_vecPins.push_back( &_UCLK82);	m_vecPins.push_back( &_UCLK83);
	m_vecPins.push_back( &_UCLK84);	m_vecPins.push_back( &_UCLK85);	m_vecPins.push_back( &_UCLK86);	m_vecPins.push_back( &_UCLK87);
	m_vecPins.push_back( &_DREL  ); m_vecPins.push_back( &_DRET  ); m_vecPins.push_back( &_DRDRE );
	m_vecPins.push_back( &_CPE0  ); m_vecPins.push_back( &_CPE1  ); m_vecPins.push_back( &_CPE2  ); m_vecPins.push_back( &_CPE3  );
	m_vecPins.push_back( &_FMA0  );	m_vecPins.push_back( &_FMA1  );	m_vecPins.push_back( &_FMA2  );	m_vecPins.push_back( &_FMA3  );
	m_vecPins.push_back( &_FMA4  );	m_vecPins.push_back( &_FMA5  );	m_vecPins.push_back( &_FMA6  );	m_vecPins.push_back( &_FMA7  );
	m_vecPins.push_back( &_FMA8  );	m_vecPins.push_back( &_FMA9  );	m_vecPins.push_back( &_FMA10 );	m_vecPins.push_back( &_FMA11 );
	m_vecPins.push_back( &_FMA12 );	m_vecPins.push_back( &_FMA13 );	m_vecPins.push_back( &_FMA14 );	m_vecPins.push_back( &_FMA15 );
	m_vecPins.push_back( &_FMA16 );	m_vecPins.push_back( &_FMA17 );	m_vecPins.push_back( &_FMA18 );	m_vecPins.push_back( &_FMA19 );
	m_vecPins.push_back( &_FMA20 );	m_vecPins.push_back( &_FMA21 );	m_vecPins.push_back( &_FMA22 );	m_vecPins.push_back( &_FMA23 );
	//m_vecPins.push_back( &_FMA24 );	m_vecPins.push_back( &_FMA25 );
	m_vecPins.push_back( &_MUT0  );	m_vecPins.push_back( &_MUT1  );	m_vecPins.push_back( &_MUT2  );	m_vecPins.push_back( &_MUT3  );
	m_vecPins.push_back( &_MUT4  );	m_vecPins.push_back( &_MUT5  );	m_vecPins.push_back( &_MUT6  );	m_vecPins.push_back( &_MUT7  );
	m_vecPins.push_back( &_MUT8  );	m_vecPins.push_back( &_MUT9  );	m_vecPins.push_back( &_MUT10 );	m_vecPins.push_back( &_MUT11 );
	m_vecPins.push_back( &_MUT12 );	m_vecPins.push_back( &_MUT13 );	m_vecPins.push_back( &_MUT14 );	m_vecPins.push_back( &_MUT15 );

}

void CPg::InitModule()
{
	for( CPin* pin : m_vecPins )
	{
		if( pin->GetType() == PIN_TYPE::PIN_DR )
		{
			pin->SetModule( true  );
			*pin = FMT_NORM::_NRZB;
			pin->SetModule( false );
		}
		else if( pin->GetType() == PIN_TYPE::PIN_IO )
		{
			pin->SetModule( true  );
			if( GetIoDdrModeGlobal() == SDR )	//2022.11.23 - KWMOON
				*pin = FMT_NORM::NRZB;
			else
				*pin = FMT_NORM::DNRZ;
			pin->SetModule( false );
		}
		else if( pin->GetType() == PIN_TYPE::PIN_DRE )
		{
			pin->SetModule( true  );
			*pin = PDS::FIXH;	//PDS FIXH -> FMT FIXL
			pin->SetModule( false );
		}
		else
		{
			pin->SetModule( true  );
			*pin = PDS::FIXL;	//PDS FIXL -> FMT FIXL
			pin->SetModule( false );
		}
	}
}

void CPg::InitPinoutEnable()
{
	SetPinOutDrv ( true, 0, 31 );
	SetPinOutIoA ( true, 0, 47 );
	SetPinOutIoB ( true, 0, 47 );
	SetPinOutUclk( true, 0, 87 );
}

void CPg::InitCyp()
{
	m_uDreSel  = 0x0;
	m_uCpeHSel = 0x0;
	m_uCpeLSel = 0x0;
}

//2023.04.21 - kwmoon
void CPg::InitPinOutVal()
{
	memset( m_uPinoutDrv , 0, sizeof(m_uPinoutDrv ) );
	memset( m_uPinoutIoA , 0, sizeof(m_uPinoutIoA ) );
	memset( m_uPinoutIoB , 0, sizeof(m_uPinoutIoB ) );
	memset( m_uPinoutUclk, 0, sizeof(m_uPinoutUclk) );
	memset( m_uPinoutMio , 0, sizeof(m_uPinoutMio ) );
}

void CPg::SetModule(bool enable)
{
	m_bModule = enable;
	for( CPin* pin : m_vecPins )
	{
		pin->SetModule( enable );
	}
}

void CPg::SetDrDelay( uint cycle )
{
	g_pcie.write_user( BITCHART::ALPG::PATTERN_DELAY_REG::_DRDLY, cycle );
	g_sys.SysDelay( 1 US );
}

void CPg::SetIoDelay( uint index, uint cycle )
{
	g_pcie.write_user( BITCHART::ALPG::PATTERN_DELAY_REG::_IODLY + (index*4), cycle );
	g_sys.SysDelay( 1 US );
}

void CPg::SetDreDelay( uint index, uint cycle )
{
	g_pcie.write_user( BITCHART::ALPG::PATTERN_DELAY_REG::_DREDLY + (index*4), cycle );
	g_sys.SysDelay( 1 US );
}

void CPg::SetCpeDelay( uint index, uint cycle )
{
	g_pcie.write_user( BITCHART::ALPG::PATTERN_DELAY_REG::_CPEDLY + (index*4), cycle );
	g_sys.SysDelay( 1 US );
}

void CPg::SetUclkDelay( uint cycle )
{
	g_pcie.write_user( BITCHART::ALPG::PATTERN_DELAY_REG::_UCLKDLY, cycle );
	g_sys.SysDelay( 1 US );
}

void CPg::SetPeReadEnableSel(int sel)
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_PE_READ_ENABLE_SEL, sel );
	g_sys.SysDelay( 1 US );
}

void CPg::SetEdqSel(uint sel)
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_EDQ_SEL, sel );
	g_sys.SysDelay( 1 US );
}

void CPg::SetDrDreSel(int sel)
{
//	sel:0 - DR DRE(PDS)
//	sel:1 - DR DRE FIXH
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DR_DRE_SEL, sel );
//	g_sys.SysDelay( 1 US );
}

//2023.08.16 - KWMOON
void CPg::SetMatchCpeDelay(uint cycle)
{
	if( cycle > 7 ) cycle = 7;
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_MATCH_CPE_DELAY, cycle );
	if( 1 )
	{
		fprintf( stdout, "MATCH CPEDLEAY (0x%08X,0x%08X)\n", BITCHART::GLOBAL_MODE_REG::_MATCH_CPE_DELAY, cycle );
	}
//	g_sys.SysDelay( 1 US );
}

//FMT
bool CPg::WriteModule()
{
	//fprintf( stdout, "WriteModule Start.....\n");
	for( CPin* pin : m_vecPins )
	{
		if( pin->GetType() == PIN_TYPE::PIN_DR )
		{
			int index  = pin->GetIndex()/4;	//0 ~ 7
			int offset = pin->GetIndex()%4;	//0 ~ 3
			int mask   = ~((0xff)<<(8*offset));
			m_ModulePD[index] &= mask;
			m_ModulePD[index] |= ( pin->GetModule()<<(8*offset) );
		}
		else if( pin->GetType() == PIN_TYPE::PIN_IO )
		{
			int index  = (pin->GetIndex() - RP0 )/4;	//0 ~ 11
			int offset = (pin->GetIndex() - RP0 )%4;	//0 ~ 3
			int mask   = ~((0xff)<<(8*offset));
			m_ModuleP[index] &= mask;
			m_ModuleP[index] |= ( pin->GetModule()<<(8*offset) );
		}
		else if( pin->GetType() == PIN_TYPE::PIN_UCLK )
		{
			int index  = (pin->GetIndex() - RUCLK0 )/4;	//0 ~ 21
			int offset = (pin->GetIndex() - RUCLK0 )%4;	//0 ~ 3
			int mask   = ~((0xff)<<(8*offset));
			m_ModuleUCLK[index] &= mask;
			m_ModuleUCLK[index] |= ( pin->GetModule()<<(8*offset) );
		}
		else if( pin->GetType() == PIN_TYPE::PIN_DRE )
		{
			int index  = (pin->GetIndex() - RDREL )/3;	//0 ~ 1
			int offset = (pin->GetIndex() - RDREL )%3;	//0 ~ 2
			int mask   = ~((0xf)<<(4*offset));
			m_ModuleDRE[index] &= mask;
			m_ModuleDRE[index] |= ( pin->GetModule()<<(4*offset) );
		}

	}
	//PD
	for( int pda=0;pda<8;pda++ )
	{
		g_pcie.write_user( BITCHART::ALPG::_FPDCH3_0 + (pda*4)  , m_ModulePD[pda] );
		if( 0 ) fprintf( stdout, "FMT PD IDX%02d WRITE(0x%08X,0x%08X)\n", pda, BITCHART::ALPG::_FPDCH3_0 + (pda*4)  , m_ModulePD[pda] );
	}
	//P
	for( int pa=0;pa<12;pa++ )
	{
		g_pcie.write_user( BITCHART::ALPG::_FPCH3_0 + (pa*4)  , m_ModuleP[pa] );
		if( 0 ) fprintf( stdout, "FMT P IDX%02d WRITE(0x%08X,0x%08X)\n", pa, BITCHART::ALPG::_FPCH3_0 + (pa*4)  , m_ModuleP[pa] );
	}
	//UCLK
	for( int puclk=0;puclk<22;puclk++ )
	{
		g_pcie.write_user( BITCHART::ALPG::_FUCLKCH3_0 + (puclk*4)  , m_ModuleUCLK[puclk] );
		if( 0 ) fprintf( stdout, "FMT UCLK IDX%02d WRITE(0x%08X,0x%08X)\n", puclk, BITCHART::ALPG::_FUCLKCH3_0 + (puclk*4)  , m_ModuleUCLK[puclk] );
	}
	//DRE
	for( int pu=0;pu<1;pu++ )
	{
		g_pcie.write_user( BITCHART::ALPG::_FDRE + (pu*4)  , m_ModuleDRE[pu] );
		if( 0 ) fprintf( stdout, "FMT DRE  IDX%02d WRITE(0x%08X,0x%08X)\n", pu, BITCHART::ALPG::_FDRE + (pu*4)  , m_ModuleDRE[pu] );
	}
	//fprintf( stdout, "WriteModule End.....\n");

	return true;
}

//2022.11.23 - kwmoon
void CPg::InitModuleIo()
{
	for( CPin* pin : m_vecPins )
	{
		if( pin->GetType() == PIN_TYPE::PIN_IO )
		{
			pin->SetModule( true  );
			if( GetIoDdrModeGlobal() == SDR )	//2022.11.23 - KWMOON
				*pin = FMT_NORM::NRZB;
			else
				*pin = FMT_NORM::DNRZ;
			pin->SetModule( false );

			int index  = (pin->GetIndex() - RP0 )/4;	//0 ~ 11
			int offset = (pin->GetIndex() - RP0 )%4;	//0 ~ 3
			int mask   = ~((0xff)<<(8*offset));
			m_ModuleP[index] &= mask;
			m_ModuleP[index] |= ( pin->GetModule()<<(8*offset) );
		}
	}
	//P
	for( int pa=0;pa<12;pa++ )
	{
		g_pcie.write_user( BITCHART::ALPG::_FPCH3_0 + (pa*4)  , m_ModuleP[pa] );
		if( 0 ) fprintf( stdout, "FMT P IDX%02d WRITE(0x%08X,0x%08X)\n", pa, BITCHART::ALPG::_FPCH3_0 + (pa*4)  , m_ModuleP[pa] );
	}
}

bool CPg::WriteCyp()
{
    //InitCyp();
	for( CPin* pin : m_vecPins )
	{
		if( pin->GetType() == PIN_TYPE::PIN_DR )
		{
			int iPDAddr = BITCHART::ALPG::_PDS_PD_CH + (pin->GetIndex()*4);
			for( int cyp=0;cyp<MAX_CYP;cyp++ )
			{
				int iWriteAddr = iPDAddr | (cyp<<12);
				int iCyp = (pin->GetCyp(cyp)&0xFF) | ((pin->GetCyp(cyp)&0xFF)<<8);
				g_pcie.write_user( iWriteAddr,  iCyp );
				if( 0 ) fprintf( stdout, "PD(%02d)   CYP%02d Write(0x%08X,0x%08X)\n", pin->GetIndex(), cyp, iWriteAddr, iCyp );
			}
		}
		else if( pin->GetType() == PIN_TYPE::PIN_IO )
		{
			int iPAddr = BITCHART::ALPG::_PDS_P_CH + ((pin->GetIndex()-PINS::RP0)*4);
			for( int cyp=0;cyp<MAX_CYP;cyp++ )
			{
				int iWriteAddr = iPAddr | (cyp<<12);
				int iCyp = (pin->GetCyp(cyp)&0xFF) | ((pin->GetCyp(cyp)&0xFF)<<8);
				g_pcie.write_user( iWriteAddr,  iCyp );
				if( 0 ) fprintf( stdout, "P(%02d)    CYP%02d Write(0x%08X,0x%08X)\n", pin->GetIndex()-PINS::RP0, cyp, iWriteAddr, iCyp );
			}
		}
		else if( pin->GetType() == PIN_TYPE::PIN_UCLK )
		{
			int iUclkAddr = BITCHART::ALPG::_PDS_UCLK_CH + ((pin->GetIndex()-PINS::RUCLK0)*4);
			for( int cyp=0;cyp<MAX_CYP;cyp++ )
			{
				int iWriteAddr = iUclkAddr | (cyp<<12);
				int iCyp = (pin->GetCyp(cyp)&0xFF) | ((pin->GetCyp(cyp)&0xFF)<<8);
				g_pcie.write_user( iWriteAddr,  iCyp );
				if( 0 ) fprintf( stdout, "UCLK(%02d) CYP%02d Write(0x%08X,0x%08X)\n", pin->GetIndex()-PINS::RUCLK0, cyp, iWriteAddr, iCyp );
			}
		}
		else if( pin->GetType() == PIN_TYPE::PIN_DRE )
		{
			int iDreAddr = BITCHART::ALPG::_DRE1 + ((pin->GetIndex()-PINS::RDREL)*4);
			int iDreIdx  = pin->GetIndex()-PINS::RDREL;
			for( int cyp=0;cyp<1;cyp++ )
			{
				int iWriteAddr = iDreAddr | (cyp<<12);
				int iCyp = (pin->GetCyp(cyp)&0xFF) | ((pin->GetCyp(cyp)&0xFF)<<8);
				g_pcie.write_user( iWriteAddr,  iCyp );
				if( 0 ) fprintf( stdout, "DRE(%02d) CYP%02d Write(0x%08X,0x%08X)\n", pin->GetIndex()-PINS::RDREL, cyp, iWriteAddr, iCyp );
			}
			switch ( iDreIdx )
			{
				case 0:	//DREL
					m_uDreSel |= (0x1<<0);
					break;
				case 1:	//DRET
					m_uDreSel |= (0x2<<4);
					break;
				case 2:	//RFU(DRDRE - not use)
					//DRDRE FMT - FIXH
					//m_uDreSel |= (0x3<<8);
					break;
				default:
					break;
			}
		}
		else if( pin->GetType() == PIN_TYPE::PIN_CPE )
		{
			int iCpeAddr = BITCHART::ALPG::_CPE1 + ((pin->GetIndex()-PINS::RCPE0)*4);
			int iCpeIdx  = pin->GetIndex()-PINS::RCPE0;
			for( int cyp=0;cyp<1;cyp++ )
			{
				int iWriteAddr = iCpeAddr | (cyp<<12);
				int iCyp = (pin->GetCyp(cyp)&0xFF) | ((pin->GetCyp(cyp)&0xFF)<<8);
				//2022.11.23 - kwmoon(CPE1 SDR/SDDR)
				if( pin->GetIndex() == PINS::RCPE1 )
				{
					if( GetIoDdrModeGlobal() == SDR )
						g_pcie.write_user( iWriteAddr, PDS::FIXL );
					else
						g_pcie.write_user( iWriteAddr,  iCyp );
				}
				else
				{
					g_pcie.write_user( iWriteAddr,  iCyp );
				}
				if( 0 ) fprintf( stdout, "CPE(%02d) CYP%02d Write(0x%08X,0x%08X)\n", pin->GetIndex()-PINS::RCPE0, cyp, iWriteAddr, iCyp );
			}
			switch ( iCpeIdx )
			{
				case 0:	//CPE0(H[0])
					m_uCpeHSel |= (0x1<<0);
					break;
				case 1:	//CPE1(L[0])
					m_uCpeLSel |= (0x2<<0);
					break;
				case 2:	//CPE2(MATCH)
					m_uCpeHSel |= (0x3<<4);
					break;
				default:
					break;
			}
		}
		else if( pin->GetType() == PIN_TYPE::PIN_FMA )
		{
			int iUclkAddr = BITCHART::ALPG::_FMA0 + ((pin->GetIndex()-PINS::RFMA0)*4);
			for( int cyp=0;cyp<1;cyp++ )
			{
				int iWriteAddr = iUclkAddr;
				int iCyp = (pin->GetCyp(cyp)&0xFF);
				g_pcie.write_user( iWriteAddr,  iCyp );
				if( 0 ) fprintf( stdout, "FMA(%02d) CYP%02d Write(0x%08X,0x%08X)\n", pin->GetIndex()-PINS::RFMA0, cyp, iWriteAddr, iCyp );
			}
		}
		//2022.10.31 - KWMOON
		else if( pin->GetType() == PIN_TYPE::PIN_MUT )
		{
			int iMutAddr = BITCHART::ALPG::_MUT0 + ((pin->GetIndex()-PINS::RMUT0)*4);
			for( int cyp=0;cyp<1;cyp++ )
			{
				int iWriteAddr = iMutAddr;
				int iCyp = (pin->GetCyp(cyp)&0xFF);
				g_pcie.write_user( iWriteAddr,  iCyp );
				if( 0 ) fprintf( stdout, "MUT(%02d) CYP%02d Write(0x%08X,0x%08X)\n", pin->GetIndex()-PINS::RMUT0, cyp, iWriteAddr, iCyp );
			}
		}
	}
	g_pcie.write_user( BITCHART::ALPG::_DRE_SEL , m_uDreSel  );
	g_pcie.write_user( BITCHART::ALPG::_HCPE_SEL, m_uCpeHSel );
	g_pcie.write_user( BITCHART::ALPG::_LCPE_SEL, m_uCpeLSel );
	if( 0 ) fprintf( stdout, "DRE  SEL Write(0x%08X,0x%08X)\n", BITCHART::ALPG::_DRE_SEL , m_uDreSel  );
	if( 0 ) fprintf( stdout, "HCPE SEL Write(0x%08X,0x%08X)\n", BITCHART::ALPG::_HCPE_SEL, m_uCpeHSel );
	if( 0 ) fprintf( stdout, "LCPE SEL Write(0x%08X,0x%08X)\n", BITCHART::ALPG::_LCPE_SEL, m_uCpeLSel );
	return true;
}

bool CPg::ClearGlobalRegister()
{
	m_vecGlobalReg.clear();
	//IDX CLEAR
	g_pcie.write_user( BITCHART::ALPG::_IDX1 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX2 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX3 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX4 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX5 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX6 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX7 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX8 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX9 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX10, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX11, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX12, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX13, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX14, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX15, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_IDX16, 0x0 );

	//D1B~D2D, LMAX~ZMAX
	g_pcie.write_user( BITCHART::ALPG::_D1B , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D1C , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D1D , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D1E , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D1F , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D1G , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D1H , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D2B , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D2C , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_D2D , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_LMAX, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_HMAX, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_XMAX, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_YMAX, 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ZMAX, 0x0 );

	//ND REG CLEAR
	g_pcie.write_user( BITCHART::ALPG::_ND2  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND3  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND4  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND5  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND6  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND7  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND8  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND9  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND10 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND11 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND12 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND13 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND14 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND15 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND16 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND17 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND18 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND19 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND20 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND21 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND22 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND23 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND24 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND25 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND26 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND27 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND28 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND29 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND30 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND31 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_ND32 , 0x0 );

	//BD REG CLEAR
	g_pcie.write_user( BITCHART::ALPG::_BD2  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD3  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD4  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD5  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD6  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD7  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD8  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD9  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD10 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD11 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD12 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD13 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD14 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD15 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD16 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD17 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD18 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD19 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD20 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD21 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD22 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD23 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD24 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD25 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD26 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD27 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD28 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD29 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD30 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD31 , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_BD32 , 0x0 );

	//L1~4(A~H)
	g_pcie.write_user( BITCHART::ALPG::_L1A  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L1B  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L1C  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L1D  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L1E  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L1F  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L1G  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L1H  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2A  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2B  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2C  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2D  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2E  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2F  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2G  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L2G  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3A  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3B  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3C  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3D  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3E  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3F  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3G  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L3H  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4A  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4B  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4C  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4D  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4E  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4F  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4G  , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_L4H  , 0x0 );

	//S1A, S_TEMP
	g_pcie.write_user( BITCHART::ALPG::_S1A    , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_S_TEMP , 0x0 );

	//V1A, V_TEMP
	g_pcie.write_user( BITCHART::ALPG::_V1A    , 0x0 );
	g_pcie.write_user( BITCHART::ALPG::_V_TEMP , 0x0 );

	return true;
}

bool CPg::SetGlobalRegister( int reg, ull64 val )
{
	REGI regi;
	regi.addr = reg;
	regi.i2   = static_cast<int>(val);
	m_vecGlobalReg.push_back( regi );
	return true;
}

void CPg::PrintGlobalReg()
{

}

bool CPg::ClearGlobalSetDSet()
{
	for( int i=0;i<16;i++ ) m_SetReg[i].clear();
	return true;
}

void CPg::PrintGlobalSetDSet()
{

}

void CPg::InitPg()
{
	PgStop();
	ChipClear( EN_LOGIC_TYPE::LOGIC_IOA  );		//2022.12.23 - kwmoon(comment) -> 2023.10.13 - kwmoon(remove comment)
	ChipClear( EN_LOGIC_TYPE::LOGIC_IOB  );		//2022.12.23 - kwmoon(comment) -> 2023.10.13 - kwmoon(remove comment)
	ChipClear( EN_LOGIC_TYPE::LOGIC_UCLK );		//2022.12.23 - kwmoon(comment) -> 2023.10.13 - kwmoon(remove comment)
	PeClear();										//2022.12.23 - kwmoon
	CpeMask( EN_LOGIC_TYPE::LOGIC_IOA, IO_CPE_ENABLE | DR_CPE_DISABLE );
	CpeMask( EN_LOGIC_TYPE::LOGIC_IOB, IO_CPE_ENABLE | DR_CPE_DISABLE );
	TpgIdelayCtrlReset();	//2022.12.01 - KWMOON
	SetCpeDelay( 0, CPE_DELAY_DEFAULT );		//2 Cycle
	SetPeReadEnableSel( 0x0 );				//2022.12.03 - KWMOON
	SetEdqSel( 0x0 );						//0:EDQ, 1:FIXL, 2:FIXH
	AlpgClear();
	DiagClear();
	DbmAddrClear();
	DrvClear();
	CmpClear();
	DfmClear();
	SetMatchCpeDelay( 3 );	//2023.08.16 - KWOON

	//2023.04.21 - fatric
	InitPinOutVal   ();
	SetPinOutDrvAct ();
	SetPinOutUclkAct();
	SetPinOutIoAAct ();
	SetPinOutIoBAct ();
	SetDrDreSel(1);		//DR DRE FIXH - DEFAULT 1, DIAG(TPG-PE) : 0
}

void CPg::AlpgClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_ALPG_CLEAR, 0x1 );
//kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::PllReset()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_FPGA_PLL_RESET, 0x1 );
//kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::DiagClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DIAG_REG_CLEAR, 0x1 );
//kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::DbmAddrClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DBM_ADDR_CLEAR, 0x1 );
//kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::DrvClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DRV_CLAER, 0x1 );
//kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::CmpClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_CMP_CLAER, 0x1 );
// kyb	g_sys.SysDelay( 2 MS );
	g_sys.SysDelay( 1 US );
}

void CPg::DfmClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_DFM_CLEAR, 0x1 );
// kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

///////////////////////////////////////////////////////////////////////////////////////
/// \brief CPg::CpuReset
/// \details Only IOA has cpu reset
void CPg::CpuReset()
{
	g_pcie.write_user( BITCHART::DRV_IOA::_CPU_RESET , 0x1 );
// kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::PeClear()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_PE_CLEAR, 0x1 );
// kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::TpgIdelayCtrlReset()	//2022.12.01
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_TPG_IDELAY_CTRL_RESET , 0x1 );
// kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::ChipClear( EN_LOGIC_TYPE t )
{
	//return;			//2022.12.23 - kwmoon( replace PeClear) --> 2023.10.13 - kwmoon( remove comment )
	switch( t )
	{
		case EN_LOGIC_TYPE::LOGIC_IOA:
			g_pcie.write_user( BITCHART::DRV_IOA::_CHIP_CLEAR , 0x1 );
			break;
		case EN_LOGIC_TYPE::LOGIC_IOB:
			g_pcie.write_user( BITCHART::DRV_IOB::_CHIP_CLEAR , 0x1 );
			break;
		case EN_LOGIC_TYPE::LOGIC_UCLK:
			g_pcie.write_user( BITCHART::DRV_UCLK::_CHIP_CLEAR, 0x1 );
			break;
		default:
			break;
	}
// kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

void CPg::DrvCalibration( EN_LOGIC_TYPE t )
{
	switch( t )
	{
		case EN_LOGIC_TYPE::LOGIC_IOA:
			g_pcie.write_user( BITCHART::DRV_IOA::_DRV_CALIBRATION , 0x1 );
			break;
		case EN_LOGIC_TYPE::LOGIC_IOB:
			g_pcie.write_user( BITCHART::DRV_IOB::_DRV_CALIBRATION , 0x1 );
			break;
		case EN_LOGIC_TYPE::LOGIC_UCLK:
			g_pcie.write_user( BITCHART::DRV_UCLK::_DRV_CALIBRATION, 0x1 );
			break;
		default:
			break;
	}
// kyb	g_sys.SysDelay( 1 MS );
	g_sys.SysDelay( 1 US );
}

void CPg::CpeMask( EN_LOGIC_TYPE t, int m )
{
	switch( t )
	{
		case EN_LOGIC_TYPE::LOGIC_IOA:
			g_pcie.write_user( BITCHART::DRV_IOA::_CPE_MASK , m );
			break;
		case EN_LOGIC_TYPE::LOGIC_IOB:
			g_pcie.write_user( BITCHART::DRV_IOB::_CPE_MASK , m );
			break;
		case EN_LOGIC_TYPE::LOGIC_UCLK:
			g_pcie.write_user( BITCHART::DRV_UCLK::_CPE_MASK, m );
			break;
		default:
			break;
	}
// kyb	g_sys.SysDelay( 10 US );
	g_sys.SysDelay( 1 US );
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPg::ClearPattern
/// \details pgc lib reset
void CPg::ClearPattern()
{
	m_pPgc->Reset();
}

void CPg::SendDefineFlag()
{
	if( 0 ) fprintf( stdout, "DEFINE FLAG : 0x%08X\n", g_sys.GetDefineFlag() );
	m_pPgc->SetCompOpt( g_sys.GetDefineFlag() );
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPg::SetDefine
/// \param nPgmLine
/// \param define_str
/// \details pgc lib set define compile, SET_DEFINE
void CPg::SetDefine(const int nPgmLine, const char* define_str)
{
	//fprintf( stdout, "==>%-20s Line:%d\n", __FUNCTION__, nPgmLine );
	//fprintf( stdout, "\n[%s]\n", define_str );

	if( !Tokenize( nPgmLine+1, define_str ) )
	{
		printf("Tokenize error!!\n");
		return;
	}

	atstring func_name = "DEFINE";
	std::vector<pat_line_data> vecDefine;
	atstring sdef_text  = "";

	tok_stuct tok = GetToken();
	while( 1 )
	{
			 if( tok.t == TOK_PAT::TOKEN_EOF  ) break;
		else if( tok.t == TOK_PAT::TOKEN_SDEF )
		{
			sdef_text += tok.s;
			tok = GetToken();
			while( 1 )
			{
				if( (tok.t == TOK_PAT::TOKEN_EOF)  || (tok.t == TOK_PAT::TOKEN_SDEF) )
				{
					pat_line_data linedata;
					linedata.line = tok.l;
					linedata.data = sdef_text;
					vecDefine.push_back( linedata );
					//printf("==>%d:%s\n", linedata.line, linedata.data.c_str() );
					sdef_text = "";
					break;
				}
				else
				{
					sdef_text += tok.s;
				}
				tok = GetToken();
			}
		}
		else
		{
			tok = GetToken();
		}
	}
	//send to pgc library to vector
	if( 0 )
	{
		for( pat_line_data d: vecDefine )
		{
			printf("==>%s:DEFINE(%d):%s\n", func_name.c_str(), d.line, d.data.c_str() );
		}
	}
	m_pPgc->SetDefine( func_name, vecDefine );
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPg::SetSub
/// \param nPgmLine
/// \param sub_name
/// \param sub_str
/// \details pgc lib sub compile, SET_SUB
void CPg::SetSub( const int nPgmLine, const char* sub_name, const char* sub_str)
{
	//fprintf( stdout, "==> %-20s %-20s Line:%d\n", __FUNCTION__, sub_name, nPgmLine );
	if( !Tokenize( nPgmLine+1, sub_str ) )
	{
		printf("Tokenize error!!\n");
		return;
	}

	std::vector<pat_line_data> vecSub;
	atstring pat_text  = "";

	tok_stuct tok = GetToken();
	while( 1 )
	{
			 if( tok.t == TOK_PAT::TOKEN_EOF  ) break;
		else if( tok.t == TOK_PAT::TOKEN_SEMICOLON )
		{
			pat_text += tok.s;

			pat_line_data linedata;
			linedata.line = tok.l;
			linedata.data = pat_text;
			vecSub.push_back( linedata );
			pat_text = "";
		}
		else
		{
			pat_text += tok.s;
		}
		tok = GetToken();
	}
	//send to pgc library subname, vector
	if( 0 ) printf("Before SetSubCompile\n");
	m_pPgc->SetSubCompile( sub_name, vecSub );

	//for debug
	if( 0 )
	{
		for( pat_line_data d: vecSub )
		{
			printf("==>%-20s(%d):%s\n", sub_name, d.line, d.data.c_str() );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPg::Tokenize
/// \param line
/// \param data
/// \details define / sub tokenize
bool CPg::Tokenize( const int line, const char* data )
{
	int sdef_line = line;
	const char* p = data;
	atstring stoken = "";

	//printf("Start Tokenize...[%s]\n", p);
	m_nCurrentTok = 0;
	m_vecToken.clear();
	while( *p )
	{
		char ch = *p;
		if( isspace(ch) )
		{
			stoken += *p++;
			while( isspace(*p) || (*p =='\t') ) p++;
			tok_stuct token;
			token.t = TOKEN_SPACE;
			token.s = stoken;
			token.l = sdef_line;
			m_vecToken.push_back( token );
			stoken = "";
		}
		else if( isalpha(ch) )
		{
			stoken += *p++;
			while ( isalnum(*p) || *p == '_' )
			{
				stoken += *p++;
			}
			tok_stuct token;
			if( stoken == "SDEF" )
			{
				token.t = TOKEN_SDEF;
				token.l = sdef_line++;
			}
			else
			{
				token.t = TOKEN_LETTER;
				token.l = sdef_line;
			}
			token.s = stoken;
			m_vecToken.push_back( token );
			stoken = "";
		}
		else //if( ispunct(ch) )
		{
			stoken += *p++;
			tok_stuct token;
			if( stoken == ";" )
			{
				token.t = TOKEN_SEMICOLON;
				token.l = sdef_line++;
			}
			else
			{
				token.t = TOKEN_PUNC;
				token.l = sdef_line;
			}
			token.s = stoken;
			m_vecToken.push_back( token );
			stoken = "";
		}
	}
	tok_stuct token;
	token.l = sdef_line;
	token.t = TOKEN_EOF;
	token.s = "";
	m_vecToken.push_back( token );
	//printf("\nToken Count : %d\n", m_vecToken.size() );
	return true;
}

tok_stuct& CPg::GetToken()
{
	if( m_nCurrentTok < m_vecToken.size() )
		return m_vecToken[m_nCurrentTok++];
	return m_vecToken[m_nCurrentTok];
}

void CPg::SetPinOutDrv( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 31  ) nEnd   = 31;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutDrv[FC00][nArray]  = (m_uPinoutDrv[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutDrv[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutDrv[FC01][nArray]  = (m_uPinoutDrv[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutDrv[FC01][nArray] |= (nEn<<nOffset);
	}
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b7_b0  , m_uPinoutDrv[FC00][0] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b7_b0  , m_uPinoutDrv[FC00][0] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b15_b8 , m_uPinoutDrv[FC00][1] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b15_b8 , m_uPinoutDrv[FC00][1] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b23_b16, m_uPinoutDrv[FC00][2] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b23_b16, m_uPinoutDrv[FC00][2] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b31_b24, m_uPinoutDrv[FC00][3] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b31_b24, m_uPinoutDrv[FC00][3] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b7_b0  , m_uPinoutDrv[FC01][0] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b7_b0  , m_uPinoutDrv[FC01][0] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b15_b8 , m_uPinoutDrv[FC01][1] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b15_b8 , m_uPinoutDrv[FC01][1] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b23_b16, m_uPinoutDrv[FC01][2] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b23_b16, m_uPinoutDrv[FC01][2] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b31_b24, m_uPinoutDrv[FC01][3] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b31_b24, m_uPinoutDrv[FC01][3] );
}

//2023.04.21 - fatric
void CPg::SetPinOutDrvVal( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 31  ) nEnd   = 31;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutDrv[FC00][nArray]  = (m_uPinoutDrv[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutDrv[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutDrv[FC01][nArray]  = (m_uPinoutDrv[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutDrv[FC01][nArray] |= (nEn<<nOffset);
	}
}

//2023.04.21 - fatric
void CPg::SetPinOutDrvAct()
{
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b7_b0  , m_uPinoutDrv[FC00][0] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b7_b0  , m_uPinoutDrv[FC00][0] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b15_b8 , m_uPinoutDrv[FC00][1] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b15_b8 , m_uPinoutDrv[FC00][1] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b23_b16, m_uPinoutDrv[FC00][2] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b23_b16, m_uPinoutDrv[FC00][2] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b31_b24, m_uPinoutDrv[FC00][3] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b31_b24, m_uPinoutDrv[FC00][3] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b7_b0  , m_uPinoutDrv[FC01][0] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b7_b0  , m_uPinoutDrv[FC01][0] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b15_b8 , m_uPinoutDrv[FC01][1] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b15_b8 , m_uPinoutDrv[FC01][1] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b23_b16, m_uPinoutDrv[FC01][2] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b23_b16, m_uPinoutDrv[FC01][2] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b31_b24, m_uPinoutDrv[FC01][3] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b31_b24, m_uPinoutDrv[FC01][3] );
}

void CPg::SetPinOutDrv( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 31  ) nEnd   = 31;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutDrv[fc][nArray]  = (m_uPinoutDrv[fc][nArray] & ~(1<<nOffset));
		m_uPinoutDrv[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b7_b0  , m_uPinoutDrv[FC00][0] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b7_b0  , m_uPinoutDrv[FC00][0] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b15_b8 , m_uPinoutDrv[FC00][1] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b15_b8 , m_uPinoutDrv[FC00][1] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b23_b16, m_uPinoutDrv[FC00][2] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b23_b16, m_uPinoutDrv[FC00][2] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_EN_b31_b24, m_uPinoutDrv[FC00][3] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC00+1, BITCHART::DRV_IOA::_FC1_DRV_EN_b31_b24, m_uPinoutDrv[FC00][3] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b7_b0  , m_uPinoutDrv[FC01][0] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b7_b0  , m_uPinoutDrv[FC01][0] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b15_b8 , m_uPinoutDrv[FC01][1] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b15_b8 , m_uPinoutDrv[FC01][1] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b23_b16, m_uPinoutDrv[FC01][2] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b23_b16, m_uPinoutDrv[FC01][2] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_EN_b31_b24, m_uPinoutDrv[FC01][3] ); if( 0 ) fprintf( stdout, "PINOUT DRV FC%d W(0x%08X) : 0x%02X\n", FC01+1, BITCHART::DRV_IOB::_FC2_DRV_EN_b31_b24, m_uPinoutDrv[FC01][3] );
	}
}

void CPg::SetPinOutIoA( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutIoA[FC00][nArray]  = (m_uPinoutIoA[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutIoA[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutIoA[FC01][nArray]  = (m_uPinoutIoA[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutIoA[FC01][nArray] |= (nEn<<nOffset);
	}
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b7_b0  , m_uPinoutIoA[FC00][0] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b7_b0  , m_uPinoutIoA[FC00][0] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b15_b8 , m_uPinoutIoA[FC00][1] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b15_b8 , m_uPinoutIoA[FC00][1] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b23_b16, m_uPinoutIoA[FC00][2] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b23_b16, m_uPinoutIoA[FC00][2] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b31_b24, m_uPinoutIoA[FC00][3] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b31_b24, m_uPinoutIoA[FC00][3] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b39_b32, m_uPinoutIoA[FC00][4] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b39_b32, m_uPinoutIoA[FC00][4] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b47_b40, m_uPinoutIoA[FC00][5] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b47_b40, m_uPinoutIoA[FC00][5] );

	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b7_b0  , m_uPinoutIoA[FC01][0] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b7_b0  , m_uPinoutIoA[FC01][0] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b15_b8 , m_uPinoutIoA[FC01][1] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b15_b8 , m_uPinoutIoA[FC01][1] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b23_b16, m_uPinoutIoA[FC01][2] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b23_b16, m_uPinoutIoA[FC01][2] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b31_b24, m_uPinoutIoA[FC01][3] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b31_b24, m_uPinoutIoA[FC01][3] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b39_b32, m_uPinoutIoA[FC01][4] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b39_b32, m_uPinoutIoA[FC01][4] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b47_b40, m_uPinoutIoA[FC01][5] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b47_b40, m_uPinoutIoA[FC01][5] );
}

//2023.04.21 - fatric
void CPg::SetPinOutIoAVal( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutIoA[FC00][nArray]  = (m_uPinoutIoA[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutIoA[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutIoA[FC01][nArray]  = (m_uPinoutIoA[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutIoA[FC01][nArray] |= (nEn<<nOffset);
	}
}

//2023.04.21 - fatric
void CPg::SetPinOutIoAAct()
{
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b7_b0  , m_uPinoutIoA[FC00][0] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b7_b0  , m_uPinoutIoA[FC00][0] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b15_b8 , m_uPinoutIoA[FC00][1] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b15_b8 , m_uPinoutIoA[FC00][1] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b23_b16, m_uPinoutIoA[FC00][2] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b23_b16, m_uPinoutIoA[FC00][2] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b31_b24, m_uPinoutIoA[FC00][3] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b31_b24, m_uPinoutIoA[FC00][3] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b39_b32, m_uPinoutIoA[FC00][4] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b39_b32, m_uPinoutIoA[FC00][4] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b47_b40, m_uPinoutIoA[FC00][5] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOA::_FC1_IOA_EN_b47_b40, m_uPinoutIoA[FC00][5] );

	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b7_b0  , m_uPinoutIoA[FC01][0] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b7_b0  , m_uPinoutIoA[FC01][0] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b15_b8 , m_uPinoutIoA[FC01][1] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b15_b8 , m_uPinoutIoA[FC01][1] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b23_b16, m_uPinoutIoA[FC01][2] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b23_b16, m_uPinoutIoA[FC01][2] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b31_b24, m_uPinoutIoA[FC01][3] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b31_b24, m_uPinoutIoA[FC01][3] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b39_b32, m_uPinoutIoA[FC01][4] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b39_b32, m_uPinoutIoA[FC01][4] );
	g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b47_b40, m_uPinoutIoA[FC01][5] ); if( 0 ) fprintf( stdout, "PINOUT IOA FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOA::_FC2_IOA_EN_b47_b40, m_uPinoutIoA[FC01][5] );
}

void CPg::SetPinOutIoA( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutIoA[fc][nArray]  = (m_uPinoutIoA[fc][nArray] & ~(1<<nOffset));
		m_uPinoutIoA[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == FC00 )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b7_b0  , m_uPinoutIoA[FC00][0] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b15_b8 , m_uPinoutIoA[FC00][1] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b23_b16, m_uPinoutIoA[FC00][2] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b31_b24, m_uPinoutIoA[FC00][3] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b39_b32, m_uPinoutIoA[FC00][4] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_EN_b47_b40, m_uPinoutIoA[FC00][5] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b7_b0  , m_uPinoutIoA[FC01][0] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b15_b8 , m_uPinoutIoA[FC01][1] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b23_b16, m_uPinoutIoA[FC01][2] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b31_b24, m_uPinoutIoA[FC01][3] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b39_b32, m_uPinoutIoA[FC01][4] );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_EN_b47_b40, m_uPinoutIoA[FC01][5] );
	}
}

void CPg::SetPinOutIoB( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutIoB[FC00][nArray]  = (m_uPinoutIoB[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutIoB[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutIoB[FC01][nArray]  = (m_uPinoutIoB[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutIoB[FC01][nArray] |= (nEn<<nOffset);
	}
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b7_b0  , m_uPinoutIoB[FC00][0] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b7_b0  , m_uPinoutIoB[FC00][0] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b15_b8 , m_uPinoutIoB[FC00][1] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b15_b8 , m_uPinoutIoB[FC00][1] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b23_b16, m_uPinoutIoB[FC00][2] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b23_b16, m_uPinoutIoB[FC00][2] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b31_b24, m_uPinoutIoB[FC00][3] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b31_b24, m_uPinoutIoB[FC00][3] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b39_b32, m_uPinoutIoB[FC00][4] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b39_b32, m_uPinoutIoB[FC00][4] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b47_b40, m_uPinoutIoB[FC00][5] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b47_b40, m_uPinoutIoB[FC00][5] );

	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b7_b0  , m_uPinoutIoB[FC01][0] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b7_b0  , m_uPinoutIoB[FC01][0] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b15_b8 , m_uPinoutIoB[FC01][1] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b15_b8 , m_uPinoutIoB[FC01][1] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b23_b16, m_uPinoutIoB[FC01][2] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b23_b16, m_uPinoutIoB[FC01][2] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b31_b24, m_uPinoutIoB[FC01][3] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b31_b24, m_uPinoutIoB[FC01][3] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b39_b32, m_uPinoutIoB[FC01][4] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b39_b32, m_uPinoutIoB[FC01][4] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b47_b40, m_uPinoutIoB[FC01][5] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b47_b40, m_uPinoutIoB[FC01][5] );
}

//2023.04.21 - fatric
void CPg::SetPinOutIoBVal( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutIoB[FC00][nArray]  = (m_uPinoutIoB[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutIoB[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutIoB[FC01][nArray]  = (m_uPinoutIoB[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutIoB[FC01][nArray] |= (nEn<<nOffset);
	}
}

//2023.04.21 - fatric
void CPg::SetPinOutIoBAct()
{
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b7_b0  , m_uPinoutIoB[FC00][0] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b7_b0  , m_uPinoutIoB[FC00][0] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b15_b8 , m_uPinoutIoB[FC00][1] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b15_b8 , m_uPinoutIoB[FC00][1] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b23_b16, m_uPinoutIoB[FC00][2] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b23_b16, m_uPinoutIoB[FC00][2] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b31_b24, m_uPinoutIoB[FC00][3] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b31_b24, m_uPinoutIoB[FC00][3] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b39_b32, m_uPinoutIoB[FC00][4] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b39_b32, m_uPinoutIoB[FC00][4] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b47_b40, m_uPinoutIoB[FC00][5] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_IOB::_FC1_IOB_EN_b47_b40, m_uPinoutIoB[FC00][5] );

	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b7_b0  , m_uPinoutIoB[FC01][0] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b7_b0  , m_uPinoutIoB[FC01][0] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b15_b8 , m_uPinoutIoB[FC01][1] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b15_b8 , m_uPinoutIoB[FC01][1] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b23_b16, m_uPinoutIoB[FC01][2] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b23_b16, m_uPinoutIoB[FC01][2] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b31_b24, m_uPinoutIoB[FC01][3] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b31_b24, m_uPinoutIoB[FC01][3] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b39_b32, m_uPinoutIoB[FC01][4] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b39_b32, m_uPinoutIoB[FC01][4] );
	g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b47_b40, m_uPinoutIoB[FC01][5] ); if( 0 ) fprintf( stdout, "PINOUT IOB FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_IOB::_FC2_IOB_EN_b47_b40, m_uPinoutIoB[FC01][5] );
}

void CPg::SetPinOutIoB( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 47  ) nEnd   = 47;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutIoB[fc][nArray]  = (m_uPinoutIoB[fc][nArray] & ~(1<<nOffset));
		m_uPinoutIoB[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == 0 )
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b7_b0  , m_uPinoutIoB[FC00][0] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b15_b8 , m_uPinoutIoB[FC00][1] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b23_b16, m_uPinoutIoB[FC00][2] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b31_b24, m_uPinoutIoB[FC00][3] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b39_b32, m_uPinoutIoB[FC00][4] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_EN_b47_b40, m_uPinoutIoB[FC00][5] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b7_b0  , m_uPinoutIoB[FC01][0] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b15_b8 , m_uPinoutIoB[FC01][1] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b23_b16, m_uPinoutIoB[FC01][2] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b31_b24, m_uPinoutIoB[FC01][3] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b39_b32, m_uPinoutIoB[FC01][4] );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_EN_b47_b40, m_uPinoutIoB[FC01][5] );
	}
}

void CPg::SetPinOutUclk( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 87  ) nEnd   = 87;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutUclk[FC00][nArray]  = (m_uPinoutUclk[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutUclk[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutUclk[FC01][nArray]  = (m_uPinoutUclk[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutUclk[FC01][nArray] |= (nEn<<nOffset);
	}
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b7_b0   , m_uPinoutUclk[FC00][ 0] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b7_b0  , m_uPinoutUclk[FC00][ 0] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b15_b8  , m_uPinoutUclk[FC00][ 1] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b15_b8 , m_uPinoutUclk[FC00][ 1] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b23_b16 , m_uPinoutUclk[FC00][ 2] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b23_b16, m_uPinoutUclk[FC00][ 2] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b31_b24 , m_uPinoutUclk[FC00][ 3] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b31_b24, m_uPinoutUclk[FC00][ 3] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b39_b32 , m_uPinoutUclk[FC00][ 4] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b39_b32, m_uPinoutUclk[FC00][ 4] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b47_b40 , m_uPinoutUclk[FC00][ 5] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b47_b40, m_uPinoutUclk[FC00][ 5] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b55_b48 , m_uPinoutUclk[FC00][ 6] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b55_b48, m_uPinoutUclk[FC00][ 6] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b63_b56 , m_uPinoutUclk[FC00][ 7] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b63_b56, m_uPinoutUclk[FC00][ 7] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b71_b64 , m_uPinoutUclk[FC00][ 8] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b71_b64, m_uPinoutUclk[FC00][ 8] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b79_b72 , m_uPinoutUclk[FC00][ 9] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b79_b72, m_uPinoutUclk[FC00][ 9] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b87_b80 , m_uPinoutUclk[FC00][10] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b87_b80, m_uPinoutUclk[FC00][10] );

	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b7_b0   , m_uPinoutUclk[FC01][ 0] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b7_b0  , m_uPinoutUclk[FC01][ 0] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b15_b8  , m_uPinoutUclk[FC01][ 1] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b15_b8 , m_uPinoutUclk[FC01][ 1] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b23_b16 , m_uPinoutUclk[FC01][ 2] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b23_b16, m_uPinoutUclk[FC01][ 2] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b31_b24 , m_uPinoutUclk[FC01][ 3] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b31_b24, m_uPinoutUclk[FC01][ 3] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b39_b32 , m_uPinoutUclk[FC01][ 4] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b39_b32, m_uPinoutUclk[FC01][ 4] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b47_b40 , m_uPinoutUclk[FC01][ 5] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b47_b40, m_uPinoutUclk[FC01][ 5] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b55_b48 , m_uPinoutUclk[FC01][ 6] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b55_b48, m_uPinoutUclk[FC01][ 6] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b63_b56 , m_uPinoutUclk[FC01][ 7] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b63_b56, m_uPinoutUclk[FC01][ 7] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b71_b64 , m_uPinoutUclk[FC01][ 8] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b71_b64, m_uPinoutUclk[FC01][ 8] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b79_b72 , m_uPinoutUclk[FC01][ 9] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b79_b72, m_uPinoutUclk[FC01][ 9] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b87_b80 , m_uPinoutUclk[FC01][10] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b87_b80, m_uPinoutUclk[FC01][10] );
}

//2023.04.21 - fatric
void CPg::SetPinOutUclkVal( bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 87  ) nEnd   = 87;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutUclk[FC00][nArray]  = (m_uPinoutUclk[FC00][nArray] & ~(1<<nOffset));
		m_uPinoutUclk[FC00][nArray] |= (nEn<<nOffset);

		m_uPinoutUclk[FC01][nArray]  = (m_uPinoutUclk[FC01][nArray] & ~(1<<nOffset));
		m_uPinoutUclk[FC01][nArray] |= (nEn<<nOffset);
	}
}

//2023.04.21 - fatric
void CPg::SetPinOutUclkAct()
{
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b7_b0   , m_uPinoutUclk[FC00][ 0] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b7_b0  , m_uPinoutUclk[FC00][ 0] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b15_b8  , m_uPinoutUclk[FC00][ 1] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b15_b8 , m_uPinoutUclk[FC00][ 1] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b23_b16 , m_uPinoutUclk[FC00][ 2] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b23_b16, m_uPinoutUclk[FC00][ 2] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b31_b24 , m_uPinoutUclk[FC00][ 3] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b31_b24, m_uPinoutUclk[FC00][ 3] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b39_b32 , m_uPinoutUclk[FC00][ 4] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b39_b32, m_uPinoutUclk[FC00][ 4] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b47_b40 , m_uPinoutUclk[FC00][ 5] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b47_b40, m_uPinoutUclk[FC00][ 5] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b55_b48 , m_uPinoutUclk[FC00][ 6] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b55_b48, m_uPinoutUclk[FC00][ 6] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b63_b56 , m_uPinoutUclk[FC00][ 7] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b63_b56, m_uPinoutUclk[FC00][ 7] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b71_b64 , m_uPinoutUclk[FC00][ 8] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b71_b64, m_uPinoutUclk[FC00][ 8] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b79_b72 , m_uPinoutUclk[FC00][ 9] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b79_b72, m_uPinoutUclk[FC00][ 9] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b87_b80 , m_uPinoutUclk[FC00][10] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC00+1,BITCHART::DRV_UCLK::_FC1_UCLK_EN_b87_b80, m_uPinoutUclk[FC00][10] );

	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b7_b0   , m_uPinoutUclk[FC01][ 0] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b7_b0  , m_uPinoutUclk[FC01][ 0] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b15_b8  , m_uPinoutUclk[FC01][ 1] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b15_b8 , m_uPinoutUclk[FC01][ 1] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b23_b16 , m_uPinoutUclk[FC01][ 2] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b23_b16, m_uPinoutUclk[FC01][ 2] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b31_b24 , m_uPinoutUclk[FC01][ 3] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b31_b24, m_uPinoutUclk[FC01][ 3] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b39_b32 , m_uPinoutUclk[FC01][ 4] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b39_b32, m_uPinoutUclk[FC01][ 4] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b47_b40 , m_uPinoutUclk[FC01][ 5] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b47_b40, m_uPinoutUclk[FC01][ 5] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b55_b48 , m_uPinoutUclk[FC01][ 6] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b55_b48, m_uPinoutUclk[FC01][ 6] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b63_b56 , m_uPinoutUclk[FC01][ 7] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b63_b56, m_uPinoutUclk[FC01][ 7] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b71_b64 , m_uPinoutUclk[FC01][ 8] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b71_b64, m_uPinoutUclk[FC01][ 8] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b79_b72 , m_uPinoutUclk[FC01][ 9] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b79_b72, m_uPinoutUclk[FC01][ 9] );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b87_b80 , m_uPinoutUclk[FC01][10] ); if( 0 ) fprintf( stdout, "PINOUT UCLK FC%d W(0x%08X) : 0x%02X\n", FC01+1,BITCHART::DRV_UCLK::_FC2_UCLK_EN_b87_b80, m_uPinoutUclk[FC01][10] );
}

void CPg::SetPinOutUclk( uint fc, bool bEn, uint nBegin ,uint nEnd )
{
	int nEn = (bEn?0x1:0x0);
	if( nBegin < 1 ) nBegin = 0;
	if( nEnd > 87  ) nEnd   = 87;
	for( uint s=nBegin;s<=nEnd;s++ )
	{
		int nArray = s / 8;
		int nOffset= s % 8;
		m_uPinoutUclk[fc][nArray]  = (m_uPinoutUclk[fc][nArray] & ~(1<<nOffset));
		m_uPinoutUclk[fc][nArray] |= (nEn<<nOffset);
	}
	if( fc == 0 )
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b7_b0   , m_uPinoutUclk[FC00][ 0] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b15_b8  , m_uPinoutUclk[FC00][ 1] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b23_b16 , m_uPinoutUclk[FC00][ 2] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b31_b24 , m_uPinoutUclk[FC00][ 3] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b39_b32 , m_uPinoutUclk[FC00][ 4] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b47_b40 , m_uPinoutUclk[FC00][ 5] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b55_b48 , m_uPinoutUclk[FC00][ 6] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b63_b56 , m_uPinoutUclk[FC00][ 7] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b71_b64 , m_uPinoutUclk[FC00][ 8] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b79_b72 , m_uPinoutUclk[FC00][ 9] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_EN_b87_b80 , m_uPinoutUclk[FC00][10] );
	}
	else
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b7_b0   , m_uPinoutUclk[FC01][ 0] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b15_b8  , m_uPinoutUclk[FC01][ 1] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b23_b16 , m_uPinoutUclk[FC01][ 2] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b31_b24 , m_uPinoutUclk[FC01][ 3] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b39_b32 , m_uPinoutUclk[FC01][ 4] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b47_b40 , m_uPinoutUclk[FC01][ 5] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b55_b48 , m_uPinoutUclk[FC01][ 6] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b63_b56 , m_uPinoutUclk[FC01][ 7] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b71_b64 , m_uPinoutUclk[FC01][ 8] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b79_b72 , m_uPinoutUclk[FC01][ 9] );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_EN_b87_b80 , m_uPinoutUclk[FC01][10] );
	}
}

void CPg::SetDrUclkFixDef()
{
////////////////////////////////////////////////////////////////////////////////
// DR Pins
	CPin* pDrPins[4][8] = {
		{ &_PD24, &_PD25, &_PD26, &_PD27, &_PD28, &_PD29, &_PD30, &_PD31 },
		{ &_PD16, &_PD17, &_PD18, &_PD19, &_PD20, &_PD21, &_PD22, &_PD23 },
		{ &_PD8 , &_PD9 , &_PD10, &_PD11, &_PD12, &_PD13, &_PD14, &_PD15 },
		{ &_PD0 , &_PD1 , &_PD2 , &_PD3 , &_PD4 , &_PD5 , &_PD6 , &_PD7  }
	};

	for( int iGrp=0; iGrp<4; ++iGrp )
	{
//CLEAR FIX_L
//		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b31_b24 + iGrp*4, 0x0 );
//		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b31_b24 + iGrp*4, 0x0 );

		uint iPinsFixH = 0;
		for( int iPin=0; iPin<8; ++iPin )
		{
			if ( pDrPins[iGrp][iPin]->GetModule() >= FMT_NORM::_RZO  &&
				 pDrPins[iGrp][iPin]->GetModule() <= FMT_NORM::_DNRZ    )
			{
				iPinsFixH |= 1<<iPin;
			}
		}

//SET FIX_H
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b31_b24 + iGrp*4, iPinsFixH );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b31_b24 + iGrp*4, iPinsFixH );
//SET FIX_L
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b31_b24 + iGrp*4, (~iPinsFixH)&0xff );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b31_b24 + iGrp*4, (~iPinsFixH)&0xff );
	}

////////////////////////////////////////////////////////////////////////////////
// UCLK Pins
	CPin* pUclkPins[11][8] = {
		{ &_UCLK80, &_UCLK81, &_UCLK82, &_UCLK83, &_UCLK84, &_UCLK85, &_UCLK86, &_UCLK87 },
		{ &_UCLK72, &_UCLK73, &_UCLK74, &_UCLK75, &_UCLK76, &_UCLK77, &_UCLK78, &_UCLK79 },
		{ &_UCLK64, &_UCLK65, &_UCLK66, &_UCLK67, &_UCLK68, &_UCLK69, &_UCLK70, &_UCLK71 },
		{ &_UCLK56, &_UCLK57, &_UCLK58, &_UCLK59, &_UCLK60, &_UCLK61, &_UCLK62, &_UCLK63 },
		{ &_UCLK48, &_UCLK49, &_UCLK50, &_UCLK51, &_UCLK52, &_UCLK53, &_UCLK54, &_UCLK55 },
		{ &_UCLK40, &_UCLK41, &_UCLK42, &_UCLK43, &_UCLK44, &_UCLK45, &_UCLK46, &_UCLK47 },
		{ &_UCLK32, &_UCLK33, &_UCLK34, &_UCLK35, &_UCLK36, &_UCLK37, &_UCLK38, &_UCLK39 },
		{ &_UCLK24, &_UCLK25, &_UCLK26, &_UCLK27, &_UCLK28, &_UCLK29, &_UCLK30, &_UCLK31 },
		{ &_UCLK16, &_UCLK17, &_UCLK18, &_UCLK19, &_UCLK20, &_UCLK21, &_UCLK22, &_UCLK23 },
		{ &_UCLK8 , &_UCLK9 , &_UCLK10, &_UCLK11, &_UCLK12, &_UCLK13, &_UCLK14, &_UCLK15 },
		{ &_UCLK0 , &_UCLK1 , &_UCLK2 , &_UCLK3 , &_UCLK4 , &_UCLK5 , &_UCLK6 , &_UCLK7  },
	};

	for( int iGrp=0; iGrp<11; ++iGrp )
	{
//CLEAR FIX_L
//		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b87_b80 + iGrp*4, 0x0 );
//		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b87_b80 + iGrp*4, 0x0 );

		uint iPinsFixH = 0;
		for( int iPin=0; iPin<8; ++iPin )
		{
			if ( pUclkPins[iGrp][iPin]->GetModule() >= FMT_NORM::_RZO  &&
				 pUclkPins[iGrp][iPin]->GetModule() <= FMT_NORM::_DNRZ    )
			{
				iPinsFixH |= 1<<iPin;
			}
		}

//SET FIX_H
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b87_b80 + iGrp*4, iPinsFixH );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b87_b80 + iGrp*4, iPinsFixH );
//SET FIX_L
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b87_b80 + iGrp*4, (~iPinsFixH)&0xff );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b87_b80 + iGrp*4, (~iPinsFixH)&0xff );
	}
}

void CPg::SetDrPinFixL()
{
	//SET DR Pins FIX_L
	for( int iGrp=0; iGrp<4; ++iGrp )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b31_b24 + iGrp*4, 0x00 );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b31_b24 + iGrp*4, 0x00 );
		g_sys.SysDelay( 1 US );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b31_b24 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b31_b24 + iGrp*4, 0xff );
	}
}

//2023.04.21 - fatric
void CPg::SetIoAPinFixL()
{
	//SET IOA Pins FIX_L
	for( int iGrp=0; iGrp<6; ++iGrp )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXH_b47_b40 + iGrp*4, 0x00 );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXH_b47_b40 + iGrp*4, 0x00 );
		g_sys.SysDelay( 1 US );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_IOA_FIXL_b47_b40 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC2_IOA_FIXL_b47_b40 + iGrp*4, 0xff );
	}
}

//2023.04.21 - fatric
void CPg::SetIoBPinFixL()
{
	//SET IOB Pins FIX_L
	for( int iGrp=0; iGrp<6; ++iGrp )
	{
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXH_b47_b40 + iGrp*4, 0x00 );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXH_b47_b40 + iGrp*4, 0x00 );
		g_sys.SysDelay( 1 US );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC1_IOB_FIXL_b47_b40 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_IOB_FIXL_b47_b40 + iGrp*4, 0xff );
	}
}

//2023.04.21 - fatric
void CPg::SetUclkPinFixL()
{
	//SET UCLK Pins FIX_L
	for( int iGrp=0; iGrp<11; ++iGrp )
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b87_b80 + iGrp*4, 0x00 );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b87_b80 + iGrp*4, 0x00 );
		g_sys.SysDelay( 1 US );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b87_b80 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b87_b80 + iGrp*4, 0xff );
	}
}


void CPg::SetDrPinFixH()	//2023.10.12 - KWMOON
{
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	for( int iGrp=0; iGrp<4; ++iGrp )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b31_b24 + iGrp*4, 0x00 );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b31_b24 + iGrp*4, 0x00 );

		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b31_b24 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b31_b24 + iGrp*4, 0xff );
	}
}


//2023.04.21 - fatric
void CPg::SetUclkPinFixH()	//2023.10.12 - KWMOON
{
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	//SET UCLK Pins FIX_L
	for( int iGrp=0; iGrp<11; ++iGrp )
	{
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b87_b80 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b87_b80 + iGrp*4, 0xff );
		g_sys.SysDelay( 1 US );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b87_b80 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b87_b80 + iGrp*4, 0xff );
	}
}

//DRV0~31
void CPg::SetCeFixH()	//2023.10.12 - KWMOON
{
	//SET DR Pins FIX_L
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	for( int iGrp=0; iGrp<4; ++iGrp )
	{
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXL_b31_b24 + iGrp*4, 0x00 );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXL_b31_b24 + iGrp*4, 0x00 );
		g_sys.SysDelay( 1 US );
		g_pcie.write_user( BITCHART::DRV_IOA::_FC1_DRV_FIXH_b31_b24 + iGrp*4, 0xff );
		g_pcie.write_user( BITCHART::DRV_IOB::_FC2_DRV_FIXH_b31_b24 + iGrp*4, 0xff );
	}
}

//UCLK 0~7
//UCLK 32 ~ 39
//UCLK 64 ~ 71
void CPg::SetWeFixH()	//2023.10.12 - KWMOON
{
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b7_b0  , 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b39_b32, 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b71_b64, 0x00 );

	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b7_b0  , 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b39_b32, 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b71_b64, 0x00 );
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b7_b0  , 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b39_b32, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b71_b64, 0xff );

	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b7_b0  , 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b39_b32, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b71_b64, 0xff );
}

//UCLK 8~15
//UCLK 40~47
//UCLK 72~79
void CPg::SetReFixH()	//2023.10.12 - KWMOON
{
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b15_b8 , 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b47_b40, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b79_b72, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b15_b8 , 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b47_b40, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b79_b72, 0xff );
}

//UCLK 28~31
//UCLK 60~61
void CPg::SetWpFixH()	//2023.10.12 - KWMOON
{
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b31_b24 , 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b63_b56 , 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b31_b24 , 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b63_b56 , 0x00 );
	//fprintf( stdout, "%s\n", __FUNCTION__ );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b31_b24 , 0xf0 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b63_b56 , 0x30 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b31_b24 , 0xf0 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b63_b56 , 0x30 );
}

//UCLK 16 ~ 23
//UCLK 48 ~ 55
//UCLK 80 ~ 87
void CPg::SetCleAleFixL()	//2023.10.12 - KWMOON
{
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b87_b80, 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b87_b80, 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b55_b48, 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b55_b48, 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXH_b23_b16, 0x00 );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXH_b23_b16, 0x00 );

	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b87_b80, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b87_b80, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b55_b48, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b55_b48, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC1_UCLK_FIXL_b23_b16, 0xff );
	g_pcie.write_user( BITCHART::DRV_UCLK::_FC2_UCLK_FIXL_b23_b16, 0xff );

}

void CPg::SetPinOutMio( bool bEn, uint nBegin ,uint nEnd )
{

}

void CPg::SetPinOutScanMask1st( bool bEn, uint nBegin, uint nEnd )
{

}

void CPg::SetPinOutScanMask2nd( bool bEn, uint nBegin, uint nEnd )
{

}

void CPg::SetPinOutScanInv( bool bEn, uint nBegin, uint nEnd )
{

}

bool CPg::SetIoMaskGlobal( int mem, long long r, long long f )
{
	int mask = 0x0;
	mask |= static_cast<int>( (r&0xFFFF) >>  0 );
	if( GetIoDdrModeGlobal() == SDR )
		mask |= static_cast<int>( 0xFFFF << 16 );		//FALLING MASK
	else
		mask |= static_cast<int>( (f&0xFFFF) << 16 );	//SET MASK

	if( (mem == 0) && ( r==0) && (f==0) )
	{
		for(int i=0;i<IO_MASK_DEPTH;i++ )
		{
			g_pcie.write_user( BITCHART::ALPG::IO_MASK_REG::_IO_MASK_MEM_0 + (i*4), 0x0 );
			if( 0 ) fprintf( stdout, "IO MASK MEM[%02d] : 0x%08X\n", i*4, 0x0 );
		}
	}
	else
	{
		g_pcie.write_user( BITCHART::ALPG::IO_MASK_REG::_IO_MASK_MEM_0 + (mem*4), mask );
		if( 0 ) fprintf( stdout, "IO MASK MEM[%02d] : 0x%08X\n", mem, mask );
	}
	return true;
}

bool CPg::PgStart()
{
	g_pcie.write_user( BITCHART::ALPG::IDXRL_REG::_PG_START  , 0x0 );		//PG START
// kyb	g_sys.SysDelay( 15 US );
	g_sys.SysDelay( 1 US );
	return true;
}

bool CPg::PgStop()
{
// kyb 230915	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_TG_STOP, 0x1 );		//PG STOP
    g_pcie.write_user( BITCHART::ALPG::IDXRL_REG::_PG_STOP  , 0x1 );		//PG START
// kyb	g_sys.SysDelay( 15 US );
	g_sys.SysDelay( 1 US );
	return true;
}

bool CPg::StartTPg()
{
	//fprintf(stdout, "before TG TgBclkReset...."); getchar();
	g_tg.TgBclkReset();
	//fprintf(stdout, "before TG TgStart...."); getchar();
	g_tg.TgStart();
	//fprintf(stdout, "before PG START...."); getchar();
	return PgStart();
}

bool CPg::StopTPg()
{
	PgStop();
	g_tg.TgStop();
	return true;
}

bool CPg::StopMpat()
{
	return StopTPg();
}

bool CPg::ClearMpat()
{
	AlpgClear();
	DiagClear();
	DbmAddrClear();
	DrvClear();
	CmpClear();
	DfmClear();
	return true;
}

bool CPg::SetPattern()
{
	ClearMpat();
	if( m_pPgc->GetErrCnt() > 0 )
	{
		std::vector<atstring> erros = m_pPgc->GetErrors();
		for( atstring error: erros )
		{
			fprintf( stderr, "%s\n", error.c_str() );
		}
		return false;
	}
	m_PGDATA = m_pPgc->GetPatternData();
	if( m_PGDATA == nullptr )
	{
		fprintf( stderr, "%s Pattern write error!\n", m_sCurrentPattern.c_str() );
		return false;
	}
	else
	{
		if( 0 ) fprintf(stdout, "%s MaxPC:%d\n", m_sCurrentPattern.c_str(), m_pPgc->GetMaxPc() );
	}
	if( 0 ) fprintf( stdout, "PGC REV:%s\n", m_PGDATA->rev );
	WritePattern();
	return true;
}

void CPg::WritePattern()
{
	PGDATA* pattern = m_PGDATA;
	const int ILMODE = 1;
	if( pattern->pcs )
	{
		int pc = 0, waypc = 0, i = 0;
		if( 0 ) printf("Write Pattern Size : %ld\n", pattern->pcs->size() );
		for( PcList::iterator pit=pattern->pcs->begin(); pit!=pattern->pcs->end(); ++pit)
		{
			PCS* pcs = (*pit);
			g_pcie.write_user( BITCHART::ALPG::_OPCODE  + pc, pcs->opcode  ); if( 0 ) fprintf( stdout, "PC:%04d[0x%04X] OPCODE:0x%02X ", i, i, pcs->opcode );
			g_pcie.write_user( BITCHART::ALPG::_OPERAND + pc, pcs->operand ); if( 0 ) fprintf( stdout, "OPR:0x%04X", pcs->operand );
			//printf("[%04d]OPCODE:%02X\n", i, pcs->opcode );
			for( int way=0;way<ILMODE;way++ )
			{
				g_pcie.write_user( BITCHART::ALPG::_DSET_INSTRUCTION      + waypc, pcs->way[way].set_group   );  if( 0 ) fprintf( stdout, "SET:0x%04X ", pcs->way[way].set_group );
				g_pcie.write_user( BITCHART::ALPG::_GROUPA_OPR            + waypc, pcs->way[way].a_operand   );  if( 0 ) fprintf( stdout, "A_OPR:0x%04X ", pcs->way[way].a_operand );
				g_pcie.write_user( BITCHART::ALPG::_GROUPB_OPR            + waypc, pcs->way[way].b_operand   );  if( 0 ) fprintf( stdout, "B_OPR:0x%04X ", pcs->way[way].b_operand );
				g_pcie.write_user( BITCHART::ALPG::_GROUP_C_OPR_b31_b0    + waypc, (pcs->way[way].c_operand>> 0)&0xffffffff );   if( 0 ) fprintf( stdout, "C_OPR:0x%-10lX ", static_cast<unsigned long>(pcs->way[way].c_operand) );
				g_pcie.write_user( BITCHART::ALPG::_GROUP_C_OPR_b35_b32   + waypc, (pcs->way[way].c_operand>>32)&0xf );
				g_pcie.write_user( BITCHART::ALPG::_GROUP_D_OPR_b31_b0    + waypc, (pcs->way[way].d_operand>> 0)&0xffffffff );   if( 0 ) fprintf( stdout, "D_OPR:0x%-10lX ", static_cast<unsigned long>(pcs->way[way].d_operand) );
				g_pcie.write_user( BITCHART::ALPG::_GROUP_D_OPR_b35_b32   + waypc, (pcs->way[way].d_operand>>32)&0xf );
				//g_pcie.write_user( BITCHART::ALPG::                     + waypc, pcs->way[way].set_use );
				g_pcie.write_user( BITCHART::ALPG::_XBASE                 + waypc, pcs->way[way].x_base      );  if( 0 ) fprintf( stdout, "XB:0x%04X ", pcs->way[way].x_base );
				g_pcie.write_user( BITCHART::ALPG::_YBASE                 + waypc, pcs->way[way].y_base      );  if( 0 ) fprintf( stdout, "YB:0x%04X ", pcs->way[way].y_base );
				g_pcie.write_user( BITCHART::ALPG::_ZBASE                 + waypc, pcs->way[way].z_base      );  if( 0 ) fprintf( stdout, "ZB:0x%04X ", pcs->way[way].z_base );
				g_pcie.write_user( BITCHART::ALPG::_XCURRENT              + waypc, pcs->way[way].x_curr      );  if( 0 ) fprintf( stdout, "XC:0x%04X ", pcs->way[way].x_curr );
				g_pcie.write_user( BITCHART::ALPG::_YCURRENT              + waypc, pcs->way[way].y_curr      );  if( 0 ) fprintf( stdout, "YC:0x%04X ", pcs->way[way].y_curr );
				g_pcie.write_user( BITCHART::ALPG::_NB                    + waypc, pcs->way[way].nb_reg      );  if( 0 ) fprintf( stdout, "NB:0x%04X ", pcs->way[way].nb_reg );
				g_pcie.write_user( BITCHART::ALPG::_D3D4                  + waypc, pcs->way[way].d3d4_reg    );  if( 0 ) fprintf( stdout, "D3D4:0x%04X ", pcs->way[way].d3d4_reg );
				g_pcie.write_user( BITCHART::ALPG::_SOURCE                + waypc, pcs->way[way].src_addr    );  if( 0 ) fprintf( stdout, "SRC:0x%04X \n", pcs->way[way].src_addr );
				//g_pcie.write_user( BITCHART::ALPG::                     + waypc, pcs->way[way].use_src_addr);
																												 if( 0 ) fprintf( stdout, "%15s", "" );
				g_pcie.write_user( BITCHART::ALPG::_MUT_CBIT              + waypc, pcs->way[way].mut_ctl     );  if( 0 ) fprintf( stdout, "MUT:0x%04X ", pcs->way[way].mut_ctl );
				g_pcie.write_user( BITCHART::ALPG::_MUT_RW                + waypc, pcs->way[way].mut_rwm     );  if( 0 ) fprintf( stdout, "RWM:0x%04X ", pcs->way[way].mut_rwm );
				g_pcie.write_user( BITCHART::ALPG::_SCAN_ADDRESS          + waypc, pcs->way[way].scan_addr   );  if( 0 ) fprintf( stdout, "SA:0x%04X ", pcs->way[way].scan_addr );
				g_pcie.write_user( BITCHART::ALPG::_VECTOR_ADDRESS        + waypc, pcs->way[way].vector_addr );  if( 0 ) fprintf( stdout, "VA:0x%04X ", pcs->way[way].vector_addr );
				g_pcie.write_user( BITCHART::ALPG::_TP1_TP2               + waypc, pcs->way[way].tp          );  if( 0 ) fprintf( stdout, "TP:0x%04X ", pcs->way[way].tp );
				g_pcie.write_user( BITCHART::ALPG::_INV_HOLD_SOURCE_DATA  + waypc, pcs->way[way].tpd         );  if( 0 ) fprintf( stdout, "TPD:0x%04X ", pcs->way[way].tpd );
				g_pcie.write_user( BITCHART::ALPG::_DBMAP                 + waypc, pcs->way[way].dbmap       );  if( 0 ) fprintf( stdout, "DBMAP:0x%04X ", pcs->way[way].dbmap );
				g_pcie.write_user( BITCHART::ALPG::_DLE                   + waypc, pcs->way[way].dle         );  if( 0 ) fprintf( stdout, "DLE:0x%01X ", pcs->way[way].dle );
				g_pcie.write_user( BITCHART::ALPG::_CYP                   + waypc, pcs->way[way].cyp         );  if( 0 ) fprintf( stdout, "CYP:0x%02X ", pcs->way[way].cyp );
				g_pcie.write_user( BITCHART::ALPG::_TIMESET               + waypc, pcs->way[way].ts          );  if( 0 ) fprintf( stdout, "TS:0x%02X \n", pcs->way[way].ts );
			}
			pc += 4;
			waypc += 4;
			i++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// Appended by fatric.
///
bool CPg::StartMpat( const char* sub )
{
	int realpc  = 0x0;
	if( !m_pPgc->GetSubPc( sub, realpc ) )
	{
		fprintf( stderr, "%s sub/pc not found!\n", sub );
		return false;
	}
	m_nPC = realpc;

	ClrStiLdi();	//2023.05.22 - fatric

	if( ! WriteRegister( realpc ) )
	{
		fprintf( stderr, "%s(0x%04X) write register error!\n", sub, realpc );
		return false;
	}
	if( ! WriteSetDSet( realpc ) )
	{
		fprintf( stderr, "%s(0x%04X) write register error!\n", sub, realpc );
		return false;
	}
	return StartTPg();
}

void CPg::ContinueMpat()
{
	g_pcie.write_user( BITCHART::ALPG::_PG_CONTINUE, 0x1 );
// kyb	g_sys.SysDelay( 100 US );
	g_sys.SysDelay( 1 US );
}

bool CPg::IsPgStop()
{
	const int END_STAT   = (0x1<<0);	//1

	int seq_stat = g_pcie.read_user( BITCHART::ALPG::_SEQUENCE_STATUS );
	if( seq_stat & END_STAT )
	{
		return true;
	}
	return false;
}

bool CPg::IsPgPause()
{
	const int PAUSE_STAT = (0x1<<1);	//2

	int seq_stat = g_pcie.read_user( BITCHART::ALPG::_SEQUENCE_STATUS );
	if( seq_stat & PAUSE_STAT )
	{
		//int nOut = g_pcie.read_user( BITCHART::ALPG::_BUMP_REGISTER );
		//printf( "PG PAUSE...\n");
		return true;
	}
	return false;
}

uint CPg::GetPgRunTime()
{
//	int nRun = g_pcie.read_user( BITCHART::ALPG::_PG_RUN_TIME );
	int nRun = 0;

	return nRun;
}

void CPg::ClearPgRunTime()
{
//	g_pcie.write_user( BITCHART::ALPG::_PG_RUN_TIME, 0x0 );
//	g_pcie.write_user( BITCHART::ALPG::_PG_RUN_TIME_CLEAR, 0x0 );
}
///////////////////////////////////////////////////////////////////////////////

uint CPg::GetBurstCode()
{
	int nOut = g_pcie.read_user( BITCHART::ALPG::_BUMP_REGISTER );
	//printf("\nPAUSE ... 0x%08X\n", nOut );
	return nOut;
}

bool CPg::WriteRegister( int realpc )
{
	ALPG *alpg = &m_PGDATA->alpg;

	//SEQUENCE MODE
	g_pcie.write_user( BITCHART::ALPG::_RERESH_PAUSE_MODE, alpg->seq_mode );

	//ALPG MODE
	g_pcie.write_user( BITCHART::ALPG::_ALPG_MODE, alpg->alpg_mode );

	for( RegList::iterator rit=alpg->reg_list.begin(); rit!=alpg->reg_list.end(); ++rit)
	{
		REGI *reg = (*rit);

		g_pcie.write_user( reg->addr, reg->i2 );
		if( 0 ) printf( "[COMMON][REG] ADDR=%x Data=0x%X\n" , reg->addr, reg->i2);
	}
	alpg = nullptr;
	for( AlpgList::iterator ait=m_PGDATA->module.begin(); ait!=m_PGDATA->module.end(); ++ait)
	{
		alpg = (*ait);
		if( alpg->pc == realpc ) break ;
		alpg=nullptr;
	}

	if( alpg == nullptr ) return false;
	if( alpg )
	{
//		write( BIT_ALPG::SEQ_MODE, alpg->seq_mode );

		for( RegList::iterator rit=alpg->reg_list.begin(); rit!=alpg->reg_list.end(); ++rit)
		{
			REGI *reg = (*rit);

			g_pcie.write_user( reg->addr, reg->i2 );
			if( 0 ) printf( "[MODULE][REG] ADDR=%x Data=0x%X\n" , reg->addr, reg->i2);
		}
		if( alpg->seq_mode )  g_pcie.write_user( BITCHART::ALPG::_RERESH_PAUSE_MODE, alpg->seq_mode );

		if( alpg->addr.mode || alpg->alpg_mode )
		{
			g_pcie.write_user( BITCHART::ALPG::_ALPG_MODE, alpg->alpg_mode );
		}

		g_pcie.write_user( BITCHART::ALPG::_START_ADDRESS, alpg->pc );
		if( 0 ) printf("STA=%s[PC:0x%x][0x%04X]\n", alpg->name, alpg->pc, realpc );

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//REG MPAT WITH PLAN
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for( std::vector<REGI>::iterator rit=m_vecGlobalReg.begin(); rit!=m_vecGlobalReg.end(); ++rit)
		{
			REGI reg = (*rit);		//.GetNext(rPOS);
			g_pcie.write_user( reg.addr, reg.i2 ) ;
			if( 0 ) printf( "[PLAN][REG] ADDR:0x%X, DATA:0x%X\n", reg.addr, reg.i2 );
		}
		m_vecGlobalReg.clear();
	}
	return true;
}

bool CPg::WriteSetDSet ( int realpc )
{
	//////////////////////////////////////////////////////////////////////////
	// Common Section, Module Section, Plan Merge
	//
	//printf("[%s]\n", __FUNCTION__ );

	SET_REG regMerged[16];	//1.regMerged ?�언
	ALPG*   alpg    = &m_PGDATA->alpg ;
	memcpy(regMerged, alpg->set_reg, sizeof(SET_REG)*16 ); //2. regMerged <- COMMON Section Merge

	alpg = nullptr;
	for( AlpgList::iterator ait=m_PGDATA->module.begin(); ait!=m_PGDATA->module.end(); ++ait)
	{
		alpg = *ait;
		if( alpg->pc == realpc ) break;
		alpg = nullptr;
	}

	if( alpg )
	{
		//	alpg->set_reg[];//Module Section
		//3. regMerged <- Module Section Merge
		for (int iSrc=0; iSrc<16; ++iSrc )
		{
			if ( ! alpg->set_reg[iSrc].reg ) break;
			for (int iGrp=0; iGrp<4; ++iGrp )
			{
				if( ! alpg->set_reg[iSrc].groupReg(iGrp) ) continue;

				for (int iDest=0; iDest<16; ++iDest )
				{
					if ( ! regMerged[iDest].groupReg(iGrp) )
					{
						regMerged[iDest].mergeGroup(iGrp, alpg->set_reg[iSrc] );
						break;
					}
					else if ( regMerged[iDest].groupReg(iGrp) == alpg->set_reg[iSrc].groupReg(iGrp) )
					{
						regMerged[iDest].mergeGroup(iGrp, alpg->set_reg[iSrc] );
						break;
					}
				}
			}
		}
	}

	//3. regMerged <- PLAN Section Merge
	for (int iSrc=0; iSrc<16; ++iSrc )
	{
		if ( ! m_SetReg[iSrc].reg ) break;
		for (int iGrp=0; iGrp<4; ++iGrp )
		{
			if( ! m_SetReg[iSrc].groupReg(iGrp) ) continue;

			for (int iDest=0; iDest<16; ++iDest )
			{
				if ( ! regMerged[iDest].groupReg(iGrp) )
				{
					regMerged[iDest].mergeGroup(iGrp, m_SetReg[iSrc] );
					break;
				}
				else if ( regMerged[iDest].groupReg(iGrp) == m_SetReg[iSrc].groupReg(iGrp) )
				{
					regMerged[iDest].mergeGroup(iGrp, m_SetReg[iSrc] );
					break;
				}
			}
		}
	}

	const int nAddrRange = 4; //32bit address->4
	const int nMaxReg    = 16; //15(SET) + 1
	const int opcode_NOP = 0xa;
	const int opcode_JMP = 0xb;

	int nProcessWay = 0;
	int	nPcRegSet  = 0;
	int	iSeqOffset;
	int addrSeq;
	int addrAlpg;
	int iReg = 0;
	int	nSeqCnt = 0;
	int	i = 0;

	nProcessWay = 1;		//1WAY(TOF)
	nSeqCnt     = 0x4000;	//1WAY(TOF)
	nPcRegSet   = nMaxReg/nProcessWay;

	iSeqOffset = nSeqCnt/nProcessWay - nPcRegSet*nAddrRange;

	for ( i=0; i<nPcRegSet; ++i )
	{
		if( regMerged[iReg].reg )
		{
			addrSeq = iSeqOffset + i*nAddrRange;

			g_pcie.write_user( BITCHART::ALPG::_OPCODE  | addrSeq, opcode_NOP ); // OPCODE : NOP
			g_pcie.write_user( BITCHART::ALPG::_OPERAND | addrSeq, 0x0        ); // OPERAND: 0
			if( 0 ) fprintf( stdout, "PC:%04d[0x%04X] OPCODE:0x%02X ", addrSeq, i, opcode_NOP );
			for(int iWay=0; iWay<nProcessWay; ++iWay)
			{
				addrAlpg = addrSeq*nProcessWay + iWay*nAddrRange;
				if( 0 ) fprintf( stdout, "PSEUDO PATTERN PC:%d[0x%04X]\n", addrAlpg/4, addrAlpg/4 );
				//Main                                                         ;
				g_pcie.write_user( BITCHART::ALPG::_DSET_INSTRUCTION    | addrAlpg,  regMerged[iReg].reg               ); // SET COMMAND GROUP A+B+C+D
				g_pcie.write_user( BITCHART::ALPG::_GROUPA_OPR          | addrAlpg,  regMerged[iReg].a                 ); // GROUP A OPERAND
				g_pcie.write_user( BITCHART::ALPG::_GROUPB_OPR          | addrAlpg,  regMerged[iReg].b                 ); // GROUP B OPERAND
				g_pcie.write_user( BITCHART::ALPG::_GROUP_C_OPR_b31_b0  | addrAlpg, (regMerged[iReg].c    )&0xffffffff ); // GROUP C OPERAND 1
				g_pcie.write_user( BITCHART::ALPG::_GROUP_C_OPR_b35_b32 | addrAlpg, (regMerged[iReg].c>>32)&0x0000000f ); // GROUP C OPERAND 2
				g_pcie.write_user( BITCHART::ALPG::_GROUP_D_OPR_b31_b0  | addrAlpg, (regMerged[iReg].d    )&0xffffffff ); // GROUP C OPERAND 1
				g_pcie.write_user( BITCHART::ALPG::_GROUP_D_OPR_b35_b32 | addrAlpg, (regMerged[iReg].d>>32)&0x0000000f ); // GROUP C OPERAND 2
				g_pcie.write_user( BITCHART::ALPG::_TIMESET             | addrAlpg,  0x1                               ); // TS1
				//g_pcie.write_user( BITCHART::ALPG::_DLE                 | addrAlpg,  0X1                               ); // DLE
				if( 0 ) fprintf( stdout, "SET:0x%04X "    , regMerged[iReg].reg );
				if( 0 ) fprintf( stdout, "A_OPR:0x%04X "  , regMerged[iReg].a );
				if( 0 ) fprintf( stdout, "B_OPR:0x%04X "  , regMerged[iReg].b );
				if( 0 ) fprintf( stdout, "C_OPR:0x%-10lX ", static_cast<unsigned long>(regMerged[iReg].c) );
				if( 0 ) fprintf( stdout, "D_OPR:0x%-10lX ", static_cast<unsigned long>(regMerged[iReg].d) );
				if( 0 ) fprintf( stdout, "TS:0x%02X \n"   , 0x1 );

				int iTp = 0x0;
				if ( regMerged[iReg].reg&0x0100 ) iTp |= 0x001;	//TP1
				if ( regMerged[iReg].reg&0x1000 ) iTp |= 0x100;	//TP2
				if ( iTp )
				{
					g_pcie.write_user( BITCHART::ALPG::_TP1_TP2 | addrAlpg , iTp );
				}
				//2022.11.03 - KWMOON(COMMENT)
				//int iDbmap = 0x0;
				//if ( (regMerged[iReg].reg&0x0f00)==0x0900 ) iDbmap = 0x5;						//DBMAR	: 20221014 - KWMOON
				//if ( iDbmap ) g_pcie.write_user( BITCHART::ALPG::_DBMAP | addrAlpg, iDbmap );	//DBMAP Register
			}
			++iReg;
		}
		else break;
	}

	//LAST OPCODE replace(NOP->JMP)
	if( iReg && iReg < 17 )
	{
		int sta;					 //0x1cf0/4 => 0x7f0
		sta = iSeqOffset/nAddrRange; //1WAY=0x7f0, 4WAY:0x3f8, MC: 0x3fe //extented 8k

		g_pcie.write_user( BITCHART::ALPG::_START_ADDRESS, sta );					// START PC SET REG
		if( 0 ) fprintf( stdout, "PSEUDO PATTERN Last OPCODE STA=0x%x, Jump to 0x%x\n", sta, m_nPC );

		addrSeq = iSeqOffset + (i-1)*nAddrRange;

		if( 0 ) fprintf( stdout, "JUMP PATTERN PC:%d[ADDR:0x%04X]\n", addrSeq/4, addrSeq );

		// JMP PC=REG MPAT PC=xxx;
		g_pcie.write_user( BITCHART::ALPG::_OPCODE  | addrSeq, opcode_JMP ); // OPCODE : NOP
		g_pcie.write_user( BITCHART::ALPG::_OPERAND | addrSeq, m_nPC      ); // OPERAND: 0
		g_pcie.write_user( BITCHART::ALPG::_TIMESET | addrSeq, 0x1        ); //timeset
		//write( BIT_ALPG::OP_CODE |addrSeq, opcode_JMP  ); // OPCODE : JMP
		//write( BIT_ALPG::OPERLAND|addrSeq, m_iModulePc ); // OPERAND: module_pc

		m_nPC = iSeqOffset/nAddrRange; //1WAY=0x7f0, 4WAY:0x3f8, MC: 0x3fe //extented 8k

		for( int i=0;i<16;i++ ) m_SetReg[i].clear();
	}

	return true;
}

//2023.06.13 - kwmoon(add fatric)
void CPg::ClrStiLdi()
{
	g_pcie.write_user( BITCHART::GLOBAL_MODE_REG::_STI_LDI_CLEAR, 1 ); //STI, LDI CLEAR
}

void CPg::DumpPattern()
{
	ALPG*     alpg   = &m_PGDATA->alpg;
	AlpgList* module = &m_PGDATA->module;
	PcList*   pcs    = m_PGDATA->pcs;
	if( 1 )
	{
		fprintf( stdout, "%s Dump Pattern [REV:%s]\n", m_PGDATA->name, m_PGDATA->rev );
		if (alpg)
		{
			fprintf( stdout, "[REGISTER]\n");
			for (RegList::iterator rit = alpg->reg_list.begin(); rit != alpg->reg_list.end(); ++rit)
			{
				REGI *reg = (*rit);
				fprintf(stdout, "0x%08X = 0x%08X\n", reg->addr, reg->i2);
			}
			fprintf(stdout, "\n");
		}
		if (module)
		{
			fprintf( stdout, "[STA]\n");
			for (AlpgList::iterator mdlIt = module->begin(); mdlIt != module->end(); ++mdlIt)
			{
				ALPG* pAlpg = *mdlIt;
				fprintf( stdout, "%s = 0x%04X\n", pAlpg->name, pAlpg->pc);
			}
			fprintf(stdout, "\n");
		}
		if (pcs)
		{
			int	pc = 0;
			int iWay = 0;
			fprintf(stdout, "%s", "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			fprintf(stdout, "%s", "PC     OPCODE      OPERAND     SET_GROUP  A_OPR     B_OPR     C_OPR            D_OPR            X_BASE  Y_BASE  Z_BASE   X_MAIN  Y_MAIN  NB     D3D4  SRC_ADR  CTRL        RWM      SCAN      \n");
			fprintf(stdout, "%s", "       VECTOR      TP        INV  DBMAP  DLE   CYP   TS    \n");
			fprintf(stdout, "%s", "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			for (PcList::iterator pit = pcs->begin(); pit != pcs->end(); ++pit)
			{
				PCS *pcs = (*pit);
				int c_1 = (pcs->way[iWay].c_operand>>32)&0xF;
				int c_2 = (pcs->way[iWay].c_operand>> 0)&0xFFFFFFFF;
				int d_1 = (pcs->way[iWay].d_operand>>32)&0xF;
				int d_2 = (pcs->way[iWay].d_operand>> 0)&0xFFFFFFFF;
				fprintf(stdout, "0x%03X  0x%08X  0x%08X  0x%04x     0x%06X  0x%06X  0x%02X:%08X  0x%02X:%08X  0x%04X  0x%04X  0x%04X   0x%04X  0x%04X  0x%04X 0x%02X  0x%04X   0x%08X  0x%05X  0x%08X \n",
					pc, pcs->opcode, pcs->operand, pcs->way[iWay].set_group, pcs->way[iWay].a_operand, pcs->way[iWay].b_operand, c_1, c_2, d_1, d_2, pcs->way[iWay].x_base, pcs->way[iWay].y_base, pcs->way[iWay].z_base,
					pcs->way[iWay].x_curr, pcs->way[iWay].y_curr, pcs->way[iWay].nb_reg, pcs->way[iWay].d3d4_reg, pcs->way[iWay].src_addr, pcs->way[iWay].mut_ctl, pcs->way[iWay].mut_rwm, pcs->way[iWay].scan_addr);
				fprintf(stdout, "       0x%06X  0x%06X  0x%02X  0x%02X  0x%02X  0x%02x  0x%06X  \n",
					pcs->way[iWay].vector_addr, pcs->way[iWay].tp, pcs->way[iWay].tpd, pcs->way[iWay].dbmap, pcs->way[iWay].dle, pcs->way[iWay].cyp, pcs->way[iWay].ts );
				fprintf(stdout, "%s", "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
				pc++;
			}
		}
	}
}

void CPg::SetCurrentPattern( atstring p)
{
	if( 0 ) printf("Current Pattern : %s\n", p.c_str() );
	m_sCurrentPattern = p;
}

atstring CPg::GetCurrentPattern()
{
	return m_sCurrentPattern;
}

void CPg::SetIoDdrModeGlobal(bool mode)
{
	m_bIoDdrMode = mode;
	//InitModuleIo();			//2022.11.23 - kwmoon
}

void CPg::SetAddrDdrModeGlobal(bool mode)
{
	m_bAddrDdrMode = mode;
}

bool CPg::GetIoDdrModeGlobal()
{
	return m_bIoDdrMode;
}

bool CPg::GetAddrDdrModeGlobal()
{
	return m_bAddrDdrMode;
}
