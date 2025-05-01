#ifndef CHANGE_ARRAY_H 
#define CHANGE_ARRAY_H 

#include "StrLib.h"
#define strnew_malloc(name,Len) newstrobj(name,1)\
name.Name._char = (char *)malloc(Len);\
name.MaxLen = Len

extern void myFree(strnew Space);

#endif
