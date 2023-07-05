#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"

typedef enum {
    BINOP_PLUS,
    BINOP_MINUS,
    BINOP_DIVISION,
    BINOP_MULTIPLICATION
}BinaryOperators;

typedef enum Token_Type {
    TOK_EQUALS,
    TOK_STR,
    TOK_NUM,
    TOK_ID,
    TOK_OPEN_PARENTHESIS,
    TOK_CLOSE_PARENTHESIS,
    TOK_WHEN,
    TOK_OPEN_CURLY,
    TOK_CLOSE_CURLY,
    TOK_BINOP
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

Token Token_New(Token_Type type, const char* str);

void Token_List_Init(Token_List* tokens);
void Token_List_Push(Token_List* tokens, Token tok);
void Token_List_Free(Token_List* tokens);

int Tokenize_File(Token_List* tokens, const char* path);
#endif // LEXER_H_
