# - Find GMP
#
#  GMP_FOUND          - True if gmp found.
#  GMP_INCLUDE_DIRS   - where to find gmp.h
#  GMP_LIBRARIES      - library when using gmp
#

find_path(GMP_INCLUDE_DIRS 
    NAMES gmp.h 
    PATH_SUFFIXES include)

if (MSVC)
    find_library(GMP_LIB_DBG
        NAMES mpird gmpd
        PATH_SUFFIXES lib)
    find_library(GMP_LIB_REL
        NAMES mpir gmp
        PATH_SUFFIXES lib)
    if (GMP_LIB_DBG AND GMP_LIB_REL)
        set(GMP_LIBRARIES
            optimized ${GMP_LIB_REL}
            debug ${GMP_LIB_DBG}
            )
    elseif (GMP_LIB_REL)
        set(GMP_LIBRARIES ${GMP_LIB_REL})
    endif ()
else ()
    find_library(GMP_LIBRARIES
        NAMES gmp mpir
        PATH_SUFFIXES lib)
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( GMP REQUIRED_VARS GMP_LIBRARIES GMP_INCLUDE_DIRS )

# eof
