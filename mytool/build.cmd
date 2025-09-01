@echo off
setlocal enabledelayedexpansion

:: �����������Ŀ¼
del /f /s /q outflie
mkdir outflie > nul

windres -i "demo.rc" -o "./outflie/demo.o"

:: MinGW �汾
set CC=gcc
set CFLAGS=-O0 -Wall
set OUTPUT=main.exe

:: ��ʼ�������ļ��б�
set OBJS=

:: ���� C_MyLib �е�Դ�ļ�
for %%f in (./C_MyLib/*.c) do (
    echo ���ڱ��� %%f...
    %CC% %CFLAGS% -c "./C_MyLib/%%f" -o "./outflie/%%~nf.o"
    if errorlevel 1 (
        echo ����ʧ��: ./C_MyLib/%%f
        exit /b 1
    )
    set OBJS=!OBJS! "./outflie/%%~nf.o"
)

:: ����������
echo ���ڱ��� main.c...
%CC% %CFLAGS% -c main.c -o "./outflie/main.o"
if errorlevel 1 (
    echo ����ʧ��: main.c
    exit /b 1
)
set OBJS=!OBJS! "./outflie/main.o"

:: ��ʾ��Ҫ���ӵĶ����ļ�
echo ���Ӷ����ļ�: !OBJS!

:: ��������Ŀ���ļ�
echo �������� %OUTPUT%...
%CC% %CFLAGS% !OBJS! "./outflie/demo.o" -o %OUTPUT% -lSetupAPI
if errorlevel 1 (
    echo ����ʧ��
    exit /b 1
)

echo ����ɹ�: %OUTPUT%
endlocal

timeout 2