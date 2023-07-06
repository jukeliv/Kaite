#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct String {
    size_t heap;
    size_t size;
    char* content;
} String;

String String_CStr(const char* cstr);
String String_Concat(String str, String str2);
bool String_Compare(String str, String str2);

char* read_file(const char* path);

bool is_from_alphabet(char c);
bool is_white_space(char c);
bool is_num(char c);

#endif // UTILS_H_
