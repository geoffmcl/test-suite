# - Find MPFR
#
#  MPFR_FOUND          - True if MPFR found.
#  MPFR_INCLUDE_DIRS   - where to find mpfr.h
#  MPFR_LIBRARIES      - library when using MPFR
#

find_path(MPFR_INCLUDE_DIRS 
    NAMES mpfr.h
    PATH_SUFFIXES include)

if (MSVC)
    find_library(MPFR_LIB_DBG
        NAMES mpfrd
        PATH_SUFFIXES lib)
    find_library(MPFR_LIB_REL
        NAMES mpfr
        PATH_SUFFIXES lib)
    if (MPFR_LIB_DBG AND MPFR_LIB_REL)
        set(MPFR_LIBRARIES
            optimized ${MPFR_LIB_REL}
            debug ${MPFR_LIB_DBG}
            )
    elseif (MPFR_LIB_REL)
        set(MPFR_LIBRARIES ${MPFR_LIB_REL})
    endif ()
else ()
    find_library(MPFR_LIBRARIES
        NAMES mpfr
        PATH_SUFFIXES lib)
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( MPFR REQUIRED_VARS MPFR_LIBRARIES MPFR_INCLUDE_DIRS )

# eof
