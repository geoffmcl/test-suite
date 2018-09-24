@setlocal
@set TMPEXE=Release\tests.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@rem set TMPPG=C:\Program Files (x86)\PostgreSQL\9.1\bin
@rem if NOT EXIST "%TMPPG%" goto NOPG
@rem set TMP3RD=X:\3rdParty.x64\bin
@REM set TMP3RD=F:\Projects\fg-64\3rdParty.x64\bin
@rem if NOT EXIST %TMP3RD%\nul goto NO3RD

@rem set PATH=%TMP3RD%;%TMPPG%;%PATH%

%TMPEXE% %*

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
