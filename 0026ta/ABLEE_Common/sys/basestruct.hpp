#ifndef BASESTRUCT_HPP
#define BASESTRUCT_HPP

#include "bitchart.hpp"

#pragma pack(1)


typedef struct _stAddSize
{
	unsigned int block_shift;
	unsigned int page_shift;
	unsigned int col_size;
	unsigned int mcp;
	unsigned int nchip;
}stAddSize;

typedef struct _stSetLimit
{
	unsigned int block_size_dut;
	unsigned int fail_block_limit_per_ce;
	unsigned int fail_add_limit_per_ce;
}stSetLimit;

typedef struct _stBibSet
{
	int visible_scan;		// nVisibleScan;
	int io_size;			//nIoSize;
	int bib_x;				//nBibX;
	int bib_y;				//nBibY;
	bool f_16mode;			//bF16Mode;
	bool invisible_scan;	//
	bool cpu2x;			//bCpux2;
}stBibSet;

typedef struct _stAdcEmt
{
	char szChName[64];
	double dExpt;
	double dScale;
	double dJudge;
}stAdcEmt;

typedef struct _stSetCe
{
	bool bUse;
	unsigned int mcp_order;
	unsigned int ioab;
	unsigned int ioab_order;
	unsigned int ce_start;
	unsigned int ce_no;
}stSetCe;

typedef struct _stSetCeCond1Dut
{
	bool bUse;
	unsigned int ioa_dutbits;
	unsigned int iob_dutbits;
	unsigned int ioa_cecnt;
	unsigned int iob_cecnt;
}stSetCeCond1Dut;

typedef struct _stSysMgnt
{
	int  nSource;
	char szName[64];
	double dExpt;
}stSysMgnt;


//PPS&S1PS
union R_STATUS_BYTE
{
	unsigned char		byte;
	struct
	{
		unsigned char	none          : 1;
		unsigned char	cml           : 1;
		unsigned char	temperature   : 1;
		unsigned char	vin_uv_fault  : 1;
		unsigned char	iout_oc_fault : 1;
		unsigned char	vout_ov_fault : 1;
		unsigned char	off           : 1;
		unsigned char	busy          : 1;
	};
};

//PPS&S1PS
union R_STATUS_WORD
{
	unsigned short		word;
	struct
	{
		unsigned short	none          : 1;
		unsigned short	cml           : 1;
		unsigned short	temperature   : 1;
		unsigned short	vin_uv_fault  : 1;
		unsigned short	iout_oc_fault : 1;
		unsigned short	vout_ov_fault : 1;
		unsigned short	off           : 1;
		unsigned short	busy          : 1;
		unsigned short	unknown       : 1;
		unsigned short	other         : 1;
		unsigned short	fans          : 1;
		unsigned short	pg_status     : 1;
		unsigned short	mfr_spec      : 1;
		unsigned short	input         : 1;
		unsigned short	iout_pout     : 1;
		unsigned short	vout          : 1;
	};
};

//PPS&S1PS
union R_STATUS_VOUT
{
	unsigned char		byte;
	struct
	{
		unsigned char	not_avail     : 1;
		unsigned char	toff_max_warn : 1;
		unsigned char	ton_max_fault : 1;
		unsigned char	vout_max_min  : 1;
		unsigned char	vout_uv_fault : 1;
		unsigned char	vout_uv_warn  : 1;
		unsigned char	vout_ov_warn  : 1;
		unsigned char	vout_ov_fault : 1;
	};
};

//PPS&S1PS
union R_STATUS_IOUT
{
	unsigned char		byte;
	struct
	{
		unsigned char	not_avail0    : 5;
		unsigned char	iout_oc_warn  : 1;
		unsigned char	not_avail1    : 1;
		unsigned char	iout_oc_fault : 1;
	};
};

//PPS&S1PS
union R_STATUS_INPUT
{
	unsigned char		byte;
	struct
	{
		unsigned char	not_avail0                : 1;
		unsigned char	iin_oc_warn               : 1;
		unsigned char	not_avail1                : 1;
		unsigned char	unit_off_for_insufficient : 1;
		unsigned char	not_avail2                : 1;
		unsigned char	vin_uv_warn               : 1;
		unsigned char	not_avail3                : 1;
		unsigned char	vin_ov_fault              : 1;
	};
};

//PPS&S1PS
union R_STATUS_TEMPERATURE
{
	unsigned char		byte;
	struct
	{
		unsigned char	not_avail0         : 6;
		unsigned char	ot_warn            : 1;
		unsigned char	ot_fault           : 1;
	};
};

//PPS&S1PS
union R_STATUS_CML
{
	unsigned char		byte;
	struct
	{
		unsigned char	not_avail0         : 1;
		unsigned char	comm_fault         : 1;
		unsigned char	reserved0          : 1;
		unsigned char	not_avail1         : 1;
		unsigned char	mem_fault_detect   : 1;
		unsigned char	packet_chk_fail    : 1;
		unsigned char	invalid_data_recv  : 1;
		unsigned char	invalid_cmd_recv   : 1;
	};
};

//PPS&S1PS
union R_STATUS_MFR
{
	unsigned char		byte;
	struct
	{
		unsigned char	yf_pgm_fail         : 1;
		unsigned char	yf_erase_fault      : 1;
		unsigned char	yf_verify_fault     : 1;
		unsigned char	il_master_fault     : 1;
		unsigned char	not_avail           : 5;
	};
};

#pragma pack()

#endif // BASESTRUCT_HPP
