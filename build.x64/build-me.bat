@setlocal
@set VCVERS=14
@set TMPPRJ=Test
@set TMPLOG=bldlog-1.txt
@set BUILD_RELDBG=0
@echo Build of '%TMPPRJ% in 64-bits 
@if NOT EXIST X:\nul goto NOXDIR
@set CONTONERR=0

@REM ############################################
@REM NOTE: MSVC 10 INSTALL LOCATION
@REM Adjust to suit your environment
@REM ##########################################
@set GENERATOR=Visual Studio %VCVERS% Win64
@set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio %VCVERS%.0
@set VC_BAT=%VS_PATH%\VC\vcvarsall.bat
@if NOT EXIST "%VS_PATH%" goto NOVS
@if NOT EXIST "%VC_BAT%" goto NOBAT
@set BUILD_BITS=%PROCESSOR_ARCHITECTURE%
@IF /i %BUILD_BITS% EQU x86_amd64 (
    @set "RDPARTY_ARCH=x64"
    @set "RDPARTY_DIR=software.x64"
) ELSE (
    @IF /i %BUILD_BITS% EQU amd64 (
        @set "RDPARTY_ARCH=x64"
        @set "RDPARTY_DIR=software.x64"
    ) ELSE (
        @echo Appears system is NOT 'x86_amd64', nor 'amd64'
        @echo Can NOT build the 64-bit version! Aborting
        @exit /b 1
    )
)

@ECHO Setting environment - CALL "%VC_BAT%" %BUILD_BITS%
@CALL "%VC_BAT%" %BUILD_BITS%
@if ERRORLEVEL 1 goto NOSETUP
@set TMPINST=C:\FG\17\%RDPARTY_DIR%
@set TMPOPTS=
@set TMPOPTS=%TMPOPTS% -DCMAKE_INSTALL_PREFIX:PATH=%TMPINST%
@set TMPOPTS=%TMPOPTS% -G "%GENERATOR%"

@REM To help find Qt4
@REM Try adding path to qmake.exe to PATH
@REM set PATH=C:\QtSDK\Desktop\Qt\4.8.0\msvc2010\bin;%PATH%
@REM THat WORKED ;=/
@REM Try adding QTDIR to environment
@REM set QTDIR=C:\QtSDK\Desktop\Qt\4.8.0\msvc2010\bin
@REM That ALSO works - now which to make PERMANENT?
@REM Ok, decide to add it to PATH
@REM And OK, that WORKS FINE
@REM 20131125 - started to FAIL again???? Try
@REM set Qt4_DIR=C:\QtSDK
@REM set Qt4_DIR=C:\QtSDK\Desktop\Qt\4.8.0\msvc2010\bin
@REM set Qt4_DIR=C:\Qt\4.8.6-x86\bin
@set Qt4_DIR=C:\Qt\4.8.6\bin
@call setupqt64
@set QTDIR=%Qt4_DIR%
@REM set QTDIR=C:\QtSDK\Desktop\Qt\4.8.0\msvc2010

@call chkmsvc %TMPPRJ%
@call setupqt64

@echo Built project %TMPPRJ% 64-bits... all ouput to %TMPLOG%

@REM A 64-bit build of simgear
@set SIMGEAR_DIR=F:\Projects\fg-64\install\SimGear
@set ZLIBDIR=F:\Projects\fg-64\3rdParty.x64
@REM set SIMGEAR_DIR=C:\FG\17\install\msvc100\simgear.x64
@REM set ZLIBDIR=C:\FG\17\3rdParty.x64
@REM set SIMGEAR_DIR=F:\FG\18\install\msvc100\simgear
@REM set ZLIBDIR=F:\FG\18\3rdParty
@if EXIST X:\nul (
@set SIMGEAR_DIR=X:\install\msvc%VCVERS%0-64\simgear
@set ZLIBDIR=X:\3rdParty.x64
)

@if NOT EXIST %SIMGEAR_DIR%\nul goto NOSGD
@if NOT EXIST %ZLIBDIR%\nul goto NOZLD

@REM set PostgreSQL_ROOT=C:\Program Files (x86)\PostgreSQL\9.1

@REM if NOT EXIST %SIMGEAR_DIR%\nul goto NOSGD
@if NOT EXIST %QTDIR%\nul goto NOQTD
@REM if NOT EXIST %ZLIBDIR%\nul goto NOZLD

@echo Set SIMGEAR_DIR=%SIMGEAR_DIR%
@echo Set QTDIR=%QTDIR%
@echo Set PostgreSQL_ROOT=%PostgreSQL_ROOT%
@echo Set ZLIBDIR=%ZLIBDIR%

@set CMOPTS=%TMPOPTS%
@set CMOPTS=%CMOPTS% -DZLIB_ROOT=%ZLIBDIR%
@set CMOPTS=%CMOPTS% -DCMAKE_PREFIX_PATH=%ZLIBDIR%
@REM set CMOPTS=%CMOPTS% -DCMAKE_PREFIX_PATH=%SIMGEAR_DIR%

@echo Build of '%TMPPRJ% in 64-bits being %DATE% at %TIME% > %TMPLOG%
@echo set Qt4_DIR=%Qt4_DIR% >> %TMPLOG%
@echo Set SIMGEAR_DIR=%SIMGEAR_DIR%
@echo Set QTDIR=%QTDIR% >> %TMPLOG%
@echo Set PostgreSQL_ROOT=%PostgreSQL_ROOT% >> %TMPLOG%

@echo Doing: 'cmake .. %CMOPTS%
@echo Doing: 'cmake .. %CMOPTS% >> %TMPLOG%
@cmake .. %CMOPTS% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

@echo Doing: 'cmake --build . --config Debug'
@echo Doing: 'cmake --build . --config Debug' >> %TMPLOG%
@cmake --build . --config Debug >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2

:DNDBGBLD
@if "%BUILD_RELDBG%x" == "0x" goto DNRELDBG
@echo Doing: 'cmake --build . --config RelWithDebInfo'
@echo Doing: 'cmake --build . --config RelWithDebInfo' >> %TMPLOG%
@cmake --build . --config RelWithDebInfo >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3

:DNRELDBG
@echo Doing: 'cmake --build . --config Release'
@echo Doing: 'cmake --build . --config Release' >> %TMPLOG%
@cmake --build . --config Release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4
@echo.
@call dirmin release\tests.exe
@echo Appears successful build of test...
@echo.

@goto END

:NOSETUP
@echo MSVC setup FAILED!
@goto ISERR

:NOBAT
@echo Can not locate "%VC_BAT%"! *** FIX ME *** for your environment
@goto ISERR

:NOVS
@echo Can not locate "%VS_PATH%"! *** FIX ME *** for your environment
@goto ISERR

:NOSGD
@echo Note: Simgear directory %SIMGEAR_DIR% does NOT EXIST!  *** FIX ME ***
@goto ISERR

:NOQTD
@echo Note: Qt directory %QTDIR% does NOT EXIST! *** FIX ME ***
@goto ISERR

:NOZLD
@echo Note: ZLIB direcotry %ZLIBDIR% does NOT EXIST! *** FIX ME ***
@goto ISERR

:ERR1
@echo FATAL ERROR: cmake configuration/generation FAILED
@echo FATAL ERROR: cmake configuration/generation FAILED >> %TMPLOG%
@goto ISERR

:ERR2
@echo ERROR: cmake build Debug
@echo ERROR: cmake build Debug >> %TMPLOG%
@if %CONTONERR% EQU 1 goto DNDBGBLD
@goto ISERR

:ERR3
@echo ERROR: cmake build RelWithDebInfo
@echo ERROR: cmake build RelWithDebInfo >> %TMPLOG%
@if %CONTONERR% EQU 1 goto DNRELDBG
@goto ISERR

:ERR4
@echo ERROR: cmake build Release
@echo ERROR: cmake build Release >> %TMPLOG%
@goto ISERR

:NOXDIR
@echo.
@echo Oops, no X: directory found! Needed for simgear, etc
@echo Run setupx, or hdem3m, etc, to establish X: drive
@echo.
@goto ISERR



:ISERR
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
