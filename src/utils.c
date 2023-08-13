#include <utils.h>

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

String String_Concat(String str1, String str2) {
    size_t len1 = str1.size;
    size_t len2 = str2.size;
    
    String result;
    result.heap = len1 + len2 + 1;
    result.size = len1 + len2;
    result.content = (char*)malloc(result.heap * sizeof(char));
    
    if (result.content == NULL) {
        fprintf(stderr, "Failed to allocate memory for String concatenation.\n");
        exit(1);
    }
    
    memcpy(result.content, str1.content, len1);
    memcpy(result.content + len1, str2.content, len2);
    result.content[result.size] = '\0';
    
    return result;
}

bool String_Compare(String str, String str2)
{
    if(str.size != str2.size)
    {
        printf("%d != %d", str.size, str2.size);
        return false;
    }
    for(size_t i = 0; i < str.size; ++i)
    {
        if(str.content[i] != str2.content[i])
            return false;
    }
    return true;
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

bool is_num(char c) {
    return (c >= '0' && c <= '9');
}