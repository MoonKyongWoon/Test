cmake_minimum_required(VERSION 2.8.11)

#project("attest")
project("testlib")

set( TARGET ${PROJECT_NAME} )

if ( NOT DEFINED ENV{ATOS_DIR} )
    message( FATAL_ERROR "Please set $ATOS_DIR system environment variable!!" )
endif()

set( LIB_PATH $ENV{ATOS_DIR}/lib )
set( INSTALL_PATH $ENV{ATOS_DIR}/lib )

#
# Qt5 support
#
# general overview:
#  * [Modern CMake with Qt and Boost](http://www.kdab.com/modern-cmake-with-qt-and-boost/)
#  * [Using CMake with Qt 5](http://www.kdab.com/using-cmake-with-qt-5/)

set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_CXX_STANDARD 17 )

#set( CMAKE_BUILD_TYPE Release )     #Debub/Release
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )

set( CMAKE_CXX_FLAGS_DEBUG "-g -fopenmp" )
set( CMAKE_CXX_FLAGS_RELEASE "-O2 -fopenmp" )
#for gdb test temp option
#set( CMAKE_CXX_FLAGS_RELEASE "-g -fopenmp" )

ADD_DEFINITIONS( -D_FILE_OFFSET_BITS=64 )
ADD_DEFINITIONS( -D_LARGE_FILE_SOURCE   )
#ADD_DEFINITIONS( -DSIMULATION           )	#PCIE I/F

#set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} )
#message( "TESTLIB INSTALL_PREFIX_DIR : " ${INSTALL_PREFIX_DIR} )
if( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_DIR} )
else( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} )
endif( INSTALL_PREFIX_DIR )
#message( "TESTLIB CMAKE_INSTALL_PREFIX : " ${CMAKE_INSTALL_PREFIX} )

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(QT5_LIBRARIES Qt5::Core Qt5::Network )

FILE(GLOB_RECURSE HEADERS
    "*.hpp"
    "../ABLEE_Common/ipc/ipc_info.hpp"
    "../ABLEE_Common/lexer/atstring.hpp"
    "../ABLEE_Common/lexer/lexer.hpp"
    "../ABLEE_Common/sys/api.hpp"
    "../ABLEE_Common/sys/bib.hpp"
    "../ABLEE_Common/sys/basestruct.hpp"
    "../ABLEE_Common/sys/bitchart.hpp"
    "../ABLEE_Common/sys/buildver.hpp"
    "../ABLEE_Common/sys/dutmap.hpp"
    "../ABLEE_Common/sys/dbm.hpp"
    "../ABLEE_Common/sys/dfm.hpp"
    "../ABLEE_Common/sys/dle.hpp"
    "../ABLEE_Common/sys/deviceif.h"
    "../ABLEE_Common/sys/deviceapp.hpp"
    "../ABLEE_Common/sys/mainapp.hpp"
    "../ABLEE_Common/sys/log.hpp"
    "../ABLEE_Common/sys/hw.hpp"
    "../ABLEE_Common/sys/io.hpp"
    "../ABLEE_Common/sys/pg.hpp"
    "../ABLEE_Common/sys/sys.hpp"
    "../ABLEE_Common/sys/sysdev.hpp"
    "../ABLEE_Common/sys/sysdef.hpp"
    "../ABLEE_Common/sys/sysver.hpp"
    "../ABLEE_Common/sys/syslog.hpp"
    "../ABLEE_Common/sys/systype.hpp"
    "../ABLEE_Common/sys/swcontrol.h"
    "../ABLEE_Common/sys/tg.hpp"
    "../ABLEE_Common/sys/pg.hpp"
    "../ABLEE_Common/sys/pps.hpp"
    "../ABLEE_Common/sys/ubm.hpp"
    "../ABLEE_Common/sys/vector.hpp"
    "../ABLEE_Common/tcp/commstruct.hpp"
    "../ABLEE_Common/tcp/devsocket.hpp"
    "../ABLEE_Common/tcp/tcpsocket.hpp"
    "../ABLEE_Common/tcp/protocol.hpp"
    "../ABLEE_Common/tool/util.hpp"
    )

FILE( GLOB SOURCES
    "./src/api.cpp"
    "./src/bib.cpp"
    "./src/dbm.cpp"
    "./src/dfm.cpp"
    "./src/deviceapp.cpp"
    "./src/log.cpp"
    "./src/hw.cpp"
    "./src/io.cpp"
    "./src/mainapp.cpp"
    "./src/pg.cpp"
    "./src/pps.cpp"
    "./src/systype.cpp"
    "./src/sys.cpp"
    "./src/tg.cpp"
    "./src/ubm.cpp"
    "./src/vector.cpp"
    "../ABLEE_Common/src/devsocket.cpp"
    "../ABLEE_Common/src/tcpsocket.cpp"
    "../ABLEE_Common/src/util.cpp"
)

#include directory
include_directories( src include  ../ABLEE_Common/lexer ../ABLEE_Common/sys ../ABLEE_Common/tcp ../ABLEE_Common/tool  ../ABLEE_PatCL/include )

#common library
link_directories( ${LIB_PATH} )
link_libraries(atpgc)

#compile, link option
add_compile_options( -Wall -Wextra -fPIC -fopenmp -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-parameter -Wno-type-limits -Wno-sign-compare -Wno-format-overflow -Wno-stringop-truncation  -Wno-stringop-overflow )

# Generate necessary headers from .ui files. (qmake lets `uic` do this job.)
# hint from [Cross-platform Qt5 project using cmake](http://stackoverflow.com/questions/21174586/cross-platform-qt5-project-using-cmake)
#
#qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_SOURCES})

#
# IMPORTANT: Adding generated headers to target as dependencies
# hint from [unable to include a ui_form header of QT5 in cmake](http://stackoverflow.com/questions/16245147/unable-to-include-a-ui-form-header-of-qt5-in-cmake)
#
#add_library( ${TARGET} STATIC ${SOURCES} ${HEADERS})
add_library( ${TARGET} SHARED ${SOURCES} ${HEADERS})

#
# `target_link_libraries()` rather than `qt5_use_modules()` for CMake 2.8.11 or later
# hint from [CMake & QT5 - QT5_WRAP_UI not generating ui header files](http://stackoverflow.com/questions/25875255/cmake-qt5-qt5-wrap-ui-not-generating-ui-header-files)
#
target_link_libraries(${TARGET} ${QT5_LIBRARIES})

#install(TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${CMAKE_INSTALL_PREFIX}/debug/cpu)
#install(TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX}/release/cpu)

install(TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${INSTALL_PATH} )
install(TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION ${INSTALL_PATH} )

install(DIRECTORY ../ABLEE_Common/lexer  DESTINATION $ENV{ATOS_DIR}/include)
install(DIRECTORY ../ABLEE_Common/sys    DESTINATION $ENV{ATOS_DIR}/include)
install(DIRECTORY ../ABLEE_Common/tcp    DESTINATION $ENV{ATOS_DIR}/include)
install(DIRECTORY ../ABLEE_Common/tool   DESTINATION $ENV{ATOS_DIR}/include)
