#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"

typedef enum {
    ARITHMETIC_PLUS,
    ARITHMETIC_MINUS,
    ARITHMETIC_DIVISION,
    ARITHMETIC_MULTIPLICATION,
    ARITHMETIC_MODULO
}ArithmeticOperators;

typedef enum {
    LOGIC_EQUALS,
    LOGIC_NOT_EQUALS,
    LOGIC_GREATER_THAN,
    LOGIC_GREATER_EQUALS,
    LOGIC_LESS_THAN,
    LOGIC_LESS_EQUALS,
    LOGIC_AND,
    LOGIC_OR
}LogicOperators;

typedef enum Token_Type {
    TOK_EQUALS,
    TOK_STR,
    TOK_NUM,
    TOK_ID,
    TOK_OPEN_PARENTHESIS,
    TOK_CLOSE_PARENTHESIS,
    TOK_WHEN,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_OPEN_CURLY,
    TOK_CLOSE_CURLY,
    TOK_ARITHMETIC,
    TOK_LOGIC
} Token_Type;

typedef struct Token {
    Token_Type type;
    String str;
    int line;
} Token;

typedef struct Token_List {
    size_t heap;
    size_t size;
    Token* content;
} Token_List;

Token Token_New(int line, Token_Type type, const char* str);

Token_Type Token_Type_From_Word(const char* word);

void Token_List_Init(Token_List* tokens);
void Token_List_Push(Token_List* tokens, Token tok);
void Token_List_Free(Token_List* tokens);

int Tokenize_File(Token_List* tokens, const char* path);
#endif // LEXER_H_