# test - 20130316 - Just testing CMAKE and other things

cmake_minimum_required (VERSION 2.8)

include (CheckFunctionExists)
include (CheckCSourceCompiles)
include (CheckCXXSourceCompiles)
include (CheckIncludeFile)
include (CheckSymbolExists)

project (Test)

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

option( TEST_MPFR        "Turn ON to add MPFR test."                 OFF )
option( TEST_CGAL        "Turn ON to add MPFR test."                 OFF )
option( TEST_CF_LOG      "Turn ON to add CF LOG test."               OFF )
option( USE_WSOCK2_LIB   "Turn ON to use Winsock2.h and ws2_32.lib"  ON  )
option( TEST_POSTGRESQL  "Turn ON to add test-sql connection"        OFF )
option( TEST_MAG_VAR     "Turn ON to add test_magvar."               OFF  )
option( TEST_MMFIO       "Trun ON to test memory mapping file IO"    ON   )


message( STATUS "*** USING CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" )
message( STATUS "*** System: ${CMAKE_SYSTEM_NAME}" )

# to distinguish between debug and release lib in windows
if (WIN32)
    set( CMAKE_DEBUG_POSTFIX "d" ) # little effect in unix
endif (WIN32)

# Setup MSVC 3rd party directories
include( ConfigureMsvc3rdParty )

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
endif()

set (BOOST_CXX_FLAGS "-DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION -DBOOST_BIMAP_DISABLE_SERIALIZATION")

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
### find_package(Boost   REQUIRED)
include_directories( C:/FG/17/boost_1_53_0 )

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
else ()
    message(STATUS "*** SimGear NOT FOUND")
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
if (MSVC)
    list(APPEND ${name}_SRCS ${dir}/winErr.cpp)
    list(APPEND ${name}_HDRS ${dir}/winErr.hpp)
endif ()
if (TEST_MMFIO)
    list(APPEND ${name}_SRCS ${dir}/MMFIODef.cpp)
    list(APPEND ${name}_HDRS ${dir}/MMFIODef.h  )
    add_definitions( -DADD_MMFIO_TEST )
endif ()
add_library( ${name} ${${name}_SRCS} ${${name}_HDRS} )
list(APPEND EXTRA_LIBS ${name})

######################################################
### EXECUTABLE 
###############
set( test_SRCS 
    ${dir}/test.cxx 
    ${dir}/cf-log.cxx
    ${dir}/test-array.c
    ${dir}/gettimeofday.c 
    ${dir}/test-magvar.cxx
    ${dir}/test-intersect.cxx
    ${dir}/test-map.cxx
    ${dir}/global.cxx
    ${dir}/utf8/test-utf8.cxx
    ${dir}/test-codeset.c
    ${dir}/test-wildcard.cxx
    ${dir}/test-math.cxx
    ${dir}/test-stg-inc.cxx
    ${dir}/test-color.cxx
    ${dir}/test-tmpname.cxx
    ${dir}/test-mmap.cxx
    ${dir}/test-secs.cxx
    ${dir}/test-scanf.cxx
    ${dir}/strtoimax.cxx
    ${dir}/test-open.cxx
    )
set( test_HDRS
    ${dir}/test.hxx 
    ${dir}/cf-log.hxx
    ${dir}/test-array.h 
    ${dir}/gettimeofday.h 
    ${dir}/test-magvar.hxx
    ${dir}/test-intersect.hxx
    ${dir}/test-map.hxx
    ${dir}/global.hxx
    ${dir}/utf8/test-utf8.hxx
    ${dir}/test-codeset.h
    ${dir}/test-wildcard.hxx
    ${dir}/test-math.hxx
    ${dir}/test-stg-inc.hxx
    ${dir}/test-color.hxx
    ${dir}/test-tmpname.hxx
    ${dir}/test-mmap.hxx
    ${dir}/test-secs.hxx
    ${dir}/test-scanf.hxx
    ${dir}/test-open.hxx
    )
if (TEST_POSTGRESQL)
    list(APPEND test_SRCS ${dir}/test-sql.cxx)
    list(APPEND test_HDRS ${dir}/test-sql.hxx)
endif ()
if (MSVC)
    list(APPEND test_SRCS ${dir}/test-fullpath.cxx ${dir}/test-redon.cxx)
    list(APPEND test_HDRS ${dir}/test-fullpath.hxx ${dir}/test-redon.hxx)
endif ()

add_executable( test ${test_SRCS} ${test_HDRS} )
if (EXTRA_LIBS)
    target_link_libraries( test ${EXTRA_LIBS} )
    message(STATUS "*** target_link_libraries( test ${EXTRA_LIBS} )")
endif (EXTRA_LIBS)
set_target_properties( test PROPERTIES DEBUG_POSTFIX d )


if (SIMGEAR_FOUND)
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
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
    # deal with install - *** FIXME: NOTE SPECIAL PERSONAL INSTALL LOCATION ***
    install( TARGETS ${name} DESTINATION C:/MDOS )
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

message(STATUS "*** set CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message(STATUS "*** set CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message(STATUS "*** set CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "*** set CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

# eof
