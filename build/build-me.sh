#!/bin/sh
#< build-me.sh - 20140827 - for test-suite project
BN=`basename $0`

BLDLOG="bldlog-1.txt"
TMPSG="/home/geoff/fg/next/install/simgear"
if [ ! -d "$TMPSG" ]; then
    echo "$BN: Can NOT locate '$TMPSG' directory"
    TMPSG="/media/pi/SAMSUNG2/projects/FG/next/install/simgear"
    if [ ! -d "$TMPSG" ]; then
        echo "$BN: Can NOT locate '$TMPSG' directory"
	TMPSG="/home/geoff/projects/FG/install/simgear"
    	if [ ! -d "$TMPSG" ]; then
	        echo "$BN: Can NOT locate '$TMPSG' directory"
		echo "$BN: FIX ME to point to a current SG install"
		exit 1
	fi
    fi
fi
if [ -f "$BLDLOG" ]; then
	rm -f $BLDLOG
fi

##############################################
### ***** NOTE THIS INSTALL LOCATION ***** ###
### Change to suit your taste, environment ###
##############################################
TMPOPTS="-DCMAKE_INSTALL_PREFIX=$HOME"
#############################################

#############################################
### *****  NOTE THE SIMGEAR INSTALL ***** ###
### Change to suit your taste, environment ###
##############################################
TMPOPTS="$TMPOPTS -DCMAKE_PREFIX_PATH:PATH=$TMPSG"
### TMPOPTS="$TMPOPTS -DCMAKE_PREFIX_PATH:PATH=/media/Disk2/FG/fg21/install/simgear"
#############################################

# Use -DCMAKE_BUILD_TYPE=Debug to add gdb symbols
# Use -DCMAKE_VERBOSE_MAKEFILE=ON
ADDEDDBG=0

for arg in $@; do
	if [ "$arg" = "VERBOSE" ]; then
		TMPOPTS="$TMPOPTS -DCMAKE_VERBOSE_MAKEFILE=ON"
	elif [ "$arg" = "DEBUG" ]; then
		TMPOPTS="$TMPOPTS -DCMAKE_BUILD_TYPE=Debug"
		ADDEDDBG=1
	else
		TMPOPTS="$TMPOPTS $arg"
	fi
done

if [ ! "$ADDEDDBG" = "1" ]; then
    TMPOPTS="$TMPOPTS -DCMAKE_BUILD_TYPE=Release"
fi    

echo "$BN: Doing: 'cmake .. $TMPOPTS' to $BLDLOG"
cmake .. $TMPOPTS >> $BLDLOG 2>&1
if [ ! "$?" = "0" ]; then
	echo "$BN: cmake confiuration, generation error"
	exit 1
fi

echo "$BN: Doing: 'make' to $BLDLOG"
make >> $BLDLOG 2>&1
if [ ! "$?" = "0" ]; then
	echo "$BN: make error - see $BLDLOG for details"
	exit 1
fi

echo ""
echo "$BN: appears a successful build... see $BLDLOG for details"
echo ""
echo "$BN: Time for 'make install' IFF desired... to $HOME/bin, unless changed..."
echo ""

# eof

