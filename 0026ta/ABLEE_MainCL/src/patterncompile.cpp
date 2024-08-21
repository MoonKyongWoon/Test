#include "patterncompile.hpp"

CPatternCompile::CPatternCompile(vector<atstring>& rv) : m_rVecError(rv)
{
	init();
}

void CPatternCompile::init()
{
	m_bDebugOpt = false;
	m_sRunDir   = QDir::currentPath();
	m_sBuildDir = QObject::tr("%1/%2").arg(m_sRunDir).arg(BUILD_DIR);

	m_veccdefine.clear();
	m_mapsdefine.clear();
	m_mapsub.clear();
	m_mappatgroup.clear();

	initSyntaxValid();
	makeSyntaxMap();
}

void CPatternCompile::initSyntaxValid()
{
	m_devsyntax.is_ifdef             = false;
	m_devsyntax.is_ifdef_block       = false;
	m_devsyntax.is_start_device      = false;
	m_devsyntax.is_end_device        = false;
	m_devsyntax.is_programStart      = false;
	m_devsyntax.is_define_flag       = false;
	m_devsyntax.is_function          = false;
	m_devsyntax.is_functionIn        = false;
	m_devsyntax.is_sub               = false;
	m_devsyntax.is_define            = false;
	m_devsyntax.is_pattern_group     = false;
	m_devsyntax.is_default_function  = false;
	m_devsyntax.is_set_pattern       = false;

	m_devsyntax.start_device     = "";
	m_devsyntax.function         = "";
	m_devsyntax.default_function = "";

}

void CPatternCompile::setMainCpp(QString cpp)
{
    m_sCpp = QObject::tr("%1/%2").arg(m_sRunDir).arg(cpp);
}

void CPatternCompile::setDebugOpt( bool debug )
{
	m_bDebugOpt = debug;
}

void CPatternCompile::setDefineOpt(std::vector<atstring> macros)
{
	for( atstring str : macros )
	{
		cppdefine cdefine;
		cdefine.key = str;
		cdefine.value = "";
		m_veccdefine.push_back( cdefine );
	}
}

void CPatternCompile::makeSyntaxMap()
{
	CMPL_COMMAND( "#"               , DEV_CPLUSPLUS  , C_SHARP          , 0  );
	CMPL_COMMAND( "define"          , DEV_CPLUSPLUS  , C_DEFINE         , 0  );		//#define
	CMPL_COMMAND( "ifdef"           , DEV_CPLUSPLUS  , C_IFDEF          , 1  );		//#ifdef
	CMPL_COMMAND( "else"            , DEV_CPLUSPLUS  , C_SHARP_ELSE     , 2  );		//#else
	CMPL_COMMAND( "endif"           , DEV_CPLUSPLUS  , C_ENDIF          , 3  );		//#endif
	CMPL_COMMAND( "undef"           , DEV_CPLUSPLUS  , C_UNDEF          , 4  );		//#endif
	CMPL_COMMAND( "START_DEVICE"    , DEV_FUNCTION   , F_START_DEVICE   , 0  );		//START_DEVICE(DEVICE)
	CMPL_COMMAND( "RATE"            , DEV_FUNCTION   , F_RATE           , 0  );		//RATE( 0, ... )
	CMPL_COMMAND( "CLK"             , DEV_FUNCTION   , F_CLK            , 0  );		//CLK( 0, ... )
	CMPL_COMMAND( "ADBCLK"          , DEV_VARIABLE   , V_CLK_NAME       , 0  );
    CMPL_COMMAND( "ADCCLK"          , DEV_VARIABLE   , V_CLK_NAME       , 1  );
	CMPL_COMMAND( "programStart"    , DEV_FUNCTION   , F_PROGRAM_START  , 0  );		//void programStart()
	CMPL_COMMAND( "SET_LIMIT"       , DEV_FUNCTION   , F_SET_LIMIT      , 0  );		//SET_LIMIT(...)
	CMPL_COMMAND( "SET_ADD_SIZE"    , DEV_FUNCTION   , F_SET_ADD_SIZE   , 0  );		//SET_ADD_SIZE
	CMPL_COMMAND( "SET_BIB"         , DEV_FUNCTION   , F_SET_BIB        , 0  );		//SET_BIB
	CMPL_COMMAND( "BIB"             , DEV_FUNCTION   , F_BIB            , 0  );		//BIB( ... )
    CMPL_COMMAND( "DEFINE_FLAG_A"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 0   );
    CMPL_COMMAND( "DEFINE_FLAG_B"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 1   );
    CMPL_COMMAND( "DEFINE_FLAG_C"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 2   );
    CMPL_COMMAND( "DEFINE_FLAG_D"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 3   );
    CMPL_COMMAND( "DEFINE_FLAG_E"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 4   );
    CMPL_COMMAND( "DEFINE_FLAG_F"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 5   );
    CMPL_COMMAND( "DEFINE_FLAG_G"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 6   );
    CMPL_COMMAND( "DEFINE_FLAG_H"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 7   );
    CMPL_COMMAND( "DEFINE_FLAG_I"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 8   );
    CMPL_COMMAND( "DEFINE_FLAG_J"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 9   );
    CMPL_COMMAND( "DEFINE_FLAG_K"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 10  );
    CMPL_COMMAND( "DEFINE_FLAG_L"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 11  );
    CMPL_COMMAND( "DEFINE_FLAG_M"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 12  );
    CMPL_COMMAND( "DEFINE_FLAG_N"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 13  );
    CMPL_COMMAND( "DEFINE_FLAG_O"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 14  );
    CMPL_COMMAND( "DEFINE_FLAG_P"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 15  );
    CMPL_COMMAND( "DEFINE_FLAG_Q"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 16  );
    CMPL_COMMAND( "DEFINE_FLAG_R"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 17  );
    CMPL_COMMAND( "DEFINE_FLAG_S"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 18  );
    CMPL_COMMAND( "DEFINE_FLAG_T"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 19  );
    CMPL_COMMAND( "DEFINE_FLAG_U"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 20  );
    CMPL_COMMAND( "DEFINE_FLAG_V"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 21  );
    CMPL_COMMAND( "DEFINE_FLAG_W"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 22  );
    CMPL_COMMAND( "DEFINE_FLAG_X"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 23  );
    CMPL_COMMAND( "DEFINE_FLAG_Y"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 24  );
    CMPL_COMMAND( "DEFINE_FLAG_Z"   , DEV_FUNCTION   , F_DEFINE_FLAG_AZ , 25  );
    CMPL_COMMAND( "SET_SCAN"        , DEV_FUNCTION   , F_SET_SCAN       , 0   );
    CMPL_COMMAND( "REG"             , DEV_FUNCTION   , F_REG            , 0   );
    CMPL_COMMAND( "MODULE"          , DEV_FUNCTION   , F_MODULE         , 0   );
    CMPL_COMMAND( "UCLK0"           , DEV_VARIABLE   , V_PIN_NAME       , 0   );
    CMPL_COMMAND( "UCLK1"           , DEV_VARIABLE   , V_PIN_NAME       , 1   );
    CMPL_COMMAND( "SET_MODULE"      , DEV_FUNCTION   , F_SET_MODULE     , 0   );
    CMPL_COMMAND( "CYP"             , DEV_FUNCTION   , F_CYP            , 0   );
    CMPL_COMMAND( "VOL"             , DEV_FUNCTION   , F_VOL            , 0   );
    CMPL_COMMAND( "SET_DEFINE"      , DEV_FUNCTION   , F_SET_DEFINE     , 0   );
    CMPL_COMMAND( "SET_SUB"         , DEV_FUNCTION   , F_SET_SUB        , 0   );
    CMPL_COMMAND( "PATTERN_GROUP"   , DEV_FUNCTION   , F_PATTERN_GROUP  , 0   );
    CMPL_COMMAND( "STEP"            , DEV_FUNCTION   , F_STEP           , 0   );
    CMPL_COMMAND( "LOAD"            , DEV_FUNCTION   , F_LOAD           , 0   );
    CMPL_COMMAND( "SEND_MPAT"       , DEV_FUNCTION   , F_SEND_MPAT      , 0   );
	CMPL_COMMAND( "STEP_RUN"        , DEV_FUNCTION   , F_STEP_RUN       , 0   );
	CMPL_COMMAND( "SET_PATTERN"     , DEV_FUNCTION   , F_SET_PATTERN    , 0   );
	CMPL_COMMAND( "END_DEVICE"      , DEV_FUNCTION   , F_END_DEVICE     , 0   );
	CMPL_COMMAND( "%IFE"            , DEV_MACRO      , M_IFE            , 0   );
	CMPL_COMMAND( "%ELSE"           , DEV_MACRO      , M_ELSE           , 0   );
	CMPL_COMMAND( "%ENDC"           , DEV_MACRO      , M_ENDC           , 0   );
}

bool CPatternCompile::compile()
{
	if( !m_lexer.LoadFile( m_sCpp.toStdString() ) )
	{
		fprintf( stderr, "%s load error!", m_sCpp.toStdString().c_str() );
		return false;
	}
	initSyntaxValid();
	return doCompile();
}

bool CPatternCompile::doCompile()
{
	token_cpp token;
	while ( 1 )
    {
		token = m_lexer.Get();
			 if( token.t == TOKEN_CPP::TOKEN_EOF   ) break;
		else if( token.t == TOKEN_CPP::TOKEN_EOL   ) continue;
		else if( token.t == TOKEN_CPP::TOKEN_SHARP ) //#
		{
			doCMacro();
		}
		else if( token.t == TOKEN_CPP::TOKEN_LETTER )
		{
			doLetter( token.s );
		}
		else if( token.t == TOKEN_CPP::TOKEN_LBRACE )
		{
			if( m_devsyntax.is_function )
			{
				m_devsyntax.is_functionIn = true;
			}
			//Block in
		}
		else if( token.t == TOKEN_CPP::TOKEN_RBRACE )		// }
		{
			if( m_devsyntax.is_function && m_devsyntax.is_functionIn )
			{
				if( m_devsyntax.is_pattern_group )	//&& m_devsyntax.is_set_pattern )	//set_pattern check remove if no needed
				{
					m_patcl.Reset();
					atstring pattern = m_devsyntax.function;
					//debug
					if( 0 ) fprintf( stdout, "%-20s\n", pattern.c_str() );

					if( m_devsyntax.is_default_function )
					{
						doSetDefineCompile( m_devsyntax.default_function );				//common define
						doSetSubCompile   ( m_devsyntax.default_function, false );		//common set_sub
					}
					doSetDefineCompile( pattern );				//common define
					doSetSubCompile   ( pattern, true );		//pattern set_sub
					doCompileFinish( pattern );
				}
				m_devsyntax.is_function   = false;
				m_devsyntax.is_functionIn = false;
				m_devsyntax.is_pattern_group    = false;
				m_devsyntax.is_default_function = false;
				m_devsyntax.default_function = "";
				m_devsyntax.function = "";
			}
		}
		else if( token.t == TOKEN_CPP::TOKEN_CDATATYPE )
		{
			isFunctionDefine();
		}
		else
		{
			//isCallPatternFunction(token.s);
		}
	}
	//debug
	//viewCDefine();
    return true;
}

bool CPatternCompile::doCMacro()
{
	token_cpp token = m_lexer.Get();
	Str2WordMap::iterator it = m_mapSyntax.find( token.s );
	if( it != m_mapSyntax.end() )
	{
		DEVCMD cmd;
		cmd.value = it->second;
		switch( cmd.opt )
		{
			case C_DEFINE:		//#define
			{
				bool bKey = false;
				atstring key = "", val = "";
				while( 1 )
				{
					token = m_lexer.Get();
					if( token.t == TOKEN_EOL )
					{
						if( m_devsyntax.is_ifdef != m_devsyntax.is_ifdef_block )
							break;
						else
							addCDefine( key, val );
						break;
					}
					else if( token.t == TOKEN_LETTER )
					{
						if( bKey == true )
						{
							val += token.s;
							continue;
						}
						key = atstring(token.s).Trim();
						bKey = true;
					}
					else if( token.t == TOKEN_SPACE )
					{
						continue;
					}
					else
					{
						val += token.s;
					}
					//fprintf( stdout, "define %s", token.s.c_str() );
				}
			}
			break;

			case C_IFDEF:		//#ifdef
			{
				bool bKey = false;
				atstring key = "";
				while( 1 )
				{
					token = m_lexer.Get();
					if( token.t == TOKEN_EOL )
					{
						break;
					}
					else if( token.t == TOKEN_LETTER )
					{
						if( bKey == true )
							continue;
						key = atstring(token.s).Trim();
						if( isCDefine(key) )
							m_devsyntax.is_ifdef_block = true;
						else
							m_devsyntax.is_ifdef_block = false;
						m_devsyntax.is_ifdef = true;
						bKey = true;
					}
					else
					{
						continue;
					}
				}
			}
			break;

			case C_SHARP_ELSE:
			{
				m_devsyntax.is_ifdef_block = !m_devsyntax.is_ifdef_block;
				while( 1 )
				{
					token = m_lexer.Get();
					if( token.t == TOKEN_EOL )
					{
						break;
					}
				}
			}
			break;

			case C_ENDIF:
			{
				m_devsyntax.is_ifdef        = false;
				m_devsyntax.is_ifdef_block  = false;
				while( 1 )
				{
					token = m_lexer.Get();
					if( token.t == TOKEN_EOL )
					{
						break;
					}
				}
			}
			break;

			case C_UNDEF:
			{
				bool bKey = false;
				atstring key = "";
				while( 1 )
				{
					token = m_lexer.Get();
					if( token.t == TOKEN_EOL )
					{
						break;
					}
					else if( token.t == TOKEN_LETTER )
					{
						if( bKey == true )
						{
							continue;
						}
						if( isCDefine(key) )
						{
							removeCDefine(key);
						}
					}
					else
					{
						continue;
					}
				}
			}
			break;

			default:
				break;
		}
	}
	return true;
}

bool CPatternCompile::doLetter( atstring pattern_letter)
{
	token_cpp token;
	Str2WordMap::iterator it = m_mapSyntax.find( pattern_letter );
	if( it != m_mapSyntax.end() )
	{
		DEVCMD cmd;
		cmd.value = it->second;
		switch( cmd.command )
		{
			case DEV_FUNCTION:
			{
				switch( cmd.opt )
				{
					case F_START_DEVICE:
					{
						atstring start_device = "";
						bool bLeftBracket = false, bRightBracket = false, bDevice = false;
						while( 1 )
						{
							token = m_lexer.Get();
								 if( token.t == TOKEN_LPARENTHESIS ) bLeftBracket = true;
							else if( token.t == TOKEN_LETTER ) { start_device = token.s; bDevice = true; }
							else if( token.t == TOKEN_RPARENTHESIS ) bRightBracket = true;
							else if( token.t == TOKEN_EOL ) break;
							else
							{
								continue;
							}
						}
						if( bLeftBracket && bDevice && bRightBracket )
						{
							if(  m_devsyntax.is_ifdef == m_devsyntax.is_ifdef_block )
							{
								m_devsyntax.start_device    = start_device;
								m_devsyntax.is_start_device = true;
								//fprintf( stdout, "START_DEVICE : %s\n", m_devsyntax.start_device.c_str() );
							}
						}
					}
					break;

					case F_PROGRAM_START:
					{
						bool bLeftBracket = false, bRightBracket = false, bPgmStart = false;
						while( 1 )
						{
							token = m_lexer.Get();
								 if( token.t == TOKEN_LPARENTHESIS ) bLeftBracket = true;
							else if( token.t == TOKEN_LETTER ) bPgmStart = true;
							else if( token.t == TOKEN_RPARENTHESIS ) bRightBracket = true;
							else if( token.t == TOKEN_EOL ) break;
							else
							{
								continue;
							}
						}
						if( bLeftBracket && bPgmStart && bRightBracket )
						{
							if(  m_devsyntax.is_ifdef == m_devsyntax.is_ifdef_block )
							{
								m_devsyntax.is_programStart = true;
							}
						}
					}
					break;

					case F_DEFINE_FLAG_AZ:
					{
						bool bLeftBracket = false, bRightBracket = false;
						while( 1 )
						{
							token = m_lexer.Get();
								 if( token.t == TOKEN_LPARENTHESIS ) bLeftBracket = true;
							else if( token.t == TOKEN_RPARENTHESIS ) bRightBracket = true;
							else if( token.t == TOKEN_EOL ) break;
							else
							{
								continue;
							}
						}
						if( bLeftBracket && bRightBracket )
						{
							if( m_devsyntax.is_ifdef == m_devsyntax.is_ifdef_block )
							{
								m_nDefineFlag |= (1<<cmd.bit);
								fprintf( stdout, "DEFINE_FLAG_%c ON\n", cmd.bit+'A' );
							}
						}
					}
					break;

					case F_SET_DEFINE:
					{
						std::vector<pat_line_data> vecsdef;
						while( 1 )
						{
							token = m_lexer.Get();
							if( (token.t == TOKEN_SPACE) || (token.t == TOKEN_EOL) )
								continue;
							else
								break;
						}
						atstring data = "";
						if( token.t == TOKEN_LPARENTHESIS )	//(
						{
							token = m_lexer.Get();
							while( token.t != TOKEN_RPARENTHESIS )	//)
							{
								if( token.t == TOKEN_EOL )
								{
									if( data.Trim().length() > 0 )
									{
										pat_line_data linedata;
										linedata.data = data;
										linedata.line = token.l;
										vecsdef.push_back( linedata );
									}
									data = "";
								}
								else
								{									
									data += token.s;
								}
								token = m_lexer.Get();
							}
							if(  m_devsyntax.is_ifdef == m_devsyntax.is_ifdef_block )
							{
								atstring mapkey = m_devsyntax.function.Trim();
								auto it = m_mapsdefine.find( mapkey );
								if( it != m_mapsdefine.end() )
								{
									for( auto item : vecsdef )
										it->second.push_back( item );
								}
								else
								{
									m_mapsdefine[mapkey] = vecsdef;
								}
								auto search = m_mapsdefine.find(mapkey);
								if( search != m_mapsdefine.end() )
								{
									//debug
									if( 0 )
									{
										fprintf( stdout, "================================================================================\n" );
										for( pat_line_data data : search->second )
											fprintf( stdout, "*** %s(%d) : %s\n", search->first.c_str(), data.line, data.data.c_str() );
										fprintf( stdout, "================================================================================\n" );
									}
								}
							}
						}
						else
						{

						}

					}
					break;
					case F_SET_SUB:
					{
						while( 1 )
						{
							token = m_lexer.Get();
							if( (token.t == TOKEN_SPACE) || (token.t == TOKEN_EOL) )
								continue;
							else
								break;
						}

						std::vector<pat_line_data> lvector;
						if( token.t == TOKEN_LPARENTHESIS )	//(
						{
							bool is_subname = false;
							atstring subname = "";
							atstring text    = "";
							while( token.t != TOKEN_RPARENTHESIS )
							{
								if( !is_subname )
								{
									atstring letter = "";
									bool bLetter = false, bComma = false;
									while( 1 )
									{
										token = m_lexer.Get();
										if( token.t == TOKEN_LETTER )
										{
											letter  += token.s;
											bLetter = true;
										}
										else if( token.t == TOKEN_DIGIT )
										{
											letter  += token.s;
											bLetter = true;
										}
										else if ( token.t == TOKEN_SHARP )
										{
											letter  += token.s;
											bLetter = true;
										}
										else if ( token.t == TOKEN_PUNC )
										{
											if( token.s == "," )
												bComma = true;
											else
												letter  += token.s;
											break;
										}
										else
										{
											//fprintf( stderr, "%s(%d) : error SET_SUB syntax '%s' error!\n", letter.c_str(), token.l, token.s.c_str() );
										}
									}
									if( bLetter && bComma )
									{
										subname = letter;
										is_subname = true;
									}
									else
									{
										fprintf( stderr, "%s(%d) : error SET_SUB name not found error '%s' !\n",
												 m_sCpp.toStdString().c_str(), token.l, letter.c_str() );
									}
								}
								else
								{
									token = m_lexer.Get();
									if( token.t == TOKEN_EOL )
									{
										if( text.Trim().size() > 0 )
										{
											pat_line_data line_data;
											line_data.line = token.l;
											line_data.data = text;
											lvector.push_back( line_data );
											text = "";
										}
									}
									else if( token.t == TOKEN_PUNC )
									{
										if( token.s == "%" )
										{
											while( token.t != TOKEN_EOL )
											{
												text += token.s;
												token = m_lexer.Get();
											}
											if( text.Trim().size() > 0 )
											{
												pat_line_data line_data;
												line_data.line = token.l;
												line_data.data = text;
												lvector.push_back( line_data );
												text = "";
											}
										}
										else
										{
											text += token.s;
										}
									}
									else
									{
										text += token.s;
									}
								}
							}
							if(  m_devsyntax.is_ifdef == m_devsyntax.is_ifdef_block )
							{
								std::map< atstring, std::vector< std::pair<atstring, std::vector<pat_line_data> > > >* pmap;
								if( m_devsyntax.is_functionIn && m_devsyntax.is_pattern_group )
									pmap = &m_mappatgroup;
								else
									pmap = &m_mapsub;
								if( lvector.size() )
								{
									std::pair<atstring, std::vector<pat_line_data> > pair = std::make_pair( subname, lvector );
									auto search = pmap->find(m_devsyntax.function);
									if( search != pmap->end() )
									{
										search->second.push_back( pair );
									}
									else
									{
										std::vector< std::pair<atstring, std::vector<pat_line_data> > > sub_vector;
										sub_vector.push_back( pair );

										(*pmap)[m_devsyntax.function] = sub_vector;
									}
								}
							}
						}
						else
						{
							if(  m_devsyntax.is_ifdef == m_devsyntax.is_ifdef_block )
								fprintf(stderr, "%s(%d) : error SET_SUB syntax error!", m_sCpp.toStdString().c_str(), token.l );
						}
					}
					break;

					case F_PATTERN_GROUP:
					{
						atstring text = "";
						//std::vector<cpp_pattern_data> vector;
						while( 1 )
						{
							token = m_lexer.Get();
							if( (token.t == TOKEN_SPACE) || (token.t == TOKEN_EOL) )
								continue;
							else if( token.t == TOKEN_LPARENTHESIS )	//(
							{
								bool bLetter = false;
								atstring pattern_name = "";
								while( token.t != TOKEN_RPARENTHESIS )	//)
								{
									token = m_lexer.Get();
									if( (token.t == TOKEN_SPACE) || (token.t == TOKEN_EOL) )
										continue;
									if( !m_devsyntax.is_pattern_group )
									{
										if( token.t == TOKEN_LETTER )
										{
											pattern_name  = token.s;
											bLetter = true;
										}
									}
								}
								if( bLetter )
								{
									m_devsyntax.is_function = true;
									m_devsyntax.is_pattern_group = true;
									m_devsyntax.function = pattern_name.Trim();
								}
							}
							if( m_devsyntax.is_pattern_group ) break;
							if( token.t == TOKEN_EOF ) break;
						}
					}
					break;

					case F_SET_PATTERN:
					{
						atstring text = "";
						if( (m_devsyntax.is_pattern_group && m_devsyntax.is_functionIn ) )
						{
							bool bLBracket = false, bRBracket = false, bSemiColon= false;

							token_cpp token = m_lexer.Get();
							while( token.t != TOKEN_EOL )
							{
									 if( token.t == TOKEN_LPARENTHESIS ) bLBracket = true;
								else if( token.t == TOKEN_RPARENTHESIS ) bRBracket = true;
								else if( token.t == TOKEN_PUNC         )
								{
									if( token.s == ";" ) bSemiColon = true;
								}
								token = m_lexer.Get();
							}
							if( bLBracket && bRBracket && bSemiColon )
							{
								m_devsyntax.is_set_pattern = true;
							}
						}
					}
					break;

					case F_END_DEVICE:
					{

					}
					break;
				}
			}
			break;

			case DEV_VARIABLE:
			{
			}
			break;

			default:
				break;
		}
	}
	else
	{
		isCallPatternFunction( pattern_letter );
	}
	return true;
}

bool CPatternCompile::addCDefine( atstring key ,atstring val )
{
	for( cppdefine cdef : m_veccdefine )
	{
		if( cdef.key == key )
		{
			return false;
		}
	}
	//if( m_bIfdef ) return false;	//#ifdef not

	cppdefine cdef;
	cdef.key   = key;
	cdef.value = val;
	m_veccdefine.push_back( cdef );
    return true;
}

bool CPatternCompile::isCDefine( atstring key )
{
	for( cppdefine cdef : m_veccdefine )
	{
		if( cdef.key == key )
		{
			return true;
		}
	}
	return false;
}

bool CPatternCompile::removeCDefine( atstring key )
{
	std::vector<cppdefine>::iterator it;
	for( it=m_veccdefine.begin();it!=m_veccdefine.end();it++ )
	{
		if( it->key == key )
		{
			m_veccdefine.erase( it );
			return true;
		}
	}
	return false;
}

//debug function
void CPatternCompile::viewCDefine()
{
	std::vector<cppdefine>::iterator it;
	for( it=m_veccdefine.begin();it!=m_veccdefine.end();it++ )
	{
		fprintf( stdout, "%-20s -> %-30s\n", it->key.c_str(), it->value.c_str() );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CPatternCompile::isFunction
/// \return  true(function) false(not function)
/// \details Check the function by analyzing the token.
/// \details void function() or int function( int xx );
bool CPatternCompile::isFunctionDefine()
{
	bool bLBracket = false, bRBracket = false;
	bool bFuncname = false, bFuncArgs = false;
	bool bLBrace   = false;
	atstring sfunc;
	token_cpp token = m_lexer.Get();
	while( token.t != TOKEN_EOL )
	{
			 if( token.t == TOKEN_LPARENTHESIS ) bLBracket = true;
		else if( token.t == TOKEN_RPARENTHESIS ) bRBracket = true;
		else if( token.t == TOKEN_LBRACE       ) bLBrace   = true;
		else if( token.t == TOKEN_LETTER )
		{
			if( bLBracket ) { bFuncArgs = true; }
			else            { bFuncname = true; sfunc = token.s; }
		}
		token = m_lexer.Get();
	}
	if( m_devsyntax.is_ifdef == m_devsyntax.is_ifdef_block )
	{
		if( bFuncname && bLBracket && bRBracket )
		{
			m_devsyntax.is_function = true;
			m_devsyntax.function    = sfunc;
			if( bLBrace ) m_devsyntax.is_functionIn = true;
			else          m_devsyntax.is_functionIn = false;
			//fprintf( stdout, "--- function : %s\n", sfunc.c_str() );
			return true;
		}
	}
	return false;
}

bool CPatternCompile::isCallPatternFunction( atstring funcname )
{
	if( (m_devsyntax.is_pattern_group && m_devsyntax.is_functionIn ) )
	{
		bool bLBracket = false, bRBracket = false;
		bool bFuncname = true , bFuncArgs = false;
		bool bLBrace   = false, bSemiColon= false;

		token_cpp token = m_lexer.Get();
		while( token.t != TOKEN_EOL )
		{
				 if( token.t == TOKEN_LPARENTHESIS ) bLBracket = true;
			else if( token.t == TOKEN_RPARENTHESIS ) bRBracket = true;
			else if( token.t == TOKEN_LBRACE       ) bLBrace   = true;
			else if( token.t == TOKEN_PUNC         )
			{
				if( token.s == ";" ) bSemiColon = true;
			}
			else if( token.t == TOKEN_LETTER )
			{
				if( bLBracket ) bFuncArgs = true;
			}
			token = m_lexer.Get();
		}
		if( m_devsyntax.is_ifdef != m_devsyntax.is_ifdef_block ) return false;

		if( bFuncname && bLBracket && bRBracket && bSemiColon )	//ex) default_function();
		{
			if( m_mapsdefine.find(funcname) != m_mapsdefine.end() )
			{
				m_devsyntax.is_default_function = true;
				m_devsyntax.default_function = funcname;
				if( 0 )
					fprintf( stdout, "%s(%d) : %s sdef called %s\n", m_sCpp.toStdString().c_str(), token.l, m_devsyntax.function.c_str(), m_devsyntax.default_function.c_str() );
			}
			if( m_mapsub.find(funcname) != m_mapsub.end() )
			{
				m_devsyntax.is_default_function = true;
				m_devsyntax.default_function = funcname;
				if( 0 )
					fprintf( stdout, "%s(%d) : %s sub called %s\n", m_sCpp.toStdString().c_str(), token.l, m_devsyntax.function.c_str(), m_devsyntax.default_function.c_str() );
			}
			return true;
		}
	}
	return false;
}

bool CPatternCompile::doSetDefineCompile(atstring func )	//function name
{
	auto search = m_mapsdefine.find(func);
	if( search != m_mapsdefine.end() )
	{
		std::vector<pat_line_data> vec = search->second;
		m_patcl.SetDefine( func, vec );
		//debug
		if( 0 )
		{
			fprintf( stdout, "[%s sdef list]\n", func.c_str());
			for( pat_line_data data : vec )
			{
				fprintf( stdout, "%s(%d) %s\n", m_sCpp.toStdString().c_str(), data.line, data.data.c_str() );
			}
		}
		// pass vector to PATCL
	}
	return true;
}

bool CPatternCompile::doSetSubCompile(atstring func, bool bPat )	//function name or patter_group_name
{
	std::map< atstring, std::vector< std::pair<atstring, std::vector<pat_line_data> > > >* pMap = nullptr;
	if( bPat ) pMap = &m_mappatgroup;
	else       pMap = &m_mapsub;

	auto search = pMap->find(func);
	if( search != m_mapsub.end() )
	{
		std::vector< std::pair<atstring, std::vector<pat_line_data> > > vec = search->second;
		for( std::pair<atstring, std::vector<pat_line_data> > pair : vec )
		{
			atstring sub_name = pair.first;
			std::vector<pat_line_data> sub_data = pair.second;
			//pass sub(pattern)_name & vector to PATCL
			m_patcl.SetSubCompile( sub_name, sub_data );
			//debug
			if( 0 )
			{
				for( pat_line_data data : sub_data )
				{
					fprintf( stdout, "%-20s (%06d) : %s\n", sub_name.c_str(), data.line, data.data.c_str() );
				}
			}
		}
	}
	return true;
}

bool CPatternCompile::doCompileFinish( atstring pattern )
{
	m_patcl.ProcNoLabelRemain();
	if( m_patcl.GetErrCnt() > 0 )
	{
		m_rVecError =  m_patcl.GetErrors();
	}
	else
	{
		PGDATA* pgdata = m_patcl.GetPatternData();
		fprintf( stdout, "%s Pattern Size:%ld MaxPC:%d\n", pattern.c_str(), pgdata->pcs->size(), m_patcl.GetMaxPc() );
	}
	return true;
}

void CPatternCompile::addPatError( EN_RESULT code, atstring pat, const char* szFmt, ... )
{
    char cBuff[1024 * 10];
    va_list argptr;
    va_start(argptr, szFmt);
    vsnprintf(cBuff, sizeof(cBuff), szFmt, argptr);
    va_end(argptr);
    (void)code;
	(void)pat;
    atstring t;
    m_rVecError.push_back(t);
}
