#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>  // 用于 _kbhit() 和 _getch()
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <tchar.h>
#include "./C_MyLib/StrLib.h"
#include "./C_MyLib/JsonDataAnalyzeLib.h"
#include "./C_MyLib/JsonSetLib.h"
#include "./C_MyLib/JsonCheckFun.h"

HANDLE hSerial;
DWORD baudRate = CBR_9600;
BYTE byteSize = 8;
BYTE stopBits = ONESTOPBIT;
BYTE parity = NOPARITY;
int isOpenCS_JSon = 0;
char portName[10] = "COM3";
void clsInputSpace(void) {
    char ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF);
}
// 线程函数：接收串口数据
DWORD WINAPI ReadSerialThread(LPVOID lpParam) {
    char buffer[256];
    DWORD bytesRead;

    while (1) {
        Sleep(1);
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';  // 确保字符串终止
                printf("%s", buffer);
                memset(buffer, 0, 256);
            }
        } else {
            printf("Error: 读取串口失败\n");
            break;
        }
    }

    return 0;
}

// 配置串口
void ConfigureSerialPort() {
    printf("\n--- 串口配置 ---\n");

    // 选择串口号
    printf("请输入串口号 (如 COM3): ");
    char userInput[10];
    scanf("%s", userInput);
    snprintf(portName, sizeof(portName), "\\\\.\\%s", userInput);  // 格式化为 \\.\COM11

    // 选择波特率
    int CBR_S[] = {CBR_1200,CBR_2400,CBR_4800,CBR_9600,CBR_19200,CBR_38400,CBR_57600,CBR_115200};
    printf("\n选择波特率:\n");
    for (int i = 0; i < ARR_SIZE(CBR_S); i++) {
        printf("%d. %d\n", i, CBR_S[i]);
    }
    printf("请输入选项 (1-5): ");
    int choice = 0xFF; // 默认 2400
    scanf("%d", &choice);
    switch (choice) {
        case 0xFF: baudRate = CBR_2400; break;
        default: baudRate = (choice < ARR_SIZE(CBR_S) ? CBR_S[choice] : CBR_2400); break;
    }

    // 选择数据位
    printf("\n选择数据位 (5-8): ");
    scanf("%d", &choice);
    byteSize = (choice >= 5 && choice <= 8) ? choice : 8;

    // 选择停止位
    printf("\n选择停止位:\n");
    printf("1. 1 位\n2. 1.5 位\n3. 2 位\n");
    printf("请输入选项 (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: stopBits = ONESTOPBIT; break;
        case 2: stopBits = ONE5STOPBITS; break;
        case 3: stopBits = TWOSTOPBITS; break;
        default: stopBits = ONESTOPBIT; break;
    }

    // 选择校验位
    printf("\n选择校验位:\n");
    printf("1. 无校验\n2. 奇校验\n3. 偶校验\n");
    printf("请输入选项 (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: parity = NOPARITY; break;
        case 2: parity = ODDPARITY; break;
        case 3: parity = EVENPARITY; break;
        default: parity = NOPARITY; break;
    }

#ifdef HY_JSON_CMD
    printf("\n是否启用 JsonCs 校验\n否:0 or 是:1 >> : ");
    scanf("%d", &isOpenCS_JSon);
#endif
}

// 打开串口
BOOL OpenSerialPort() {
    hSerial = CreateFile(
        portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    printf("波特率: %ld\n", baudRate);
    printf("数据位: %d\n", byteSize);
    printf("停止位: %s\n",
        (stopBits == ONESTOPBIT ? "1" :
        (stopBits == ONE5STOPBITS ? "1.5" :
        (stopBits == TWOSTOPBITS ? "2" : "error"))));
    printf("校验位: %s\n",
        (parity == NOPARITY) ? "无" :
        (parity == ODDPARITY) ? "奇校验" : "偶校验");

    // 添加等待以提高打开串口的成功率
    Sleep(100);
    if (hSerial == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Error: 无法打开串口 %s, 错误代码: %lu\n", portName, error);

        if (error == ERROR_FILE_NOT_FOUND) {
            printf("提示: 串口 %s 不存在，请检查设备连接或串口号输入是否正确。\n", portName);
        } else if (error == ERROR_ACCESS_DENIED) {
            printf("提示: 串口 %s 已被占用，请关闭其他占用该串口的程序。\n", portName);
        } else {
            printf("提示: 请检查串口设备和驱动是否正常。\n");
        }
        getch();
        return FALSE;
    }

    // 配置串口参数
    COMMTIMEOUTS timeouts; //定义超时结构，并填写该结构 
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Error: 无法设置串口超时\n");
        CloseHandle(hSerial);
        getch();
        return FALSE;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error: 无法获取串口配置\n");
        CloseHandle(hSerial);
        getch();
        return FALSE;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = byteSize;
    dcbSerialParams.StopBits = stopBits;
    dcbSerialParams.Parity = parity;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error: 无法设置串口参数\n");
        CloseHandle(hSerial);
        getch();
        return FALSE;
    }

    printf("\n串口 %s 已打开\n", portName);
    return TRUE;
}

// 获取缓冲区的字符，并返回是否按下回车
bool myGetS(strnew SpaceBuf) {
    if (_kbhit()) {  // 检测是否有键盘输入
        gets(SpaceBuf.Name._char);
        return true;
    }
    return false;  // 未按下回车
}

void DisplayHelp(strnew CmdName, strnew CmdVar) {
    printf("\n>> %s : \n   %s\n", CmdName.Name._char, CmdVar.Name._char);
}

#define initString_Txt "{\
    \"cmd_Name_Array\": [\
        \"init\",\
        \"read\",\
        \"set\",\
        \"reboot\"\
    ],\
    \"cmd_Var_Array\": [\
        \"{'Write':'AT24DataJSON','gw_id':'02345678903','username':'admin','password':'njhy1234','heating_start':'20XX-09-01','heating_end':'20XX-08-29','NET_Local_IP':'192.168.2.218','NET_Local_MASK':'255.255.255.0','NET_Local_GATEWAY':'192.168.2.1','remote_url':'59.110.170.225','remote_port':1883,'main_interval':10,'copy_interval':60,'_copy_statistics':1,'not_intimer_interval':1440,'GW_model':73,'NetCheckENableFlag':true,'IsColorDislay':false,'DaysNumberOfCCLK':7,'main_meter_total':0,'copy_meter_total':0,'Time_Data':'2025-04-26 13:30:18'}\",\
        \"{'Read':'AT24DataJSON'}\",\
        \"{'SetCmd':'ReadBoard','MUBS_B':'Read','RS4851_B':'Read','RS4852_B':'Read'}\",\
        \"{'SetCmd':'ReadBoard','Reboot':true}\"\
    ]\
}"

const char * initStr = initString_Txt;
void CMD_ChooseFun(strnew InputBuff, bool UserFlag) {
    FILE * configFile = fopen("config.json", "r");
    if (configFile == NULL) {
        printf("Error: 无法打开配置文件 config.json\n");
        FILE * configFileW = fopen("config.json", "w");
        if (configFileW != NULL) {
            fprintf(configFileW, "%s", initStr);
            fclose(configFileW);
        } else {
            printf("Error: 无法创建配置文件 config.json\n");
        }
        system("timeout 2");
        printf("config.json success\n");
        return;
    }
    fseek(configFile, 0, SEEK_END);
    long fileSize = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    strnew_malloc(JsonConfig, fileSize + 20);
    char ch;
    size_t index = 0;
    while ((ch = fgetc(configFile)) != EOF) {
        JsonConfig.Name._char[index++] = ch;
    }
    fclose(configFile);
    JsonObject JsonConfigObj = newJsonObjectByString(JsonConfig);
    // 读取 cmd_num 
    if (JsonConfigObj.isJsonNull(&JsonConfigObj, "cmd_Name_Array") < 0) {
        goto EndOver2;
    }
    if (JsonConfigObj.isJsonNull(&JsonConfigObj, "cmd_Var_Array") < 0) {
        goto EndOver2;
    }
    strnew_malloc(ArrNameSpace, fileSize + 20);
    strnew_malloc(ArrVarSpace, fileSize + 20);
    JsonArray Cmd_Array_Name = JsonConfigObj.getArray(&JsonConfigObj, "cmd_Name_Array", ArrNameSpace);
    JsonArray Cmd_Array_Var = JsonConfigObj.getArray(&JsonConfigObj, "cmd_Var_Array", ArrVarSpace);

    int cmd_num = (Cmd_Array_Name.sizeItemNum(&Cmd_Array_Name) == Cmd_Array_Var.sizeItemNum(&Cmd_Array_Var) ?
        Cmd_Array_Name.sizeItemNum(&Cmd_Array_Name) : 0);
    newString(TempCmd_Name, 2048);
    newString(TempCmd_Var, 2048);
    for (int i = 0; i < cmd_num; i++) {
        memset(TempCmd_Name.Name._char, 0, TempCmd_Name.MaxLen);
        memset(TempCmd_Var.Name._char, 0, TempCmd_Var.MaxLen);
        Cmd_Array_Name.get(&Cmd_Array_Name, TempCmd_Name, i);
        Cmd_Array_Var.get(&Cmd_Array_Var, TempCmd_Var, i);
        if (UserFlag == true) {
            DisplayHelp(TempCmd_Name, TempCmd_Var);
        } else {
            if (strcmp(InputBuff.Name._char, TempCmd_Name.Name._char) == 0) {
                memset(InputBuff.Name._char, 0, InputBuff.MaxLen);
                memcpy(InputBuff.Name._char, TempCmd_Var.Name._char, strlen(TempCmd_Var.Name._char));
                goto EndOver1;
            }
        }
    }
EndOver1:
    free(ArrNameSpace.Name._char);
    free(ArrVarSpace.Name._char);
EndOver2:
    free(JsonConfig.Name._char);
    if (isOpenCS_JSon == 1) {
        char * P_Node = NULL;
        do {
            P_Node = NULL;
            P_Node = strchr(InputBuff.Name._char, '\'');
            if (P_Node == NULL) {
                break;
            }
            (*P_Node) = '\"';
        } while (P_Node != NULL);
    }
    return;
}

// 交互模式（发送和接收数据）
void InteractiveMode() {
    printf("\n--- 串口交互模式 (按 ESC 退出) ---\n");

    // 创建线程接收数据
    HANDLE hThread = CreateThread(NULL, 0, ReadSerialThread, NULL, 0, NULL);
    if (hThread == NULL) {
        printf("Error: 无法创建接收线程\n");
        return;
    }

    bool isScanOver = false;
    // 主线程发送数据
    newString(InputBuff, 1024);
    while (1) {
        Sleep(0);
        isScanOver = myGetS(InputBuff);
        DWORD bytesWritten;
        if (isScanOver) {
            if (strcmp(StrInputBuff, "recom") == 0) {
                // 退出重连 COM 口
                OpenSerialPort();
            } else if (strcmp(StrInputBuff, "clear") == 0) {
                system("cls");
                isScanOver = false;
                continue;
            } else if (strcmp(StrInputBuff, "help") == 0) {
                system("cls");
#ifdef HY_JSON_CMD
                printf("\n 基础指令格式 : {'Write':'AT24DataJSON','name':'var'}");
                printf("\n\n 特殊指令: ===============>\n time_init : {'Write':'AT24DataJSON','Time_Data':'NowTime'}");
                printf("\n idset:xxxxxx : {'Write':'AT24DataJSON','gw_id':'xxxxxx'}");
#endif
                printf("\n\n 快捷指令: ===============>\n");
                CMD_ChooseFun(InputBuff, true);
                memset(StrInputBuff, 0, 256);
                isScanOver = false;
                continue;
            }
#ifdef HY_JSON_CMD
            else if (strcmp(StrInputBuff, "time_init") == 0) {
                memset(InputBuff.Name._char, 0, InputBuff.MaxLen);
                newString(TimeStr, 25);
                //2025-04-26 13:30:18
                SYSTEMTIME st;
                GetLocalTime(&st);  // 获取当前电脑时间
                snprintf(TimeStr.Name._char, TimeStr.MaxLen, "%04d-%02d-%02d %02d:%02d:%02d",
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
                AddJsonItemData(InputBuff, "{");
                AddJsonItemData(InputBuff, "Write:\"%s\",", "AT24DataJSON");
                AddJsonItemData(InputBuff, "Time_Data:\"%s\"", TimeStr.Name._char);
                AddJsonItemData(InputBuff, "}");
            } else if (strstr(StrInputBuff, "idset:") != NULL) {
                newString(gwidstr, 20);
                char * p = strchr(StrInputBuff, ':');  // 查找冒号的位置
                if (p != NULL) {
                    p++;  // 移动到冒号后面
                    copyString(gwidstr.Name._char, p, gwidstr.MaxLen, strlen(p));  // 复制字符串
                } else {
                    printf("Error: 无效的输入格式\n");
                    continue;
                }
                memset(InputBuff.Name._char, 0, InputBuff.MaxLen);
                AddJsonItemData(InputBuff, "{");
                AddJsonItemData(InputBuff, "Write:\"%s\",", "AT24DataJSON");
                AddJsonItemData(InputBuff, "gw_id:\"%s\"", gwidstr.Name._char);
                AddJsonItemData(InputBuff, "}");
            }
#endif 
            else {
                CMD_ChooseFun(InputBuff, false);
            }
            if (isOpenCS_JSon == 1) {
                AddCsToJsonAndPushJsonStr(newJsonObjectByString(NEW_NAME(StrInputBuff)));
            }
            catString(StrInputBuff, "\n", InputBuff.MaxLen, 1);
            if (!WriteFile(hSerial, StrInputBuff, strlen(StrInputBuff), &bytesWritten, NULL)) {
                DWORD error = GetLastError();
                printf("Error: 发送失败, 错误代码: %lu\n", error);
                printf("输入 \"recom\" 重连 COM 口\n");
            } else {
                memset(StrInputBuff, 0, 256);
            }
            isScanOver = false;
        }
    }

    // 关闭线程
    TerminateThread(hThread, 0);
    CloseHandle(hThread);
}

// 显示所有可用串口
void ListAvailablePorts() {
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;

    // 获取设备信息集合
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        printf("Error: 无法获取设备信息集合\n");
        return;
    }

    // 枚举设备
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
        TCHAR deviceName[256];
        TCHAR devicePort[256];

        // 获取设备名称
        if (SetupDiGetDeviceRegistryProperty(
            hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME,
            NULL, (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            // 获取端口号
            if (SetupDiGetDeviceRegistryProperty(
                hDevInfo, &DeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
                NULL, (PBYTE)devicePort, sizeof(devicePort), NULL)) {
                _tprintf(_T("设备: %s\n"), deviceName);
            }
        }
    }

    // 清理
    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int main() {
    do {
        system("cls");
        // 显示所有可用串口
        printf("\n--- 可用串口列表 ---\n");
        ListAvailablePorts();
        printf("===== 串口终端模拟器 =====\n");
        ConfigureSerialPort();  // 配置串口
        clsInputSpace();
    } while (!OpenSerialPort());

    InteractiveMode();  // 进入交互模式

    CloseHandle(hSerial);  // 关闭串口
    printf("\n串口已关闭, 程序结束。\n");
    return 0;
}
