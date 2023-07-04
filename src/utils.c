#include "..\include\utils.h"

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

char* read_file(const char* path) {
    FILE* fp = fopen(path, "rb");
    if (!fp) {
        printf("ERROR: Couldn't read file %s\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t fs = ftell(fp);
    rewind(fp);

    char* buf = malloc(fs + 1); // Increase size by 1 for null terminator
    if (!buf) {
        printf("ERROR: Couldn't allocate memory for file!!!\n");
        fclose(fp);
        return NULL;
    }

    if (fread(buf, sizeof(char), fs, fp) != fs) {
        printf("ERROR: Couldn't read file!!!\n");
        fclose(fp);
        free(buf);
        return NULL;
    }

    buf[fs] = '\0'; // Add null terminator
    fclose(fp);

    return buf;
}

bool is_from_alphabet(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_white_space(char c) {
    return (c == '\n' || c == ' ') || (c == '\r' || c == '\t');
}