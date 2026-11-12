@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0make_release.ps1"
endlocal
