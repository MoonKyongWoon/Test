#ifndef PMBUS_HPP
#define PMBUS_HPP

#include <iostream>
#include <string>

enum SIDE_AB
{
	SIDE_A  ,	//SLOT1-8
	SIDE_B  ,	//SLOT9-16
	MAX_SIDE,
	SIDE_ALL,
};

enum SMPS_LINE
{
	LINE1,
	LINE2,
	LINE3,
	MAX_LINE,
};

enum LINE_ITEM
{
	LINE_V,
	LINE_A,
	MAX_LINE_ITEM,
};

enum SMPS_MODULE
{
	MODULE1,
	MODULE2,
	MODULE3,
	MODULE4,
	MODULE5,
	MODULE6,
	MODULE7,
	MODULE8,
	MAX_MODULE,
};

enum MODULE_ITEM
{
	MODULE_VOUT,
	MODULE_IOUT,
	MODULE_POUT,
	MODULE_TEMP,
	MODULE_STAT,
	MAX_MODULE_ITEM,
};


enum PMBUS_COMMAND
{
	CMD_OPERATION         = 0x1,
	CMD_CLEAR_FAULTS      = 0x3,
	CMD_WRITE_PROTECT     = 0x10,
	CMD_VOUT_MAX          = 0x24,
	CMD_POUT_MAX          = 0x31,
	CMD_STATUS_BYTE       = 0x78,
	CMD_STATUS_WORD       = 0x79,
	CMD_STATUS_VOUT       = 0x7A,
	CMD_STATUS_IOUT       = 0x7B,
	CMD_STATUS_IINPUT     = 0x7C,
	CMD_STATUS_TEMPERATUR = 0x7D,
    CMD_STATUS_CML        = 0x7E,
	CMD_READ_VOUT         = 0x8B,
	CMD_READ_IOUT         = 0x8C,
	CMD_READ_TEMPERATUR_1 = 0x8D,
	CMD_READ_TEMPERATUR_2 = 0x8F,
	CMD_READ_FAN1_SPEED   = 0x90,
	CMD_READ_FAN2_SPEED   = 0x91,
	CMD_READ_FAN3_SPEED   = 0x92,
	CMD_READ_FAN4_SPEED   = 0x93,
	CMD_READ_POUT         = 0x96,
	CMD_MFR_ID            = 0x99,
	CMD_MFR_MODEL         = 0x9A,
	CMD_MFR_LOCATION      = 0x9C,
	CMD_MFR_DATE          = 0x9D,
	CMD_MFR_VOUT_MAX      = 0xA5,
	CMD_MFR_IOUT_MAX      = 0xA6,
	CMD_MFR_POUT_MAX      = 0xA7,
	CMD_VREF              = 0xB1,
	CMD_SET_VOLT_RISE_SET = 0xB5,
	CMD_SET_CURR_RISE_SET = 0xB6,
	CMD_READ_VIN1         = 0xCA,
	CMD_READ_VIN2         = 0xCB,
	CMD_READ_VIN3         = 0xCC,
	CMD_READ_IIN1         = 0xCD,
	CMD_READ_IIN2         = 0xCE,
	CMD_READ_IIN3         = 0xCF,
};


enum ERROR_CODE
{
	ERR_SUCCESS      = 0x0  ,
	ERR_RACK_NOT_EXISTING   ,
	ERR_DEVICE_NOT_EXISTING ,
	ERR_UNSUPPORTED_CMD     ,
	ERR_OPERATION_INVALID   ,
	ERR_LENGTH_INVALID      ,
	ERR_DATA_INVALID        ,
	ERR_WRITE_PROTECT       ,
	ERR_PROTOCOL_INVALID    ,
	ERR_CUSTOM_INVALID      ,

};

enum DEVICE_ADDRESS
{
	DEV_ISOCOMM       =  0x0,
	DEV_PFC1          =  0x7,
	DEV_PFC2          =  0x8,
	DEV_MODULE1       = 0x10,
	DEV_MODULE2       = 0x11,
	DEV_MODULE3       = 0x12,
	DEV_MODULE4       = 0x13,
	DEV_MODULE5       = 0x14,
	DEV_MODULE6       = 0x15,
	DEV_MODULE7       = 0x16,
	DEV_MODULE8       = 0x17,
};

enum PMBUS_OPENRATION
{
	PMBUS_WRIT        = 0x0,
	PMBUS_READ             ,
};

enum PMBUS_DATA_FORMAT
{
	FORMAT_BITMAPPED   = 0x0,
	FORMAT_LINEAR           ,
	FORMAT_ASCII            ,
	FORMAT_DIRECT           ,
    FORMAT_NA               ,
};


#pragma pack(1)

typedef struct _stPmubsCommand
{
	unsigned char			uCommand;
	std::string				sCommand;
	PMBUS_OPENRATION		enCmdType;
	PMBUS_DATA_FORMAT		enDataFormat;
	unsigned char			uNumOfByte;
	unsigned short			uMultiplier;
	unsigned char			uOffsetData;
}stPmubusCommand;


typedef struct _stUiRequest
{
	SIDE_AB				uSide;
	int					uDevice;		//ISOCOMM or MODULE#1-8
	int					uData;			//Write Data
	stPmubusCommand		stPmbusCmd;
}stUiRequest;


//PC -> iHP
typedef struct _stPmbusReqHead
{
	char szMsgId[4];
	union
	{
		unsigned char	szByte5;
		struct {
			unsigned char NoOfCmds  : 1;
			unsigned char Reserved0 : 4;
			unsigned char SplitType : 1;
			unsigned char Reserved1 : 1;
			unsigned char Type      : 1;
		};
	};
	union
	{
		unsigned char	szByte6;
		struct {
			unsigned char InternalDevAddr : 5;
			unsigned char iHpRackAddr     : 3;
		};
	};
	union
	{
		unsigned char	szByte7;
		struct {
			unsigned char CmdDataLen  : 6;
			unsigned char Operation   : 1;
			unsigned char Reserved2   : 1;
		};
	};
	unsigned char Command;
}stPmbusReqHead;

//iHP -> PC , ...
typedef struct _stPmbusResponse
{
	char szMsgId[4];
	union
	{
		unsigned char	szByte5;
		struct {
			unsigned char NoOfCmds  : 1;
			unsigned char Reserved0 : 4;
			unsigned char FinalBit  : 1;
			unsigned char Errorbit  : 1;
			unsigned char Reserved1 : 1;
		};
	};
	union
	{
		unsigned char	szByte6;
		struct {
			unsigned char Reserved2   : 4;
			unsigned char ErrorCode   : 4;
		};
	};
	union
	{
		unsigned char	szByte7;
		struct {
			unsigned char InternalDevAddr : 5;
			unsigned char iHpRackAddr     : 3;
		};
	};
	union
	{
		unsigned char	szByte8;
		struct {
			unsigned char ResponseDataLen : 6;
			unsigned char Reserved3       : 2;
		};
	};
	unsigned char Command;
}stPmbusResponse;


//iHP -> PC , ...
typedef struct _stPmbusError
{
	char szMsgId[4];
	union
	{
		unsigned char	szByte5;
		struct {
			unsigned char DataLength : 1;
			unsigned char Reserved0  : 5;
			unsigned char Errorbit   : 1;
			unsigned char Reserved1  : 1;
		};
	};
	union
	{
		unsigned char	szByte6;
		struct {
			unsigned char CommandSeqId : 4;
			unsigned char ErrorCode    : 4;
		};
	};
}stPmbusError;

typedef struct _stLineMeas
{
    double Voltage[MAX_LINE];
    double Current[MAX_LINE];
}stLineMeas;


typedef struct _stModulelMeas
{
    double Voltage[MAX_MODULE];
    double Current[MAX_MODULE];
    double Watt[MAX_MODULE];
    double Temp[MAX_MODULE];
    uint Status[MAX_MODULE];
}stModulelMeas;


#pragma pack()


#endif // PMBUS_HPP

