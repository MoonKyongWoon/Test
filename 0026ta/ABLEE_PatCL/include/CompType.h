#pragma once

#include <string.h>
#include <list>
#include <map>
#include <string>

using namespace std;

#define VER_COMPILER			"0.01.01:2022/10/24"

#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))

class MPAT_ADDRESS {				// 0x0d0c
public:
	MPAT_ADDRESS()
	  : mode(0)
	{
		memset(x, 0, sizeof(x));
		memset(y, 0, sizeof(y));
	}
//	unsigned	short		x[18];	// bit[7..6] = 1:Z 2:N 3:B bit[5..0]=addr
//	unsigned	short		y[18];	// bit[7..6] = 1:Z 2:N 3:B bit[5..0]=addr
	unsigned	short		x[24];	// bit[7..6] = 1:Z 2:N 3:B bit[6..0]=addr
	unsigned	short		y[24];	// bit[7..6] = 1:Z 2:N 3:B bit[6..0]=addr
	int						mode ;	// mode.add_def  0:XY.OR.ZB 1:ZN
};

class SET_REG //DSET
{
public:
	SET_REG();
	unsigned int groupReg(int iGrp);
	void mergeGroup(int iGrp, SET_REG& _set_reg);
	void clear();
public:
	unsigned int 		reg;
	unsigned int		a;
	unsigned int		b;
	unsigned long long	c;
	unsigned long long	d;
};

class REGI
{
public:
	REGI();

	unsigned int		i2  ;
	unsigned int		addr;
};
typedef list< REGI *>		RegList;

class ALPG
{
public:
	ALPG();
	~ALPG();

//	size_t			nStatic  ;	// Static size
	unsigned int	seq_mode ;
	unsigned int	alpg_mode;
//	unsigned short	fpx      ;
	MPAT_ADDRESS	addr     ;
//	int				data     ;
	int				sta	  	 ;	//START #xx
	int				pc	     ;	//PAT MEM PC
	char			name[33] ;
	int				out		 ;

	SET_REG	set_reg[16] ;

// Dynamic size
	RegList			reg_list;
//	BurstList		burst   ;
};
typedef list < ALPG * >			AlpgList;

class  WAYS
{
public:
	union {
		unsigned int		set_group;				// 0x301
		struct {
			unsigned int	group_a			: 4;	//XH,D1A,D3B,NH,XOS,XT,ZD,ND,DIGIT1
			unsigned int	group_b			: 4;	//YH,D2A,D4B,BH,ZH,YOS,YT,Z,BD,DIGIT2
			unsigned int	group_c			: 4;	//TP1,TPH1,D5,DCMR1,XMASK,CBMA,RCD,DSD,DBMAR,DIGIT3
			unsigned int	group_d			: 4;	//TP2,TPH2,D6,DCMR2,YMASK,DSC,CCD
		};
	};

	unsigned int			a_operand		: 24;	//0x030a
	unsigned int			b_operand		: 24; 	//0x030b
	unsigned long long		c_operand		: 36;	//0x0342, 0x0343
	unsigned long long		d_operand		: 36; 	//0x0344, 0x0345

	unsigned int			set_use			: 4;

	union {
		unsigned int		x_base;					//0x302
		struct {
			unsigned int	x_base_alu		:  8;
			unsigned int	x_base_d		:  6;
		} ;
	};

	union {
		unsigned int		y_base;					//0x303
		struct {
			unsigned int	y_base_alu		:  8;
			unsigned int	y_base_d		:  6;
		} ;
	};

	union {
		unsigned int		z_base;					//0x304
		struct {
			unsigned int	z_base_alu		: 8;
		} ;
	};

	union {
		unsigned int		x_curr;					//0x305
		struct {
			unsigned int	x_curr_dest		: 5;
			unsigned int	x_curr_alu		: 5;
			unsigned int	x_curr_src		: 3;	//3<-2	//2023.06.13 - kwmoon(modify fatric)
			unsigned int	x_curr_d		: 6;
		} ;
	};

	union {
		unsigned int		y_curr;					//0x306
		struct {
			unsigned int	y_curr_dest		: 5;
			unsigned int	y_curr_alu		: 5;
			unsigned int	y_curr_src		: 3;	//3<-2	//2023.06.13 - kwmoon(modify fatric)
			unsigned int	y_curr_d		: 6;
		} ;
	};

	union {
		unsigned int		nb_reg;					//0x307
		struct {
			unsigned int	nb_reg_n_alu	: 4;
			unsigned int	nb_reg_b_alu	: 4;
			unsigned int	nb_reg_n_d		: 8;
			unsigned int	nb_reg_b_d		: 8;
		};
	};

	union {
		unsigned int		d3d4_reg;				//0x308
		struct {
			unsigned int	d3d4_reg_d3		: 4;
			unsigned int	d3d4_reg_d4		: 4;
		};
	};
//2023.06.13 - kwmoon(comment fatric)
//	union {
//		unsigned int		src_addr		: 16;	//0x309
//		struct {
//			unsigned int	src_addr_x_out	: 4;	//X<XB,XC,XS,XK,YB,...
//			unsigned int	src_addr_y_out	: 4;	//Y<YB,YC,YS,YK,XB,...
//			unsigned int	src_addr_x_inv	: 1;	// /X
//			unsigned int	src_addr_y_inv	: 1;	// /Y
//			unsigned int	src_addr_z_inv	: 1;	// /Z
//			unsigned int	src_addr_null	: 1;	//space
//			unsigned int	src_addr_xt		: 1;	//XT
//			unsigned int	src_addr_yt		: 1;	//YT
//			unsigned int	src_addr_swap	: 1;	//LMAX<>HMAX
//			unsigned int	src_addr_rf		: 1;	//RF<RF
//		};
//	};

	union {											//Append for HOT
		unsigned int		src_addr		: 24;	//0x309
		struct {
			unsigned int	src_addr_x_out	: 8;	//2023.06.13 - kwmoon(modify fatric) //X<XB,XC,XS,XK,YB,...
			unsigned int	src_addr_y_out	: 8;	//2023.06.13 - kwmoon(modify fatric) //Y<YB,YC,YS,YK,XB,...
			unsigned int	src_addr_x_inv	: 1;	// /X
			unsigned int	src_addr_y_inv	: 1;	// /Y
			unsigned int	src_addr_z_inv	: 1;	// /Z
			unsigned int	src_addr_null	: 1;	//space
			unsigned int	src_addr_xt		: 1;	//XT
			unsigned int	src_addr_yt		: 1;	//YT
			unsigned int	src_addr_swap	: 1;	//LMAX<>HMAX
			unsigned int	src_addr_rf		: 1;	//RF<RF
		};
	};
	union {
		unsigned char		use_src_addr;
		struct {
			bool			b_src_addr_x_out : 1;
			bool			b_src_addr_y_out : 1;
		};
	};

	struct {
		unsigned int		mut_ctl;				//C0~C31		//0x330
		unsigned int		mut_rwm;				//W, R, M1, M2	//0x331
	};

	union {
		unsigned int		scan_addr;					//0x480
		struct {
			unsigned int	scan_addr_dest	: 4;
			unsigned int	scan_addr_alu	: 4;
			unsigned int	scan_addr_src_a	: 2;
			unsigned int	scan_addr_src_b	: 2;
			unsigned int	scan_addr_out	: 2;
		} ;
	};

	union {
		unsigned int		vector_addr;				//0x498
		struct {
			unsigned int	vector_addr_dest	: 4;
			unsigned int	vector_addr_alu		: 4;
			unsigned int	vector_addr_src_a	: 2;
			unsigned int	vector_addr_src_b	: 2;
			unsigned int	vector_addr_out		: 2;
		} ;
	};

	union {
		unsigned int		tp;						//0x341
		struct {
			unsigned int	tp1				: 8;	//TP<???
			unsigned int	tp2				: 8;	//TP2<??
		} ;
	};

	union {
		unsigned int		tpd;					//0x346
		struct {
			unsigned int	tpd_inv_d		: 1;
			unsigned int	tpd_inv_d2		: 1;
			unsigned int	tpd_src_data	: 2;	//TP1, TP2, TPXOR
			unsigned int	tpd_data_hold	: 1;
		};
	};
	union {
		unsigned int		dbmap;					//0x350
		struct {
			unsigned int	dbmap_op		: 8;	//0: DBMAP<DBMAP (HOLD), 1: DBMAP<DBMAP/2, 2: DBMAP<DBMAP+1, 3: DBMAP<DBMAP-1, 4: DBMAP<0, 5: DBMAP<DBMAR, 6: DBMAP<DBMAP+DBMAR, , 7: DBMAP<DBMAP-DBMAR, 8: DBMAP<DIGIT3, 9: DBMAP<DBMAP+DIGIT3, A: DBMAP<DBMAP-DIGIT3
		};
	};

	unsigned int			dle             : 1;	//DAIG LATCH ENABLE 0x390

	//Cycle Palatte
	unsigned int			cyp             : 5;	//0x380
	unsigned int			ts              : 5;	//0x3f0

public:
	WAYS();
};

class PCS
{
public:
	// Static size
	int						nStatic;
	unsigned int			opcode ;			// 0x200
	unsigned int			operand;			// 0x201

	unsigned int			op_use;				//WAY->PCS

	WAYS					way[4];				//MAX 4way

public:
	PCS();
};

typedef list < PCS * >		PcList;

class PGDATA
{
public:
	char	    rev [32];
	char		name[33];	//??
	ALPG		alpg    ; //Common Section
	AlpgList	module  ; //Module Section
	PcList		*pcs    ;
//	T_DBM		dbm     ;

public:
	PGDATA();
	~PGDATA();
	void Init();
	PCS *GetPCS(unsigned int iPc);	//2023.06.13 - kwmoon(add fatric)
};

typedef map< string, unsigned int >		Str2WordMap;
typedef map< string, string >			Str2StrMap;

class NOLABEL {
public:
	NOLABEL();
	char 			label[33];
	int  			pc;
	unsigned int 	*reg;
};
typedef list <NOLABEL *> NoLabelList;

class IFE_EMT
{
public:
	int opr;
	int r;

	IFE_EMT();
	IFE_EMT(int _opr, int _r);
};
typedef list<IFE_EMT *>	IFE_LIST;

class IFE_GRADE
{
public:
	int			grade;
	int			opr;
	IFE_LIST	ife_list;

	IFE_GRADE(int _grade);
	~IFE_GRADE();
};
typedef list<IFE_GRADE *> IFE_GREADE_LIST;

union PGCMD
{
	unsigned int		value ;
	struct
	{
		unsigned int	bit     : 16;
		unsigned int	opt     :  8;
		unsigned int	command :  8;
	};
};

enum PTCM_SYNTAX {
	ALPG_NOSUPPORT			,
//	ALPG_LABEL				,
	ALPG_COLON				,
	ALPG_LINE				,
	ALPG_OPLINE				,
	ALPG_TEMP				,
	ALPG_MPAT				,
	ALPG_SUBROUTINE_BEGIN	,
	ALPG_SUBROUTINE_END		,
	ALPG_MODULE_BEGIN		,
	ALPG_MODULE_END			,
	SEQ_MODE				,
	ALPG_MODE				,
	ALPG_REG				,
	ALPG_REG0				,
	ALPG_REG2 				,
	ALPG_REG4				,
	ALPG_SET				,
	ALPG_SET2				,
	APLG_ARIRAM				,
	APLG_ADDRESS			,
	ALPG_BURST_START		,
	ALPG_BURST_END			,
	ALPG_BURST				,
	ALPG_DATA				,
	ALPG_OPCODE				,
	ALPG_MUT				,
	ALPG_CONTROL			,
	ALPG_ETC				,
	ALPG_CYP				,
	ALPG_TIMESET			,
	ALPG_TP					,
	ALPG_D					,
	ALPG_FCD				,
	ALPG_DSC				,
	ALPG_FP					,
	ALPG_CBMA				,
	ALPG_RANDOM				,
	ALPG_ADDR_BASE			,
	ALPG_ADDR_CURR_X		,
	ALPG_ADDR_CURR_Y		,
	ALPG_NB_REG				,
	ALPG_D3D4_REG			,
	ALPG_ADDR_SRC			,
	ALPG_DBMAP              ,
	ALPG_SCAN_ADDR          ,
	ALPG_VECTOR_ADDR        ,
	ALPG_OPEN_ASSIGNMENT
};

enum PTCM_MPAT {
	MPAT     	,
	BEGIN    	,
	END      	,
	INSERT   	,
	MODULE   	,
	RDX      	,
	MODE     	,
	MUX      	,
	DATEN    	,
	PAUSE    	,
	REFRESHA 	,
	REFRESHB 	,
	PARITY   	,
	TP2INV   	,
	TPM      	,
	BWPEM    	,
	DFCA     	,
	DFCB     	,
	ECBM     	,
	DBMLP    	,
	CYPV15   	,
	REGISTER
};

enum BIT_ALPG {
	REG              = 0x2020000,
	REG_D            = 0x3200000,	//D1?, D2?, LMAX, HMAX, XMAX, YMAX, ZMAX, ND?, BD?, S1A, S_TEMP, V1A, V_TEMP
	DRE              = 0x3810400,
	ADDR_DEF_X       = 0x3200080,
	ADDR_DEF_Y       = 0x3200098,
	ADDR_DEF_Z_OFF   = 0x0      ,
	ADDR_DEF_N_OFF   = 0x18     ,
	ADDR_DEF_B_OFF   = 0x20     ,

	BIT_TIMER        = 0x60     ,
	BIT_TREFRESH     = 0x64     ,
	BIT_TPAUSE       = 0x68     ,
	BIT_TJET         = 0x6C     ,
};

enum OUT_MODE {
	pretest_result    = 0x80000001,
	bitest_result     = 0x80000002,
	latch             = 0x80000003,	//
	pretest_data      = 0x80000004,
	bitest_data       = 0x80000005,	//

	pre_result        = 0x0000800f,
	fm_block          = 0x80000011,
	extfm_result      = 0x80000012,
	L1_result         = 0x80000013,
	latch_result      = 0x80000014,	//
	init_result       = 0x80000015,	//
	fm_read_data      = 0x80000016,

	bist_io_latch     = 0x80000020,
	bist_result       = 0x80000022,
	bist2_result      = 0x80000023,

	emmc_io_latch     = 0x80000030,
	emmc_read_drv     = 0x80000031,
	emmc_init         = 0x80000032,
	emmc_result       = 0x80000033,
	emmc_block_result = 0x80000034,
	emmc_chip_id      = 0x80000035,
	emmc_read_data    = 0x80000036
};
