@setlocal
@set TMPEXE=Release\tests.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@set TMPPG=C:\Program Files (x86)\PostgreSQL\9.1\bin
@if NOT EXIST "%TMPPG%" goto NOPG
@set TMP3RD=C:\FG\17\3rdParty\bin
@if NOT EXIST %TMP3RD%\nul goto NO3RD

@set TMPCMD=
:RPT
@if "%~1x" == "x" goto GOTCMD
@set TMPCMD=%TMPCMD% %1
@shift
@goto RPT
:GOTCMD

@set PATH=%TMPPG%;%TMP3RD%;%PATH%

%TMPEXE% %TMPCMD%

@goto END

:NOEXE
@echo Error: Can NOT locate %TMPEXE%! *** FIX ME ***
@echo Has it been built? Use build-me.bat
@goto END

:NOPG
@echo Error: Can NOT locate %TMPPG%! *** FIX ME *** for your environment
@goto END

:NO3RD
@echo Error: Can NOT locate %TMP3RD%! *** FIX ME *** for your environment
@goto END

:END
