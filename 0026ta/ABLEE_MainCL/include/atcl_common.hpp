#ifndef ATCL_COMMON_HPP
#define ATCL_COMMON_HPP

#include "atstring.hpp"
#include "sysini.hpp"
#include <map>

#define BUILD_DIR           ".build"
#define CMAKELIST_TXT       "CMakeLists.txt"
#define CMAKE_DOT_DOT       "cmake .."
#define CMAKE               "cmake"
#define CXXCOMPILE          "make"
#define CXX_CLEAN           "clean"
#define CXX_INSTALL         "install"
#define CXXMAKEFILE         "Makefile"

#define COMMAND(a,b,c)								( (a<<24) | (b<<16) | c )
#define CMPL_COMMAND(syntax, command, opt, bit)		m_mapSyntax[syntax] = COMMAND(command, opt, bit);

#pragma pack(1)


//typedef std::map< atstring, unsigned int >	Str2WordMap;
//typedef std::map< atstring, atstring >		Str2StrMap;
//typedef std::map< atstring, double  >		Str2DoubleMap;

union DEVCMD
{
	unsigned int		value;
	struct
	{
		unsigned int	bit     : 16;
		unsigned int	opt     : 8;
		unsigned int	command : 8;
	};
};

typedef struct _devsyntax
{
	bool is_ifdef;
	bool is_ifdef_block;
	bool is_start_device;
	bool is_end_device;
	bool is_programStart;
	bool is_define_flag;
	bool is_function;
	bool is_functionIn;
	bool is_sub;
	bool is_define;
	bool is_pattern_group;
	bool is_default_function;
	bool is_set_pattern;
	atstring start_device;
	atstring function;
	atstring default_function;
} devsyntax;

enum DEV_CATEGORY
{
	DEV_CPLUSPLUS        = 0x0,
	DEV_FUNCTION              ,
	DEV_VARIABLE              ,
	DEV_MACRO                 ,
};

enum DEV_SYNTAX
{
	C_SHARP         = 0x0,
	C_DEFINE             ,
	C_IFDEF              ,
    C_SHARP_ELSE         ,
    C_ENDIF              ,
	C_UNDEF              ,
	F_START_DEVICE       ,
    F_RATE               ,
    F_CLK                ,
    V_CLK_NAME           ,
    F_PROGRAM_START      ,
    F_SET_LIMIT          ,
    F_SET_ADD_SIZE       ,
    F_SET_BIB            ,
    F_BIB                ,
    F_DEFINE_FLAG_AZ     ,
    F_SET_SCAN           ,
    F_REG                ,
    F_MODULE             ,
    V_PIN_NAME           ,
    F_SET_MODULE         ,
    F_CYP                ,
    F_VOL                ,
    F_SET_DEFINE         ,
    F_SET_SUB            ,
    F_PATTERN_GROUP      ,
    M_IFE                ,
    M_ELSE               ,
    M_ENDC               ,
    F_STEP               ,
    F_LOAD               ,
    F_SEND_MPAT          ,
    F_STEP_RUN           ,
	F_SET_PATTERN        ,
    F_END_DEVICE         ,
};

enum EN_CL_TYPE
{
    EN_MAIN     = 0x0,
    EN_BIB           ,
    EN_PATTERN       ,
    EN_TIMESET       ,
    EN_CYP           ,
    EN_REG           ,
    EN_VOL           ,
    EN_MAX_TYPE      ,
};

enum EN_RESULT
{
    EN_NORMAL       ,
    EN_WARNING      ,
    EN_ERROR        ,
};


#pragma pack()
#endif // ATCL_COMMON_HPP
