@setlocal
@if "%~1x" == "x" (
@echo Give word to find in source
@exit /b 1
)

@set TMPFND=%~1
@shift
@set TMPOPTS=
:RPT
@if "%~1x" == "x" goto GOTOPTS
@set TMPOPTS=%TMPOPTS% %1
@shift
@goto RPT
:GOTOPTS

fa4 "%TMPFND%" ..\* -r -b- -x:build %TMPOPT%%

