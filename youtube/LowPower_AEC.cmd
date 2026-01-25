@echo off  
cd /d "%~dp0"
echo 当前运行路径是：%CD%

TITLE 降低 ACE 优先级
powershell.exe -ExecutionPolicy Bypass -File "%CD%\ACE_Low.ps1"
exit