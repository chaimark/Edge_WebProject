#include "JsonDataAnalyzeLib.h"
#include "JsonSetLib.h"
#include <stdarg.h>
#include <stdio.h>

void AddJsonItemData(strnew JsonStringSpace, ...) {

    // 初始化可变参数列表
    va_list args;
    va_start(args, JsonStringSpace);

    // catString(JsonStringSpace.Name._char, "\"", JsonStringSpace.MaxLen, strlen("\""));

    const char * FromStr = va_arg(args, const char *);
    char KeyName[200] = {0};
    // 查找 :
    char * Addr_OverName = strchr(FromStr, ':');
    const char * Addr_Start = strchr(FromStr, ',');
    if ((Addr_Start != NULL) && (Addr_Start < Addr_OverName)) {
        Addr_Start++;
        catString(KeyName, FromStr, 100, (Addr_Start - FromStr));
    } else {
        Addr_Start = FromStr;
    }

    if (Addr_OverName != NULL) {
        catString(KeyName, "\"", 100, 1);
        catString(KeyName, Addr_Start, 100, (Addr_OverName - Addr_Start));
        catString(KeyName, "\"", 100, 1);
        catString(KeyName, Addr_OverName, 100, strlen(Addr_OverName));
    } else {
        memcpy(KeyName, Addr_Start, (strlen(Addr_Start) < 200 ? strlen(Addr_Start) : 200));
    }

    // 获取当前字符串的长度
    int Addr_Over = JsonStringSpace.getStrlen(&JsonStringSpace);
    vsprintf(&JsonStringSpace.Name._char[Addr_Over], KeyName, args);
    // 结束可变参数处理
    va_end(args);
}

