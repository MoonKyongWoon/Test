/*
	SODIMM 8GB ( FC1 : 4GB , FC2 : 4GB )
	DFM STRUCT 128bit ( 16 BYTE )
	EXTFM_MAX = 4GB / 16 BYTE (268435456) 256 MB
*/
#ifndef CDFM_HPP
#define CDFM_HPP

#include "testlib_global.h"
#include "sysdef.hpp"
#include "atstring.hpp"

#define FC1_DFM_STA_ADDR				( 0x200000000 )
#define FC1_DFM_END_ADDR				( 0x2FFFFFFFC )
//#define FC2_DFM_STA_ADDR				( 0x220000000 )
//#define FC2_DFM_END_ADDR				( 0x22FFFFFFC )
#define FC2_DFM_STA_ADDR				( 0x300000000 )	//2022.12.19 - kwmoon
#define FC2_DFM_END_ADDR				( 0x3FFFFFFFC )	//2022.12.19 - kwmoon
#define ONCE_FM_WR_COUNT				( DMA_WR_MAX_SIZE / FM_MEMORY_DATA_BYTES )	//2048 / 16 = 128
#define FM_READ_MINIMUM_COUNT			(  4)											//64Byte offset
#define FM_COMP_MODE_GAIN				(  4)

#pragma pack(1)
union FM_HEADER
{
	unsigned int	header;
	struct
	{
		unsigned int address : 24;
		unsigned int scan    :  8;
	};
};

union FM_DATA
{
	unsigned int	ioab;
	struct
	{
		unsigned int ioa_l : 8;
		unsigned int ioa_h : 8;
		unsigned int iob_l : 8;
		unsigned int iob_h : 8;
	};
};
////////////////////////////////////////////////////////////////


typedef struct _FM_MEM
{
	FM_HEADER header;
	FM_DATA data[3];
}FM_MEM;

#pragma pack()


class TESTLIBSHARED_EXPORT CDfm
{
public:
	CDfm();
	~CDfm();
public:
	void DfmCtrlReset();
	void ClearFmPointer(uint fc=ALL_FC);
	bool ReadExtFm(uint);
	bool ReadExtFm();
	bool ReadExtFmComp();
	FM_MEM* GetFmFailMemory(uint);
	void ClrExtFmMemory(uint size);
	uint GetFmModeGlobal();
	uint GetFailLimitGlobal();
	uint GetFailMultiIoGlobal();
	bool GetFmWrite();
	void PrintFmMode();
public:
	void SetFailLimitGlobal(uint);
	void SetFailMultiIoGlobal(uint);
	void ClrExtFmGlobal();
	void FmWriteGlobal(bool);
	void ResetFmModeGlobal(uint);
	void SetFmModeGlobal(uint);
	void PrintExtFm(uint fc, uint limit);
	void PrintCompExtFm(uint fc, FM_MEM*, uint index);
	uint ReadExtFmFcnt(uint fc);
	uint ReadExtFmCompFcnt();
	bool ReadExtFmData(uint,uint, unsigned long long data[4]);
private:
	void SetFmWriteMode();
private:
	bool m_bFmWrite;
	unsigned int m_FmMode;
	FM_MEM* m_pFmMemory[USE_FC];
	uint m_nFmFailCount [USE_FC];
	uint m_nFmCompFailCount [USE_FC];
	uint m_nFailLimitGlobal;
	uint m_nFailMultiIo;
};

#endif // AFM_HPP
