cmake_minimum_required(VERSION 2.8.11)

project(atmain)

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
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_VERBOSE_MAKEFILE OFF )
set( CMAKE_CXX_FLAGS_DEBUG "-g -fopenmp" )
set( CMAKE_CXX_FLAGS_RELEASE "-O2 -fopenmp" )

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Xml REQUIRED)

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif( NOT CMAKE_BUILD_TYPE )
#message( "-- BUILD_TYPE : " ${CMAKE_BUILD_TYPE} )

#message( "IO INSTALL_PREFIX_DIR : " ${INSTALL_PREFIX_DIR} )
if( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_DIR} )
else( INSTALL_PREFIX_DIR )
    set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} )
endif( INSTALL_PREFIX_DIR )
#message( "IO CMAKE_INSTALL_PREFIX : " ${CMAKE_INSTALL_PREFIX} )

set( QT5_LIBRARIES Qt5::Widgets Qt5::Core Qt5::Network Qt5::Xml )

FILE( GLOB_RECURSE HEADERS
    include/common.hpp
    include/engrtab.hpp
    include/chambermgr.hpp
    include/errorpopup.hpp
    include/logview.hpp
    include/mainui.hpp
    include/masstab.hpp
    include/progressdlg.hpp
    include/setup.hpp
    include/statusbar.hpp
    include/slotconnview.hpp
    include/jumpstepdlg.hpp
    include/failrate.hpp
    include/opentimedelay.hpp
    include/versioninfo.hpp
    include/plotchamber.hpp
    include/savedatadlg.hpp
    include/slotview.hpp
    include/plotdatareader.hpp
    include/hsmsmanager.hpp
    include/downloader.hpp
    include/hsmsmessagequeue.hpp
    ../ABLEE_Common/sys/chmbprotocol.hpp
    ../ABLEE_Common/sys/logger.hpp
    ../ABLEE_Common/sys/sysdef.hpp
    ../ABLEE_Common/sys/sysini.hpp
    ../ABLEE_Common/sys/sysver.hpp
    ../ABLEE_Common/sys/syslog.hpp
    ../ABLEE_Common/sys/filemaplib.hpp
    ../ABLEE_Common/sys/secsgem.hpp
    ../ABLEE_Common/sys/secsmsg.hpp
    ../ABLEE_Common/tcp/protocol.hpp
    ../ABLEE_Common/tcp/tcpclient.hpp
    ../ABLEE_Common/tcp/tcpserver.hpp
    ../ABLEE_Common/ipc/ipcinfo.hpp
    ../ABLEE_Common/tool/util.hpp
    ../ABLEE_Common/tcp/commstruct.hpp
    ../ABLEE_Common/tcp/localclient.hpp
    ../ABLEE_Common/tcp/localsocket.hpp
    ../ABLEE_Common/lexer/atstring.hpp
    ../ABLEE_Common/tool/scanner.hpp
    )

set( SOURCES
    src/chambermgr.cpp
    src/engrtab.cpp
    src/errorpopup.cpp
    src/hsmsmanager.cpp
    src/logview.cpp
    src/main.cpp
    src/mainui.cpp
    src/masstab.cpp
    src/setup.cpp
    src/progressdlg.cpp
    src/statusbar.cpp
    src/slotconnview.cpp
    src/jumpstepdlg.cpp
    src/failrate.cpp
    src/opentimedelay.cpp
    src/versioninfo.cpp
    src/plotchamber.cpp
    src/plotdatareader.cpp
    src/savedatadlg.cpp
    src/hsmsmanager.cpp
    src/downloader.cpp
    src/slotview.cpp
    src/hsmsmessagequeue.cpp
    ../ABLEE_Common/src/logger.cpp
    ../ABLEE_Common/src/util.cpp
    ../ABLEE_Common/src/localclient.cpp
    ../ABLEE_Common/src/localsocket.cpp
    ../ABLEE_Common/src/tcpclient.cpp
    ../ABLEE_Common/src/tcpserver.cpp
    ../ABLEE_Common/src/filemaplib.cpp
    ../ABLEE_Common/src/secsgem.cpp
    ../ABLEE_Common/src/secsmsg.cpp
    ../ABLEE_Common/src/scanner.cpp
)

set( UI_SOURCES
    ./ui/chmbdlg.ui
    ./ui/engrtab.ui
    ./ui/errorpopup.ui
    ./ui/logview.ui
    ./ui/mainui.ui
    ./ui/masstab.ui
    ./ui/progressdlg.ui
    ./ui/setup.ui
    ./ui/slotview.ui
    ./ui/statusbar.ui
    ./ui/slotconnview.ui
    ./ui/jumpstepdlg.ui
    ./ui/failrate.ui
    ./ui/opentimedelay.ui
    ./ui/versioninfo.ui
    ./ui/plotchamber.ui
    ./ui/savedatadlg.ui
)

qt5_add_resources(resources MainUi.qrc)

#include directory
include_directories( include src ui ../ABLEE_Common/ipc ../ABLEE_Common/lexer ../ABLEE_Common/src  ../ABLEE_Common/sys ../ABLEE_Common/tcp  ../ABLEE_Common/tool /usr/include/qwt )

#common library
#link_directories( /usr/local/qwt-6.1.6/lib )
link_libraries(qwt-qt5)

#compile, link option
add_compile_options( -Wall -Wextra -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-parameter -fPIC -fopenmp -Wno-format-truncation )

# Generate necessary headers from .ui files. (qmake lets `uic` do this job.)
# hint from [Cross-platform Qt5 project using cmake](http://stackoverflow.com/questions/21174586/cross-platform-qt5-project-using-cmake)

qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_SOURCES} )

# IMPORTANT: Adding generated headers to target as dependencies
# hint from [unable to include a ui_form header of QT5 in cmake](http://stackoverflow.com/questions/16245147/unable-to-include-a-ui-form-header-of-qt5-in-cmake)

add_executable(${TARGET} ${SOURCES} ${UI_GENERATED_HEADERS} ${resources} ${HEADERS} )

# `target_link_libraries()` rather than `qt5_use_modules()` for CMake 2.8.11 or later
# hint from [CMake & QT5 - QT5_WRAP_UI not generating ui header files](http://stackoverflow.com/questions/25875255/cmake-qt5-qt5-wrap-ui-not-generating-ui-header-files)

target_link_libraries(${TARGET} ${QT5_LIBRARIES})

install(TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${CMAKE_INSTALL_PREFIX}/debug/ews)
install(TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX}/release/ews)

install(TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${INSTALL_PATH} )
install(TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION ${INSTALL_PATH} )
