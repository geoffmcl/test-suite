@setlocal
@set TMPEXE=Release\test.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@set TMPCMD=
:RPT
@if "%~1x" == "x" goto GOTCMD
@set TMPCMD=%TMPCMD% %1
@shift
@goto RPT
:GOTCMD

%TMPEXE% %TMPCMD%

@goto END

:NOEXE
@echo Can NOT find %TMPEXE%! Has it been built? *** FIX ME ***
:END

