@setlocal
@set TMPEXE=Release\test.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@set TMPPG=C:\Program Files (x86)\PostgreSQL\9.1\bin
@if NOT EXIST "%TMPPG%" goto NOPG

@set PATH=%TMPPG%;%PATH%

%TMPEXE%

@goto END

:NOEXE
@echo Error: Can NOT locate %TMPEXE%! *** FIX ME ***
@echo Has it been built? Use build-me.bat
@goto END

:NOPG
@echo Error: Can NOT locate %TMPPG%! *** FIX ME *** for your environment
@goto END

:END
