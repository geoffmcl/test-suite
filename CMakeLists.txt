# test suite - bunch of test code 
# 20200528 - build in DELL03 MSVC 16 2019
# 20140827 repo : https://github.com/geoffmcl/test-suite
# test - 20130316 - Initial cut

cmake_minimum_required (VERSION 3.6)

if(COMMAND cmake_policy)
  cmake_policy (SET CMP0074 NEW)
endif()

include (CheckFunctionExists)
include (CheckCSourceCompiles)
include (CheckCXXSourceCompiles)
include (CheckIncludeFile)
include (CheckSymbolExists)

project (Test)

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

option( TEST_MPFR        "Turn ON to add MPFR test."                 OFF )
option( TEST_CGAL        "Turn ON to add CGAL test."                 OFF )
option( TEST_CF_LOG      "Turn ON to add CF LOG test."               OFF )
option( USE_WSOCK2_LIB   "Turn ON to use Winsock2.h and ws2_32.lib"  ON  )
option( TEST_POSTGRESQL  "Turn ON to add test-sql connection"        OFF )
option( TEST_MAG_VAR     "Turn ON to add test_magvar."               OFF  )
option( TEST_MMFIO       "Turn ON to test memory mapping file IO"    ON   )
option( USE_CURL_LIB     "Turn ON the use CURL library."             ON  )
option( ADD_SG_MATH      "Turn ON some SG math tests"                ON  )
option( ADD_TEST_CODESET "Turn ON some codeset tests"                OFF )

message( STATUS "*** USING CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" )
message( STATUS "*** System: ${CMAKE_SYSTEM_NAME}" )

# Setup MSVC 3rd party directories
# include( ConfigureMsvc3rdParty )

if(WIN32 AND MSVC)
    # turn off various warnings
    foreach(warning 4244 4251 4267 4275 4290 4786 4305 4996)
        SET(WARNING_FLAGS "${WARNING_FLAGS} /wd${warning}")
    endforeach()

    set(MSVC_FLAGS "-DNOMINMAX -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D__CRT_NONSTDC_NO_WARNINGS -Dstrdup=_strdup")
    #if (${MSVC_VERSION} GREATER 1599)
    #    set( MSVC_LD_FLAGS "/FORCE:MULTIPLE" )
    #endif ()
    set(NOMINMAX 1)
    # to distinguish between debug and release lib in windows MSVC build
    set( CMAKE_DEBUG_POSTFIX "d" ) # little effect in unix
else()
    set(TEST_MMFIO 0)
    message(STATUS "*** Memory mapping still to be ported to unix mmap")
    set(WARNING_FLAGS "${WARNING_FLAGS} -Wno-trigraphs")
    # Need  -std=c++11 to remove some warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# not sure this is required
# set (BOOST_CXX_FLAGS "-DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION -DBOOST_BIMAP_DISABLE_SERIALIZATION")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${MSVC_FLAGS} -D_REENTRANT ${BOOST_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MSVC_LD_FLAGS}")

message(STATUS "set CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message(STATUS "set CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message(STATUS "set CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")

MACRO ( CHECK_SYMBOL symbol header variable )
    check_c_source_compiles("
        #include <${header}>
        int main() {
            ${symbol} val = 1;
            return 0; }" ${variable} )
ENDMACRO ()

check_symbol_exists( int64_t "stdint.h" HAVE_INT64_T)
if (HAVE_UINT64_T)
    message(STATUS "*** Found 'int64_t' in 'stdint.h' using check_symbol_exists.")
else ()
    CHECK_SYMBOL( int64_t stdint.h HAVE_UINT64_T )
    if (HAVE_UINT64_T)
        message(STATUS "*** Found 'int64_t' in 'stdint.h' using compile.")
    else ()
        message(STATUS "*** 'int64_t' in 'stdint.h' NOT FOUND!")
    endif ()
endif ()

# find_package(Qt4 PATHS "C:/QtSDK" REQUIRED)
#  find_package(Qt4 REQUIRED)

#  Boost_FOUND            - True if headers and requested libraries were found
#  Boost_INCLUDE_DIRS     - Boost include directories
#  Boost_LIBRARY_DIRS     - Link directories for Boost libraries
#  Boost_LIBRARIES        - Boost component libraries to be linked
find_package(Boost)
if(Boost_FOUND)
    message( STATUS "*** Boost found inc '${Boost_INCLUDE_DIRS}'")
    include_directories( ${Boost_INCLUDE_DIRS} )
    add_definitions( -DBoost_FOUND )
else()
    message( STATUS "*** Boost NOT FOUND")
endif()
    
if (TEST_MPFR)
    add_definitions( -DADD_MPFR_TEST -DMPFR_STATIC )
    include_directories( C:/FG/17/3rdParty/include )
    link_directories( C:/FG/17/3rdParty/lib )
    list(APPEND EXTRA_LIBS optimized mpfr debug mpfrd)
endif ()

if (TEST_CGAL)
    add_definitions( -DADD_CGAL_TEST )
    include_directories( C:/FG/17/3rdParty/include )
    link_directories( C:/FG/17/3rdParty/lib C:/FG/17/boost_1_53_0/lib )
    list(APPEND EXTRA_LIBS optimized CGAL_Core-vc100-mt-4.1.lib debug CGAL_Core-vc100-mt-gd-4.1.lib)
endif ()

#  CURL_INCLUDE_DIRS   - where to find curl/curl.h, etc.
#  CURL_LIBRARIES      - List of libraries when using curl.
#  CURL_FOUND          - True if curl found.
#  CURL_VERSION_STRING - the version of curl found (since CMake 2.8.8)
if (USE_CURL_LIB)
    find_package(CURL)
    if (CURL_FOUND)
        message(STATUS "*** Found CURL library version ${CURL_VERSION_STRING}" )
        include_directories( ${CURL_INCLUDE_DIRS} )
        ###link_directories( C:/FG/17/3rdParty/lib )
        list(APPEND EXTRA_LIBS ${CURL_LIBRARIES} )
        message(STATUS "*** CURL lib ${CURL_LIBRARIES} inc ${CURL_INCLUDE_DIRS}" )
        add_definitions( -DHAVE_CURL_LIB )
        if (MSVC)
            #   ZLIB_INCLUDE_DIRS   - where to find zlib.h, etc.
            #   ZLIB_LIBRARIES      - List of libraries when using zlib.
            #   ZLIB_FOUND          - True if zlib found.
            find_package( ZLIB )
            if (ZLIB_FOUND)
                message(STATUS "*** ZLIB found inc ${ZLIB_INCLUDE_DIRS}, lib ${ZLIB_LIBRARIES}" )
                include_directories( ${ZLIB_INCLUDE_DIRS} )
                list(APPEND EXTRA_LIBS ${ZLIB_LIBRARIES} )
            else ()
                message(FATAL_ERROR "*** ZLIB NOT FOUND!" )
            endif ()    
            # using a local FindCURL.cmake, find static libcurls.lib here
            add_definitions( -DCURL_STATICLIB )
            list(APPEND EXTRA_LIBS Ws2_32 Wldap32 Crypt32 )
        endif ()
    else ()
        message(STATUS "*** Search for CURL library FAILED" )
    endif ()
else ()
    message(STATUS "*** No search for CURL library" )
endif ()

# SIMGEAR_FOUND, if false, do not try to link to SimGear
# SIMGEAR_CORE_LIBRARIES, a list of the core static libraries
# SIMGEAR_LIBRARIES, a list of all the static libraries (core + scene)
# SIMGEAR_INCLUDE_DIR, where to find the headers
###find_package(SimGear)
find_package(SimGear 2.11.0)
if (SIMGEAR_FOUND)
    message(STATUS "*** SimGear FOUND")
    list(APPEND EXTRA_LIBS ${SIMGEAR_LIBRARIES})
    add_definitions( -DHAVE_SIMGEAR )
    include_directories( ${SIMGEAR_INCLUDE_DIR} )
    if (UNIX)
        if (Threads_FOUND)
            list(APPEND EXTRA_LIBS ${CMAKE_THREAD_LIBS_INIT})
            message(STATUS "*** Threads FOUND lib ${CMAKE_THREAD_LIBS_INIT}")
        else ()
            message(STATUS "*** Threads NOT FOUND")
        endif ()
    endif ()    
else ()
    message(STATUS "*** SimGear NOT FOUND")
endif ()

if (SIMGEAR_FOUND AND ADD_SG_MATH)
    add_definitions( -DADD_SG_MATH )
endif ()

if (TEST_CF_LOG)
    if (SIMGEAR_FOUND)
        add_definitions( -DADD_CF_LOG_TEST )
        include_directories( C:/FG/17/3rdParty/include C:/FG/17/install/msvc100/simgear/include )
        link_directories( C:/FG/17/3rdParty/lib C:/FG/17/install/msvc100/simgear/lib )
    else ()
        message(STATUS "*** Unable to enable TEST_CF_LOG since SG NOT FOUND")
    endif ()
endif ()
if (TEST_MAG_VAR)
    if (SIMGEAR_FOUND)
        add_definitions( -DTEST_MAG_VAR )
    else ()
        message(STATUS "Unable to add TEST_MAG_VAR, due SimGear NOT found!")
    endif ()
endif ()

if (TEST_POSTGRESQL)
    message(STATUS "*** TEST_POSTGRESQL is ON")
    find_package(PostgreSQL)
    if (PostgreSQL_FOUND)
        message(STATUS "*** Found PostgreSQL lib ${PostgreSQL_LIBRARIES} inc ${PostgreSQL_INCLUDE_DIRS}")
        add_definitions( -DDO_POSTGRESQL_TEST -DADD_POSTGRESQL_TEST )
        list(APPEND EXTRA_LIBS ${PostgreSQL_LIBRARIES})
        include_directories( ${PostgreSQL_INCLUDE_DIRS} )
        if (MSVC)
            link_directories( ${PostgreSQL_LIBRARY_DIR} )
        endif ()
    else ()
        message(STATUS "*** PostgresSQL NOT FOUND")
        set( TEST_POSTGRESQL OFF )
        set( _test_ENV $ENV{PostgreSQL_ROOT} )
        if ( _test_ENV )
            message(STATUS "*** ENV PostgreSQL_ROOT=${_test_ENV}, but this has NOT helped!")
        else ()
            message(STATUS "*** Maybe set ENV PostgreSQL_ROOT to point to the installation")
        endif ()
    endif ()

else ()
    message(STATUS "*** TEST_POSTGRESQL is OFF")
endif ()

if (MSVC)
    if (USE_WSOCK2_LIB)
        ##list(APPEND EXTRA_LIBS wsock32)
        list(APPEND EXTRA_LIBS ws2_32)
        add_definitions( -DUSE_WSOCK2_LIB )
    else ()
        list(APPEND EXTRA_LIBS wsock32)
        ###list(APPEND EXTRA_LIBS ws2_32)
    endif ()
endif ()

########################################################
check_function_exists(asprintf HAVE_ASPRINTF)
# UGH! For some reason this FAILS in Windows!!!
check_function_exists(vsnprintf HAVE_VSNPRINTF)
check_function_exists(_vsnprintf HAVE__VSNPRINTF)
# An attempt to overcome the FAILS in Windows, 64-bit, 
# and it compiles, as it should, so do HAVE_VSNPRINTF!!!
if (NOT HAVE_VSNPRINTF)
check_c_source_compiles(
"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
void test(const char *frm, ...) {
    int size = 0;
    va_list args;
    va_start(args, frm);
    size = vsnprintf(NULL, size, frm, args);
    va_end(args);
}
int main(int argc, char **argv) {
    test(\"%s\", argv[0] );
    return (0);
}
"
    CAN_COMP_VSNPRINTF)
    if (CAN_COMP_VSNPRINTF)
        message(STATUS "*** check_function_exists(vsnprintf HAVE_VSNPRINTF) FAILED")
        set(HAVE_VSNPRINTF 1)
    else ()
        message(STATUS "*** can not FIND nor COMPILE 'vsnprinf'!")
    endif ()
endif ()

# configure a header file to pass some of the CMake settings to the source code
configure_file ( "${PROJECT_SOURCE_DIR}/test_conf.h.cmake" "${PROJECT_BINARY_DIR}/test_conf.h" )

include_directories( ${PROJECT_BINARY_DIR} )

####################################################
### COMMON LIBRARY
##################
set(dir src)
set(name libutils)
set(${name}_SRCS
    ${dir}/sprtf.cxx
    ${dir}/utils.cxx
    ${dir}/getline.c
    )
set(${name}_HDRS
    ${dir}/sprtf.hxx
    ${dir}/utils.hxx
    ${dir}/getline.h
    )
    
# 'asprintf' NOT found    
if (NOT HAVE_ASPRINTF)
    # NOTE: This contains 'vsnprintf' and compiles fine?!
    list(APPEND ${name}_SRCS ${dir}/vsprtf.cxx)
    list(APPEND ${name}_HDRS ${dir}/vsprtf.hxx)
endif ()    
if (MSVC)
    list(APPEND ${name}_SRCS ${dir}/winErr.cpp)
    list(APPEND ${name}_HDRS ${dir}/winErr.hpp)
endif ()
if (TEST_MMFIO)
    if(Boost_FOUND)
        list(APPEND ${name}_SRCS ${dir}/MMFIODef.cpp)
        list(APPEND ${name}_HDRS ${dir}/MMFIODef.h  )
        add_definitions( -DADD_MMFIO_TEST )
    else()
        set(TEST_MMFIO 0)
        message(STATUS "*** Memory mapping need Boost")
    endif()
endif ()
if (SIMGEAR_FOUND AND ADD_SG_MATH)
    list(APPEND ${name}_SRCS ${dir}/sg-maths.cxx)
    list(APPEND ${name}_HDRS ${dir}/sg-maths.hxx)
endif ()
add_library( ${name} ${${name}_SRCS} ${${name}_HDRS} )
list(APPEND EXTRA_LIBS ${name})

######################################################
### EXECUTABLE 
###############
set(name tests)
set( test_SRCS 
    ${dir}/test.cxx 
    ${dir}/cf-log.cxx
    ${dir}/test-array.c
    ${dir}/gettimeofday.c 
    ${dir}/test-intersect.cxx
    ${dir}/test-map.cxx
    ${dir}/global.cxx
    ${dir}/utf8/test-utf8.cxx
    ${dir}/test-wildcard.cxx
    ${dir}/test-stg-inc.cxx
    ${dir}/test-color.cxx
    ${dir}/test-tmpname.cxx
    ${dir}/test-secs.cxx
    ${dir}/test-scanf.cxx
    ${dir}/strtoimax.cxx
    ${dir}/test-open.cxx
    ${dir}/test-knums.cxx
    ${dir}/test-trim.cxx
    ${dir}/test-area.cxx
    ${dir}/test-http.cxx
    ${dir}/test-ofstream.cxx
    ${dir}/test-rand.cxx
    ${dir}/test-bad-alloc.cxx
    ${dir}/test-tidy.cxx
    ${dir}/test-deldir.cxx
    ${dir}/test-text.c
    ${dir}/test-ptrdiff.cxx
    )
set( test_HDRS
    ${dir}/test.hxx 
    ${dir}/cf-log.hxx
    ${dir}/test-array.h 
    ${dir}/gettimeofday.h 
    ${dir}/test-intersect.hxx
    ${dir}/test-map.hxx
    ${dir}/global.hxx
    ${dir}/utf8/test-utf8.hxx
    ${dir}/test-wildcard.hxx
    ${dir}/test-stg-inc.hxx
    ${dir}/test-color.hxx
    ${dir}/test-tmpname.hxx
    ${dir}/test-secs.hxx
    ${dir}/test-scanf.hxx
    ${dir}/test-open.hxx
    ${dir}/test-knums.hxx
    ${dir}/test-trim.hxx
    ${dir}/test-area.hxx
    ${dir}/test-http.hxx
    ${dir}/test-ofstream.hxx
    ${dir}/test-rand.hxx
    ${dir}/test-bad-alloc.hxx
    ${dir}/test-tidy.hxx
    ${dir}/test-deldir.hxx
    ${dir}/test-text.h
    )

if (ADD_TEST_CODESET)
    list(APPEND test_SRCS ${dir}/test-codeset.c)
    list(APPEND test_HDRS ${dir}/test-codeset.h)
    add_definitions( -DADD_TEST_CODESET )
endif ()

if (TEST_MMFIO)
    list(APPEND test_SRCS ${dir}/test-mmap.cxx)
    list(APPEND test_HDRS ${dir}/test-mmap.hxx)
endif ()    
if (TEST_POSTGRESQL)
    list(APPEND test_SRCS ${dir}/test-sql.cxx)
    list(APPEND test_HDRS ${dir}/test-sql.hxx)
endif ()
if (MSVC)
    list(APPEND test_SRCS ${dir}/test-fullpath.cxx ${dir}/test-redon.cxx)
    list(APPEND test_HDRS ${dir}/test-fullpath.hxx ${dir}/test-redon.hxx)
endif ()
if (SIMGEAR_FOUND AND TEST_MAG_VAR)
    list(APPEND test_SRCS ${dir}/test-magvar.cxx)
    list(APPEND test_HDRS ${dir}/test-magvar.hxx)
endif ()
if (SIMGEAR_FOUND AND ADD_SG_MATH)
    list(APPEND test_SRCS ${dir}/test-math.cxx)
    list(APPEND test_HDRS ${dir}/test-math.hxx)
endif ()
add_executable( ${name} ${test_SRCS} ${test_HDRS} )
if (EXTRA_LIBS)
    target_link_libraries( ${name} ${EXTRA_LIBS} )
    message(STATUS "*** target_link_libraries( ${name} ${EXTRA_LIBS} )")
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install - *** FIXME: NOTE SPECIAL PERSONAL INSTALL LOCATION ***
if (MSVC)
    install( TARGETS ${name} DESTINATION C:/MDOS )
else ()
    install( TARGETS ${name} DESTINATION bin )
endif ()

if (SIMGEAR_FOUND AND TEST_MAG_VAR)
    set(name magvar)
    add_definitions( -DTEST_NHV_HACKS )
    set(${name}_SRCS 
        ${dir}/magvar.cxx
        ${dir}/coremag.cxx
        )
    set(${name}_HDRS
        ${dir}/coremag.hxx
        )
    add_executable( ${name} ${${name}_SRCS} ${${name}_HDRS} )
    target_link_libraries( ${name} ${EXTRA_LIBS} )
    if (MSVC)
        set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
    endif ()    
    # deal with install - *** FIXME: NOTE SPECIAL PERSONAL INSTALL LOCATION ***
    if (MSVC)
        install( TARGETS ${name} DESTINATION C:/MDOS )
    else ()
        install( TARGETS ${name} DESTINATION bin )
    endif ()
endif ()

if (MSVC)
    # Interesting Stuff
    # NOTE: TO use global memory space, need to run these as 'Aministrator', so
    # note the SPECIAL LINK_FLAG. When running these, a dialog will appear
    # User Account Control, with the serious message like 'Do you want to allow
    # the following program from an unknown publisher to make changes to this computer'.
    # The default is NO. If YES clicked it will be run in a new console.
    set(name mmap1)
    add_executable( ${name} ${dir}/${name}.cpp )
    target_link_libraries( ${name} ${EXTRA_LIBS} )
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
    set_target_properties( ${name} PROPERTIES LINK_FLAGS "/level='requireAdministrator' /uiAccess='false'" )
    
    set(name mmap2)
    add_executable( ${name} ${dir}/${name}.cpp )
    target_link_libraries( ${name} ${EXTRA_LIBS} )
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
    set_target_properties( ${name} PROPERTIES LINK_FLAGS "/level='requireAdministrator' /uiAccess='false'" )
    
endif ()

######################################################################
# sockets stuff - TODO: Port to linux
set(name nslookup2)
set(dir src/sockets)
add_executable( ${name} ${dir}/${name}.cxx )
if (EXTRA_LIBS)
    target_link_libraries( ${name} ${EXTRA_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
# deal with install - *** FIXME: NOTE SPECIAL PERSONAL INSTALL LOCATION ***
if (MSVC)
    install( TARGETS ${name} DESTINATION C:/MDOS )
else ()
    install( TARGETS ${name} DESTINATION bin )
endif ()
#######################################################################
# fold this project into here
set(name tester)
set(dir tester)
add_executable( ${name} ${dir}/${name}.cxx )
if (EXTRA_LIBS)
    target_link_libraries( ${name} ${EXTRA_LIBS} )
endif ()
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()

#######################################################################
message(STATUS "*** set CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message(STATUS "*** set CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message(STATUS "*** set CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "*** set CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

# eof

