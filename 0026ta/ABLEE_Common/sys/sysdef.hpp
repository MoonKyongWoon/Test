#ifndef SYSDEF_HPP
#define SYSDEF_HPP

typedef unsigned long long int  ull64;
typedef signed long long int     ll64;
typedef unsigned char uchar;

#define ASIZE(arry)					(sizeof(arry)/sizeof(arry[0]))
#define MAX_SYS_CPU					(  2)
#define BIB0						(  0)
#define MAX_FC						(  4)
#define USE_FC						(  2)
#define MAX_DUT						(512)
#define MAX_MCP						( 16)
#define MAX_SLOT					( 16)
#define MAX_CYP						( 16)
#define MAX_TS						( 32)
#define MAX_SYS_IO_BIT				(384)
#define MAX_1CPU_IO_BIT				(192)

#define IO_PIN_PER_FC				( MAX_SYS_IO_BIT / MAX_FC )	//384/4 = 96
#define IOAB_PIN_PER_FC				( IO_PIN_PER_FC / 2)		// 96/2 = 48

#define MAX_SYS_DRV					( 32)
#define MAX_SYS_MIO					(  4)
#define MAX_SYS_UCLK				( 88)
#define IO_MASK_DEPTH				( 16)
#define SCAN_MEM_DEPTH				(256)
#define UBM_DEPTH					(16384)

#define DMA_WR_MAX_SIZE				( 2048 )				//BYTE * sizeof(DBM_STRUCT)
#define FM_MEMORY_DATA_BITS			(  128 )
#define FM_MEMORY_DATA_BYTES		(FM_MEMORY_DATA_BITS/8)
#define EXTFM_MAX_LIMIT_BYTES		( 8ll * 1024 * 1024 * 1024 )						//8 GByte
#define EXTFM_FC_MAX_LIMIT_BYTES	( 4ll * 1024 * 1024 * 1024 )						//4 GByte
#define EXTFM_MAX_LIMIT_BITS		( EXTFM_FC_MAX_LIMIT_BYTES * 8 )						//4 GByte (32Gb) * 8
#define EXTFM_MAX_LIMIT				( EXTFM_FC_MAX_LIMIT_BYTES / FM_MEMORY_DATA_BYTES )		//4 GB(32Gb) / 16 Byte(128bit) -- count
#define EXTFM_SW_MAX_LIMIT			( 160000 )

//#define S							* 1.0e0
#define MS							* 1.0e-3
#define US							* 1.0e-6
#define NS							* 1.0e-9
#define PS							* 1.0e-12

#define AMP							* 1.0
#define amp							* 1.0
//#define A							* 1.0
#define MA							* 1.0e-3
#define UA							* 1.0e-6
#define NA							* 1.0e-9

#define VOLT						* 1.0e0
#define volt						* 1.0e0
//#define V							* 1.0e0
#define MV							* 1.0e-3
#define UV							* 1.0e-6

#define OHM							* 1.0e0
#define KOHM						* 1.0e3
#define MOHM						* 1.0e6

//#define SEC                         * 1
#define MIN							* 60
#define HOUR						* 3600

#define IO_SAVE						(  1)
#define IO_FAIL						(  0)
#define ON							(  1)
#define OFF							(  0)

#define DDR_IO						(  2)
#define IO_PER_FC					( 96)

#define OPEN						( -1)
#define LATCH_CNT					(  6)

#define PGF_RUN						(  1)
#define ALL_BOARD					( -1)
#define ALLSCAN						( -1)

#define EMMC_DRV_MODE				(  0)
#define EMMC_INIT_MODE				(  1)

#define EMMC_IO_MODE				(  0)
#define EMMC_CHIP_MODE				(  1)
#define EMMC_BLOCK_RESULT			(  2)
#define EMMC_IO_USER				(  3)
#define EMMC_FORMAT_0				(  0)
#define EMMC_FORMAT_1				(  1)
#define EMMC_FORMAT_2				(  2)
#define EMMC_FORMAT_3				(  3)

#define EMMC_FORMAT_14				( 14)
#define EMMC_FORMAT_15				( 15)


enum PDS	//CYP
{
	//UCLK
	C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19,
	C20, C21, C22, C23,
	FIXH,
	FIXL,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17,
    Y0, Y1, Y2, Y3, Y4, Y5, Y6, Y7, Y8, Y9, Y10, Y11, Y12, Y13, Y14, Y15, Y16, Y17,
	//DR
	S0 = 0x40, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16, S17,
	S18, S19, S20, S21, S22, S23, S24, S25, S26, S27, S28, S29, S30, S31, S32, S33, S34, S35,
	S36, S37, S38, S39, S40, S41, S42, S43, S44, S45, S46, S47, S48, S49, S50, S51, S52, S53,
	S54, S55, S56, S57, S58, S59, S60, S61, S62, S63,
	//IO OFFSET : 0x42
	Z0=0x80, Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, Z9, Z10, Z11, Z12, Z13, Z14, Z15, Z16, Z17,
	D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15,
	SD0=0xA4, SD1, SD2, SD3, SD4, SD5, SD6, SD7, SD8, SD9, SD10, SD11, SD12, SD13, SD14, SD15,
	V0=0xB6, V1, V2, V3, V4, V5, V6, V7,
};

enum FMT_NORM
{
	RZO        = 0x0,
	RZZ             ,
	RZX             ,
	NRZB            ,
	NRZC            ,
	DNRZ            ,
	NRZB_B          ,	//2024-03-05 fatric
	_RZO       = 0x8,
	_RZZ            ,
	_RZX            ,
	_NRZB           ,
	_NRZC           ,
	_DNRZ           ,
	FL         =0x20,
	FH         =0x30,
};


enum FMT_DRE
{
	DRERZ        = 0x0,
//	DRENRZ            ,	//2024-01-12 fatric
	DRENRZB           ,	//2024-01-12 fatric
	DRENRZC           ,	//2024-01-12 fatric
	DREFIXL      = 0x8,
	DREFIXH      = 0xC,
};

enum PIN_TYPE
{
	PIN_DR     = 0x0,
	PIN_IO          ,
	PIN_UCLK        ,
	PIN_DRE         ,
	PIN_CPE         ,
	PIN_FMA         ,
	PIN_MUT         ,
	MAX_PIN_TYPE    ,
};

enum PINS
{
	RPD0, RPD1, RPD2, RPD3, RPD4, RPD5, RPD6, RPD7, RPD8, RPD9, RPD10, RPD11,
	RPD12, RPD13, RPD14, RPD15, RPD16, RPD17, RPD18,RPD19, RPD20, RPD21, RPD22, RPD23, RPD24,
	RPD25, RPD26, RPD27, RPD28, RPD29, RPD30, RPD31,
	RP0, RP1, RP2, RP3, RP4, RP5, RP6, RP7, RP8, RP9, RP10, RP11, RP12, RP13, RP14, RP15, RP16, RP17, RP18,
	RP19, RP20, RP21, RP22, RP23, RP24 ,RP25, RP26, RP27, RP28, RP29, RP30, RP31, RP32, RP33, RP34, RP35, RP36,
	RP37, RP38, RP39, RP40, RP41, RP42, RP43, RP44, RP45, RP46, RP47,
	RUCLK0 , RUCLK1,  RUCLK2 , RUCLK3 , RUCLK4 , RUCLK5 , RUCLK6 , RUCLK7 , RUCLK8 , RUCLK9 , RUCLK10, RUCLK11,
	RUCLK12, RUCLK13, RUCLK14, RUCLK15, RUCLK16, RUCLK17, RUCLK18, RUCLK19, RUCLK20, RUCLK21, RUCLK22,
	RUCLK23, RUCLK24, RUCLK25, RUCLK26, RUCLK27, RUCLK28, RUCLK29, RUCLK30, RUCLK31, RUCLK32, RUCLK33, RUCLK34,
	RUCLK35, RUCLK36, RUCLK37, RUCLK38, RUCLK39, RUCLK40, RUCLK41, RUCLK42, RUCLK43, RUCLK44, RUCLK45, RUCLK46,
	RUCLK47, RUCLK48, RUCLK49, RUCLK50, RUCLK51, RUCLK52, RUCLK53, RUCLK54, RUCLK55, RUCLK56, RUCLK57, RUCLK58,
	RUCLK59, RUCLK60, RUCLK61, RUCLK62, RUCLK63, RUCLK64, RUCLK65, RUCLK66, RUCLK67, RUCLK68, RUCLK69, RUCLK70,
	RUCLK71, RUCLK72, RUCLK73, RUCLK74, RUCLK75, RUCLK76, RUCLK77, RUCLK78, RUCLK79, RUCLK80, RUCLK81, RUCLK82,
	RUCLK83, RUCLK84, RUCLK85, RUCLK86, RUCLK87,
	RDREL, RDRET, RDRE2, RDRE3, RDRE4, RDRE5, RDRE6, RDRE7, RDRE8,
	RCPE0, RCPE1, RCPE2, RCPE3, RCPE4, RCPE5, RCPE6, RCPE7, RCPE8,	
	RFMA0, RFMA1, RFMA2, RFMA3, RFMA4, RFMA5, RFMA6, RFMA7, RFMA8, RFMA9, RFMA10, RFMA11, RFMA12, RFMA13, RFMA14,
	RFMA15, RFMA16, RFMA17, RFMA18, RFMA19, RFMA20, RFMA21, RFMA22, RFMA23, RFMA24, RFMA25,
	RMUT0, RMUT1, RMUT2, RMUT3, RMUT4, RMUT5, RMUT6, RMUT7, RMUT8, RMUT9, RMUT10, RMUT11, RMUT12, RMUT13, RMUT14, RMUT15,
	RDBMA0, RDBMA1, RDBMA2, RDBMA, RDBMA3, RDBMA4, RDBMA5, RDBMA6, RDBMA7, RDBMA8, RDBMA9, RDBMA10, RDBMA11, RDBMA12, RDBMA13,
	RDBMA14, RDBMA15, RDBMA16, RDBMA17, RDBMA18, RDBMA19, RDBMA20, RDBMA21, RDBMA22, RDBMA23,
	RSCMA0, RSCMA1, RSCMA2, RSCMA3, RSCMA4, RSCMA5, RSCMA6, RSCMA7, RSCMA8, RSCMA9, RSCMA10, RSCMA11, RSCMA12, RSCMA13, RSCMA14,
	RCMA15, RSCMA16, RSCMA17
};

enum GLOBAL_REG
{
	RIDX0, RIDX1, RIDX2, RIDX3 , RIDX4 , RIDX5 , RIDX6 , RIDX7 , RIDX8 ,
	RIDX9, RIDX10, RIDX11, RIDX12, RIDX13, RIDX14, RIDX15, RIDX16, RCFLAG,
	RISP , RBAR , RMLOOP,
//	RD1B , RD1C , RD1D  , RD1E  , RD1F  , RD1G  , RD1H  ,		//2023.06.13 - kwmoon(comment fatric)
	RD2B , RD2C , RD2D  ,
	RLMAX, RHMAX, RXMAX , RYMAX , RZMAX ,
	RND2 , RND3 , RND4  , RND5  , RND6  , RND7  , RND8  , RND9  , RND10 , RND11 , RND12 ,
	RND13, RND14, RND15 , RND16 , RND17 , RND18 , RND19 , RND20 , RND21 , RND22 , RND23 ,
	RND24, RND25, RND26 , RND27 , RND28 , RND29 , RND30 , RND31 , RND32 ,
	RBD2 , RBD3 , RBD4  , RBD5  , RBD6  , RBD7  , RBD8  , RBD9  , RBD10 , RBD11 , RBD12 ,
	RBD13, RBD14, RBD15 , RBD16 , RBD17 , RBD18 , RBD19 , RBD20 , RBD21 , RBD22 , RBD23 ,
	RBD24, RBD25, RBD26 , RBD27 , RBD28 , RBD29 , RBD30 , RBD31 , RBD32 ,
	RL1A , RL1B , RL1C  , RL1D  , RL1E  , RL1F  , RL1G  , RL1H  ,
	RL2A , RL2B , RL2C  , RL2D  , RL2E  , RL2F  , RL2G  , RL2H  ,
	RL3A , RL3B , RL3C  , RL3D  , RL3E  , RL3F  , RL3G  , RL3H  ,
	RL4A , RL4B , RL4C  , RL4D  , RL4E  , RL4F  , RL4G  , RL4H  ,
	RS1A ,
	RV1A ,
	RGREG_COUNT,
};

//2023.06.13 - kwmoon(comment fatric)
//enum GLOBAL_SET
//{
//	RXH  , RYH  , RTP1  , RTP2  ,
//	RD1A , RD2A , RTPH1 , RTPH2 ,
//	RD3B , RD4B , RD5   , RD6   ,
//	RNH  , RBH  , RDCMR1, RDCMR2,
//		   RZH  , RXMASK, RYMASK,
//	RXOS , RYOS , RCBMA , RDSC  ,
//	RXT  , RYT  , RRCD  , RCCD  ,
//	RZD  , RZ   , RDSD  ,
//	RND  , RBD  , RDBMAR,
//	RDIG1, RDIG2, RDIG3 ,
//	RTPH1A, RTPH2A,
//	RTPH1B, RTPH2B,
//	RTPH1C, RTPH2C,
//	RTPH1D, RTPH2D,
//	RGSET_COUNT,
//};

enum GLOBAL_SET
{
	RXH  , RYH  , RTP1  , RTP2  ,
	RD1A , RD2A , RTPH1 , RTPH2 ,
	RD3B , RD4B , RD5   , RD6   ,
	RNH  , RBH  , RDCMR1, RDCMR2,
	RD1B , RZH  , RXMASK, RYMASK,	//2023.06.13 - kwmoon(add fatric)
	RXOS , RYOS , RCBMA , RDSC  ,
	RXT  , RYT  , RRCD  , RCCD  ,
	RZD  , RZ   , RDSD  ,
	RD1C , RBD  , RDBMAR,			//2023.06.13 - kwmoon(add fatric)
	RDIG1, RDIG2, RDIG3 ,
	RD1D ,        RTPH1A, RTPH2A,	//2023.06.13 - kwmoon(add fatric)
	RD1E ,        RTPH1B, RTPH2B,	//2023.06.13 - kwmoon(add fatric)
	RD1F ,        RTPH1C, RTPH2C,	//2023.06.13 - kwmoon(add fatric)
	RD1G ,        RTPH1D, RTPH2D,	//2023.06.13 - kwmoon(add fatric)
	RD1H ,							//2023.06.13 - kwmoon(add fatric)
	RGSET_COUNT,					
};

//BIB
enum IO_AB
{
	IOA        = 0x1,
	IOB        = 0x2,
	IOAB       = 0x3,
	INVALID_IOAB    ,
};

//BIB
enum BIB_FC
{
	FC1        = 0x10,
	FC2        = 0x20,
};

//BIB
enum BIB_CPU
{
	CPU1       = 0x100,
	CPU2       = 0x200,
	INVALID_CPU       ,
};

enum DEVICE_TYPE
{
    EMMC       = 0x0,
    FLASH_DEVICE    ,
    SRAM            ,
    DRAM            ,
	DIAGCAL         ,	//2023.01.18
    UNKNOWN_DEVICE  ,
};

enum DRV
{
    DRV1       = 0x0,
    DRV2            ,
    DRV3            ,
    DRV4            ,
    DRV5            ,
    DRV6            ,
    DRV7            ,
    DRV8            ,
    DRV9            ,
    DRV10           ,
    DRV11           ,
    DRV12           ,
    DRV13           ,
    DRV14           ,
    DRV15           ,
    DRV16           ,
    DRV17           ,
    DRV18           ,
    DRV19           ,
    DRV20           ,
    DRV21           ,
    DRV22           ,
    DRV23           ,
    DRV24           ,
    DRV25           ,
    DRV26           ,
    DRV27           ,
    DRV28           ,
    DRV29           ,
    DRV30           ,
    DRV31           ,
    DRV32           ,
};

enum TS_CLOCK_LIST
{
	DREL1     = 0x0 ,
	DRET1           ,
    BCLK1           ,
	CCLK1           ,
	BCLK2           ,
	CCLK2           ,
	BCLK3           ,
	CCLK3           ,
	BCLK4           ,
	CCLK4           ,
	BCLK5           ,
	CCLK5           ,
	BCLK6           ,
	CCLK6           ,
	BCLK7           ,
	CCLK7           ,
	BCLK8           ,
	CCLK8           ,
	IOBCLK          ,	//BCLK9
	IOCCLK          ,	//CCLK9
	ADBCLK          ,	//BCLK10
	ADCCLK          ,	//CCLK10
	STBCLK          ,	//STRB1
	STCCLK          ,	//STRB2
	STRB3           ,	//STB
	STRB4           ,	//STC
	STRB5           ,	//STB
	STRB6           ,	//STC
	STRB7           ,	//STB
	STRB8           ,	//STC
	//RSTRB1          ,		 //2022.11.21 - kwmoon(comment)
	//RSTRB2          ,		 //2022.11.21 - kwmoon(comment)
	//RSTRB3          ,		 //2022.11.21 - kwmoon(comment)
	//RSTRB4          ,		 //2022.11.21 - kwmoon(comment)
	//RSTRB5          ,		 //2022.11.21 - kwmoon(comment)
	//RSTRB6          ,		 //2022.11.21 - kwmoon(comment)
	//RSTRB7          ,		 //2022.11.21 - kwmoon(comment)
	//RSTRB8          ,		 //2022.11.21 - kwmoon(comment)
	MAX_CLOCK_LIST  ,
};

enum IDELAY_CLOCK_LIST
{
	IOA_IOCLK      = 0x0,
	IOA_ADCLK           ,
	IOA_CPE0            ,
	IOA_CPE1            ,
	IOA_STRB1_0         ,
	IOA_STRB1_1         ,
	IOA_STRB2_0         ,
	IOA_STRB2_1         ,
	IOB_IOCLK           ,
	IOB_ADCLK           ,
	IOB_CPE0            ,
	IOB_CPE1            ,
	IOB_STRB1_0         ,
	IOB_STRB1_1         ,
	IOB_STRB2_0         ,
	IOB_STRB2_1         ,
	UCLK_BCLK1          ,
	UCLK_BCLK2          ,
	UCLK_BCLK3          ,
	UCLK_BCLK4          ,
	UCLK_BCLK5          ,
	UCLK_BCLK6          ,
	UCLK_BCLK7          ,
	UCLK_BCLK8          ,
	UCLK_STRB1_0        ,
	UCLK_STRB1_1        ,
	UCLK_CPE            ,
	MAX_IDELAY_LIST     ,
};

//PGM API
enum FC00_01
{
	FC00      = 0x0,
	FC01      = 0x1,
};

//SYSTEM
enum CPU00_01
{
	CPU00      = 0x0,
	CPU01      = 0x1,
};

//LIB(CPU1/CPU2 - FC)
enum SYS_FC
{
	FC_12     = 0x0,
	FC_34     = 0x1,
	ALL_FC         ,
};

enum CE_NUM
{
    CE1       = 0x0,
    CE2            ,
    CE3            ,
    CE4            ,
    CE5            ,
    CE6            ,
    CE7            ,
    CE8            ,
    CE9            ,
    CE10           ,
    CE11           ,
    CE12           ,
    CE13           ,
    CE14           ,
    CE15           ,
    CE16           ,
    MAX_CE_NUM     ,
};

enum _IOAB
{
	_IOA      = 0x0,
	IOAA      = 0x0,
	_IOB      = 0x1,
	IOBB      = 0x1,
	_IOAB     = 0x2,
	IOAABB    = 0x2,
};

enum TIMESET
{
	TS1            ,
	TS2            ,
	TS3            ,
	TS4            ,
	TS5            ,
	TS6            ,
	TS7            ,
	TS8            ,
	TS9            ,
	TS10           ,
	TS11           ,
	TS12           ,
	TS13           ,
	TS14           ,
	TS15           ,
	TS16           ,
	TS17           ,
	TS18           ,
	TS19           ,
	TS20           ,
	TS21           ,
	TS22           ,
	TS23           ,
	TS24           ,
	TS25           ,
	TS26           ,
	TS27           ,
	TS28           ,
	TS29           ,
	TS30           ,
	TS31           ,
	TS32           ,
	MAX_TIMESET    ,
};

enum FM_MODE
{
	FAIL_WRITE_MODE       = (0x0<<0),	//FAIL_SAVE_MODE
	ALL_WRITE_MODE        = (0x1<<0),
	FM_MODE_D01           = (0x1<<1),
	EFM_COMP_WRITE_MODE   = (0x1<<2),	//Same Block Address Compress
	EFM_FAIL_LIMIT_MODE   = (0x1<<3),

	EFM_BG_WRITE          = (0x1<<4),
	EFM_RUN               = (0x1<<5),
	EFM_RUN_CLEAR         = (0x1<<6),
	FM_MODE_007           = (0x1<<7),

	EDBM_BG_WRITE         = (0x1<<8),
	EDBM_RUN              = (0x1<<9),
	EDBM_RUN_CLEAR        = (0x1<<10),
	EDBM_XY_MODE          = (0x1<<11),

	EDBM_TP_MODE          = (0x1<<12),
	FM_MODE_D13           = (0x1<<13),
	FM_MODE_D14           = (0x1<<14),
	FM_MODE_D15           = (0x1<<15),

	MATCH_IOnRB           = (0x1<<16),
	MATCH_DIAG_MODE       = (0x1<<17),
	FM_MODE_D18           = (0x1<<18),
	FM_MODE_D19           = (0x1<<19),

	FM_MODE_D20           = (0x1<<20),
	FM_MODE_D21           = (0x1<<21),
	FM_MODE_D22           = (0x1<<22),
	EFM_ECC_16BIT_FCNT    = (0x1<<23),

	EFM_IO_COMP_MODE      = (0x1<<24),
	EFM_FCNT_SAVE_MODE    = (0x1<<25),
	EFM_AUTO_MASK_MODE    = (0x1<<26),
	EFM_REAL_TIME_MODE    = (0x1<<27),

	EFM_BLOCK_MASK_MODE   = (0x1<<28),
	FM_MODE_D29           = (0x1<<29),
	FM_MODE_D30           = (0x1<<30),
	FM_MODE_D31           = (0x1<<31),
};


enum PPS_UNIT
{
    PPS       = 0x0,
	HCPPS     = 0x0,
	S1PS           ,
	S2PS           ,
	VCCIO          ,
	MAX_PPS_TYPE   ,
};

enum VS_TYPE
{
    VS1       = 0x0,
    VS2            ,
    VS3            ,
    VIH            ,
    MAX_VS_TYPE    ,
};

enum PPS_INDEX
{
    PPS_CH0   = 0x0,
    PPS_CH1        ,
    PPS_CH2        ,
    PPS_CH3        ,
    PPS_CH4        ,
    PPS_CH5        ,
    PPS_CH6        ,
    PPS_CH7        ,
    PPS_COUNT      ,
};

enum S1PS_INDEX
{
    S1PS_CH0  = 0x0,
    S1PS_CH1       ,
    S1PS_CH2       ,
    S1PS_CH3       ,
    S1PS_CH4       ,
    S1PS_CH5       ,
    S1PS_CH6       ,
    S1PS_CH7       ,
    S1PS_COUNT     ,
};

enum DEFINE_FLAG
{
    FLAG_A   = (1<<0 ),
    FLAG_B   = (1<<1 ),
    FLAG_C   = (1<<2 ),
    FLAG_D   = (1<<3 ),
    FLAG_E   = (1<<4 ),
    FLAG_F   = (1<<5 ),
    FLAG_G   = (1<<6 ),
    FLAG_H   = (1<<7 ),
    FLAG_I   = (1<<8 ),
    FLAG_J   = (1<<9 ),
    FLAG_K   = (1<<10),
    FLAG_L   = (1<<11),
    FLAG_M   = (1<<12),
    FLAG_N   = (1<<13),
    FLAG_O   = (1<<14),
    FLAG_P   = (1<<15),
    FLAG_Q   = (1<<16),
    FLAG_R   = (1<<17),
    FLAG_S   = (1<<18),
    FLAG_T   = (1<<19),
    FLAG_U   = (1<<20),
    FLAG_V   = (1<<21),
    FLAG_W   = (1<<22),
    FLAG_X   = (1<<23),
    FLAG_Y   = (1<<24),
    FLAG_Z   = (1<<25),
};

enum BIT00_64
{
    BIT00   =  0,
    BIT02   =  2,
    BIT04   =  4,
    BIT08   =  8,
    BIT16   = 16,
    BIT32   = 32,
    BIT64   = 64,
};


enum EMMC_TEST_MODE
{
	BISTX1     = 0x0,
	BISTX2     = 0x2,
	BISTX4     = 0x4,
	BISTX8     = 0x8,
};

enum SDR_DDR
{
	SDR        = 0x0,
	DDR             ,
};

#endif
