#include "..\include\lexer.h"

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