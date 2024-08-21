#ifndef CSYSTYPE_HPP
#define CSYSTYPE_HPP

#include <ostream>
#include "atstring.hpp"
#include "sysdef.hpp"
#include "sysver.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
/// Function Macro Define
/////////////////////////////////////////////////////////////////////////////////////////////
#define STR(x)              #x
#define REG(a)              void REG_##a()
#define CYP(a)              void CYP_##a()                              \
							{                                           \
								g_pg.InitCyp();                         \
								_CYP_##a();                             \
							}                                           \
							void _CYP_##a()

#define VOL(a)              void VOL_##a()
#define MODULE(a)           void MODULE_##a()
#define LOAD(a,b)           do{ a##_##b(); } while(0)
#define SET_DEFINE(a)       do{ g_pg.SetDefine(__LINE__, #a); } while(0)
#define SET_SUB(a,b)        do{ g_pg.SetSub(__LINE__, #a, #b); } while(0)
#define PATTERN_GROUP(a)    void PATTERN_GROUP_##a()                    \
                            {                                           \
								g_pg.ClearPattern();                    \
								g_pg.SetCurrentPattern(#a);				\
								g_pg.SendDefineFlag();   				\
								PATTERN_##a();                          \
                            }                                           \
							void PATTERN_##a(const char* name=#a)

#define SEND_MPAT(a)        do{ PATTERN_GROUP_##a(); } while(0)

#define STEP(a,b)           void STEP_##a()                                         \
							{                                                       \
								if( !IS_TEST(b) ) return;							\
								STEP_BEGIN();										\
								STEP_RUN_##a(b);                                    \
								STEP_END();  										\
							}                                                       \
							void STEP_RUN_##a(const char* ITEM_NAME)                \

#define STEP_RUN(a)         do{ STEP_##a(); } while(0)
#define SI(scan,group)		(((scan<<20)|(group)<<12))

//2023.04.03 - kwmoon
#define STOP_RETURN			if( !GET_RUNMODE() ) return;
#define STOP_RETURN_FALSE	if( !GET_RUNMODE() ) return false;
#define STOP_RETURN_TRUE	if( !GET_RUNMODE() ) return true;
#define STOP_BREAK			if( !GET_RUNMODE() ) break;

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GLOBAL REGISTER & SET/DSET
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IDX0                g_pg._IDX0
#define IDX1                g_pg._IDX1
#define IDX2                g_pg._IDX2
#define IDX3                g_pg._IDX3
#define IDX4                g_pg._IDX4
#define IDX5                g_pg._IDX5
#define IDX6                g_pg._IDX6
#define IDX7                g_pg._IDX7
#define IDX8                g_pg._IDX8
#define IDX9                g_pg._IDX9
#define IDX10               g_pg._IDX10
#define IDX11               g_pg._IDX11
#define IDX12               g_pg._IDX12
#define IDX13               g_pg._IDX13
#define IDX14               g_pg._IDX14
#define IDX15               g_pg._IDX15
//#define IDX16               g_pg._IDX16
#define CFLAG               g_pg._CFLAG
#define ISP                 g_pg._ISP
#define BAR                 g_pg._BAR
#define MLOOP               g_pg._MLOOP

#define D1B                 g_pg._D1B
#define D1C                 g_pg._D1C
#define D1D                 g_pg._D1D
#define D1E                 g_pg._D1E
#define D1F                 g_pg._D1F
#define D1G                 g_pg._D1G
#define D1H                 g_pg._D1H

#define D2B                 g_pg._D2B
#define D2C                 g_pg._D2C
#define D2D                 g_pg._D2D

#define LMAX                g_pg._LMAX
#define HMAX                g_pg._HMAX
#define XMAX                g_pg._XMAX
#define YMAX                g_pg._YMAX
#define ZMAX                g_pg._ZMAX

#define ND2					g_pg._ND2
#define ND3					g_pg._ND3
#define ND4					g_pg._ND4
#define ND5					g_pg._ND5
#define ND6					g_pg._ND6
#define ND7					g_pg._ND7
#define ND8					g_pg._ND8
#define ND9					g_pg._ND9
#define ND10				g_pg._ND10
#define ND11				g_pg._ND11
#define ND12				g_pg._ND12
#define ND13				g_pg._ND13
#define ND14				g_pg._ND14
#define ND15				g_pg._ND15
#define ND16				g_pg._ND16
#define ND17				g_pg._ND17
#define ND18				g_pg._ND18
#define ND19				g_pg._ND19
#define ND20				g_pg._ND20
#define ND21				g_pg._ND21
#define ND22				g_pg._ND22
#define ND23				g_pg._ND23
#define ND24				g_pg._ND24
#define ND25				g_pg._ND25
#define ND26				g_pg._ND26
#define ND27				g_pg._ND27
#define ND28				g_pg._ND28
#define ND29				g_pg._ND29
#define ND30				g_pg._ND30
#define ND31				g_pg._ND31
#define ND32				g_pg._ND32

#define BD2					g_pg._BD2
#define BD3					g_pg._BD3
#define BD4					g_pg._BD4
#define BD5					g_pg._BD5
#define BD6					g_pg._BD6
#define BD7					g_pg._BD7
#define BD8					g_pg._BD8
#define BD9					g_pg._BD9
#define BD10				g_pg._BD10
#define BD11				g_pg._BD11
#define BD12				g_pg._BD12
#define BD13				g_pg._BD13
#define BD14				g_pg._BD14
#define BD15				g_pg._BD15
#define BD16				g_pg._BD16
#define BD17				g_pg._BD17
#define BD18				g_pg._BD18
#define BD19				g_pg._BD19
#define BD20				g_pg._BD20
#define BD21				g_pg._BD21
#define BD22				g_pg._BD22
#define BD23				g_pg._BD23
#define BD24				g_pg._BD24
#define BD25				g_pg._BD25
#define BD26				g_pg._BD26
#define BD27				g_pg._BD27
#define BD28				g_pg._BD28
#define BD29				g_pg._BD29
#define BD30				g_pg._BD30
#define BD31				g_pg._BD31
#define BD32				g_pg._BD32

#define L1A					g_pg._L1A
#define L1B					g_pg._L1B
#define L1C					g_pg._L1C
#define L1D					g_pg._L1D
#define L1E					g_pg._L1E
#define L1F					g_pg._L1F
#define L1G					g_pg._L1G
#define L1H					g_pg._L1H

#define L2A					g_pg._L2A
#define L2B					g_pg._L2B
#define L2C					g_pg._L2C
#define L2D					g_pg._L2D
#define L2E					g_pg._L2E
#define L2F					g_pg._L2F
#define L2G					g_pg._L2G
#define L2H					g_pg._L2H

#define L3A					g_pg._L3A
#define L3B					g_pg._L3B
#define L3C					g_pg._L3C
#define L3D					g_pg._L3D
#define L3E					g_pg._L3E
#define L3F					g_pg._L3F
#define L3G					g_pg._L3G
#define L3H					g_pg._L3H

#define L4A					g_pg._L4A
#define L4B					g_pg._L4B
#define L4C					g_pg._L4C
#define L4D					g_pg._L4D
#define L4E					g_pg._L4E
#define L4F					g_pg._L4F
#define L4G					g_pg._L4G
#define L4H					g_pg._L4H

#define S1A					g_pg._S1A
#define V1A					g_pg._V1A

#define XH                  g_pg._XH
#define YH                  g_pg._YH
#define TP                  g_pg._TP1
#define TP1                 g_pg._TP1
#define TP2                 g_pg._TP2

//#define D1                  g_pg._D1A	//conflict CYP D0 ~ D15
#define D1A                 g_pg._D1A
#define D2A                 g_pg._D2A
#define TPH                 g_pg._TPH1
#define TPH1                g_pg._TPH1
#define TPH2                g_pg._TPH2

#define D3B                 g_pg._D3B
#define D4B                 g_pg._D4B
#define D5A                 g_pg._D5
#define D6A                 g_pg._D6

#define NH                  g_pg._NH
#define BH                  g_pg._BH
#define DCMR1               g_pg._DCMR1
#define DCMR2               g_pg._DCMR2

#define ZH                  g_pg._ZH
#define XMASK               g_pg._XMASK
#define YMASK               g_pg._YMASK

#define XOS                 g_pg._XOS
#define YOS                 g_pg._YOS
#define CBMA                g_pg._CBMA
#define DSC                 g_pg._DSC

#define XT                  g_pg._XT
#define YT                  g_pg._YT
#define RCD                 g_pg._RCD
#define CCD                 g_pg._CCD

#define ZD                  g_pg._ZD
#define Z                   g_pg._Z
#define DSD                 g_pg._DSD

#define ND                  g_pg._ND
#define BD                  g_pg._BD
#define DBMAR               g_pg._DBMAR

#define TPH1A				g_pg._TPH1A
#define TPH1B				g_pg._TPH1B
#define TPH1C				g_pg._TPH1C
#define TPH1D				g_pg._TPH1D

#define TPH2A				g_pg._TPH2A
#define TPH2B				g_pg._TPH2B
#define TPH2C				g_pg._TPH2C
#define TPH2D				g_pg._TPH2D

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PIN FORMAT/CYP
////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PD0					g_pg._PD0
#define PD1					g_pg._PD1
#define PD2					g_pg._PD2
#define PD3					g_pg._PD3
#define PD4					g_pg._PD4
#define PD5					g_pg._PD5
#define PD6					g_pg._PD6
#define PD7					g_pg._PD7
#define PD8					g_pg._PD8
#define PD9					g_pg._PD9
#define PD10				g_pg._PD10
#define PD11				g_pg._PD11
#define PD12				g_pg._PD12
#define PD13				g_pg._PD13
#define PD14				g_pg._PD14
#define PD15				g_pg._PD15
#define PD16				g_pg._PD16
#define PD17				g_pg._PD17
#define PD18				g_pg._PD18
#define PD19				g_pg._PD19
#define PD20				g_pg._PD20
#define PD21				g_pg._PD21
#define PD22				g_pg._PD22
#define PD23				g_pg._PD23
#define PD24				g_pg._PD24
#define PD25				g_pg._PD25
#define PD26				g_pg._PD26
#define PD27				g_pg._PD27
#define PD28				g_pg._PD28
#define PD29				g_pg._PD29
#define PD30				g_pg._PD30
#define PD31				g_pg._PD31

#define P0					g_pg._P0
#define P1					g_pg._P1
#define P2					g_pg._P2
#define P3					g_pg._P3
#define P4					g_pg._P4
#define P5					g_pg._P5
#define P6					g_pg._P6
#define P7					g_pg._P7
#define P8					g_pg._P8
#define P9					g_pg._P9
#define P10					g_pg._P10
#define P11					g_pg._P11
#define P12					g_pg._P12
#define P13					g_pg._P13
#define P14					g_pg._P14
#define P15					g_pg._P15

#define DREL				g_pg._DREL
#define DRET				g_pg._DRET

#define CPE0				g_pg._CPE0
#define CPE1				g_pg._CPE1
#define CPE2				g_pg._CPE2
#define CPE3				g_pg._CPE3

#define UCLK0 				g_pg._UCLK0
#define UCLK1 				g_pg._UCLK1
#define UCLK2 				g_pg._UCLK2
#define UCLK3 				g_pg._UCLK3
#define UCLK4 				g_pg._UCLK4
#define UCLK5 				g_pg._UCLK5
#define UCLK6 				g_pg._UCLK6
#define UCLK7 				g_pg._UCLK7
#define UCLK8 				g_pg._UCLK8
#define UCLK9 				g_pg._UCLK9
#define UCLK10				g_pg._UCLK10
#define UCLK11				g_pg._UCLK11
#define UCLK12				g_pg._UCLK12
#define UCLK13				g_pg._UCLK13
#define UCLK14				g_pg._UCLK14
#define UCLK15				g_pg._UCLK15
#define UCLK16				g_pg._UCLK16
#define UCLK17				g_pg._UCLK17
#define UCLK18				g_pg._UCLK18
#define UCLK19				g_pg._UCLK19
#define UCLK20				g_pg._UCLK20
#define UCLK21				g_pg._UCLK21
#define UCLK22				g_pg._UCLK22
#define UCLK23				g_pg._UCLK23
#define UCLK24				g_pg._UCLK24
#define UCLK25				g_pg._UCLK25
#define UCLK26				g_pg._UCLK26
#define UCLK27				g_pg._UCLK27
#define UCLK28				g_pg._UCLK28
#define UCLK29				g_pg._UCLK29
#define UCLK30				g_pg._UCLK30
#define UCLK31				g_pg._UCLK31
#define UCLK32				g_pg._UCLK32
#define UCLK33				g_pg._UCLK33
#define UCLK34				g_pg._UCLK34
#define UCLK35				g_pg._UCLK35
#define UCLK36				g_pg._UCLK36
#define UCLK37				g_pg._UCLK37
#define UCLK38				g_pg._UCLK38
#define UCLK39				g_pg._UCLK39
#define UCLK40				g_pg._UCLK40
#define UCLK41				g_pg._UCLK41
#define UCLK42				g_pg._UCLK42
#define UCLK43				g_pg._UCLK43
#define UCLK44				g_pg._UCLK44
#define UCLK45				g_pg._UCLK45
#define UCLK46				g_pg._UCLK46
#define UCLK47				g_pg._UCLK47
#define UCLK48				g_pg._UCLK48
#define UCLK49				g_pg._UCLK49
#define UCLK50				g_pg._UCLK50
#define UCLK51				g_pg._UCLK51
#define UCLK52				g_pg._UCLK52
#define UCLK53				g_pg._UCLK53
#define UCLK54				g_pg._UCLK54
#define UCLK55				g_pg._UCLK55
#define UCLK56				g_pg._UCLK56
#define UCLK57				g_pg._UCLK57
#define UCLK58				g_pg._UCLK58
#define UCLK59				g_pg._UCLK59
#define UCLK60				g_pg._UCLK60
#define UCLK61				g_pg._UCLK61
#define UCLK62				g_pg._UCLK62
#define UCLK63				g_pg._UCLK63
#define UCLK64				g_pg._UCLK64
#define UCLK65				g_pg._UCLK65
#define UCLK66				g_pg._UCLK66
#define UCLK67				g_pg._UCLK67
#define UCLK68				g_pg._UCLK68
#define UCLK69				g_pg._UCLK69
#define UCLK70				g_pg._UCLK70
#define UCLK71				g_pg._UCLK71
#define UCLK72				g_pg._UCLK72
#define UCLK73				g_pg._UCLK73
#define UCLK74				g_pg._UCLK74
#define UCLK75				g_pg._UCLK75
#define UCLK76				g_pg._UCLK76
#define UCLK77				g_pg._UCLK77
#define UCLK78				g_pg._UCLK78
#define UCLK79				g_pg._UCLK79
#define UCLK80				g_pg._UCLK80
#define UCLK81				g_pg._UCLK81
#define UCLK82				g_pg._UCLK82
#define UCLK83				g_pg._UCLK83
#define UCLK84				g_pg._UCLK84
#define UCLK85				g_pg._UCLK85
#define UCLK86				g_pg._UCLK86
#define UCLK87				g_pg._UCLK87

#define FMA0 				g_pg._FMA0
#define FMA1 				g_pg._FMA1
#define FMA2 				g_pg._FMA2
#define FMA3 				g_pg._FMA3
#define FMA4 				g_pg._FMA4
#define FMA5 				g_pg._FMA5
#define FMA6 				g_pg._FMA6
#define FMA7 				g_pg._FMA7
#define FMA8 				g_pg._FMA8
#define FMA9 				g_pg._FMA9
#define FMA10				g_pg._FMA10
#define FMA11				g_pg._FMA11
#define FMA12				g_pg._FMA12
#define FMA13				g_pg._FMA13
#define FMA14				g_pg._FMA14
#define FMA15				g_pg._FMA15
#define FMA16				g_pg._FMA16
#define FMA17				g_pg._FMA17
#define FMA18				g_pg._FMA18
#define FMA19				g_pg._FMA19
#define FMA20				g_pg._FMA20
#define FMA21				g_pg._FMA21
#define FMA22				g_pg._FMA22
#define FMA23				g_pg._FMA23
#define FMA24				g_pg._FMA24	//Only for compile
#define FMA25				g_pg._FMA25	//Only for compile

#define MUT0 				g_pg._MUT0
#define MUT1 				g_pg._MUT1
#define MUT2 				g_pg._MUT2
#define MUT3 				g_pg._MUT3
#define MUT4 				g_pg._MUT4
#define MUT5 				g_pg._MUT5
#define MUT6 				g_pg._MUT6
#define MUT7 				g_pg._MUT7
#define MUT8 				g_pg._MUT8
#define MUT9 				g_pg._MUT9
#define MUT10				g_pg._MUT10
#define MUT11				g_pg._MUT11
#define MUT12				g_pg._MUT12
#define MUT13				g_pg._MUT13
#define MUT14				g_pg._MUT14
#define MUT15				g_pg._MUT15


/////////////////////////////////////////
/// \brief REG / CYP
/// \details PATTERN GLOBAL REGISTER
/////////////////////////////////////////
class CGlobalSet;	//2023.06.13 - kwmoon(add fatric)
class CGlobalReg
{
public:
	CGlobalReg(GLOBAL_REG,int addr, bool bt32=false);
public:
	CGlobalReg& operator=(ll64);
	int operator=(CGlobalReg&);	//2022.12.19 - kwmoon
	int operator=(CGlobalSet&);	//2023.05.19 - fatric
	ll64 GetValue64();
	uint GetValue32();
	GLOBAL_REG GetReg();
	int GetHwAddress();
	ll64* Ptr();
private:
	int m_nVal;
    GLOBAL_REG  m_nReg;
    bool m_isBigger32;
	int m_nHwAddress;
};

class CGlobalSet
{
public:
	CGlobalSet(GLOBAL_SET, int group, int index);
public:
	CGlobalSet& operator=(int);
//	CGlobalSet& operator=(ll64);	//2023.06.13 - kwmoon(comment fatric)
	int operator=(CGlobalSet&);		//2022.12.19 - kwmoon
	int operator=(CGlobalReg&);		//2023.05.19 - fatric
	ll64 GetValue64();
	uint GetValue32();
	GLOBAL_SET GetReg();
	int GetGroup();
	int GetIndex();
private:
	int m_nVal;
	GLOBAL_SET  m_nReg;
	int m_nGroup;
	int m_nIndex;
};


/////////////////////////////////////////
/// \brief The CYP
/// \details Cycle Palette & Fmt
/////////////////////////////////////////
class CPin
{
public:
	CPin(PIN_TYPE,int reg);
public:
	CPin& operator=(int);
	CPin& operator,(int);
    void SetModule(bool);
	PIN_TYPE GetType();
	int GetIndex();
	int GetModule();
	int GetCyp(int);
private:
	PIN_TYPE m_type;
    int m_nCyp[MAX_CYP];
    int m_nFmt;
    int m_nReg;
    int m_nPos;
    bool m_bModule;
};

/////////////////////////////////////////////
/// \brief The Pattern Line Text
/// \details Source line number, Line text
/////////////////////////////////////////////
typedef struct _pat_line_data
{
	uint32_t line;
	atstring data;
}pat_line_data;


//temp for compile
typedef struct _stBiFind
{
	bool read;
}stBiFind;

#endif // CSYSTYPE_HPP
