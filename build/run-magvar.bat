@setlocal
@set TMPEXE=Release\magvar.exe
@if NOT EXIST %TMPEXE% goto ERR1
@REM lat_deg lon_deg alt mm dd yy [model]
@if "%~4x" == "x" goto SHOWDEFS
@goto DONEDEFS
:SHOWDEFS
@echo.
@echo KSFO - San Francisco International
@echo Latitude: 37.618999 Longitude: -122.375000 (37.61867421125,-122.37500760875)
@echo Field elevation: 13 ft/4 m MSL Scenery Tile: -123:37:2:4:w130n30/w123n37/942050
@echo Magnetic variation: 14.4 E from : http://www.ourairports.com/airports/KSFO/pilot-info.html
%TMPEXE% -v 37.618999 -122.375000 0.0 11 14 13
@echo.

@echo.
@echo LFPO Paris, Orly
@echo Latitude: 48.725300 Longitude: 2.359440 (48.7269692925,2.3699923175)
@echo Field elevation: 291 ft/89 m MSL Scenery Tile: 2:48:1:5:e000n40/e002n48/2990761
@echo Magnetic variation: -1.038 W from : http://www.ourairports.com/airports/LFPO/pilot-info.html
@echo.
%TMPEXE% -v 48.725300 2.359440 0.0 11 14 13
@echo.

:DONEDEFS

%TMPEXE% %1 %2 %3 %4 %5 %6 %7 %8 %9

@goto END

:ERR1
@echo Error: Can NOT locate %TMPEXE%
:END
