@echo off  
cd /d "%~dp0"
echo 当前运行路径是：%CD%

TITLE youtube或恢复代理设置
powershell.exe -ExecutionPolicy Bypass -File "%CD%\youtube.ps1"
exit