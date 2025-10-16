@echo off
setlocal enabledelayedexpansion

REM === 配置：glslc 要在 PATH 里，或把下一行改成 glslc.exe 的完整路径 ===
set "GLSLC=glslc.exe"
REM 可选编译参数（例如调试/优化等）：set "GLSLC_FLAGS=-O"
set "GLSLC_FLAGS="

REM 切到脚本所在目录，便于双击运行
pushd "%~dp0"

for %%f in (*.vert *.frag) do (
    set "ext=%%~xf"
    set "name=%%~nf"
    if /i "!ext!"==".vert" (
        echo [VERT] %%f  ->  !name!_vert.spv
        "%GLSLC%" %GLSLC_FLAGS% "%%f" -o "!name!_vert.spv" || goto :err
    ) else (
        echo [FRAG] %%f  ->  !name!_frag.spv
        "%GLSLC%" %GLSLC_FLAGS% "%%f" -o "!name!_frag.spv" || goto :err
    )
)

echo.
echo Done.
popd
pause
exit /b 0

:err
echo.
echo *** glslc compile failed. ***
popd
pause
exit /b 1