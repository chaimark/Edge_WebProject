#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

// 兼容性处理：如果头文件里没有这个宏，手动定义它
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

HANDLE hSerial;
BOOL bRunning = TRUE;

// 初始化控制台，支持 ANSI 转义序列
void EnableVTMode() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode)) {
            printf("Warning: Could not enable VT mode. Colors and cursor control might not work.\n");
        }
    }
}

// 串口读取线程：负责把开发板传回的数据实时显示
void ReadThread(void * param) {
    char buf[1024];
    DWORD bytesRead;
    while (bRunning) {
        if (ReadFile(hSerial, buf, sizeof(buf) - 1, &bytesRead, NULL) && bytesRead > 0) {
            // 直接写入标准输出，保持最快响应
            fwrite(buf, 1, bytesRead, stdout);
            fflush(stdout);
        }
    }
}

// 扫描可用串口
void ListPorts() {
    char portName[20];
    printf("--- [ comx ] Available Ports ---\n");
    int found = 0;
    for (int i = 1; i <= 30; i++) {
        sprintf(portName, "\\\\.\\COM%d", i);
        HANDLE hTest = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hTest != INVALID_HANDLE_VALUE) {
            printf("  -> COM%d\n", i);
            found++;
            CloseHandle(hTest);
        }
    }
    if (found == 0) printf("  No active COM ports found.\n");
    printf("-------------------------------\n");
}

int main() {
    char portInput[20];
    int baudRate = 115200;

    ListPorts();
    printf("Enter port (e.g. COM3): ");
    scanf("%s", portInput);
    printf("Enter baudrate (default 115200): ");
    if (scanf("%d", &baudRate) != 1) baudRate = 115200;

    char fullPortName[20];
    sprintf(fullPortName, "\\\\.\\%s", portInput);

    // 打开串口
    hSerial = CreateFile(fullPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error: Could not open %s. (Error code: %lu)\n", portInput, GetLastError());
        system("pause");
        return 1;
    }

    // 配置串口参数
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(hSerial, &dcb);
    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    SetCommState(hSerial, &dcb);

    // 设置超时：防止 ReadFile 永久阻塞
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    SetCommTimeouts(hSerial, &timeouts);

    // 启用 ANSI 模式，解决 Linux Shell 的颜色显示和光标控制
    EnableVTMode();
    printf("\n--- Connected to %s at %d baud ---\n", portInput, baudRate);
    printf("--- Press Ctrl+C to close session ---\n\n");

    // 启动接收线程
    _beginthread(ReadThread, 0, NULL);

    // 主线程：监听键盘并实时发送
    while (bRunning) {
        if (_kbhit()) {
            int ch = _getch();
            DWORD written;

            if (ch == 0 || ch == 0xE0) {
                // 处理方向键 (转换为 ANSI 转义序列)
                ch = _getch();
                const char * seq = NULL;
                switch (ch) {
                    case 72: seq = "\x1b[A"; break; // Up
                    case 80: seq = "\x1b[B"; break; // Down
                    case 77: seq = "\x1b[C"; break; // Right
                    case 75: seq = "\x1b[D"; break; // Left
                }
                if (seq) WriteFile(hSerial, seq, (DWORD)strlen(seq), &written, NULL);
            } else if (ch == 13) {
                // 处理回车：Linux 通常需要 \r
                char r = '\r';
                WriteFile(hSerial, &r, 1, &written, NULL);
            } else {
                // 处理普通按键
                char c = (char)ch;
                DWORD written;
                WriteFile(hSerial, &c, 1, &written, NULL);

                // // --- 新增：本地回显 ---
                // // 这样你按下的字符会立即显示在 CMD 窗口中
                // if (c == '\r') {
                //     putchar('\n'); // 如果是回车，手动补个换行
                // } else {
                //     putchar(c);
                // }
                // fflush(stdout);
                // // -----------------------
            }
        }
        Sleep(1); // 降低 CPU 占用
    }

    CloseHandle(hSerial);
    return 0;
}