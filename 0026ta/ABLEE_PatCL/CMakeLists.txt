cmake_minimum_required(VERSION 2.8.11)

#project("attest")
project("atpgc")

if ( NOT DEFINED ENV{ATOS_DIR} )
    message( FATAL_ERROR "Please set $ATOS_DIR system environment variable!!" )
endif()

set( TARGET_LIB ${PROJECT_NAME} )
set( TARGET_EXE "patcl" )
set( PATCL_HPP ${CMAKE_CURRENT_SOURCE_DIR}/include/patcl.hpp )

if ( NOT DEFINED ENV{ATOS_DIR} )
    message( FATAL_ERROR "Please set $ATOS_DIR system environment variable!!" )
endif()

set( INSTALL_PATH $ENV{ATOS_DIR}/lib )	#at6200 /opt/at6200
set( INSTALL_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib )
set( INSTALL_EXE ${CMAKE_CURRENT_SOURCE_DIR}/bin )

#
# Qt5 support
#
# general overview:
#  * [Modern CMake with Qt and Boost](http://www.kdab.com/modern-cmake-with-qt-and-boost/)
#  * [Using CMake with Qt 5](http://www.kdab.com/using-cmake-with-qt-5/)

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_CXX_STANDARD 17 )

#set( CMAKE_BUILD_TYPE Release )     #Debub/Release
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )

set( CMAKE_CXX_FLAGS_DEBUG "-g -fopenmp" )
set( CMAKE_CXX_FLAGS_RELEASE "-o2 -fopenmp" )

ADD_DEFINITIONS( -D_FILE_OFFSET_BITS=64 )
ADD_DEFINITIONS( -D_LARGE_FILE_SOURCE   )
ADD_DEFINITIONS( -DSIMULATION           )

#set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} )
#message( "TESTLIB INSTALL_PREFIX_DIR : " ${INSTALL_PREFIX_DIR} )
if( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_DIR} )
else( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} )
endif( INSTALL_PREFIX_DIR )
#message( "TESTLIB CMAKE_INSTALL_PREFIX : " ${CMAKE_INSTALL_PREFIX} )

FILE( GLOB LIB_HEADERS
	"./include/patcl.hpp"
	"./include/CompType.h"
	"./include/PatternSyntax.h"
	"./include/CurrAddrXSyntax.h"
	"./include/CurrAddrYSyntax.h"
	"./include/Compiler.h"
	"./include/AlpgCompiler.h"
	)

FILE( GLOB LIB_SOURCES
	"./src/patcl.cpp"
	"./src/CompType.cpp"
	"./src/Compiler.cpp"
	"./src/AlpgCompiler.cpp"
)

FILE( GLOB EXE_SOURCES
	"./src/patcl.cpp"
	"./src/CompType.cpp"
	"./src/Compiler.cpp"
	"./src/AlpgCompiler.cpp"
	"./src/main.cpp"
)

#include directory
include_directories( src include  ../ABLEE_Common/lexer ../ABLEE_Common/sys  )

#common library
#link_libraries()

#compile, link option
add_compile_options( -Wall -Wextra -fPIC -fopenmp -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-parameter -Wno-class-memaccess )  #-Wno-unused-but-set-variables

# Generate necessary headers from .ui files. (qmake lets `uic` do this job.)
# hint from [Cross-platform Qt5 project using cmake](http://stackoverflow.com/questions/21174586/cross-platform-qt5-project-using-cmake)
#
#qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_SOURCES})

#
# IMPORTANT: Adding generated headers to target as dependencies
# hint from [unable to include a ui_form header of QT5 in cmake](http://stackoverflow.com/questions/16245147/unable-to-include-a-ui-form-header-of-qt5-in-cmake)
#
add_library   ( ${TARGET_LIB} SHARED ${LIB_SOURCES} ${LIB_HEADERS})		#dynamic library build
add_executable( ${TARGET_EXE} ${EXE_SOURCES} ${LIB_HEADERS})			#executable file build

#
# `target_link_libraries()` rather than `qt5_use_modules()` for CMake 2.8.11 or later
# hint from [CMake & QT5 - QT5_WRAP_UI not generating ui header files](http://stackoverflow.com/questions/25875255/cmake-qt5-qt5-wrap-ui-not-generating-ui-header-files)
#
#target_link_libraries(${TARGET} ${QT5_LIBRARIES})

#install(TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${INSTALL_PATH} )
install(TARGETS ${TARGET_LIB} CONFIGURATIONS Release DESTINATION ${INSTALL_PATH} )
#install(TARGETS ${TARGET_LIB} CONFIGURATIONS Release DESTINATION ${INSTALL_LIB}  )
install(TARGETS ${TARGET_EXE} CONFIGURATIONS Release DESTINATION ${INSTALL_EXE}  )
install(FILES ${LIB_HEADERS}  DESTINATION $ENV{ATOS_DIR}/include/pgc )
