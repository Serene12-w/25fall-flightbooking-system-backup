@echo off
setlocal

REM === 配置 WinLibs 安装路径（如有不同请修改此行） ===
set "WINLIBS_ROOT=C:\WinLibs\mingw64\bin"

REM 检查路径是否存在
if not exist "%WINLIBS_ROOT%\g++.exe" (
  echo [ERROR] 未找到 g++.exe 于: %WINLIBS_ROOT%
  echo 请修改 copy_winlibs_dlls.bat 内的 WINLIBS_ROOT 变量为你的實際 WinLibs\mingw64\bin 路径。
  pause
  exit /b 1
)

REM 要复制的运行时 DLL 列表（常见的 GCC 运行所需）
set DLL_LIST=libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll

echo ===== 复制 WinLibs 运行所需 DLL 到当前目录: %cd% =====
for %%F in (%DLL_LIST%) do (
  if exist "%WINLIBS_ROOT%\%%F" (
    copy /Y "%WINLIBS_ROOT%\%%F" . >nul
    echo Copied %%F
  ) else (
    echo [WARN] 缺失 %%F (路径: %WINLIBS_ROOT%\%%F)
  )
)

echo 完成。如果仍然出现入口点或 DLL 缺失错误，可再检查是否需要其他库：
echo   1) 若使用额外压缩/安全功能可能需要 libssp-0.dll 或 zlib*.dll
echo   2) 可使用依赖分析工具 (Dependencies.exe) 查看缺失项

echo 正在尝试启动程序...
if exist airline_reservation.exe (
  start "AirlineReservation" airline_reservation.exe
  echo 已启动 airline_reservation.exe
) else (
  echo [ERROR] 未找到 airline_reservation.exe，请先构建。
)

endlocal
