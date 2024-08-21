#include <deviceif.h>
#include <math.h>
#include <string.h>

const char	*_VER	= "1.0.0";
const char	*FTP_DEST	= "/home/atsys/atsystem/dist_logs";


int D1B_ADD=0 ;
int D1C_ADD=0 ;
int D1D_ADD=0 ;

#define SDP

#ifdef ODP2CE
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x2000				// Block address size per 1CE 
#define CE_CNT		2				// CE Count
#define CHIP_CNT	4				// Chip count per 1CE
#define ST_CNT		4				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	8 
#define ST_YS		0x40
#define ST_YB		0x10
#define ABN_LIMIT	2                               // Interlock Dut Noise Check
#endif

#ifdef HDP8CE
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x1000				// Block address size per 1CE
#define CE_CNT		8				// CE Count
#define CHIP_CNT	2				// Chip count per 1CE
#define ST_CNT		2				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	32 
#define ST_YS		0x40
#define ST_YB		0x10
#define ABN_LIMIT	8                               // Interlock Dut Noise Check
#endif

#ifdef ODP8CE
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x800				// Block address size per 1CE 
#define CE_CNT		8				// CE Count
#define CHIP_CNT	1				// Chip count per 1CE
#define ST_CNT		1				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	32 
#define ST_YS		0x40
#define ST_YB		0x10
#define ABN_LIMIT	8                               // Interlock Dut Noise Check
#endif

#ifdef HDP
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x2000				// Block address size per 1CE
#define CE_CNT		4				// CE Count
#define CHIP_CNT	4				// Chip count per 1CE
#define ST_CNT		4				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	16 
#define ST_YS		0x40
#define ST_YB		0x10
#define ABN_LIMIT	4                               // Interlock Dut Noise Check
#endif

#ifdef ODP
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x1000				// Block address size per 1CE 
#define CE_CNT		4				// CE Count
#define CHIP_CNT	2				// Chip count per 1CE
#define ST_CNT		2				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	16 
#define ST_YS		0x40
#define ST_YB		0x10
#define ABN_LIMIT	4                               // Interlock Dut Noise Check
#endif

#ifdef QDP
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x800				// Block address size per 1CE 
#define CE_CNT		4				// CE Count
#define CHIP_CNT	1				// Chip count per 1CE
#define ST_CNT		1				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	16 
#define ST_YS		0x00
#define ST_YB		0x00
#define ABN_LIMIT	4                               // Interlock Dut Noise Check
#endif

#ifdef DDP
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x800				// Block address size per 1CE 
#define CE_CNT		2				// CE Count
#define CHIP_CNT	1				// Chip count per 1CE
#define ST_CNT		1				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	8 
#define ST_YS		0x00
#define ST_YB		0x00
#define ABN_LIMIT	2                               // Interlock Dut Noise Check
#endif

#ifdef SDP
#define CHIP_SIZE	0x800				// Block address size per 1chip
#define BLOCK_SIZE      0x800				// Block address size per 1CE 
#define CE_CNT		1				// CE Count
#define CHIP_CNT	1				// Chip count per 1CE
#define ST_CNT		1				// Chip count per 1CE
#define BIB_SCAN	4 
#define REAL_SCAN	4 
#define ST_YS		0x00
#define ST_YB		0x00
#endif

#ifdef B152
#define PKGTYPE		0				// 152Ball PKG
#endif

#ifdef B316
#define PKGTYPE		1				// 316Ball PKG
#endif

#define DEVICE_CODE     0xAD7E                          // PE 512G TLC 
//#define MAX_FAIL        (CE_CNT*CHIP_CNT*CBADLMT)	// Limit Fail Block+1
#define MAX_FAIL        (CHIP_CNT*CBADLMT)		// Limit Fail Block+1
#define ADD_LIMIT       0				// Add Item Limit Fail Block
#define COLUMN_SIZE     18432				// Main (16384) + Spare (2048)
#define BLK_CPRS        11				// FM One Block Size X11~
#define BANK_CNT	4
//ADVAN
#define BIB_DUT		192
#define CBADLMT		36				// Bad block limit per Chip (@ Class1)
#define PBADLMT		11				// Bad block limit per Plane (@ Class1)
#define PBADLMT2	9				// Bad block limit per Plane (@ Class1)

//#define PBADLMT 	CBADLMT				// Bad block limit per Plane (@ Class1)
//#define PBADLMT	CBADLMT/4			// Bad block limit per Plane (@ Class1)

#define BADLMT		(CE_CNT*CHIP_CNT*CBADLMT)	// Total bad block limit
#define FUNCECC		70				// Function ECC BIT (@ Class1)
#define BLKCOUNT	1084				// Real Block Count per Chip
#define LASTBLK		0x43B				// Real Last Block Address per Chip (0x43B=1083 @ S176 512Gb TLC)


///=============RRT SCREEN ============
#define REPEAT_OFFSET          700     //Repeat offset
#define INVERSE_OFFSET         350     //Inverse offset
#define RR_REG_CNT               7     //COL 8~15

///=============SLC RRT SCREEN ========
#define REPEAT_OFFSET_SLC	100    //Repeat offset
#define INVERSE_OFFSET_SLC	50     //Inverse offset
#define RR_REG_CNT_SLC		1      //COL 8~15

///=============RRT SCREEN ============
#define REPEAT_START            16     //First Repeat index
#define RR_INFO_CNT              2     //Information Loop count(total cnt, rr reg cnt)
#define TOTAL_RR_CNT            50     //COL 0~7 
#define RR_INFO_REPEAT           8     //RR Information Repeat count
#define RR_REPEAT_CNT            8     //RR Data Repeat count

///=============PACT SCREEN ============
#define REPEAT_OFFSET_PACT      4096   //Repeat offset
#define INVERSE_OFFSET_PACT     2048   //Inverse offset
#define PACT_REPEAT_CNT            8   //RR Data Repeat count

#define CAMRST		198		// 2ms
#define ERATM		1998		// 20ms
#define MXERTM		1998		// 20ms
#define PGMTM		598		// 6ms
#define MXPGTM		998		// 10ms

#define CAMRST_100	19998		// 2ms
#define ERATM_100	199998		// 20ms
#define MXERTM_100	199998		// 20ms
#define PGMTM_100	59998		// 6ms
#define MXPGTM_100	99998		// 10ms


#ifdef RRT_REV1
///---- PE 512Gb TLC RRT Rev.1.1 ------------------------------------------------------------
unsigned int MAJOR_RRT[REPEAT_OFFSET+REPEAT_START] ={0,};
unsigned int RRT_TABLE[2][INVERSE_OFFSET]=    {
//TLC RRT
{ 0,  0,  0,  0,  0,  0,  0,
  3,  3,  4,  5,  3,  3,  3,
  7,  2,  4,  0,246,251,251,
253,246,250,248,247,249,248,
  3,  5,  0,248,238,234,226,
  8,  7,  5,  1,253,249,244,
 15,  6,  4,254,251,247,246,
  8,  5,244,253,248,244,246,
255,  4,  4,254,253,248,242,
252,252,  1,251,251,252,244,
245,  2,  0,252,252,246,240,
242,  0,255,251,250,246,242,
250,254,249,250,  6,244,237,
234,250,  2,245,245,239,248,
 10,  8,  2,251,250,237,237,
 10,  8,241,248,242,231,236,
  1,  7,  2,255,243,243,220,
241,246,  1,244,246,238,230,
250,  5,239,  2,245,  6,229,
248,  6,245,250,255,235,226,
246,254,244,245,241,236,232,
241,251,245,248,237,246,221,
241,252,254,249,231,242,233,
251,  2,254,  1,240,  4,225,
239,  2,253,248,253,228,216,
239,  1,252,250,239,239,226,
235,255,253,247,231,234,221,
234,254,250,242,235,229,228,
246,253,250,238,236,221,221,
246,250,248,244,231,231,226,
  9,248,246,240,255,228,215,
235,245,241,235,250,220,222,
 19,  6,241,  3,  0,  4,215,
  6,  9,247,  4,252,253,237,
  3,  2,  7,244,252,236,250,
245,  4,  6,  0,246,251,  7,
250,  1,254,254,252,249,248,
  9,249,248,247,  2,243,214,
  6, 12,249, 12,  6,  8,243,
  3, 12, 13,  8, 10,  3,  6,
  0,  9, 11, 11,248,  6,  2,
253,  9,  9, 10,  3,  3,253,
 10,  7,  8,  6,  6,  0,254,
248,245,255,241,243,236,244,
  5,250,250,247,250,241,239,
240,246,221,236,229,219,216,
234,221,240,216,228,216,213,
  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0},
//SLC RRT
{ 0, 32, 50, 17, 65,246,234, 68,  8,218,
 62, 70, 56, 53, 75, 44, 41, 80, 35, 29,
 85, 23, 20, 14, 11,  5,  2,255,252,249,
243,240,237,224, 90,220,216,100,212,210,
110,206,204,120,200,198,196,190,176,156}
};
#endif

#ifdef RRT_REV2
///---- PE 512Gb TLC RRT Rev.2 ------------------------------------------------------------
unsigned int MAJOR_RRT[REPEAT_OFFSET+REPEAT_START] ={0,};
unsigned int RRT_TABLE[2][INVERSE_OFFSET]=    {
//TLC RRT
{ 0,  0,  0,  0,  0,  0,  0,
  0,  1,  3,  3,  4,  4,  3,
  9,  2,  4,251,  2,251,251,
253,  2,250,252,247,242,248,
  3,  5,  0,243,238,234,226,
  8,  7,254,252,253,249,233,
 15,251,252,250,251,250,235,
  8,  5,  0,248,248,244,235,
255,  4,  4,  4,253,  6,242,
252,252,  1,246,251,252,244,
245,  2,  0,247,252,246,240,
242,  0,255,246,250,246,242,
250,254,249,245,  6,244,237,
 10,250,  2,240,250,239,248,
 10,  8,  2,246,242,237,237,
  1,  8,241,243,243,231,236,
241,  7,  1,250,246,243,230,
250,246,239,239,245,238,229,
248,  1,245,250,255,242,226,
246,248,244,246,241,248,232,
241,248,245,248,237,242,221,
251,  3,254,255,240,247,225,
  4,  2,253,243,  3,228,216,
  5,  1,252,245,  1,239,226,
 10,255,253,242,  6,234,221,
246,254,250,237,236,229,228,
  9,253,250,233,255,221,221,
  7,250,248,239,  2,231,226,
 19,248,246,235,  0,228,215,
  6,245,241,230,252,220,222,
  3,  6,247,254,252,  4,237,
245,  9,  7,255,246,253,250,
250,  0,  6,239,252,236,  7,
  7,  3,254,249,246,249,248,
  6,249,  3,242,  6,243, 10,
  3,  6,249,  6, 10, 10,243,
  0, 12, 13,  3,248,  3,  6,
253,  9, 11,  6,  3,  6,  2,
  9,255,  9,250,  4,239,253,
248,  7,252,  1,243,  0,241,
  5,252,252,250,250,246,230,
240,244,250,240,229,247,239,
 11,221,221,216,  3,216,216,
 13,254,240,244,  2,240,213,
 13,  4,249,253,254,245,232,
 16,  2,254,246,  1,242,230,
253,  1,254,248,243,238,237,
  0,  4,250,249,245,236,225,
  4,  0,252,245,242,235,223,
  5,  5,251,252,247,239,233},
//SLC RRT
{ 0, 32, 50, 17, 65,246,234, 68,  8,218,
 62, 70, 56, 53, 75, 44, 41, 80, 35, 29,
 85, 23, 20, 14, 11,  5,  2,255,252,249,
243,240,237,224, 90,220,216,100,212,210,
110,206,204,120,200,198,196,190,176,156}
};
#endif

#ifdef RRT_REV3
///---- PE 512Gb TLC RRT Rev.3 ------------------------------------------------------------
unsigned int MAJOR_RRT[REPEAT_OFFSET+REPEAT_START] ={0,};
unsigned int RRT_TABLE[2][INVERSE_OFFSET]=    {
//TLC RRT
{ 0,  0,  0,  0,  0,  0,  0,
  0,  1,  3,  3,  4,  4,  3,
  9,  2,  4,251,  2,251,251,
253,  2,250,252,247,242,248,
  3,251,  0,250,238,250,226,
  8,  7,254,252,253,249,233,
 15,  5,252,243,251,234,235,
  8,  5,  0,248,248,244,235,
255,  4,  4,  4,253,  6,242,
252,252,  1,246,251,252,244,
245,  2,  0,247,252,246,240,
242,  0,255,246,250,246,242,
250,254,249,245,  6,244,237,
 10,250,  2,240,250,239,248,
 10,  8,  2,246,242,237,237,
  1,  8,241,243,243,231,236,
241,  7,  1,250,246,243,230,
250,246,239,239,245,238,229,
248,  1,245,250,255,242,226,
246,248,244,246,241,248,232,
241,248,245,248,237,242,221,
251,  3,254,255,240,247,225,
  4,  2,253,243,  3,228,216,
  5,  1,252,245,  1,239,226,
 10,255,253,242,  6,234,221,
246,254,250,237,236,229,228,
  9,253,250,233,255,221,221,
  7,250,248,239,  2,231,226,
 19,248,246,235,  0,228,215,
  6,245,241,230,252,220,222,
  3,  6,247,254,252,  4,237,
245,  9,  7,255,246,253,250,
250,  0,  6,239,252,236,  7,
  7,  3,254,249,246,249,248,
  6,249,  3,242,  6,243, 10,
  3,  6,249,  6, 10, 10,243,
  0, 12, 13,  3,248,  3,  6,
253,  9, 11,  6,  3,  6,  2,
  9,255,  9,250,  4,239,253,
248,  7,252,  1,243,  0,241,
  5,252,252,250,250,246,230,
240,244,250,240,229,247,239,
 11,221,221,216,  3,216,216,
 13,254,240,244,  2,240,213,
 13,  4,249,253,254,245,232,
 16,  2,254,246,  1,242,230,
253,  1,254,248,243,238,237,
  0,  4,250,249,245,236,225,
  4,  0,252,245,242,235,223,
  5,  5,251,252,247,239,233},
//SLC RRT
{ 0, 32, 50, 17, 65,246,234, 68,  8,218,
 62, 70, 56, 53, 75, 44, 41, 80, 35, 29,
 85, 23, 20, 14, 11,  5,  2,255,252,249,
243,240,237,224, 90,220,216,100,212,210,
110,206,204,120,200,198,196,190,176,156}
};
#endif

#ifdef RRT_REV4
///---- PE 512Gb TLC RRT Rev.4 ------------------------------------------------------------
unsigned int MAJOR_RRT[REPEAT_OFFSET+REPEAT_START] ={0,};
unsigned int RRT_TABLE[2][INVERSE_OFFSET]=    {
//TLC RRT
{ 0,  0,  0,  0,  0,  0,  0,
  0,  1,  3,  3,  4,  4,  3,
  9,  2,  4,251,  2,251,251,
253,  2,250,252,247,242,248,
  3,251,  0,250,238,250,226,
  8,  7,254,252,253,249,233,
 15,  5,252,243,251,234,235,
  8,  5,  0,248,248,244,235,
255,  4,  4,  4,253,  6,242,
252,252,  1,246,251,252,244,
245,  7,  0,254,252,244,240,
242,  0,255,246,250,246,242,
250,254,  3,245,  6,244,227,
 10,250,  2,240,250,239,248,
 10,  8,  2,246,242,237,237,
  1,  8,241,243,243,231,236,
241,  7,  1,250,246,243,230,
250,246,239,239,245,238,229,
248,  7,245,  0,255,246,226,
246,248,244,246,241,248,232,
241,248,245,248,237,242,221,
251,  3,254,255,240,247,225,
  4,  2,253,243,  3,228,216,
  5,  1,252,245,  1,239,226,
 10,255,253,242,  6,234,221,
246,254,250,237,236,229,228,
  9,253,250,233,255,221,221,
  7,250,248,239,  2,231,226,
 19,248,246,235,  0,228,215,
  6,245,241,230,252,220,222,
  3,  6,248,254,252,  4,237,
245,  9,  7,255,246,253,250,
250,  0,  6,239,252,236,  7,
  7,  3,254,249,246,249,248,
  6,249,  3,242,  6,243, 10,
  3,  6,249,  6, 10, 10,243,
  0, 12, 13,  3,248,  3,  6,
253,  9, 11,  6,  3,  6,  2,
  9,255,  9,250,  4,239,253,
248,  7,252,  1,243,  0,241,
  5,252,  0,250,250,246,232,
 13,244,250,240,  8,247,239,
 11,221,221,216,  3,216,216,
 13,254,240,244,  2,240,213,
 13,  4,249,253,254,245,232,
 16,  2,253,246,  1,242,230,
253,  1,254,248,243,238,237,
  0,  4,250,249,245,236,225,
  4,  0,252,245,242,235,223,
  5,  5,251,252,247,239,233},
//SLC RRT
{ 0, 32, 50, 17, 65,246,234, 68,  8,218,
 62, 70, 56, 53, 75, 44, 41, 80, 35, 29,
 85, 23, 20, 14, 11,  5,  2,255,252,249,
243,240,237,224, 90,220,216,100,212,210,
110,206,204,120,200,198,196,190,176,156}
};
#endif

unsigned int CRCCUM =0 ;	
unsigned int CRCREF[512];	
unsigned int CHIP = 1;
unsigned int CHIP_REP = 1;
unsigned int SCANLOOP ;
unsigned int SCAN_NUM ;
unsigned int CUSTOM ;

unsigned int _SITE_SEL		=0;
unsigned int _SITE_NUM		=0;

double set_vdd = 0;
double set_vddq = 0;
double set_vcc = 0;
double set_vccq = 0;
double set_vih = 0; //df2200
double set_vth = 0;
double set_vtt = 0;

unsigned char m_l1_result[MAX_DUT][9000*3]; // 211027 BYS
unsigned int m_l1_result_cnt[MAX_DUT];      // 211027 BYS

double HXVCC = 0.0 ;
double REVCC = 0.0 ;
double REVCCQ =0.0 ;

double MNVCC = 2.3 ;
double MDVCC = 2.7 ;
double MMVCC = 3.3 ;
double MXVCC = 3.7 ;

double MNVCCQ = 1.1  ;
double MDVCCQ = 1.14 ;
double MMVCCQ = 1.2  ;
double MXVCCQ = 1.33  ;

double PS1 ;
double PS2 ;
double PS3 ;


//=====================================================================================
//       Function Item 1024 Byte ECC Variable      by yckim
// =====================================================================================
//unsigned int bbcol[BIB_DUT][6];  	// added by SKH
unsigned int bbcol[BIB_DUT][12];  	// added by SKH
unsigned int bbcnt[BIB_DUT][CE_CNT];  	// added by SKH
unsigned int TDBIFBIT[BIB_DUT][CE_CNT];

unsigned int B0FBIT[BIB_DUT];
unsigned int B1FBIT[BIB_DUT];
unsigned int B2FBIT[BIB_DUT];
unsigned int B3FBIT[BIB_DUT];
unsigned int FBIT[BIB_DUT];
unsigned int BFBIT[BIB_DUT];
unsigned int SFBIT[BIB_DUT];
unsigned int BCFBIT[BIB_DUT][CE_CNT][ST_CNT];  // 191010 RealPF

unsigned int STADD[REAL_SCAN];

//unsigned int TBAD[BIB_DUT];
//unsigned int CBAD1[BIB_DUT];
//unsigned int CBAD2[BIB_DUT];

unsigned int CPF[BIB_DUT];
unsigned int FPF[BIB_DUT];
unsigned int BADD[BADLMT];
unsigned int BAD_IMSI[BADLMT];
unsigned int BADDX[BIB_DUT][BADLMT];
//unsigned int BAD1, BAD2;
unsigned int FBL[BADLMT];
//unsigned int XCEBAD1[BIB_DUT][128];
//unsigned int XCEBAD1[BIB_DUT][2048];
//unsigned int BADD5[BADLMT];
//unsigned int BADD6[BADLMT];
//unsigned int UDT1[BIB_DUT][9000];
//unsigned int UDT2[BIB_DUT][9000];
//unsigned int P0DT[BIB_DUT][1000];
//unsigned int P1DT[BIB_DUT][1000];
//unsigned int P2DT[BIB_DUT][1000];
//unsigned int P3DT[BIB_DUT][1000];
//unsigned int P4DT[BIB_DUT][1000];
//unsigned int P5DT[BIB_DUT][1000];
//unsigned int P6DT[BIB_DUT][1000];
//unsigned int P7DT[BIB_DUT][1000];
//unsigned int P8DT[BIB_DUT][1000];

unsigned int WTBAD[BIB_DUT][16][16];
unsigned int WTCNT[BIB_DUT][16];

unsigned int WTBAD1[BIB_DUT][16][CBADLMT*4]; // for HDP
unsigned int WTCNT1[BIB_DUT][16];

//For AWLRC
unsigned int wlrc_sum[BIB_DUT][REAL_SCAN][500];
unsigned int for_median[BIB_DUT][REAL_SCAN][500];
unsigned int wlrc_median[BIB_DUT][REAL_SCAN][500];
unsigned int wlrc_delta_up[BIB_DUT][REAL_SCAN][500];
unsigned int wlrc_delta_down[BIB_DUT][REAL_SCAN][500];
unsigned int group_median[BIB_DUT][REAL_SCAN][500];
unsigned int group_median1[BIB_DUT][REAL_SCAN][500];
unsigned int group_median2[BIB_DUT][REAL_SCAN][500];
unsigned int median_delta[BIB_DUT][REAL_SCAN][500];

//static 
//unsigned int LEVSKIP[BIB_DUT][CE_CNT][CHIP_CNT];
//unsigned int FUNBAD[BIB_DUT][128];
//unsigned int FUNBAD_CNT[BIB_DUT];

//BLKCAMPGM();
//new variable sjkim
static unsigned int BADC0[BIB_DUT][CE_CNT*CHIP_CNT][600];
//unsigned short BADC1[BIB_DUT][600];
//unsigned short BADC2[BIB_DUT][600];
//unsigned short BADC3[BIB_DUT][600];
//unsigned short BADC4[BIB_DUT][600];
//unsigned short BADC5[BIB_DUT][600];
//unsigned short BADC6[BIB_DUT][600];
//unsigned short BADC7[BIB_DUT][600];
//unsigned short BADC8[BIB_DUT][600];

int C0NR[BIB_DUT][CE_CNT*CHIP_CNT];
//int C1NR[CE_CNT*CHIP_CNT];
//int C2NR[CE_CNT*CHIP_CNT];
//int C3NR[CE_CNT*CHIP_CNT];
//int C4NR[CE_CNT*CHIP_CNT];
//int C5NR[CE_CNT*CHIP_CNT];
//int C6NR[CE_CNT*CHIP_CNT];
//int C7NR[CE_CNT*CHIP_CNT];
//int C8NR[CE_CNT*CHIP_CNT];

unsigned int FAILVCCI[BIB_DUT][CE_CNT][4];

//UID Check FABMIX PROTECT 
unsigned int FAB_CODE[4] = {0x0b, 0x0c, 0x0e, 0x0f};	/// FAB CODE [ M11, M12, M14, M15 ] for Reference
unsigned int REF_FAB_CODE = 0;				/// FAB CODE for compare
unsigned int M1_CNT, M2_CNT, M4_CNT, M5_CNT = 0;	/// CHIP_CNT per FAB
unsigned int CUID3 = 0;					/// (Temp) STORE FAB CODE
unsigned int UID[BIB_DUT][ST_CNT][REAL_SCAN];		/// Store FAB CODE per CHIP
unsigned int Major_FAB_CODE_CNT = 0;			/// Most FAB CODE Count Store


#ifdef HDP
START_DEVICE(NAND)
#else
START_DEVICE(NAND)
#endif


int parseCommand( int cmd_c, char *cmd_v[] )
{
    return DeviceIF::parseCommand(cmd_c , cmd_v );
}


void TS025M(double BSET=25.0)
{

	double CSET = (BSET/2.0);
	//SET_STROBE_DELAY(0,20,1000);	//s=0~19, 10NS
	SET_STROBE_DELAY(0,32,1000);	//s=0~31, 10NS

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//	       TS1     TS2     TS3     TS4     TS5     TS6     TS7     TS8     TS9    TS10    TS11    TS12    TS13    TS14    TS15    TS16
	//	       CMD     ADD    DATA      tR      RD   MATCH   CE#=H    tWHR    tADL     tCS  TM_CMD   tDBSY  TM_ADD  TM_BIT    tRHW   RST_R
	//-----------------------------------------------------------------------------------------------------------------------------------------
	RATE(           25,     25,     25, 160000,     25,  10000,     25,    120,    300,     20,    100,   1000,    100,    100,     92, 200000);

	CLK(ADBCLK,      2,      2,      2,      2,      2,      2,      2,      2,      2,     10,      2,      2,      2,      2,      2,      2); //PD0~PD31 CLK (scan)
	//CLK(ADBCLK,    2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2); //PD0~PD31 CLK (scan)
	CLK(ADCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,     18,      8,      8,      8,      8,      8,      8);

	CLK(IOBCLK,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0); //DREL_DRET = NRZB (IO)
	CLK(IOCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8);

	CLK(STBCLK,      8,      8,      8,      8,     13,   4000,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8); //CPE0-CPE1 = RZ0 (Strbe)
	CLK(STCCLK,     14,     14,     14,     14,     16,   6000,     14,     14,     14,     14,     14,     14,     14,     14,     14,     14);

	CLK( BCLK1,      3,      3,      3,      3,      3,      3,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //WE(ROW1_8)
	CLK( CCLK1,     15,     15,     15,     15,     15,     15,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);
	CLK( BCLK2,      3,      3,      3,      3,      3,      3,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //WE(ROW9_16)
	CLK( CCLK2,     15,     15,     15,     15,     15,     15,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);

	CLK( BCLK3,      3,      3,      3,      3,      3,      5,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //RE(ROW1_8)
	CLK( CCLK3,     15,     15,     15,     15,     15,   9900,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);
	CLK( BCLK4,      3,      3,      3,      3,      3,      5,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //RE(ROW9_16)
	CLK( CCLK4,     15,     15,     15,     15,     15,   9900,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);

	CLK( BCLK5,      5,      5,      5,      5,      5,      5,      5,      5,     15,     10,     15,     15,     15,     15,     15,     15); //CLE
	CLK( CCLK5,     20,     20,     20,     20,     20,     20,     20,     20,     90,     15,     90,     90,     90,     90,     90,     90);
	CLK( BCLK6,      5,      5,      5,      5,      5,      5,      5,      5,     15,     10,     15,     15,     15,     15,     15,     15); //ALE
	CLK( CCLK6,     20,     20,     20,     20,     20,     20,     20,     20,     90,     15,     90,     90,     90,     90,     90,     90);
	CLK( BCLK7,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //DQS
	CLK( CCLK7,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	CLK( BCLK8,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //WP
	CLK( CCLK8,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	//-----------------------------------------------------------------------------------------------------------------------------------------
}

void TS025M_TS6_100NS(double BSET=25.0)
{

	double CSET = (BSET/2.0);
	//SET_STROBE_DELAY(0,20,1000);	//s=0~19, 10NS
	SET_STROBE_DELAY(0,32,1000);	//s=0~31, 10NS

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//	       TS1     TS2     TS3     TS4     TS5     TS6     TS7     TS8     TS9    TS10    TS11    TS12    TS13    TS14    TS15    TS16
	//	       CMD     ADD    DATA      tR      RD   MATCH   CE#=H    tWHR    tADL     tCS  TM_CMD   tDBSY  TM_ADD  TM_BIT    tRHW   RST_R
	//-----------------------------------------------------------------------------------------------------------------------------------------
	RATE(           25,     25,     25, 160000,     25,    100,     25,    120,    300,     20,    100,   1000,    100,    100,     92, 200000);

	CLK(ADBCLK,      2,      2,      2,      2,      2,      2,      2,      2,      2,     10,      2,      2,      2,      2,      2,      2); //PD0~PD31 CLK (scan)
	//CLK(ADBCLK,    2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2); //PD0~PD31 CLK (scan)
	CLK(ADCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,     18,      8,      8,      8,      8,      8,      8);

	CLK(IOBCLK,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0); //DREL_DRET = NRZB (IO)
	CLK(IOCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8);

	CLK(STBCLK,      8,      8,      8,      8,     13,     40,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8); //CPE0-CPE1 = RZ0 (Strbe)
	CLK(STCCLK,     14,     14,     14,     14,     16,     60,     14,     14,     14,     14,     14,     14,     14,     14,     14,     14);

	CLK( BCLK1,      3,      3,      3,      3,      3,      3,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //WE(ROW1_8)
	CLK( CCLK1,     15,     15,     15,     15,     15,     15,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);
	CLK( BCLK2,      3,      3,      3,      3,      3,      3,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //WE(ROW9_16)
	CLK( CCLK2,     15,     15,     15,     15,     15,     15,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);

	CLK( BCLK3,      3,      3,      3,      3,      3,      5,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //RE(ROW1_8)
	CLK( CCLK3,     15,     15,     15,     15,     15,     90,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);
	CLK( BCLK4,      3,      3,      3,      3,      3,      5,      3,      3,     20,      3,     20,     20,     20,     20,     20,     20); //RE(ROW9_16)
	CLK( CCLK4,     15,     15,     15,     15,     15,     90,     15,     15,     70,     15,     70,     70,     70,     70,     70,     70);

	CLK( BCLK5,      5,      5,      5,      5,      5,      5,      5,      5,     15,     10,     15,     15,     15,     15,     15,     15); //CLE
	CLK( CCLK5,     20,     20,     20,     20,     20,     20,     20,     20,     90,     15,     90,     90,     90,     90,     90,     90);
	CLK( BCLK6,      5,      5,      5,      5,      5,      5,      5,      5,     15,     10,     15,     15,     15,     15,     15,     15); //ALE
	CLK( CCLK6,     20,     20,     20,     20,     20,     20,     20,     20,     90,     15,     90,     90,     90,     90,     90,     90);
	CLK( BCLK7,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //DQS
	CLK( CCLK7,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	CLK( BCLK8,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //WP
	CLK( CCLK8,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	//-----------------------------------------------------------------------------------------------------------------------------------------
}

void TS100M(double BSET=100.0)
{
	SET_STROBE_DELAY(0,32,1000);	//s=0~31, 10NS

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//	       TS1     TS2     TS3     TS4     TS5     TS6     TS7     TS8     TS9    TS10    TS11    TS12    TS13    TS14    TS15    TS16
	//	       CMD     ADD    DATA      tR      RD   MATCH   CE#=H    tWHR    tADL     tCS  TM_CMD   tDBSY  TM_ADD  TM_BIT    tRHW   RST_R
	//-----------------------------------------------------------------------------------------------------------------------------------------
	RATE(          100,    100,    100, 160000,    100,  10000,    100,    300,    300,    100,    100,   1000,    100,    100,    200, 500000);

	CLK(ADBCLK,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2); //PD0~PD31 CLK (scan)
	CLK(ADCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8);

	CLK(IOBCLK,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0); //DREL_DRET = NRZB (IO)
	CLK(IOCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8);

	CLK(STBCLK,      8,      8,      8,      8,     60,   4000,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8); //CPE0-CPE1 = RZ0 (Strbe)
	CLK(STCCLK,     14,     14,     14,     14,     70,   6000,     14,     14,     14,     14,     14,     14,     14,     14,     14,     14);

	CLK( BCLK1,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //WE(ROW1_8)
	CLK( CCLK1,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);
	CLK( BCLK2,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //WE(ROW9_16)
	CLK( CCLK2,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);

	CLK( BCLK3,     20,     20,     20,     20,     20,      5,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //RE(ROW1_8)
	CLK( CCLK3,     70,     70,     70,     70,     70,   9900,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);
	CLK( BCLK4,     20,     20,     20,     20,     20,      5,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //RE(ROW9_16)
	CLK( CCLK4,     70,     70,     70,     70,     70,   9900,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);

	CLK( BCLK5,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15); //CLE
	CLK( CCLK5,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90);
	CLK( BCLK6,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15); //ALE
	CLK( CCLK6,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90);

	CLK( BCLK7,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //DQS
	CLK( CCLK7,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	CLK( BCLK8,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //WP
	CLK( CCLK8,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	//-----------------------------------------------------------------------------------------------------------------------------------------
}

void TS100M_TS6_100NS(double BSET=100.0)
{

	SET_STROBE_DELAY(0,32,1000);	//s=0~31, 10NS

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//	       TS1     TS2     TS3     TS4     TS5     TS6     TS7     TS8     TS9    TS10    TS11    TS12    TS13    TS14    TS15    TS16
	//	       CMD     ADD    DATA      tR      RD   MATCH   CE#=H    tWHR    tADL     tCS  TM_CMD   tDBSY  TM_ADD  TM_BIT    tRHW   RST_R
	//-----------------------------------------------------------------------------------------------------------------------------------------
	RATE(          100,    100,    100, 160000,    100,    100,    100,    300,    300,    100,    100,   1000,    100,    100,    200, 500000);

	CLK(ADBCLK,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2,      2); //PD0~PD31 CLK (scan)
	CLK(ADCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8);

	CLK(IOBCLK,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0); //DREL_DRET = NRZB (IO)
	CLK(IOCCLK,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8);

	CLK(STBCLK,      8,      8,      8,      8,     60,     40,      8,      8,      8,      8,      8,      8,      8,      8,      8,      8); //CPE0-CPE1 = RZ0 (Strbe)
	CLK(STCCLK,     14,     14,     14,     14,     70,     60,     14,     14,     14,     14,     14,     14,     14,     14,     14,     14);

	CLK( BCLK1,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //WE(ROW1_8)
	CLK( CCLK1,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);
	CLK( BCLK2,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //WE(ROW9_16)
	CLK( CCLK2,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);

	CLK( BCLK3,     20,     20,     20,     20,     20,      5,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //RE(ROW1_8)
	CLK( CCLK3,     70,     70,     70,     70,     70,     90,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);
	CLK( BCLK4,     20,     20,     20,     20,     20,      5,     20,     20,     20,     20,     20,     20,     20,     20,     20,     20); //RE(ROW9_16)
	CLK( CCLK4,     70,     70,     70,     70,     70,     90,     70,     70,     70,     70,     70,     70,     70,     70,     70,     70);

	CLK( BCLK5,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15); //CLE
	CLK( CCLK5,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90);
	CLK( BCLK6,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15); //ALE
	CLK( CCLK6,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90,     90);

	CLK( BCLK7,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //DQS
	CLK( CCLK7,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	CLK( BCLK8,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0 ,     0,      0); //WP
	CLK( CCLK8,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15,     15);
	//-----------------------------------------------------------------------------------------------------------------------------------------
}

void PIN_OUT_ENABLE()
{
	SET_PINOUT_DRV( true ,0, 31 );	
	SET_PINOUT_IOA( true ,0, 47 );
	SET_PINOUT_IOB( true ,0, 47 );	
	SET_PINOUT_UCLK( true ,0, 87 );
	SET_PINOUT_MIO(false, 0,1);	
	SET_PINOUT_SCANMASK_1ST( false ,0, 31 );
	SET_PINOUT_SCANMASK_2ND( true ,0, 31 );
	SET_PINOUT_SCAN_INV(true, 0, 63);

	SET_VS_ON( 1 ,  0x33);	//VTT
	SET_VS_ON( 0 ,  0xff);
	SET_VS_ON( 3 ,  0xff);	//VIH
	sysDelay(1000 MS);
}

void PIN_OUT_ENABLE_VCC()
{
	SET_PINOUT_DRV( true ,0, 31 );	
	SET_PINOUT_IOA( true ,0, 47 );
	SET_PINOUT_IOB( true ,0, 47 );	
	SET_PINOUT_UCLK( true ,0, 87 );
	SET_PINOUT_MIO(false, 0,1);	
	SET_PINOUT_SCANMASK_1ST( false ,0, 31 );
	SET_PINOUT_SCANMASK_2ND( true ,0, 31 );
	SET_PINOUT_SCAN_INV(true, 0, 63);

	///New Power up Sequence VCC > VCCQ & Others
	SET_VS_ON( 0 ,  0x55);  //VCC
	sysDelay(1000 MS);      //Wait time 1sec
	SET_VS_ON( 0 ,  0xff);  //VCC & VCCQ
	SET_VS_ON( 1 ,  0x33);	//VTT
	SET_VS_ON( 3 ,  0xff);	//VIH
	sysDelay(1000 MS);
}


//프로그램 로딩 시 한번 수행
void programStart()
{
	setVersion("ABlee Sample NandPgm") ;
	SET_FAULTBIN(9);	
	
	SET_SCAN(
			0x00000001,           //all scan
			0x00000002,
			0x00000004,           //all scan
			0x00000008,
			0x00000010,           //all scan
			0x00000020,
			0x00000040,           //all scan
			0x00000080,
			0x00000100,
			0x00000200,
			0x00000400,
			0x00000800,
			0x00001000,
			0x00002000,
			0x00004000,
			0x00008000,
			0x00010000,
			0x00020000,
			0x00040000,
			0x00080000,
			0x00100000,
			0x00200000,
			0x00400000,
			0x00800000,
			0x01000000,
			0x02000000,
			0x04000000,
			0x08000000,
			0x10000000,
			0x20000000,
			0x40000000,
			0x80000000,
		    0 );	

	SET_BIB( BIB_SCAN, 8, 16, 12 , false, true );		//BIB visible SCAN, BIB board io 		

	//BIB(
	//	SI(1,23)|CPU2|FC2|IOA, SI(2,23)|CPU2|FC2|IOA, SI(3,23)|CPU2|FC2|IOA, SI(4,23)|CPU2|FC2|IOA	, SI(1,24)|CPU2|FC2|IOA, SI(2,24)|CPU2|FC2|IOA, SI(3,24)|CPU2|FC2|IOA, SI(4,24)|CPU2|FC2|IOA	, SI(1,47)|CPU2|FC2|IOB, SI(2,47)|CPU2|FC2|IOB, SI(3,47)|CPU2|FC2|IOB, SI(4,47)|CPU2|FC2|IOB	, SI(1,48)|CPU2|FC2|IOB , SI(2,48)|CPU2|FC2|IOB , SI(3,48)|CPU2|FC2|IOB , SI(4,48)|CPU2|FC2|IOB ,
	//	SI(1,21)|CPU2|FC2|IOA, SI(2,21)|CPU2|FC2|IOA, SI(3,21)|CPU2|FC2|IOA, SI(4,21)|CPU2|FC2|IOA 	, SI(1,22)|CPU2|FC2|IOA, SI(2,22)|CPU2|FC2|IOA, SI(3,22)|CPU2|FC2|IOA, SI(4,22)|CPU2|FC2|IOA	, SI(1,45)|CPU2|FC2|IOB, SI(2,45)|CPU2|FC2|IOB, SI(3,45)|CPU2|FC2|IOB, SI(4,45)|CPU2|FC2|IOB 	, SI(1,46)|CPU2|FC2|IOB , SI(2,46)|CPU2|FC2|IOB , SI(3,46)|CPU2|FC2|IOB , SI(4,46)|CPU2|FC2|IOB ,
	//	SI(1,19)|CPU2|FC2|IOA, SI(2,19)|CPU2|FC2|IOA, SI(3,19)|CPU2|FC2|IOA, SI(4,19)|CPU2|FC2|IOA 	, SI(1,20)|CPU2|FC2|IOA, SI(2,20)|CPU2|FC2|IOA, SI(3,20)|CPU2|FC2|IOA, SI(4,20)|CPU2|FC2|IOA 	, SI(1,43)|CPU2|FC2|IOB, SI(2,43)|CPU2|FC2|IOB, SI(3,43)|CPU2|FC2|IOB, SI(4,43)|CPU2|FC2|IOB 	, SI(1,44)|CPU2|FC2|IOB , SI(2,44)|CPU2|FC2|IOB , SI(3,44)|CPU2|FC2|IOB , SI(4,44)|CPU2|FC2|IOB ,
    //
	//	SI(1,17)|CPU2|FC1|IOA, SI(2,17)|CPU2|FC1|IOA, SI(3,17)|CPU2|FC1|IOA, SI(4,17)|CPU2|FC1|IOA 	, SI(1,18)|CPU2|FC1|IOA, SI(2,18)|CPU2|FC1|IOA, SI(3,18)|CPU2|FC1|IOA, SI(4,18)|CPU2|FC1|IOA 	, SI(1,41)|CPU2|FC1|IOB, SI(2,41)|CPU2|FC1|IOB, SI(3,41)|CPU2|FC1|IOB, SI(4,41)|CPU2|FC1|IOB 	, SI(1,42)|CPU2|FC1|IOB , SI(2,42)|CPU2|FC1|IOB , SI(3,42)|CPU2|FC1|IOB , SI(4,42)|CPU2|FC1|IOB ,
	//	SI(1,15)|CPU2|FC1|IOA, SI(2,15)|CPU2|FC1|IOA, SI(3,15)|CPU2|FC1|IOA, SI(4,15)|CPU2|FC1|IOA 	, SI(1,16)|CPU2|FC1|IOA, SI(2,16)|CPU2|FC1|IOA, SI(3,16)|CPU2|FC1|IOA, SI(4,16)|CPU2|FC1|IOA 	, SI(1,39)|CPU2|FC1|IOB, SI(2,39)|CPU2|FC1|IOB, SI(3,39)|CPU2|FC1|IOB, SI(4,39)|CPU2|FC1|IOB 	, SI(1,40)|CPU2|FC1|IOB , SI(2,40)|CPU2|FC1|IOB , SI(3,40)|CPU2|FC1|IOB , SI(4,40)|CPU2|FC1|IOB ,
	//	SI(1,13)|CPU2|FC1|IOA, SI(2,13)|CPU2|FC1|IOA, SI(3,13)|CPU2|FC1|IOA, SI(4,13)|CPU2|FC1|IOA 	, SI(1,14)|CPU2|FC1|IOA, SI(2,14)|CPU2|FC1|IOA, SI(3,14)|CPU2|FC1|IOA, SI(4,14)|CPU2|FC1|IOA 	, SI(1,37)|CPU2|FC1|IOB, SI(2,37)|CPU2|FC1|IOB, SI(3,37)|CPU2|FC1|IOB, SI(4,37)|CPU2|FC1|IOB 	, SI(1,38)|CPU2|FC1|IOB , SI(2,38)|CPU2|FC1|IOB , SI(3,38)|CPU2|FC1|IOB , SI(4,38)|CPU2|FC1|IOB ,
    //
	//	SI(1,11)|CPU1|FC2|IOA, SI(2,11)|CPU1|FC2|IOA, SI(3,11)|CPU1|FC2|IOA, SI(4,11)|CPU1|FC2|IOA	, SI(1,12)|CPU1|FC2|IOA, SI(2,12)|CPU1|FC2|IOA, SI(3,12)|CPU1|FC2|IOA, SI(4,12)|CPU1|FC2|IOA	, SI(1,35)|CPU1|FC2|IOB, SI(2,35)|CPU1|FC2|IOB, SI(3,35)|CPU1|FC2|IOB, SI(4,35)|CPU1|FC2|IOB	, SI(1,36)|CPU1|FC2|IOB , SI(2,36)|CPU1|FC2|IOB , SI(3,36)|CPU1|FC2|IOB , SI(4,36)|CPU1|FC2|IOB ,
	//	SI(1,9)|CPU1|FC2|IOA,  SI(2,9)|CPU1|FC2|IOA , SI(3,9)|CPU1|FC2|IOA , SI(4,9)|CPU1|FC2|IOA 	, SI(1,10)|CPU1|FC2|IOA, SI(2,10)|CPU1|FC2|IOA, SI(3,10)|CPU1|FC2|IOA, SI(4,10)|CPU1|FC2|IOA	, SI(1,33)|CPU1|FC2|IOB, SI(2,33)|CPU1|FC2|IOB, SI(3,33)|CPU1|FC2|IOB, SI(4,33)|CPU1|FC2|IOB 	, SI(1,34)|CPU1|FC2|IOB , SI(2,34)|CPU1|FC2|IOB , SI(3,34)|CPU1|FC2|IOB , SI(4,34)|CPU1|FC2|IOB ,
	//	SI(1,7)|CPU1|FC2|IOA,  SI(2,7)|CPU1|FC2|IOA , SI(3,7)|CPU1|FC2|IOA , SI(4,7)|CPU1|FC2|IOA 	, SI(1,8)|CPU1|FC2|IOA , SI(2,8)|CPU1|FC2|IOA , SI(3,8)|CPU1|FC2|IOA , SI(4,8)|CPU1|FC2|IOA 	, SI(1,31)|CPU1|FC2|IOB, SI(2,31)|CPU1|FC2|IOB, SI(3,31)|CPU1|FC2|IOB, SI(4,31)|CPU1|FC2|IOB 	, SI(1,32)|CPU1|FC2|IOB , SI(2,32)|CPU1|FC2|IOB , SI(3,32)|CPU1|FC2|IOB , SI(4,32)|CPU1|FC2|IOB ,
    //
	//	SI(1,5)|CPU1|FC1|IOA,  SI(2,5)|CPU1|FC1|IOA , SI(3,5)|CPU1|FC1|IOA , SI(4,5)|CPU1|FC1|IOA 	, SI(1,6)|CPU1|FC1|IOA , SI(2,6)|CPU1|FC1|IOA , SI(3,6)|CPU1|FC1|IOA , SI(4,6)|CPU1|FC1|IOA 	, SI(1,29)|CPU1|FC1|IOB, SI(2,29)|CPU1|FC1|IOB, SI(3,29)|CPU1|FC1|IOB, SI(4,29)|CPU1|FC1|IOB 	, SI(1,30)|CPU1|FC1|IOB , SI(2,30)|CPU1|FC1|IOB , SI(3,30)|CPU1|FC1|IOB , SI(4,30)|CPU1|FC1|IOB ,
	//	SI(1,3)|CPU1|FC1|IOA,  SI(2,3)|CPU1|FC1|IOA , SI(3,3)|CPU1|FC1|IOA , SI(4,3)|CPU1|FC1|IOA 	, SI(1,4)|CPU1|FC1|IOA , SI(2,4)|CPU1|FC1|IOA , SI(3,4)|CPU1|FC1|IOA , SI(4,4)|CPU1|FC1|IOA 	, SI(1,27)|CPU1|FC1|IOB, SI(2,27)|CPU1|FC1|IOB, SI(3,27)|CPU1|FC1|IOB, SI(4,27)|CPU1|FC1|IOB 	, SI(1,28)|CPU1|FC1|IOB , SI(2,28)|CPU1|FC1|IOB , SI(3,28)|CPU1|FC1|IOB , SI(4,28)|CPU1|FC1|IOB ,
	//	SI(1,1)|CPU1|FC1|IOA,  SI(2,1)|CPU1|FC1|IOA , SI(3,1)|CPU1|FC1|IOA , SI(4,1)|CPU1|FC1|IOA 	, SI(1,2)|CPU1|FC1|IOA , SI(2,2)|CPU1|FC1|IOA , SI(3,2)|CPU1|FC1|IOA , SI(4,2)|CPU1|FC1|IOA 	, SI(1,25)|CPU1|FC1|IOB, SI(2,25)|CPU1|FC1|IOB, SI(3,25)|CPU1|FC1|IOB, SI(4,25)|CPU1|FC1|IOB 	, SI(1,26)|CPU1|FC1|IOB , SI(2,26)|CPU1|FC1|IOB , SI(3,26)|CPU1|FC1|IOB , SI(4,26)|CPU1|FC1|IOB ,
	//0 ) ;

#ifdef HDP
        SET_CE(0        ,IOA   ,0      ,0x0 	,CE1);
        SET_CE(1        ,IOA   ,1      ,0x2000	,CE2);
        SET_CE(2        ,IOA   ,2      ,0x4000	,CE3);
        SET_CE(3        ,IOA   ,3      ,0x6000	,CE4);

        SET_CE(0        ,IOB   ,0      ,0x0 	,CE1);
        SET_CE(1        ,IOB   ,1      ,0x2000	,CE2);
        SET_CE(2        ,IOB   ,2      ,0x4000	,CE3);
        SET_CE(3        ,IOB   ,3      ,0x6000	,CE4);
#endif

#ifdef ODP
        SET_CE(0        ,IOA   ,0      ,0x0 	,CE1);
        SET_CE(1        ,IOA   ,1      ,0x1000	,CE2);
        SET_CE(2        ,IOA   ,2      ,0x2000	,CE3);
        SET_CE(3        ,IOA   ,3      ,0x3000	,CE4);

        SET_CE(0        ,IOB   ,0      ,0x0 	,CE1);
        SET_CE(1        ,IOB   ,1      ,0x1000	,CE2);
        SET_CE(2        ,IOB   ,2      ,0x2000	,CE3);
        SET_CE(3        ,IOB   ,3      ,0x3000	,CE4);
#endif

	SET_BITIME( 48 HOUR );
	define_flag();
	    
}

void define_flag()
{

#ifdef SDP
	DEFINE_FLAG_S();	// 1CE SDP
#endif

#ifdef DDP
	DEFINE_FLAG_D();	// 2CE DDP
#endif

#ifdef QDP
	DEFINE_FLAG_Q();	// 4CE QDP
#endif

#ifdef ODP
	DEFINE_FLAG_O();	// 4CE ODP
#endif

#ifdef HDP
	DEFINE_FLAG_K();	// 4CE HDP
#endif

#ifdef ODP8CE
	DEFINE_FLAG_T();	// 8CE ODP
#endif

#ifdef HDP8CE
	DEFINE_FLAG_H();	// 8CE HDP
#endif

#ifdef ODP2CE
	DEFINE_FLAG_B();	// 8CE ODP
#endif

//	DEFINE_FLAG_S();	// 1CE SDP
//	DEFINE_FLAG_D();	// 2CE DDP
//	DEFINE_FLAG_Q();	// 4CE QDP
//	DEFINE_FLAG_O();	// 4CE ODP
//	DEFINE_FLAG_K();	// 4CE HDP
//	DEFINE_FLAG_T();	// 8CE ODP
//	DEFINE_FLAG_H();	// 8CE HDP
}

REG(CON)
{
	printf("REG CON\n");
//	LMAX=0x3ffff;
//	HMAX=0x3ffff;
//	ZMAX=0x3ffff;
//	XMAX=0x3ffff;
//	YMAX=0x3ffff;
}

CYP(CYP1)
{
	printf("CYP CYP1\n");
}

MODULE(NORMAL)
{
	printf("MODULE NORMAL\n");
	SET_MODULE( true);
//
//	UCLK0   =	RZX	;	// ADDRESS
//	UCLK1   =	_RZX	;
//	UCLK2   =	_NRZB	;
//	UCLK3   =	FIXL	;	// ADDRESS
//	UCLK4   =	RZZ	;
//	UCLK5   =	FIXH	;
//	UCLK6   =	RZO	;
//	UCLK7   =	_RZO	;
//	UCLK8   =	_NRZB	;
//	UCLK9   =	_NRZB	;
//	UCLK10  =	_RZO	;
//	UCLK11  =	_RZZ	;
//	UCLK16  =	_NRZB	;
//	UCLK17  =	_NRZB	;
//	
//	DREL = NRZB;
//	DRET = NRZB;
//
//	CPE0 = RZO;
//	CPE1 = RZO;
//
//	SET_MODULE( false);
}


VOL(LEVCP_S96_152)	//S72 152
{
	printf("VOL LEVCP_S96_152\n");
	set_vih = (HXVCC*2)+0.0;	//20170524 woo
	set_vth = HXVCC-0.0;

	double set_vmax ;
	double set_vmin ;
	double VPP =12.0 ;

	set_vmax = (HXVCC*2)-0.3;
	set_vmin = 0.40;

	SET_VS( 1 ,  0 , set_vmax , (HXVCC*2)+0.5 , set_vmax-0.5  , 3.0);
	SET_VS( 1 ,  1 , set_vmin , set_vmin+0.5 , set_vmin-0.3  , 3.0);
	SET_VS( 1 ,  2 , VPP , VPP+1.0 , VPP-1.0  , 3.0);	//VPP
	SET_VS( 1 ,  3 , VPP , VPP+1.0 , VPP-1.0  , 3.0);	//VPP
	SET_VS( 1 ,  4 , set_vmax , (HXVCC*2)+0.5 , set_vmax-0.5  , 3.0);
	SET_VS( 1 ,  5 , set_vmin , set_vmin+0.5 , set_vmin-0.3  , 3.0);
	SET_VS( 1 ,  6 , VPP , VPP+1.0 , VPP-1.0  , 3.0);	//VPP
	SET_VS( 1 ,  7 , set_vmin , set_vmin+0.5 , set_vmin-0.3  , 3.0);

	SET_VIH( set_vih);
	SET_VTH( set_vth);
	SET_VIH2(set_vih,set_vth);
}

void device_default( const char* name){


SET_DEFINE(
//      SDEF SCAN_LOOP	2
        SDEF STROBE	C21 C17
        SDEF DRE	C19

        SDEF CmdIn	C2 	C0 C19
        SDEF AleIn	C3 	C0 C19  CYP1
        SDEF WriteTmD		C0 C19
        SDEF AddIn1	C3 	C0 C19	CYP1
        SDEF AddIn2	C3 	C0 C19	CYP2
        SDEF AddIn3	C3 	C0 C19	CYP3
        SDEF ReadID	C1
        SDEF allscan	C31
        SDEF Write		C0 C19

        SDEF PSR_EN	C26	///FIX
        SDEF DBM_EN	C27	///FIX
        SDEF DBM_GET	C28	DBMAP<DBMAP+1 ///FIX
        SDEF DBM_IGET	C28	DBMAP<DBMAP-1 ///FIX
        SDEF DBM_ASET	C29	///FIX
        SDEF MATCH_RB	C30	///FIX
        SDEF ALLSC	C31	///FIX

	SDEF BCLR       XC<0 YC<0 XB<0 YB<0 SA<0 Z<0 TPH<0 TPH2<0
	SDEF XCLR       XA<0 YA<0 
	SDEF FCLR       XF<0 YF<0 
//	SDEF XAYA	X<XC Y<YC
	SDEF INC_SCAN	SA<SA+1

	SDEF M1       CYP2       //X Address
	SDEF M2       CYP5       //Y Address
	SDEF W        C0 C19     //WE DRE 
	SDEF R        C1 C21 C17 //RE STROBE
	SDEF WE       C0         //WE
	SDEF RE       C1         //RE
	SDEF CLE      C2         //CLE
	SDEF ALE      C3         //ALE

        SDEF ACLR     X<XB Y<YB XB<0 YB<0 XC<0 YC<0 D3<D3B D4<D4B Z<0
        SDEF SCLR     XS<0 YS<0

        SDEF XBCLR    XB<0 YB<YB
        SDEF XCCLR    XC<0 YC<YC
        SDEF XSCLR    XS<0 YS<YS

        SDEF XYSHD    X<YS Y<YS
        SDEF XXSHD    X<XS Y<XS
        SDEF YXSHD    Y<XS X<XS

        SDEF YYKHD    X<YK Y<YK

        SDEF XYCHD    X<YC Y<YC
        SDEF YYCHD    Y<YC X<YC
        SDEF XXCHD    X<XC Y<XC
        SDEF YXCHD    Y<XC X<XC
        SDEF YYSHD    Y<YS X<YS

        SDEF XYBHD    X<YB Y<YB
        SDEF XXBHD    X<XB Y<XB
        SDEF YYBHD    X<YB Y<YB

        SDEF CBOUT    X<XC Y<YB
        SDEF SBOUT    X<XS Y<YB
        SDEF BKOUT    X<XB Y<YK

        SDEF COUT     X<XC Y<YC
        SDEF SOUT     X<XS Y<YC
        SDEF KOUT     X<XK Y<YC
        SDEF BOUT     X<XB Y<YC

        SDEF BCARY    XB<XB+1 YB<YB+1^BX

        SDEF AHLDB    XB<XB YB<YB
        SDEF AHLDC    XC<XC YC<YC
        SDEF AHLDS    XS<XS YS<YS
//       SDEF YSD4     YS<D4
//       SDEF XCD1     XC<D1

        SDEF XBINC    XB<XB+1
        SDEF XBDEC    XB<XB-1
        SDEF XB1INC   XB<XB+D1
        SDEF XB2INC   XB<XB+D2
        SDEF XB1DEC   XB<XB-D1
        SDEF XB2DEC   XB<XB-D2

        SDEF XCINC    XC<XC+1
        SDEF XCDEC    XC<XC-1
        SDEF XC1INC   XC<XC+D1
        SDEF XC1DEC   XC<XC-D1
        SDEF XC2INC   XC<XC+D2
        SDEF XC2DEC   XC<XC-D2
        SDEF XC3INC   XC<XC+D3
        SDEF XC3DEC   XC<XC-D3
        SDEF XC4INC   XC<XC+D4
        SDEF XC4DEC   XC<XC-D4

        SDEF XSINC    XS<XS+1
        SDEF XSDEC    XS<XS-1
        SDEF XS1INC   XS<XS+D1
        SDEF XS1DEC   XS<XS-D1
        SDEF XS2INC   XS<XS+D2
        SDEF XS2DEC   XS<XS-D2

        SDEF YBINC    YB<YB+1
        SDEF YBDEC    YB<YB-1

        SDEF YCINC    YC<YC+1
        SDEF YCDEC    YC<YC-1
        SDEF YC1INC   YC<YC+D1
        SDEF YC2INC   YC<YC+D2
        SDEF YC3INC   YC<YC+D3
        SDEF YC3DEC   YC<YC-D3
        SDEF YC4INC   YC<YC+D4
        SDEF YC4DEC   YC<YC-D4

        SDEF YSINC    YS<YS+1
        SDEF YSDEC    YS<YS-1
        SDEF YS1INC   YS<YS+D1
        SDEF YS2INC   YS<YS+D2
        SDEF YS3INC   YS<YS+D3
        SDEF YS4INC   YS<YS+D4
        SDEF YS4DEC   YS<YS-D4

        SDEF YCAINC   YC<YC+D1A
        SDEF YINC     YC<YC+D1F
        SDEF ZINC     Z<Z+ZD

//;==================================================================================
        SDEF DBINC     DBMAP<DBMAP+1
//;==================================================================================
        SDEF DT
        SDEF DB         /D
        SDEF DT2
        SDEF DB2        /D2
        SDEF TPIN       TP<TPH TP2<TPH2
        SDEF TPINH      TPH<TP TPH2<TP2
        SDEF TPINC      TP<TP+1 TP2<TP2+1
        SDEF TPIND      TP<TP+D5 TP2<TP2+D6
        SDEF R2         C10

//;###########[ For 400Mbps Test ] ##################################################
//;==========================M1 W C0 C1 CE1 CE2 CE3 CE4 CLE ALE WE  RE  ========================
//;==========================           C2  C3  C4  C5  C6  C7  C8      ========================
        SDEF CEBH       M1
        SDEF NOOP       M1         C2
        SDEF CMDI       M1 W C0    C2              C6      C8
        SDEF ADDI       M1 W       C2                  C7  C8
        SDEF WPRE       M1         C2
        SDEF WRIT       M1 W C0    C2
        SDEF WRITA      M1 W C0    C2                      C8
        SDEF WPST       M1         C2
        SDEF RPRE       M1         C2
        SDEF READ       M1   C0    C2
        SDEF RPST       M1   C0    C2
        SDEF FCHK       M1   C0 C1 C2
        SDEF RDQS       M1         C2
);


	SET_SUB(REG_CLEAR,

	RPT	100	TS16						; //Power Sequence Delay 100MS
	NOP		TS1		XB<0	YB<0	XC<0	YC<0	;
	NOP		TS1		D3<D3B	D4<D4B	XS<0	YS<0	;
	NOP		TS1		TP<0	TP2<0	XK<0	YK<0	;
	NOP		TS1		SA<0	D5<0	XA<0	YA<0	;
	NOP		TS1		CA<0		XD<0	YD<0	;
	NOP		TS1		VA<0		XE<0	YE<0	;
	NOP		TS1		DBMAR<0		XF<0	YF<0	;
	NOP		TS1		DCMR<0				;
	RTN		TS1						;
	);

	SET_SUB(REG_CLEAR_NO_SCAN,

	RPT	100	TS16						; //Power Sequence Delay 100MS
	NOP		TS1		XB<0	YB<0	XC<0	YC<0	;
	NOP		TS1		D3<D3B	D4<D4B	XS<0	YS<0	;
	NOP		TS1		TP<0	TP2<0	XK<0	YK<0	;
	NOP		TS1			D5<0	XA<0	YA<0	;
	NOP		TS1		CA<0		XD<0	YD<0	;
	NOP		TS1		VA<0		XE<0	YE<0	;
	NOP		TS1		DBMAR<0		XF<0	YF<0	;
	NOP		TS1		DCMR<0				;
	RTN		TS1						;
	);

	SET_SUB(DBM_LOAD_00,

        NOP      	TS15 		DBMAR<0				allscan ;
	NOP		TS15		DBMAP<DBMAR			allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);
	
	SET_SUB(DBM_LOAD_ADDR,
	
//	NOP		TS15		DBMAP<DBMAR			allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);
	
	SET_SUB(DBM_SUB_ADDR,

	NOP		TS15		DBMAP<DBMAP-DBMAR		allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);
	
	SET_SUB(DBM_ADD_ADDR,

	NOP		TS15		DBMAP<DBMAP+DBMAR		allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);

	SET_SUB(DBM_SUB_ADDR_18432,

	NOP		TS15		DBMAP<DBMAP-18432		allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);

	SET_SUB(DBM_ADD_ADDR_18432,

	NOP		TS15		DBMAP<DBMAP+18432		allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);
	
	SET_SUB(DBM_SUB_ADDR_1PAGE,

	NOP		TS15		DBMAP<DBMAP-55296		allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);

	SET_SUB(DBM_ADD_ADDR_1PAGE,

	NOP		TS15		DBMAP<DBMAP+55296		allscan ;
	NOP		TS15		DBM_ASET			allscan ;
	RPT	50	TS15						allscan ;
	RTN		TS15						allscan ;
	);

	SET_SUB(CAMRESET,

	NOP				M1				TS5  ;
        NOP				M1		       TP<0xFF  TS5  ;
        NOP				M1    W CYP1 C2			TS11 ;
        JNI7 .				M1                       	TS6  ;
	RTN				M1                       	TS5  ;
	);

	SET_SUB(CAMRESETALL,

	NOP				M1				TS5  allscan ;
        NOP				M1		       TP<0xFF  TS5  allscan ;
        NOP				M1    W CYP1 C2			TS11 allscan ;
        JNI7 .				M1                       	TS6  allscan ;
	RTN				M1                       	TS5  allscan ;
	);

	SET_SUB(CAM38H,

	NOP				M1				TS5  ;
        NOP				M1		       TP<0x38  TS5  ;
        NOP				M1    W CYP1 C2			TS11 ;
        JNI7 .				M1                       	TS6  ;
	RTN				M1                       	TS5  ;
	);

	SET_SUB(CAM38HALL,

	NOP				M1				TS5  allscan ;
        NOP				M1		       TP<0x38  TS5  allscan ;
        NOP				M1    W CYP1 C2			TS11 allscan ;
        JNI7 .				M1                       	TS6  allscan ;
	RTN				M1                       	TS5  allscan ;
	);

	SET_SUB(CAM39H,

	NOP				M1				TS5  ;
        NOP				M1		       TP<0x39  TS5  ;
        NOP				M1    W CYP1 C2			TS11 ;
        JNI7 .				M1                       	TS6  ;
	RTN				M1                       	TS5  ;
	);

	SET_SUB(CAM39HALL,

	NOP				M1				TS5  allscan ;
        NOP				M1		       TP<0x39  TS5  allscan ;
        NOP				M1    W CYP1 C2			TS11 allscan ;
        JNI7 .				M1                       	TS6  allscan ;
	RTN				M1                       	TS5  allscan ;
	);

	SET_SUB(FTMADD,

        NOP	 YS<D1B			M1		       TP<0x80  TS5  ;
        NOP				M1    W CYP1 C2			TS11 ;
        NOP      XYCHD			M1    W      C3			TS13 ;
        NOP      XYCHD			M1 M2 W      C3			TS13 ;
        NOP      XXCHD			M1    W      C3			TS13 ;
        NOP      XXCHD			M1 M2 W      C3			TS13 ;
        NOP      YYSHD			M1    W      C3			TS13 ;
        NOP				M1           			TS9  ;
	RTN				M1           			TS5  ;
	);

	SET_SUB(FTMADDALL,

        NOP	 YS<D1B			M1		       TP<0x80  TS5  allscan ;
        NOP				M1    W CYP1 C2			TS11 allscan ;
        NOP	 XYCHD			M1    W      C3			TS13 allscan ;
        NOP      XYCHD			M1 M2 W      C3			TS13 allscan ;
        NOP      XXCHD			M1    W      C3			TS13 allscan ;
        NOP      XXCHD			M1 M2 W      C3			TS13 allscan ;
        NOP      YYSHD			M1    W      C3			TS13 allscan ;
        NOP				M1           			TS9  allscan ;
	RTN				M1           			TS5  allscan ;
	);

	SET_SUB(TMENTRY,

        NOP                         	M1                     TP<0x52  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0xB4  TS11 ;
        NOP                         	M1    W CYP1 C2        TP<0x53  TS11 ;
        NOP                         	M1    W CYP1 C2        TP<0xC8  TS11 ;
        NOP                         	M1    W CYP1 C2           	TS11 ;
        NOP                         	M1                        	TS5  ;

        NOP                         	M1                     TP<0x40  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;
	RTN				M1           			TS5  ;
	);

	SET_SUB(TMENTRYALL,

        NOP                         	M1                     TP<0x52  TS5  allscan ;
        NOP                         	M1    W CYP1 C2        TP<0xB4  TS11 allscan ;
        NOP                         	M1    W CYP1 C2        TP<0x53  TS11 allscan ;
        NOP                         	M1    W CYP1 C2        TP<0xC8  TS11 allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
        NOP                         	M1                        	TS5  allscan ;

        NOP                         	M1                     TP<0x40  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	RTN				M1           			TS5  allscan ;
	);

	SET_SUB(TMCSCALL,

        NOP                         	M1                     TP<0x50  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	//-------------- Erase max pulse condition ----------------------------------
	NOP                         	M1                     TP<0x2C  TS8  allscan ; // TM_2C
	NOP                         	M1    W CYP1 C3        TP<0x50  TS13 allscan ; // 50:B10
	NOP                         	M1    W CYP1              	TS14 allscan ;
	NOP                         	M1                     TP<0x80  TS8  allscan ; // B10x80<7:4>
	NOP                         	M1    W CYP1 C3        TP<0x10  TS13 allscan ; // ERAPLSLOOP_SLC 1pulse
	NOP                         	M1    W CYP1              	TS14 allscan ;
	NOP                         	M1                     TP<0x81  TS8  allscan ; // B10x81<7:4>
	NOP                         	M1    W CYP1 C3        TP<0x10  TS13 allscan ; // ERAPLSLOOP_TLC 1pulse
	NOP                         	M1    W CYP1              	TS14 allscan ;
	//-------------- Erase CSC BIT condition ------------------------------------
	NOP                         	M1                     TP<0xD0  TS8  allscan ; // B10xD0<4:0>
	NOP                         	M1    W CYP1 C3        TP<0x08  TS13 allscan ; // ERAPLSCSCFAILBIT_SLC 8bit
	NOP                         	M1    W CYP1              	TS14 allscan ;
	NOP                         	M1                     TP<0xD1  TS8  allscan ; // ZxA3<4:0>
	NOP                         	M1    W CYP1 C3        TP<0x08  TS13 allscan ; // ERAPLSCSCFAILBIT_TLC 8bit
	NOP                         	M1    W CYP1              	TS14 allscan ;
	//-------------- Program CSC BIT condition ----------------------------------
	NOP                         	M1                     TP<0x2C  TS8  allscan ; // TM_2C
	NOP                         	M1    W CYP1 C3        TP<0x40  TS13 allscan ; // 40:B8
	NOP                         	M1    W CYP1              	TS14 allscan ;
	NOP                         	M1                     TP<0xA0  TS8  allscan ; // B8xA0<7:0>
	NOP                         	M1    W CYP1 C3        TP<0x1B  TS13 allscan ; // CSCBIT_PGM_SLC 27bit
	NOP                         	M1    W CYP1              	TS14 allscan ;
	NOP                         	M1                     TP<0xA1  TS8  allscan ; // B8xA1<7:0>
	NOP                         	M1    W CYP1 C3        TP<0x1B  TS13 allscan ; // CSCBIT_PGM_TLC 27bit
	NOP                         	M1    W CYP1              	TS14 allscan ;
	//---------------------------------------------------------------------------
        NOP                         	M1                     TP<0x2C  TS8  allscan ; // TM_2C
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 allscan ; // 00:B0
        NOP                         	M1    W CYP1              	TS14 allscan ;

        NOP                         	M1                     TP<0x40  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	RTN				M1           			TS5  allscan ;
	);

	SET_SUB(ECCBIT,

        NOP                         	M1                     TP<0x50  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;

        ///------------ FBC Chunk Size & M2C Disable
        NOP                         	M1                     TP<0xC5  TS8  ; // B0xC5
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 ; // CNF_RD_M2CEN<4> : Diable
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xF6  TS8  ; // B0xF6
        NOP                         	M1    W CYP1 C3        TP<0x90  TS13 ; // CNF_FBC_BIT<10:3>
        NOP                         	M1    W CYP1              	TS14 ; // CNF_FBC_BIT<14:3> = h'090 = 1K chunk (h'090/120/240/480/900 = 1/2/4/8/16K chunk)
        NOP                         	M1                     TP<0xF7  TS8  ; // B0xF7
        NOP                         	M1    W CYP1 C3        TP<0x60  TS13 ; // CNF_FBC_BIT<14:11>
        NOP                         	M1    W CYP1              	TS14 ;
        ///------------ FBC Bit (ECC)
        NOP                         	M1                     TP<0xFD  TS8  ; // B0xFD
        NOP                         	M1    W CYP1 C3        TP<0x3C  TS13 ; // CNF_FAILBIT<7:0>
        NOP                         	M1    W CYP1              	TS14 ; // ECC 60bit
        NOP                         	M1                     TP<0xFE  TS8  ; // B0xFE
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 ; // CNF_FAILBIT<15:8>
        NOP                         	M1    W CYP1              	TS14 ;
        ///------------ Fast FBC Enable
        NOP                         	M1                     TP<0x01  TS8  ; // TM_01
        NOP                         	M1    W CYP1 C3        TP<0x20  TS13 ; // FAST_FBC_OSCEN
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xE4  TS8  ; // B0xE4
        NOP                         	M1    W CYP1 C3        TP<0x08  TS13 ; // CNF_GDLLOADIMIT<3>
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xFF  TS8  ; // B0xFF
        NOP                         	M1    W CYP1 C3        TP<0x30  TS13 ; // CNF_FBC_CK_SEL<3:2> : 25ns Fast FBC
        NOP                         	M1    W CYP1              	TS14 ;

        NOP                         	M1                     TP<0x40  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;
	RTN                         	M1                        	TS5  ; 
	);

	SET_SUB(ECCBITALL,

        NOP                         	M1                     TP<0x50  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;

        ///------------ FBC Chunk Size & M2C Disable
        NOP                         	M1                     TP<0xC5  TS8  allscan ; // B0xC5
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 allscan ; // CNF_RD_M2CEN<4> : Diable
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xF6  TS8  allscan ; // B0xF6
        NOP                         	M1    W CYP1 C3        TP<0x90  TS13 allscan ; // CNF_FBC_BIT<10:3>
        NOP                         	M1    W CYP1              	TS14 allscan ; // CNF_FBC_BIT<14:3> = h'090 = 1K chunk (h'090/120/240/480/900 = 1/2/4/8/16K chunk)
        NOP                         	M1                     TP<0xF7  TS8  allscan ; // B0xF7
        NOP                         	M1    W CYP1 C3        TP<0x60  TS13 allscan ; // CNF_FBC_BIT<14:11>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        ///------------ FBC Bit (ECC)
        NOP                         	M1                     TP<0xFD  TS8  allscan ; // B0xFD
        NOP                         	M1    W CYP1 C3        TP<0x3C  TS13 allscan ; // CNF_FAILBIT<7:0>
        NOP                         	M1    W CYP1              	TS14 allscan ; // ECC 60bit
        NOP                         	M1                     TP<0xFE  TS8  allscan ; // B0xFE
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 allscan ; // CNF_FAILBIT<15:8>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        ///------------ Fast FBC Enable
        NOP                         	M1                     TP<0x01  TS8  allscan ; // TM_01
        NOP                         	M1    W CYP1 C3        TP<0x20  TS13 allscan ; // FAST_FBC_OSCEN
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xE4  TS8  allscan ; // B0xE4
        NOP                         	M1    W CYP1 C3        TP<0x08  TS13 allscan ; // CNF_GDLLOADIMIT<3>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xFF  TS8  allscan ; // B0xFF
        NOP                         	M1    W CYP1 C3        TP<0x30  TS13 allscan ; // CNF_FBC_CK_SEL<3:2> : 25ns Fast FBC
        NOP                         	M1    W CYP1              	TS14 allscan ;

        NOP                         	M1                     TP<0x40  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	RTN                         	M1                        	TS5  allscan ; 
	);

	SET_SUB(SHORTBLKRR,

        NOP                         	M1                     TP<0xA9  TS11 ; //B1xA9
        NOP                         	M1    W CYP1 C3           	TS13 ; //R1
        NOP                         	M1                     TP<0x00  TS9  ; //+00mV
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xAA  TS11 ; //B1xAA
        NOP                         	M1    W CYP1 C3           	TS13 ; //R2
        NOP                         	M1                     TP<0x01  TS9  ; //+20mV
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xAB  TS11 ; //B1xAB
        NOP                         	M1    W CYP1 C3           	TS13 ; //R3
        NOP                         	M1                     TP<0x03  TS9  ; //+60mV
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xAC  TS11 ; //B1xAC
        NOP                         	M1    W CYP1 C3           	TS13 ; //R4
        NOP                         	M1                     TP<0x03  TS9  ; //+60mV
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xAD  TS11 ; //B1xAD
        NOP                         	M1    W CYP1 C3           	TS13 ; //R5
        NOP                         	M1                     TP<0x04  TS9  ; //+80mV
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xAE  TS11 ; //B1xAE
        NOP                         	M1    W CYP1 C3           	TS13 ; //R6
        NOP                         	M1                     TP<0x04  TS9  ; //+80mV
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xAF  TS11 ; //B1xAF
        NOP                         	M1    W CYP1 C3           	TS13 ; //R7
        NOP                         	M1                     TP<0x03  TS9  ; //+60mV
        NOP                         	M1    W CYP1              	TS14 ;
	RTN                         	M1                        	TS5  ; 
	);

	SET_SUB(SHORTBLKRR_ALL,

        NOP                         	M1                     TP<0xA9  TS11 allscan ; //B1xA9
        NOP                         	M1    W CYP1 C3           	TS13 allscan ; //R1
        NOP                         	M1                     TP<0x00  TS9  allscan ; //+00mV
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xAA  TS11 allscan ; //B1xAA
        NOP                         	M1    W CYP1 C3           	TS13 allscan ; //R2
        NOP                         	M1                     TP<0x01  TS9  allscan ; //+20mV
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xAB  TS11 allscan ; //B1xAB
        NOP                         	M1    W CYP1 C3           	TS13 allscan ; //R3
        NOP                         	M1                     TP<0x03  TS9  allscan ; //+60mV
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xAC  TS11 allscan ; //B1xAC
        NOP                         	M1    W CYP1 C3           	TS13 allscan ; //R4
        NOP                         	M1                     TP<0x03  TS9  allscan ; //+60mV
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xAD  TS11 allscan ; //B1xAD
        NOP                         	M1    W CYP1 C3           	TS13 allscan ; //R5
        NOP                         	M1                     TP<0x04  TS9  allscan ; //+80mV
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xAE  TS11 allscan ; //B1xAE
        NOP                         	M1    W CYP1 C3           	TS13 allscan ; //R6
        NOP                         	M1                     TP<0x04  TS9  allscan ; //+80mV
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xAF  TS11 allscan ; //B1xAF
        NOP                         	M1    W CYP1 C3           	TS13 allscan ; //R7
        NOP                         	M1                     TP<0x03  TS9  allscan ; //+60mV
        NOP                         	M1    W CYP1              	TS14 allscan ;
	RTN                         	M1                        	TS5  allscan ; 
	);

	SET_SUB(RRTABLE,

	//-------------- RRT for short function test -----------------------
	NOP                         	M1                     TP<0x36  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x40  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                     TP<0x03  TS9  ; //B1
        NOP                         	M1    W CYP1              	TS14 ;

	JSR SHORTBLKRR			M1				TS5  ;

        NOP                         	M1                     TP<0x40  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                     TP<0x0E  TS9  ; //B12
        NOP                         	M1    W CYP1              	TS14 ;

	JSR SHORTBLKRR			M1				TS5  ;

        NOP                         	M1                     TP<0x40  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                     TP<0x10  TS9  ; //B14
        NOP                         	M1    W CYP1              	TS14 ;

	JSR SHORTBLKRR			M1				TS5  ;

        NOP                         	M1                     TP<0x40  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                     TP<0x12  TS9  ; //B16
        NOP                         	M1    W CYP1              	TS14 ;

	JSR SHORTBLKRR			M1				TS5  ;

        NOP                         	M1                     TP<0x16  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;
        NOP                         	M1                        	TS6  ;
	RTN                         	M1                        	TS5  ; 
	);

	SET_SUB(RRTABLEALL,

	//-------------- RRT for short function test -----------------------
	NOP                         	M1                     TP<0x36  TS5  allscan ;
        NOP                         	M1    W CYP1 C2        TP<0x40  TS11 allscan ;
        NOP                         	M1    W CYP1 C3           	TS13 allscan ;
        NOP                         	M1                     TP<0x03  TS9  allscan ; //B1
        NOP                         	M1    W CYP1              	TS14 allscan ;

	JSR SHORTBLKRR_ALL		M1				TS5  allscan ;

        NOP                         	M1                     TP<0x40  TS11 allscan ;
        NOP                         	M1    W CYP1 C3           	TS13 allscan ;
        NOP                         	M1                     TP<0x0E  TS9  allscan ; //B12
        NOP                         	M1    W CYP1              	TS14 allscan ;

	JSR SHORTBLKRR_ALL		M1				TS5  allscan ;

        NOP                         	M1                     TP<0x40  TS11 allscan ;
        NOP                         	M1    W CYP1 C3           	TS13 allscan ;
        NOP                         	M1                     TP<0x10  TS9  allscan ; //B14
        NOP                         	M1    W CYP1              	TS14 allscan ;

	JSR SHORTBLKRR_ALL		M1				TS5  allscan ;

        NOP                         	M1                     TP<0x40  TS11 allscan ;
        NOP                         	M1    W CYP1 C3           	TS13 allscan ;
        NOP                         	M1                     TP<0x12  TS9  allscan ; //B16
        NOP                         	M1    W CYP1              	TS14 allscan ;

	JSR SHORTBLKRR_ALL		M1				TS5  allscan ;

        NOP                         	M1                     TP<0x16  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
        NOP                         	M1                        	TS6  allscan ;
	RTN                         	M1                        	TS5  allscan ; 

	);

	SET_SUB(ECCBITFUN,

        NOP                         	M1                     TP<0x50  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;

        ///------------ FBC Chunk Size & M2C Disable
        NOP                         	M1                     TP<0xC5  TS8  ; // B0xC5
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 ; // CNF_RD_M2CEN<4> : Diable
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xF6  TS8  ; // B0xF6
        NOP                         	M1    W CYP1 C3        TP<0x90  TS13 ; // CNF_FBC_BIT<10:3>
        NOP                         	M1    W CYP1              	TS14 ; // CNF_FBC_BIT<14:3> = h'090 = 1K chunk (h'090/120/240/480/900 = 1/2/4/8/16K chunk)
        NOP                         	M1                     TP<0xF7  TS8  ; // B0xF7
        NOP                         	M1    W CYP1 C3        TP<0x60  TS13 ; // CNF_FBC_BIT<14:11>
        NOP                         	M1    W CYP1              	TS14 ;
        ///------------ FBC Bit (ECC)
        NOP                         	M1                     TP<0xFD  TS8  ; // B0xFD
        NOP                         	M1    W CYP1 C3        TP<0x46  TS13 ; // CNF_FAILBIT<7:0>
        NOP                         	M1    W CYP1              	TS14 ; // ECC 70bit
        NOP                         	M1                     TP<0xFE  TS8  ; // B0xFE
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 ; // CNF_FAILBIT<15:8>
        NOP                         	M1    W CYP1              	TS14 ;
        ///------------ Fast FBC Enable
        NOP                         	M1                     TP<0x01  TS8  ; // TM_01
        NOP                         	M1    W CYP1 C3        TP<0x20  TS13 ; // FAST_FBC_OSCEN
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xE4  TS8  ; // B0xE4
        NOP                         	M1    W CYP1 C3        TP<0x08  TS13 ; // CNF_GDLLOADIMIT<3>
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xFF  TS8  ; // B0xFF
        NOP                         	M1    W CYP1 C3        TP<0x30  TS13 ; // CNF_FBC_CK_SEL<3:2> : 25ns Fast FBC
        NOP                         	M1    W CYP1              	TS14 ;

        NOP                         	M1                     TP<0x40  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;
	RTN                         	M1                        	TS5  ; 
	);

	SET_SUB(ECCBITFUNALL,

        NOP                         	M1                     TP<0x50  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;

        ///------------ FBC Chunk Size & M2C Disable
        NOP                         	M1                     TP<0xC5  TS8  allscan ; // B0xC5
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 allscan ; // CNF_RD_M2CEN<4> : Diable
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xF6  TS8  allscan ; // B0xF6
        NOP                         	M1    W CYP1 C3        TP<0x90  TS13 allscan ; // CNF_FBC_BIT<10:3>
        NOP                         	M1    W CYP1              	TS14 allscan ; // CNF_FBC_BIT<14:3> = h'090 = 1K chunk (h'090/120/240/480/900 = 1/2/4/8/16K chunk)
        NOP                         	M1                     TP<0xF7  TS8  allscan ; // B0xF7
        NOP                         	M1    W CYP1 C3        TP<0x60  TS13 allscan ; // CNF_FBC_BIT<14:11>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        ///------------ FBC Bit (ECC)
        NOP                         	M1                     TP<0xFD  TS8  allscan ; // B0xFD
        NOP                         	M1    W CYP1 C3        TP<0x46  TS13 allscan ; // CNF_FAILBIT<7:0>
        NOP                         	M1    W CYP1              	TS14 allscan ; // ECC 70bit
        NOP                         	M1                     TP<0xFE  TS8  allscan ; // B0xFE
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 allscan ; // CNF_FAILBIT<15:8>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        ///------------ Fast FBC Enable
        NOP                         	M1                     TP<0x01  TS8  allscan ; // TM_01
        NOP                         	M1    W CYP1 C3        TP<0x20  TS13 allscan ; // FAST_FBC_OSCEN
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xE4  TS8  allscan ; // B0xE4
        NOP                         	M1    W CYP1 C3        TP<0x08  TS13 allscan ; // CNF_GDLLOADIMIT<3>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xFF  TS8  allscan ; // B0xFF
        NOP                         	M1    W CYP1 C3        TP<0x30  TS13 allscan ; // CNF_FBC_CK_SEL<3:2> : 25ns Fast FBC
        NOP                         	M1    W CYP1              	TS14 allscan ;

        NOP                         	M1                     TP<0x40  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	RTN                         	M1                        	TS5  allscan ; 
	);

	SET_SUB(ECCBITSLC,

        NOP                         	M1                     TP<0x50  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;

        ///------------ FBC Chunk Size & M2C Disable
        NOP                         	M1                     TP<0xC5  TS8  ; // B0xC5
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 ; // CNF_RD_M2CEN<4> : Diable
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xF6  TS8  ; // B0xF6
        NOP                         	M1    W CYP1 C3        TP<0x90  TS13 ; // CNF_FBC_BIT<10:3>
        NOP                         	M1    W CYP1              	TS14 ; // CNF_FBC_BIT<14:3> = h'090 = 1K chunk (h'090/120/240/480/900 = 1/2/4/8/16K chunk)
        NOP                         	M1                     TP<0xF7  TS8  ; // B0xF7
        NOP                         	M1    W CYP1 C3        TP<0x60  TS13 ; // CNF_FBC_BIT<14:11>
        NOP                         	M1    W CYP1              	TS14 ;
        ///------------ FBC Bit (ECC)
        NOP                         	M1                     TP<0xFD  TS8  ; // B0xFD
        NOP                         	M1    W CYP1 C3        TP<0x1E  TS13 ; // CNF_FAILBIT<7:0>
        NOP                         	M1    W CYP1              	TS14 ; // ECC 30bit
        NOP                         	M1                     TP<0xFE  TS8  ; // B0xFE
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 ; // CNF_FAILBIT<15:8>
        NOP                         	M1    W CYP1              	TS14 ;
        ///------------ Fast FBC Enable
        NOP                         	M1                     TP<0x01  TS8  ; // TM_01
        NOP                         	M1    W CYP1 C3        TP<0x20  TS13 ; // FAST_FBC_OSCEN
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xE4  TS8  ; // B0xE4
        NOP                         	M1    W CYP1 C3        TP<0x08  TS13 ; // CNF_GDLLOADIMIT<3>
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                         	M1                     TP<0xFF  TS8  ; // B0xFF
        NOP                         	M1    W CYP1 C3        TP<0x30  TS13 ; // CNF_FBC_CK_SEL<3:2> : 25ns Fast FBC
        NOP                         	M1    W CYP1              	TS14 ;

        NOP                         	M1                     TP<0x40  TS5  ;
        NOP                         	M1    W CYP1 C2           	TS11 ;
	RTN                         	M1                        	TS5  ; 
	);

	SET_SUB(ECCBITSLCALL,

        NOP                         	M1                     TP<0x50  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;

        ///------------ FBC Chunk Size & M2C Disable
        NOP                         	M1                     TP<0xC5  TS8  allscan ; // B0xC5
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 allscan ; // CNF_RD_M2CEN<4> : Diable
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xF6  TS8  allscan ; // B0xF6
        NOP                         	M1    W CYP1 C3        TP<0x90  TS13 allscan ; // CNF_FBC_BIT<10:3>
        NOP                         	M1    W CYP1              	TS14 allscan ; // CNF_FBC_BIT<14:3> = h'090 = 1K chunk (h'090/120/240/480/900 = 1/2/4/8/16K chunk)
        NOP                         	M1                     TP<0xF7  TS8  allscan ; // B0xF7
        NOP                         	M1    W CYP1 C3        TP<0x60  TS13 allscan ; // CNF_FBC_BIT<14:11>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        ///------------ FBC Bit (ECC)
        NOP                         	M1                     TP<0xFD  TS8  allscan ; // B0xFD
        NOP                         	M1    W CYP1 C3        TP<0x1E  TS13 allscan ; // CNF_FAILBIT<7:0>
        NOP                         	M1    W CYP1              	TS14 allscan ; // ECC 30bit
        NOP                         	M1                     TP<0xFE  TS8  allscan ; // B0xFE
        NOP                         	M1    W CYP1 C3        TP<0x00  TS13 allscan ; // CNF_FAILBIT<15:8>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        ///------------ Fast FBC Enable
        NOP                         	M1                     TP<0x01  TS8  allscan ; // TM_01
        NOP                         	M1    W CYP1 C3        TP<0x20  TS13 allscan ; // FAST_FBC_OSCEN
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xE4  TS8  allscan ; // B0xE4
        NOP                         	M1    W CYP1 C3        TP<0x08  TS13 allscan ; // CNF_GDLLOADIMIT<3>
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                         	M1                     TP<0xFF  TS8  allscan ; // B0xFF
        NOP                         	M1    W CYP1 C3        TP<0x30  TS13 allscan ; // CNF_FBC_CK_SEL<3:2> : 25ns Fast FBC
        NOP                         	M1    W CYP1              	TS14 allscan ;

        NOP                         	M1                     TP<0x40  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	RTN                         	M1                        	TS5  allscan ; 
	);

	SET_SUB(SETFEAT,

        NOP                         	M1                     TP<0xEF  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x01  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                        	TS9  ;

        NOP                         	M1                     TP<0x00  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
	RTN                         	M1                        	TS5  ;
	);

	SET_SUB(SETFEATALL,

        NOP                         	M1                     TP<0xEF  TS5  allscan ;
        NOP                         	M1    W CYP1 C2        TP<0x01  TS11 allscan ;
        NOP                         	M1    W CYP1 C3           	TS13 allscan ;
        NOP                         	M1                        	TS9  allscan ;

        NOP                         	M1                     TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
        NOP	                      	M1                        	TS14 allscan ;
	RTN                         	M1                        	TS5  allscan ;
	);

	SET_SUB(SETFEAT_VPP,	// VPP OPTION ENABLE

        NOP                         	M1                     TP<0xEF  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x30  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                        	TS9  ;

        NOP                         	M1                     TP<0x01  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
	RTN                         	M1                        	TS5  ;
	);

	SET_SUB(SETFEAT_VPPALL, // VPP OPTION ENABLE

        NOP                         	M1                     TP<0xEF  TS5  allscan ;
        NOP                         	M1    W CYP1 C2        TP<0x30  TS11 allscan ;
        NOP                         	M1    W CYP1 C3           	TS13 allscan ;
        NOP                         	M1                        	TS9  allscan ;

        NOP                         	M1                     TP<0x01  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
	RTN                         	M1                        	TS5  allscan ;
	);

	SET_SUB(SETFEAT_PI, // PLANE INTERLEAVE ENABLE

	NOP                         	M1                     TP<0xEF  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x87  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                        	TS9  ;

        NOP                         	M1                     TP<0x01  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1              	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
        NOP                      	M1                        	TS14 ;
	RTN				M1                       	TS5  ;
	);

	SET_SUB(SETFEAT_PIALL, // PLANE INTERLEAVE ENABLE

	NOP                         	M1                     TP<0xEF  TS5  allscan ;
        NOP                         	M1    W CYP1 C2        TP<0x87  TS11 allscan ;
        NOP                         	M1    W CYP1 C3           	TS13 allscan ;
        NOP                         	M1                        	TS9  allscan ;

        NOP                         	M1                     TP<0x01  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1           TP<0x00  TS14 allscan ;
        NOP                         	M1    W CYP1              	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
        NOP                      	M1                        	TS14 allscan ;
	RTN				M1                       	TS5  allscan ;
	);

	SET_SUB(SETFEAT_FR, // FAST READ ENABLE

 	NOP      	           	M1                     TP<0xEF  TS5  ;
	NOP				M1    W CYP1 C2        TP<0xF5  TS11 ;
	NOP				M1    W CYP1 C2                 TS13 ;
	NOP				M1				TS9  ;

	NOP				M1		       TP<0x02  TS14 ;
	NOP				M1    W CYP1 C2        TP<0x00  TS14 ;
	NOP				M1    W CYP1 C2        TP<0x00  TS14 ;
	NOP				M1    W CYP1 C2        TP<0x00  TS14 ;
	NOP				M1    W CYP1 C2                 TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	NOP				M1				TS14 ;
	RTN				M1				TS5  ;
	);

	SET_SUB(SETFEAT_FRALL, // FAST READ ENABLE

 	NOP      	           	M1                     TP<0xEF  TS5  allscan ;
	NOP				M1    W CYP1 C2        TP<0xF5  TS11 allscan ;
	NOP				M1    W CYP1 C2                 TS13 allscan ;
	NOP				M1				TS9  allscan ;

	NOP				M1		       TP<0x02  TS14 allscan ;
	NOP				M1    W CYP1 C2        TP<0x00  TS14 allscan ;
	NOP				M1    W CYP1 C2        TP<0x00  TS14 allscan ;
	NOP				M1    W CYP1 C2        TP<0x00  TS14 allscan ;
	NOP				M1    W CYP1 C2                 TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	NOP				M1				TS14 allscan ;
	RTN				M1				TS5  allscan ;
	);

	SET_SUB(TM_RESUME, // FAST READ ENABLE

        NOP                         	M1                     TP<0x50  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	RTN				M1				TS5  allscan ;
	);

	SET_SUB(TM_SUSPEND, // FAST READ ENABLE

        NOP                         	M1                     TP<0x40  TS5  allscan ;
        NOP                         	M1    W CYP1 C2           	TS11 allscan ;
	RTN				M1				TS5  allscan ;
	);
}

PATTERN_GROUP( PFCT1F2A )
{
	printf("name = %s\n", name );
	device_default(name);
	SET_SUB(SETFE,

//---------------------------------------------------------------------------------- SET Feature
	NOP                         	M1                     TP<0xEF  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x01  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                        	TS6  ; 

        NOP                         	M1                     TP<0x05  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1              	TS14 ;
        JNI5 .                      	M1                        	TS14 ;

        NOP                         	M1                     TP<0xEF  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x80  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                        	TS6  ; 

        NOP                         	M1                     TP<0x00  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x03  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1              	TS14 ;
        JNI5 .                      	M1                        	TS14 ;

	NOP                         	M1                     TP<0xEF  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x83  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                        	TS6  ;

        NOP                         	M1                     TP<0x8F  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x8F  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x01  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1              	TS14 ;
        JNI5 .                      	M1                        	TS14 ;

	NOP                         	M1                     TP<0xEF  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0xF5  TS11 ;
        NOP                         	M1    W CYP1 C3           	TS13 ;
        NOP                         	M1                        	TS6  ;

        NOP                         	M1                     TP<0x06  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ; 
        NOP                         	M1    W CYP1           TP<0x00  TS14 ;
        NOP                         	M1    W CYP1              	TS14 ;
        JNI5 .                      	M1                        	TS14 ;

//---------------------------------------------------------------------------------- GET Feature
        NOP                         	M1                     TP<0xEE  TS5  ;
        NOP                         	M1    W CYP1 C2        TP<0x01  TS1  ;
        NOP                         	M1    W CYP1 C3           	TS2  ;
        JNI5 .                      	M1                        	TS14 ;

        NOP                         	M1                     TP<0x05  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1              	TS5  ;

        NOP                         	M1                     TP<0xEE  TS15 ;
        NOP                         	M1    W CYP1 C2        TP<0x80  TS1  ;
        NOP                         	M1    W CYP1 C3           	TS2  ;
        JNI5 .                      	M1                        	TS14 ;

        NOP                         	M1                     TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x03  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1              	TS5  ;

        NOP                         	M1                     TP<0xEE  TS15 ;
        NOP                         	M1    W CYP1 C2        TP<0x83  TS1  ;
        NOP                         	M1    W CYP1 C3           	TS2  ;
        JNI5 .                      	M1                        	TS14 ;

        NOP                         	M1                     TP<0x8F  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x8F  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x01  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1              	TS5  ;

        NOP                         	M1                     TP<0xEE  TS15 ;
        NOP                         	M1    W CYP1 C2        TP<0xF5  TS1  ;
        NOP                         	M1    W CYP1 C3           	TS2  ;
        JNI5 .                      	M1                        	TS14 ;

        NOP                         	M1                     TP<0x06  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1           TP<0x00  TS5  ;
        NOP                         	M1                        	TS5  ;
        NOP	 COUT XCINC         	M1    R CYP1              	TS5  ;
        RTN                         	M1                        	TS5  ;
	);
	
}

STEP( TEST1 , "TEST1" )
{
	printf("============ STEP TEST1 =============\n");
	IDX0 = 0xff;
	IDX2 = 0xff;
	XH   = 0xffff;
	PRINTF_REG	();
	printf("\n");
	
	IDX0 = 0xff;
	IDX5 = 0xff;
	YH   = 0xffff;
	PRINTF_REG	();
	printf("\n");

	LMAX=0x3ffff;
	HMAX=0x3ffff;
	ZMAX=0x3ffff;
	XMAX=0x3ffff;
	YMAX=0x3ffff;
	PRINTF_REG	();
	printf("\n");

	LOAD(REG,CON);
	LOAD(CYP,CYP1);
	LOAD(MODULE,NORMAL);
	LOAD(VOL,LEVCP_S96_152);
	SEND_MPAT( PFCT1F2A );
}

STEP( TEST2 , "TEST2" )
{
	printf("============ STEP TEST2 =============\n");
	LOAD(REG,CON);
	LOAD(CYP,CYP1);
	LOAD(MODULE,NORMAL);
	LOAD(VOL,LEVCP_S96_152);
	SEND_MPAT( PFCT1F2A );
}

//CONTACT TEST
void initTest()
{
    std::cout << __FUNCTION__ << "The current thread is " << QThread::currentThread() <<std::endl;
}

//TEST ENTRY 작성
void testEntry()
{
    std::cout << __FUNCTION__ << std::endl;
	
	STEP_RUN(TEST1);
	STEP_RUN(TEST2);

}

END_DEVICE()
