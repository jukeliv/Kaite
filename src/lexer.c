#include "../include/lexer.h"

Token Token_New(int line, Token_Type type, const char* str) {
    Token tok;
	tok.type = type;
	tok.str = String_CStr(str);
    tok.line = line;
	return tok;
}

Token_Type Token_Type_From_Word(const char* word)
{
    if(!strcmp(word, "when"))
    {
        return TOK_WHEN;
    }
    else if(!strcmp(word, "if"))
    {
        return TOK_IF;
    }
    else if(!strcmp(word, "else"))
    {
        return TOK_ELSE;
    }
    else if(!strcmp(word, "while"))
    {
        return TOK_WHILE;
    }
    return TOK_ID;
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

    int line = 1;

    for (size_t i = 0; i < size; ++i) {

        while (is_white_space(source[i]))
        {
            if(source[i] == '\n')
                ++line;
            ++i;
        }
        lex_i = 0;
        memset(lex, 0, sizeof(lex));

        switch (source[i]) {
            case ';': // Comment
                if(source[++i] == ';')
                {
                    ++i;
                    while(source[i] != ';' && source[i+1] != ';')
                        ++i;
                }
                else
                {
                    while (source[i] != '\n')
                        ++i;
                }
            break;

            case '=':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
                else
                {
                    lex[0] = '=';
                    Token_List_Push(tokens, Token_New(line, TOK_EQUALS, lex));
                }
            break;

            case '!':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_NOT_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
            break;

            case '<':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_LESS_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
                else
                {
                    lex[0] = LOGIC_LESS_THAN;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
            break;

            case '>':
                if(source[i+1] == '=')
                {
                    lex[0] = LOGIC_GREATER_EQUALS;
                    ++i;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
                else
                {
                    lex[0] = LOGIC_GREATER_THAN;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
            break;

            case '&':
                if(source[i+1] == '&')
                {
                    lex[0] = LOGIC_AND;
                    ++i;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
                else
                {
                    printf("ERROR: Bitwise \"&\" is not implemented!!!\n");
                    exit(1);
                }
            break;
            case '|':
                if(source[i+1] == '|')
                {
                    lex[0] = LOGIC_OR;
                    ++i;
                    Token_List_Push(tokens, Token_New(line, TOK_LOGIC, lex));
                }
                else
                {
                    printf("ERROR: Bitwise \"|\" is not implemented!!!\n");
                    exit(1);
                }
            break;

            case '+':
                lex[0] = ARITHMETIC_PLUS;
                Token_List_Push(tokens, Token_New(line, TOK_ARITHMETIC, lex));
            break;
            
            case '-':
                lex[0] = ARITHMETIC_MINUS;
                Token_List_Push(tokens, Token_New(line, TOK_ARITHMETIC, lex));
            break;
            
            case '*':
                lex[0] = ARITHMETIC_MULTIPLICATION;
                Token_List_Push(tokens, Token_New(line, TOK_ARITHMETIC, lex));
            break;
            
            case '/':
                lex[0] = ARITHMETIC_DIVISION;
                Token_List_Push(tokens, Token_New(line, TOK_ARITHMETIC, lex));
            break;

            case '%':
                lex[0] = ARITHMETIC_MODULO;
                Token_List_Push(tokens, Token_New(line, TOK_ARITHMETIC, lex));
            break;

            case '(':
                lex[0] = '(';
                Token_List_Push(tokens, Token_New(line, TOK_OPEN_PARENTHESIS, lex));
            break;

            case ')':
                lex[0] = ')';
                Token_List_Push(tokens, Token_New(line, TOK_CLOSE_PARENTHESIS, lex));
            break;
                
            case '{':
                lex[0] = '{';
                Token_List_Push(tokens, Token_New(line, TOK_OPEN_CURLY, lex));
            break;

            case '}':
                lex[0] = '}';
                Token_List_Push(tokens, Token_New(line, TOK_CLOSE_CURLY, lex));
            break;

            case '"':
                ++i;
                while (source[i] != '"' && lex_i < sizeof(lex) - 1) {
                    lex[lex_i++] = source[i++];
                }
                lex[lex_i] = '\0';
                Token_List_Push(tokens, Token_New(line, TOK_STR, lex));
            break;

            default:
            {
                if (is_from_alphabet(source[i])) {
					while (is_from_alphabet(source[i]) || is_num(source[i]) || source[i] == '_')
                        lex[lex_i++] = source[i++];
                    lex[lex_i] = '\0';
                    
                    Token_List_Push(tokens, Token_New(line, Token_Type_From_Word(lex), lex));
                    
                    i--;
                    continue;
                }
                else if(is_num(source[i]))
                {
                    while (is_num(source[i]) || source[i] == '.')
                    {
                        lex[lex_i++] = source[i++];
                    }
                    Token_List_Push(tokens, Token_New(line, TOK_NUM, lex));
					
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