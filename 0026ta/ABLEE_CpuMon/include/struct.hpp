#ifndef PFM_STRUCT_HPP
#define PFM_STRUCT_HPP

#pragma pack(1)

union UPFM_STAUS
{
	int pfm_status;
	struct
	{
		int status_pps   :  8;
		int status_s1ps  :  8;
		int status_dummy : 16;
	};
};

union UFAULT_MASK
{
	int mask;
	struct
	{
		int status_word : 16;
		int status_vout :  8;
		int dummy       :  8;
	};
};

union UREAD_IOUT_VOUT
{
	int read_iout_vout;
	struct
	{
		int read_vout : 16;
		int read_iout : 16;
	};
};

union UREAD_SCALE_WORD
{
	int read_scale_word;
	struct
	{
		int status_word     : 16;
		int vout_scale_loop : 16;
	};
};

union UREAD_STATUS_VOUT
{
	int read_status_vout;
	struct
	{
		int status_vout     :  8;
		int dummy           : 24;
	};
};

#pragma pack()


#endif // STRUCT_HPP
