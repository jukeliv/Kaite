#include "../include/lexer.h"

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
    free(tokens->content);
}

int Tokenize_File(Token_List* tokens, const char* source) {

    char lex[256] = {0};
    unsigned char lex_i = 0;

    size_t size = strlen(source);

    for (size_t i = 0; i < size; ++i) {

        while (is_white_space(source[i]))
            ++i;

        lex_i = 0;
        memset(lex, 0, sizeof(lex));

        switch (source[i]) {
            case ';': // Comment
                while (source[i] != '\n')
                    ++i;
            break;

            case '=':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(TOK_LOGIC, lex));
                }
                else
                {
                    lex[0] = '=';
                    Token_List_Push(tokens, Token_New(TOK_EQUALS, lex));
                }
            break;

            case '!':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_NOT_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(TOK_LOGIC, lex));
                }
            break;

            case '<':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_LESS_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(TOK_LOGIC, lex));
                }
                else
                {
                    lex[0] = LOGIC_LESS_THAN;
                    Token_List_Push(tokens, Token_New(TOK_LOGIC, lex));
                }
            break;

            case '>':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_GREATER_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(TOK_LOGIC, lex));
                }
                else
                {
                    lex[0] = LOGIC_GREATER_THAN;
                    Token_List_Push(tokens, Token_New(TOK_LOGIC, lex));
                }
            break;

            case '+':
                lex[0] = ARITHMETIC_PLUS;
                Token_List_Push(tokens, Token_New(TOK_ARITHMETIC, lex));
            break;
            
            case '-':
                if(is_num(source[i+1]))
                {
                    lex[lex_i++] = source[i++];
                    while (is_num(source[i]) || source[i] == '.')
                    {
                        lex[lex_i++] = source[i++];
                    }
                    Token_List_Push(tokens, Token_New(TOK_NUM, lex));
					
                    i--;
                }
                else
                {
                    lex[0] = ARITHMETIC_MINUS;
                    Token_List_Push(tokens, Token_New(TOK_ARITHMETIC, lex));
                }
            break;
            
            case '*':
                lex[0] = ARITHMETIC_MULTIPLICATION;
                Token_List_Push(tokens, Token_New(TOK_ARITHMETIC, lex));
            break;
            
            case '/':
                lex[0] = ARITHMETIC_DIVISION;
                Token_List_Push(tokens, Token_New(TOK_ARITHMETIC, lex));
            break;

            case '(':
                lex[0] = '(';
                Token_List_Push(tokens, Token_New(TOK_OPEN_PARENTHESIS, lex));
            break;

            case ')':
                lex[0] = ')';
                Token_List_Push(tokens, Token_New(TOK_CLOSE_PARENTHESIS, lex));
            break;
                
            case '{':
                lex[0] = '{';
                Token_List_Push(tokens, Token_New(TOK_OPEN_CURLY, lex));
            break;

            case '}':
                lex[0] = '}';
                Token_List_Push(tokens, Token_New(TOK_CLOSE_CURLY, lex));
            break;

            case '"':
                ++i;
                while (source[i] != '"' && lex_i < sizeof(lex) - 1) {
                    lex[lex_i++] = source[i++];
                }
                lex[lex_i] = '\0';
                Token_List_Push(tokens, Token_New(TOK_STR, lex));
            break;

            default:
            {
                if (is_from_alphabet(source[i])) {
					while (is_from_alphabet(source[i]) || is_num(source[i]) || source[i] == '_')
                        lex[lex_i++] = source[i++];
                    lex[lex_i] = '\0';
                    if(!strcmp(lex, "when"))
                    {
                        Token_List_Push(tokens, Token_New(TOK_WHEN, lex));
                    }
                    else if(!strcmp(lex, "if"))
                    {
                        Token_List_Push(tokens, Token_New(TOK_IF, lex));
                    }
                    else if(!strcmp(lex, "while"))
                    {
                        Token_List_Push(tokens, Token_New(TOK_WHILE, lex));
                    }
                    else
                    {
                        Token_List_Push(tokens, Token_New(TOK_ID, lex));
                    }
					i--;
                    continue;
                }
                else if(is_num(source[i]))
                {
                    while (is_num(source[i]) || source[i] == '.')
                    {
                        lex[lex_i++] = source[i++];
                    }
                    Token_List_Push(tokens, Token_New(TOK_NUM, lex));
					
                    i--;
                    continue;
                }
                else {
                    printf("ERROR: Unknown character found in file!!! ( %c : %d )\n", source[i], source[i]);
                    return 1;
                }
            }
            break;
        }
    }

    return 0;
}