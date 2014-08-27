@setlocal
@set TMPOUT=temp.diff

@set TMPFIL1=MMFIODef.h
@set TMPFIL2=MMFIODef.cpp

@set TMPDST=C:\FG\17\test
@set TMPSRC=C:\GTools\ConApps\MMFIO_CP\src
@REM set TMPSRC=C:\FG\17\test
@REM set TMPDST=C:\GTools\ConApps\MMFIO_CP\src

@echo REM Diff %DATE% %TIME% > %TMPOUT%

diff -uw %TMPDST%\%TMPFIL1% %TMPSRC%\%TMPFIL1% >> %TMPOUT%
diff -uw %TMPDST%\%TMPFIL2% %TMPSRC%\%TMPFIL2% >> %TMPOUT%
@echo.
@echo ********* Diff in %TMPOUT% **********
@echo.
@echo wm /e %TMPDST%\%TMPFIL1% %TMPSRC%\%TMPFIL1%
@echo wm /e %TMPDST%\%TMPFIL2% %TMPSRC%\%TMPFIL2%

