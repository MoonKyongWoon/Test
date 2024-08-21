cmake_minimum_required(VERSION 2.8.11)

project( "atchmbcomm" )

set( TARGET ${PROJECT_NAME} )

if ( NOT DEFINED ENV{ATOS_DIR} )
    message( FATAL_ERROR "Please set $ATOS_DIR system environment variable!!" )
endif()

set( INSTALL_PATH $ENV{ATOS_DIR}/bin )

#
# Qt5 support
#
# general overview:
#  * [Modern CMake with Qt and Boost](http://www.kdab.com/modern-cmake-with-qt-and-boost/)
#  * [Using CMake with Qt 5](http://www.kdab.com/using-cmake-with-qt-5/)

set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_VERBOSE_MAKEFILE OFF )
set( CMAKE_CXX_FLAGS_DEBUG "-g" )
set( CMAKE_CXX_FLAGS_RELEASE "-O2" )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )

#message( "IO INSTALL_PREFIX_DIR : " ${INSTALL_PREFIX_DIR} )
if( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_DIR} )
else( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} )
endif( INSTALL_PREFIX_DIR )
#message( "IO CMAKE_INSTALL_PREFIX : " ${CMAKE_INSTALL_PREFIX} )

find_package( Qt5Core    REQUIRED )
find_package( Qt5Network REQUIRED )
set( QT5_LIBRARIES Qt5::Core Qt5::Network )

set( SOURCES
	src/main.cpp
#    src/chmbinstance.cpp
    src/chmbmanager.cpp
    src/yokogawaplc.cpp
    src/chmbclient.cpp
    "../ABLEE_Common/src/logger.cpp"
    "../ABLEE_Common/src/localserver.cpp"
    "../ABLEE_Common/src/util.cpp"    
)

set( HEADERS
#    include/chmbinstance.hpp
    include/chmbmanager.hpp
    include/yokogawaplc.hpp
    include/chmbclient.hpp
    "../ABLEE_Common/sys/chmbprotocol.hpp"
    "../ABLEE_Common/sys/logger.hpp"
    "../ABLEE_Common/sys/syslog.hpp"
    "../ABLEE_Common/sys/sysini.hpp"
    "../ABLEE_Common/tcp/commstruct.hpp"
    "../ABLEE_Common/tcp/localserver.hpp"
    "../ABLEE_Common/tcp/protocol.hpp"
    "../ABLEE_Common/tool/util.hpp"
)

#include directory
include_directories( src include  ../ABLEE_Common/lexer ../ABLEE_Common/sys ../ABLEE_Common/tcp ../ABLEE_Common/tool )

#compile, link option
add_compile_options( -Wall -Wextra -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-variable -fPIC -fopenmp )

# Generate necessary headers from .ui files. (qmake lets `uic` do this job.)
# hint from [Cross-platform Qt5 project using cmake](http://stackoverflow.com/questions/21174586/cross-platform-qt5-project-using-cmake)
#
#qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_SOURCES})

#
# IMPORTANT: Adding generated headers to target as dependencies
# hint from [unable to include a ui_form header of QT5 in cmake](http://stackoverflow.com/questions/16245147/unable-to-include-a-ui-form-header-of-qt5-in-cmake)
#

add_executable( ${TARGET} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET} ${QT5_LIBRARIES})

#
# `target_link_libraries()` rather than `qt5_use_modules()` for CMake 2.8.11 or later
# hint from [CMake & QT5 - QT5_WRAP_UI not generating ui header files](http://stackoverflow.com/questions/25875255/cmake-qt5-qt5-wrap-ui-not-generating-ui-header-files)

install(TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${INSTALL_PATH} )
install(TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION ${INSTALL_PATH} )
