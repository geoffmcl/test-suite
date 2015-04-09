@setlocal
@set TMPEXE=Release\tests.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@set TMP3RD=C:\FG\17\3rdParty\bin
@if NOT EXIST %TMP3RD%\nul goto NO3RD


@set TMPCMD=
:RPT
@if "%~1x" == "x" goto GOTCMD
@set TMPCMD=%TMPCMD% %1
@shift
@goto RPT
:GOTCMD

@set PATH=%TMP3RD%;%PATH%

%TMPEXE% %TMPCMD%

@goto END

:NO3RD
@echo Can NOT find %TMP3RD%! Where is say libcurl DLL?
@goto END

:NOEXE
@echo Can NOT find %TMPEXE%! Has it been built? *** FIX ME ***
:END

