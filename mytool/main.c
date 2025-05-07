#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>  // ���� _kbhit() �� _getch()
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
// �̺߳��������մ�������
DWORD WINAPI ReadSerialThread(LPVOID lpParam) {
    char buffer[256];
    DWORD bytesRead;

    while (1) {
        Sleep(1);
        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';  // ȷ���ַ�����ֹ
                printf("%s", buffer);
                memset(buffer, 0, 256);
            }
        } else {
            printf("Error: ��ȡ����ʧ��\n");
            break;
        }
    }

    return 0;
}

// ���ô���
void ConfigureSerialPort() {
    printf("\n--- �������� ---\n");

    // ѡ�񴮿ں�
    printf("�����봮�ں� (�� COM3): ");
    char userInput[10];
    scanf("%s", userInput);
    snprintf(portName, sizeof(portName), "\\\\.\\%s", userInput);  // ��ʽ��Ϊ \\.\COM11

    // ѡ������
    int CBR_S[] = {CBR_1200,CBR_2400,CBR_4800,CBR_9600,CBR_19200,CBR_38400,CBR_57600,CBR_115200};
    printf("\nѡ������:\n");
    for (int i = 0; i < ARR_SIZE(CBR_S); i++) {
        printf("%d. %d\n", i, CBR_S[i]);
    }
    printf("������ѡ�� (1-5): ");
    int choice = 0xFF; // Ĭ�� 2400
    scanf("%d", &choice);
    switch (choice) {
        case 0xFF: baudRate = CBR_2400; break;
        default: baudRate = (choice < ARR_SIZE(CBR_S) ? CBR_S[choice] : CBR_2400); break;
    }

    // ѡ������λ
    printf("\nѡ������λ (5-8): ");
    scanf("%d", &choice);
    byteSize = (choice >= 5 && choice <= 8) ? choice : 8;

    // ѡ��ֹͣλ
    printf("\nѡ��ֹͣλ:\n");
    printf("1. 1 λ\n2. 1.5 λ\n3. 2 λ\n");
    printf("������ѡ�� (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: stopBits = ONESTOPBIT; break;
        case 2: stopBits = ONE5STOPBITS; break;
        case 3: stopBits = TWOSTOPBITS; break;
        default: stopBits = ONESTOPBIT; break;
    }

    // ѡ��У��λ
    printf("\nѡ��У��λ:\n");
    printf("1. ��У��\n2. ��У��\n3. żУ��\n");
    printf("������ѡ�� (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: parity = NOPARITY; break;
        case 2: parity = ODDPARITY; break;
        case 3: parity = EVENPARITY; break;
        default: parity = NOPARITY; break;
    }

#ifdef HY_JSON_CMD
    printf("\n�Ƿ����� JsonCs У��\n��:0 or ��:1 >> : ");
    scanf("%d", &isOpenCS_JSon);
#endif
}

// �򿪴���
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

    printf("������: %ld\n", baudRate);
    printf("����λ: %d\n", byteSize);
    printf("ֹͣλ: %s\n",
        (stopBits == ONESTOPBIT ? "1" :
        (stopBits == ONE5STOPBITS ? "1.5" :
        (stopBits == TWOSTOPBITS ? "2" : "error"))));
    printf("У��λ: %s\n",
        (parity == NOPARITY) ? "��" :
        (parity == ODDPARITY) ? "��У��" : "żУ��");

    // ��ӵȴ�����ߴ򿪴��ڵĳɹ���
    Sleep(100);
    if (hSerial == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Error: �޷��򿪴��� %s, �������: %lu\n", portName, error);

        if (error == ERROR_FILE_NOT_FOUND) {
            printf("��ʾ: ���� %s �����ڣ������豸���ӻ򴮿ں������Ƿ���ȷ��\n", portName);
        } else if (error == ERROR_ACCESS_DENIED) {
            printf("��ʾ: ���� %s �ѱ�ռ�ã���ر�����ռ�øô��ڵĳ���\n", portName);
        } else {
            printf("��ʾ: ���鴮���豸�������Ƿ�������\n");
        }
        getch();
        return FALSE;
    }

    // ���ô��ڲ���
    COMMTIMEOUTS timeouts; //���峬ʱ�ṹ������д�ýṹ 
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Error: �޷����ô��ڳ�ʱ\n");
        CloseHandle(hSerial);
        getch();
        return FALSE;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error: �޷���ȡ��������\n");
        CloseHandle(hSerial);
        getch();
        return FALSE;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = byteSize;
    dcbSerialParams.StopBits = stopBits;
    dcbSerialParams.Parity = parity;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error: �޷����ô��ڲ���\n");
        CloseHandle(hSerial);
        getch();
        return FALSE;
    }

    printf("\n���� %s �Ѵ�\n", portName);
    return TRUE;
}

// ��ȡ���������ַ����������Ƿ��»س�
bool myGetS(strnew SpaceBuf) {
    if (_kbhit()) {  // ����Ƿ��м�������
        gets(SpaceBuf.Name._char);
        return true;
    }
    return false;  // δ���»س�
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
        printf("Error: �޷��������ļ� config.json\n");
        FILE * configFileW = fopen("config.json", "w");
        if (configFileW != NULL) {
            fprintf(configFileW, "%s", initStr);
            fclose(configFileW);
        } else {
            printf("Error: �޷����������ļ� config.json\n");
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
    // ��ȡ cmd_num 
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

// ����ģʽ�����ͺͽ������ݣ�
void InteractiveMode() {
    printf("\n--- ���ڽ���ģʽ (�� ESC �˳�) ---\n");

    // �����߳̽�������
    HANDLE hThread = CreateThread(NULL, 0, ReadSerialThread, NULL, 0, NULL);
    if (hThread == NULL) {
        printf("Error: �޷����������߳�\n");
        return;
    }

    bool isScanOver = false;
    // ���̷߳�������
    newString(InputBuff, 1024);
    while (1) {
        Sleep(0);
        isScanOver = myGetS(InputBuff);
        DWORD bytesWritten;
        if (isScanOver) {
            if (strcmp(StrInputBuff, "recom") == 0) {
                // �˳����� COM ��
                OpenSerialPort();
            } else if (strcmp(StrInputBuff, "clear") == 0) {
                system("cls");
                isScanOver = false;
                continue;
            } else if (strcmp(StrInputBuff, "help") == 0) {
                system("cls");
#ifdef HY_JSON_CMD
                printf("\n ����ָ���ʽ : {'Write':'AT24DataJSON','name':'var'}");
                printf("\n\n ����ָ��: ===============>\n time_init : {'Write':'AT24DataJSON','Time_Data':'NowTime'}");
                printf("\n idset:xxxxxx : {'Write':'AT24DataJSON','gw_id':'xxxxxx'}");
#endif
                printf("\n\n ���ָ��: ===============>\n");
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
                GetLocalTime(&st);  // ��ȡ��ǰ����ʱ��
                snprintf(TimeStr.Name._char, TimeStr.MaxLen, "%04d-%02d-%02d %02d:%02d:%02d",
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
                AddJsonItemData(InputBuff, "{");
                AddJsonItemData(InputBuff, "Write:\"%s\",", "AT24DataJSON");
                AddJsonItemData(InputBuff, "Time_Data:\"%s\"", TimeStr.Name._char);
                AddJsonItemData(InputBuff, "}");
            } else if (strstr(StrInputBuff, "idset:") != NULL) {
                newString(gwidstr, 20);
                char * p = strchr(StrInputBuff, ':');  // ����ð�ŵ�λ��
                if (p != NULL) {
                    p++;  // �ƶ���ð�ź���
                    copyString(gwidstr.Name._char, p, gwidstr.MaxLen, strlen(p));  // �����ַ���
                } else {
                    printf("Error: ��Ч�������ʽ\n");
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
                printf("Error: ����ʧ��, �������: %lu\n", error);
                printf("���� \"recom\" ���� COM ��\n");
            } else {
                memset(StrInputBuff, 0, 256);
            }
            isScanOver = false;
        }
    }

    // �ر��߳�
    TerminateThread(hThread, 0);
    CloseHandle(hThread);
}

// ��ʾ���п��ô���
void ListAvailablePorts() {
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;

    // ��ȡ�豸��Ϣ����
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        printf("Error: �޷���ȡ�豸��Ϣ����\n");
        return;
    }

    // ö���豸
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
        TCHAR deviceName[256];
        TCHAR devicePort[256];

        // ��ȡ�豸����
        if (SetupDiGetDeviceRegistryProperty(
            hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME,
            NULL, (PBYTE)deviceName, sizeof(deviceName), NULL)) {
            // ��ȡ�˿ں�
            if (SetupDiGetDeviceRegistryProperty(
                hDevInfo, &DeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
                NULL, (PBYTE)devicePort, sizeof(devicePort), NULL)) {
                _tprintf(_T("�豸: %s\n"), deviceName);
            }
        }
    }

    // ����
    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int main() {
    do {
        system("cls");
        // ��ʾ���п��ô���
        printf("\n--- ���ô����б� ---\n");
        ListAvailablePorts();
        printf("===== �����ն�ģ���� =====\n");
        ConfigureSerialPort();  // ���ô���
        clsInputSpace();
    } while (!OpenSerialPort());

    InteractiveMode();  // ���뽻��ģʽ

    CloseHandle(hSerial);  // �رմ���
    printf("\n�����ѹر�, ���������\n");
    return 0;
}
