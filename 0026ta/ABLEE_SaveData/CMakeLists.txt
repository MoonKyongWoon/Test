cmake_minimum_required(VERSION 2.8.11)

project( "atsavedata" )
set( TARGET ${PROJECT_NAME} )

if ( NOT DEFINED ENV{ATOS_DIR} )
    message( FATAL_ERROR "Please set $ATOS_DIR system environment variable!!" )
endif()

set( INSTALL_PATH $ENV{ATOS_DIR} )

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
set( CMAKE_CXX_FLAGS_DEBUG "-g -fopenmp" )
set( CMAKE_CXX_FLAGS_RELEASE "-O2 -fopenmp" )

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

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set( QT5_LIBRARIES Qt5::Core  Qt5::Network )

set( SOURCES
    src/main.cpp
    src/savedata.cpp
    src/cumfailblock.cpp
    src/fls.cpp
    src/mrd.cpp
    src/chipid.cpp
    src/eqpr.cpp
    src/chdr.cpp
    src/itemcum.cpp
    src/filesavelib.cpp
    ../ABLEE_Common/src/od.cpp
    ../ABLEE_Common/src/filemaplib.cpp
    ../ABLEE_Common/src/util.cpp
)

FILE( GLOB_RECURSE HEADERS
    include/savedata.hpp
    include/common.hpp
    include/cumfailblock.hpp
    include/fls.hpp
    include/mrd.hpp
    include/chipid.hpp
    include/eqpr.hpp
    include/chdr.hpp
    include/itemcum.hpp
    include/filesavelib.hpp
    ../ABLEE_Common/sys/od.hpp
    ../ABLEE_Common/sys/sysdev.hpp
    ../ABLEE_Common/sys/filemaplib.hpp
    ../ABLEE_Common/tcp/commstruct.hpp
    ../ABLEE_Common/tool/util.hpp
    ../ABLEE_Common/lexer/atstring.hpp
    )

#include directory
include_directories( include ../ABLEE_TestLib/ ../ABLEE_Common/lexer ../ABLEE_Common/tcp ../ABLEE_Common/tool ../ABLEE_Common/sys )

#common library
#link_directories( /home/ablee/at6000/project/anchoby/ABLEE_TestLib/release/cpu )
#link_libraries(TestLib)

#compile, link option
add_compile_options( -Wall -Wextra -Wno-unused-but-set-variable -Wno-format -fPIC -fopenmp -Wno-unused-variable -Wno-unused-parameter)

#qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_SOURCES})

add_executable( ${TARGET} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET} ${QT5_LIBRARIES})

#
# `target_link_libraries()` rather than `qt5_use_modules()` for CMake 2.8.11 or later
# hint from [CMake & QT5 - QT5_WRAP_UI not generating ui header files](http://stackoverflow.com/questions/25875255/cmake-qt5-qt5-wrap-ui-not-generating-ui-header-files)

install(TARGETS ${TARGET} CONFIGURATIONS Debug   DESTINATION ${INSTALL_PATH}/bin)
install(TARGETS ${TARGET} CONFIGURATIONS Release DESTINATION ${INSTALL_PATH}/bin)
