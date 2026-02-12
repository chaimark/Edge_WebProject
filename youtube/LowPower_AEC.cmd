@echo off  
setlocal EnableDelayedExpansion
cd /d "%~dp0"
echo 当前运行路径是：%CD%

start "" "C:\WeGameApps\DeltaForce(2001918)\DeltaForceClient.exe"

REM 三加州启动，但 wegame 未运行
:check0
tasklist | findstr /i "DeltaForceClient-Win64-Sh" >nul
if errorlevel 1 (
    echo No1-等待游戏启动中，请勿关闭...
    timeout /t 2 /nobreak >nul
    goto check0
)
timeout /t 5 /nobreak >nul

REM 检测 wegame
:check1
tasklist | findstr /i "wegame.exe" >nul
if errorlevel 1 (
    echo No1-等待游戏启动中，请勿关闭...
    timeout /t 2 /nobreak >nul
    goto check1
)
tasklist | findstr /i "browser.exe" >nul
if errorlevel 1 (
    echo No1-等待游戏启动中，请勿关闭...
    timeout /t 2 /nobreak >nul
    goto check1
)
timeout /t 10 /nobreak >nul
echo 请登录 wegame

REM ============================================================
set MaxLen=100
set NowLen=0
set NextLen=0

:checkDeltaForceClient_Start
cls
set /a Max_SecRun=40
set /a Now_SecRun=0
REM 等待 DeltaForceClient-Win64-Sh 启动
:check_Run
tasklist | findstr /i "DeltaForceClient-Win64-Sh" >nul
if errorlevel 1 (
    echo No2-等待游戏启动中，请勿关闭...
    timeout /t 2 /nobreak >nul
    goto checkDeltaForceClient_Start
) else (
    REM DeltaForceClient已启动，开始计时监控
    set /a NextLen=NextLen+1
    call :showProcess
    goto monitor_Run
)
:monitor_Run
REM 检查是否超时
if !Now_SecRun! GEQ !Max_SecRun! (
    set /a Now_SecRun=0
    timeout /t 3 /nobreak >nul
    goto LowAce
)
REM 未超时，继续等待
set /a Now_SecRun=Now_SecRun+1
timeout /t 1 /nobreak >nul
goto check_Run

REM ============================================================

:LowAce
tasklist | findstr /i "SGuard64.exe" >nul
if errorlevel 1 (
    echo No3-等待SGuard64启动中，请勿关闭...
    timeout /t 2 /nobreak >nul
    goto LowAce
)

:LowAce2
tasklist | findstr /i "SGuardSvc64.exe" >nul
if errorlevel 1 (
    echo No3-等待SGuardSvc64启动中，请勿关闭...
    timeout /t 2 /nobreak >nul
    goto LowAce2
)

TITLE 降低 ACE 优先级
powershell.exe -ExecutionPolicy Bypass -File "%CD%\ACE_Low.ps1"
exit

REM 进度条

:showProcess
set NeedLen=0
if %NextLen% leq %MaxLen% (
    set /a NeedLen=%NextLen%-%NowLen%
) else (
    set /a NeedLen=%MaxLen%-%NowLen%
)
for /L %%i in (1 1 %NeedLen%) do set /p a=■<nul&timeout /t 1 > nul
set /a NowLen=%NowLen%+%NeedLen%
if %NowLen%==%MaxLen% (
    echo.
    set NowLen=0
)
exit /b 0

