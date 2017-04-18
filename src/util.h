#ifndef UTIL_H
#define UTIL_H

#include <Windows.h>

static char* CombineStrings(const char * str1, const char* str2)
{
    char * str3 = (char *) malloc(1 + strlen(str1) + strlen(str2));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}

#endif
