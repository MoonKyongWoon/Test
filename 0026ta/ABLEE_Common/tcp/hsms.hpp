#ifndef HSMS_HPP
#define HSMS_HPP

#define HSMS_DEFAULT_PORT			(5000)

enum class TIMEOUT_TYPE
{
	_T3 = 0,
	_T5    ,
	_T6    ,
	_T7    ,
	_T8    ,
};

enum HSMS_FMT_CODE
{
	FMT_LIST	= 000,
	FMT_BIN		= 010,
	FMT_BOOL	= 011,
	FMT_ASCII	= 020,
	FMT_JIS_8	= 021,
	FMT_I8		= 030,
	FMT_I1		= 031,
	FMT_I2		= 032,
	FMT_I4		= 034,
	FMT_F8		= 040,
	FMT_F4		= 044,
	FMT_U8		= 050,
	FMT_U1		= 051,
	FMT_U2		= 052,
	FMT_U4		= 054,
	FMT_UNKNOWN	= 999,
};

enum class HSMS_STATUS
{
	NOT_CONNECTED = 0,
	CONNECTED		 ,
	NOT_SELECTED	 ,
	SELECTED		 ,
};

enum class SELECT_STATUS
{
	COMMUNICATION_ESTABLISHED		= 0,
	COMMUNICATION_ALREADY_ACTIVE	   ,
	CONNECTION_NOT_READY			   ,
	CONNECT_EXHAUST					   ,
};

enum class DESELECT_STATUS
{
	COMMUNICATION_ENDED				= 0,
	COMMUNICATION_NOT_ESTABLISHED	   ,
	COMMUNICATION_BUSY				   ,
};

enum class SType
{
	DATA_MESSAGE = 0,
	SELECT_REQ		,
	SELECT_RSP		,
	DESELECT_REQ	,
	DESELECT_RSP	,
	LINKTEST_REQ	,
	LINKTEST_RSP	,
	REJECT_REQ		,
	SEPARATE_REQ = 9,
};

enum class REASON_CODE
{
	STYPE_NOT_SUPPORTED = 1,
	PTYPE_NOT_SUPPORTED    ,
	TRANSACTION_NOT_OPEN   ,
	ENTITY_NOT_SELECTED    ,
};

#endif // HSMS_HPP
