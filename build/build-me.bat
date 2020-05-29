@setlocal
@set DOINST=0
@set TMPPRJ=Test
@set TMPLOG=bldlog-1.txt
@set BUILD_RELDBG=0
@set TMPBITS=64
@REM if NOT EXIST X:\nul goto NOXDIR

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
@set Qt5_DIR=D:\Qt\5.14.2\msvc2017_64\bin
@call setupqt5
@REM set Qt4_DIR=C:\Qt\4.8.6\bin
@REM call setupqt32
@REM set QTDIR=%Qt4_DIR%
@set QTDIR=%Qt5_DIR%
@REM set QTDIR=C:\QtSDK\Desktop\Qt\4.8.0\msvc2010

@REM ***************************************************
@REM NOTE WELL: FIXME: NOTE SPECIAL INSTALL PREFIX
@REM ***************************************************
@set TMPINST=C:/MDOS
@REM ***************************************************

@call chkmsvc %TMPPRJ%

@echo Built project %TMPPRJ% in %TMPBITS%-bits... all ouput to %TMPLOG%

@REM set SIMGEAR_DIR=C:\FG\17\install\msvc100\simgear
@REM set SIMGEAR_DIR=D:\FG\fg-64\install\simgear
@set SIMGEAR_DIR=D:\FG\next2\install\msvc140-64
@REM set ZLIBDIR=C:\FG\18\3rdParty
@set ZLIBDIR=D:\Projects\3rdParty.x64
@REM set ZLIBDIR=C:\FG\17\3rdParty
@REM set SIMGEAR_DIR=F:\FG\18\install\msvc100\simgear
@REM set ZLIBDIR=F:\FG\18\3rdParty
@REM if EXIST X:\nul (
@REM set SIMGEAR_DIR=X:\install\msvc100\simgear
@REM set ZLIBDIR=X:\3rdParty
@REM )
@REM set PostgreSQL_ROOT=C:\Program Files (x86)\PostgreSQL\9.1

@if NOT EXIST %SIMGEAR_DIR%\nul goto NOSGD
@if NOT EXIST %QTDIR%\nul goto NOQTD
@if NOT EXIST %ZLIBDIR%\nul goto NOZLD

@echo Set SIMGEAR_DIR=%SIMGEAR_DIR%
@echo Set QTDIR=%QTDIR%
@REM echo Set PostgreSQL_ROOT=%PostgreSQL_ROOT%

@set CMOPTS=
@set CMOPTS=%CMOPTS% -DZLIB_ROOT=%ZLIBDIR%
@set CMOPTS=%CMOPTS% -DCMAKE_PREFIX_PATH=%SIMGEAR_DIR%
@set CMOPTS=%CMOPTS% -DCMAKE_INSTALL_PREFIX=%TMPINST%
@set CMOPTS=%CMOPTS%  -DADD_SG_MATH:BOOL=ON

@echo Build of '%TMPPRJ% in %TMPBITS%-bits - begin %DATE% at %TIME% > %TMPLOG%
@REM echo set Qt4_DIR=%Qt4_DIR% >> %TMPLOG%
@echo set Qt5_DIR=%Qt5_DIR% >> %TMPLOG%
@echo Set SIMGEAR_DIR=%SIMGEAR_DIR%
@echo Set QTDIR=%QTDIR% >> %TMPLOG%
@REM echo Set PostgreSQL_ROOT=%PostgreSQL_ROOT% >> %TMPLOG%

cmake .. %CMOPTS% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

cmake --build . --config Debug >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4

@if "%BUILD_RELDBG%x" == "0x" goto DNRELDBG
cmake --build . --config RelWithDebInfo >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2
:DNRELDBG

cmake --build . --config Release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3

@echo Appears successful build of %TMPPRJ%...
@echo.
@if NOT "%DOINST%x" == "1x" (
@echo No install configured... Set DOINST=1
@echo.
@goto END
)

@echo Continue to install Release magvar and tests into %TMPINST%
@echo Only Ctrl+C aborts... all other keys continue..
@echo.
@pause

cmake --build . --config Release --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR5

@fa4 " -- " %TMPLOG%

@echo.
@echo Done build and install... see %TMPLOG% for details...
@echo.

@goto END

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
@echo ERROR: cmake config/gen
@goto ISERR

:ERR2
@echo ERROR: cmake build RelWitDebInfo
@goto ISERR

:ERR3
@echo ERROR: cmake build Release
@goto ISERR

:ERR4
@echo ERROR: cmake build Debug
@goto ISERR

:ERR5
@echo ERROR: cmake install
@goto ISERR

@REM :NOXDIR
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


