#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "str.h"

typedef enum Token_Type {
    TOK_EQUALS,
    TOK_STR,
    TOK_ID,
    TOK_OPEN_PARENTHESIS,
    TOK_CLOSE_PARENTHESIS,
    TOK_WHEN,
    TOK_OPEN_CURLY,
    TOK_CLOSE_CURLY
} Token_Type;

typedef struct Token {
    Token_Type type;
    String str;
} Token;

typedef struct Token_List {
    size_t heap;
    size_t size;
    Token* content;
} Token_List;

Token Token_New(Token_Type type, const char* str) {
    Token tok;
	tok.type = type;
	tok.str = String_CStr(str);
	return tok;
}

void Token_List_Init(Token_List* tokens) {
    tokens->size = 0;
    tokens->heap = 1;
    tokens->content = malloc(tokens->heap * sizeof(Token));
    if (!tokens->content) {
        printf("ERROR: Couldn't allocate memory for Token_List\n");
        exit(1);
    }
}

void Token_List_Push(Token_List* tokens, Token tok) {
    if (tokens->size >= tokens->heap) {
        tokens->heap *= 2;
        tokens->content = realloc(tokens->content, tokens->heap * sizeof(Token));
        if (!tokens->content) {
            printf("ERROR: Couldn't allocate memory for Token_List\n");
            exit(1);
        }
    }
    tokens->content[tokens->size++] = tok;
}

void Token_List_Free(Token_List* tokens)
{
    tokens->heap = 0;
    tokens->size = 0;
    if(tokens->content)
        free(tokens->content);
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

int Tokenize_File(Token_List* tokens, const char* path) {
    const char* source = read_file(path);

    if (!source)
        exit(1);

    char lex[256] = {0};
    unsigned char lex_i = 0;

    for (size_t i = 0; i < strlen(source); ++i) {
        while (is_white_space(source[i]))
            ++i;

        lex_i = 0;
        memset(lex, 0, sizeof(lex));

        switch (source[i]) {
            case ';': // Comment
                while (source[i] != '\n')
                    ++i;
                continue;

            case '=':
                lex[0] = '=';
                Token_List_Push(tokens, Token_New(TOK_EQUALS, lex));
                continue;

            case '"':
                ++i;
                while (source[i] != '"' && lex_i < sizeof(lex) - 1) {
                    lex[lex_i++] = source[i++];
                }
                Token_List_Push(tokens, Token_New(TOK_STR, lex));
                continue;

            case '(':
                lex[0] = '(';
                Token_List_Push(tokens, Token_New(TOK_OPEN_PARENTHESIS, lex));
                continue;

            case ')':
                lex[0] = ')';
                Token_List_Push(tokens, Token_New(TOK_CLOSE_PARENTHESIS, lex));
                continue;
                
            case '{':
                lex[0] = '{';
                Token_List_Push(tokens, Token_New(TOK_OPEN_CURLY, lex));
                continue;

            case '}':
                lex[0] = '}';
                Token_List_Push(tokens, Token_New(TOK_CLOSE_CURLY, lex));
                continue;

            default:
                if (is_from_alphabet(source[i])) {
					while (is_from_alphabet(source[i]))
                        lex[lex_i++] = source[i++];
                    if(!strcmp(lex, "when"))
                    {
                        Token_List_Push(tokens, Token_New(TOK_WHEN, lex));
                    }
                    else
                    {
                        Token_List_Push(tokens, Token_New(TOK_ID, lex));
                    }
					i--;
                    continue;
                } else if (!is_white_space(source[i]) && source[i] != '\0') {
                    printf("ERROR: Unknown character found in file!!! ( %c : %d )\n", source[i], source[i]);
                    return 1;
                }
                break;
        }
    }

    return 0;
}

#endif // LEXER_H_
