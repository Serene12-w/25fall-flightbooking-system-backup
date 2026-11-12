@echo off
setlocal

REM Configure WinLibs path here if different
set "WINLIBS=C:\WinLibs\mingw64\bin"

if not exist "%WINLIBS%\g++.exe" (
  echo [run.bat] WinLibs not found at %WINLIBS%
  echo Please edit run.bat and set WINLIBS to your actual WinLibs\mingw64\bin path.
  pause
  exit /b 1
)

set "PATH=%WINLIBS%;%PATH%"

if not exist "airline_reservation.exe" (
  echo [run.bat] airline_reservation.exe not found in %cd%
  echo Please build first (Ctrl+Shift+B in VS Code).
  pause
  exit /b 1
)

echo Launching airline_reservation.exe with WinLibs runtime first in PATH...
start "AirlineReservation" /D "%cd%" airline_reservation.exe

endlocal
