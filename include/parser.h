#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"

/*
    conditional -> "when" STRING program "end"
    binOp -> (literal | binOp) ("+" | "-" | "*" | "/") (literal | binOp)
    program -> list[expresion]
    expresion -> group | set | function
    function -> ID group
    group -> "(" list[literal] ")"
    Set -> ID "=" literal
    literal -> STRING | ID
*/

typedef enum
{
    Program,
    Group,
    Set,
    Function,
    Conditional,
    Literal,
    BinOp,
    Parenthesized
}Expr_Type;

struct Expr;
typedef struct
{
    size_t heap;
    size_t size;
    struct Expr* content;
}Expr_List;

typedef enum
{
    LITERAL_String,
    LITERAL_Number,
    LITERAL_Id
}Literal_Type;

typedef struct Expr
{
    Expr_Type type;
    union
    {
		struct
		{
			Expr_List program;
		}Program;

		struct{
            Expr_List literals;
		}Group;

		struct{
			String ID;
            struct Expr* literal;
		}Set;

		struct{
			String ID;
			struct Expr* group;
		}Function;

        struct{
            String ID; // when ID [program] end
			struct Expr* program;
		}Conditional;

        struct{
            struct Expr* left;
            BinaryOperators op;
            struct Expr* right;
        }BinOp;

        struct{
            struct Expr* expr;
        }Parenthesized;

        struct{
            Literal_Type type;
            String value;
        }Literal;
    }e;
}Expr;

void Expr_List_Init(Expr_List* list);
void Expr_List_Push(Expr_List* list, Expr expr);

Expr Expr_Group(Expr_List list);
Expr Expr_Function(String ID, Expr group);
Expr Expr_Literal(Token tok);
Expr Expr_BinOp(BinaryOperators op, Expr left, Expr right);
Expr Expr_Parenthesized(Expr node);
Expr Expr_Set(String ID, Expr literal);
Expr Expr_Conditional(String ID, Expr program);
Expr Expr_Program();

Expr Parse_Group(size_t* i, Token_List* tokens);
Expr Parse_Function(size_t* i, Token_List* tokens);
Expr Parse_BinOp(size_t* i, Token_List* tokens);
Expr Parse_Tokens(size_t* i, Token_List* tokens);
Expr Parse_Code_Block(size_t* i, Token_List* tokens);// `i` should start at `{`
Expr Parse_Conditional(size_t* i, Token_List* tokens);
Expr Parse_Tokens(size_t* i, Token_List* tokens);

void Parse_Program(Expr* program, Token_List* tokens);

#endif // PARSER_H_