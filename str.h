#ifndef STR_H_
#define STR_H_

#include <stdlib.h>
#include <string.h>

typedef struct String {
    size_t heap;
    size_t size;
    char* content;
} String;

String String_CStr(const char* cstr) {
    String str;
    str.heap = strlen(cstr) + 1;
    str.size = strlen(cstr);
    str.content = malloc(str.heap * sizeof(char));
    if (str.content != NULL) {
        strcpy(str.content, cstr);
    }
    
    return str;
}

#endif // STR_H_
