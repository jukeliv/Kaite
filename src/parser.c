#include "..\include\parser.h"

void Expr_List_Init(Expr_List* list)
{
    list->heap = 1;
    list->size = 0;
    list->content = malloc(list->heap * sizeof(Expr));
    if (!list->content)
    {
        printf("ERROR: Couldn't allocate memory for Expr_List\n");
        exit(1);
    }
}

void Expr_List_Push(Expr_List* list, Expr expr)
{
    if (list->size >= list->heap)
    {
        list->heap *= 2;
        list->content = realloc(list->content, list->heap * sizeof(Expr));
        if (!list->content)
        {
            printf("ERROR: Couldn't allocate memory for Expr_List\n");
            exit(1);
        }
    }
    list->content[list->size++] = expr;
}

Expr Expr_Program()
{
    Expr expr;
    expr.type = Program;
    Expr_List_Init(&expr.e.Program.program);
    return expr;
}

Expr Expr_Group(Expr_List list)
{
    Expr expr;
    expr.type = Group;
    expr.e.Group.literals.size = list.size;
    expr.e.Group.literals.content = malloc(list.size * sizeof(Expr));

    if (expr.e.Group.literals.content == NULL)
    {
        printf("ERROR: Couldn't allocate memory for Token_List\n");
        exit(1);
    }

    for (size_t i = 0; i < list.size; i++)
    {
        expr.e.Group.literals.content[i] = list.content[i];
    }

    return expr;
}

Expr Expr_Function(String ID, Expr group)
{
    Expr expr;
    expr.type = Function;
    expr.e.Function.ID = ID;

    expr.e.Function.group = malloc(sizeof(Expr));
    *expr.e.Function.group = group;

    return expr;
}

Expr Expr_Literal(Token tok)
{
    Expr literal;
    literal.type = Literal;
    literal.e.Literal.value = tok.str;
    switch(tok.type)
    {
        case TOK_STR:
        {
            literal.e.Literal.type = LITERAL_String;
        }
        break;

        case TOK_NUM:
        {
            literal.e.Literal.type = LITERAL_Number;
        }
        break;
        
        case TOK_ID:
        {
            literal.e.Literal.type = LITERAL_Id;
        }
        break;

        default:
        {
            printf("ERROR: Can't set Literal to Token Type!!! (%d : %s )\n", tok.type, tok.str.content);
            exit(1);
        }
        break;
    }
    return literal;
}

Expr Expr_BinOp(BinaryOperators op, Expr left, Expr right)
{
    Expr expr;
    
    expr.type = BinOp;

    // Assign the left operand directly
    expr.e.BinOp.left = malloc(sizeof(Expr));
    *expr.e.BinOp.left = left;


    expr.e.BinOp.op = op;
    
    // Assign the right operand directly
    expr.e.BinOp.right = malloc(sizeof(Expr));
    *expr.e.BinOp.right = right;

    return expr;
}

Expr Expr_Parenthesized(Expr node)
{
    Expr expr;
    
    expr.type = Parenthesized;

    // Assign the left operand directly
    expr.e.Parenthesized.expr = malloc(sizeof(Expr));
    *expr.e.Parenthesized.expr = node;

    return expr;
}

Expr Expr_Set(String ID, Expr literal)
{
    Expr expr;
    expr.type = Set;
    expr.e.Set.ID = ID;
    
    expr.e.Set.literal = malloc(sizeof(Expr));
    *expr.e.Set.literal = literal;

    return expr;
}

Expr Expr_Conditional(String ID, Expr program)
{
    Expr expr;
    expr.type = Conditional;
    expr.e.Conditional.ID = ID;
    
    expr.e.Conditional.program = malloc(sizeof(Expr));
    *expr.e.Conditional.program = program;

    return expr;
}

Expr Parse_Group(size_t* i, Token_List* tokens)
{
    Expr_List list;
    Expr_List_Init(&list);

    size_t j = (*i);
    ++j;  // it needs to skip the first TOK_OPEN_PARENTHESIS so it can parse properly
    
    size_t end = j;
    for(;tokens->content[end].type != TOK_CLOSE_PARENTHESIS; ++end);

    for(; j < end; ++j)
    {
        if(tokens->content[j+1].type == TOK_BINOP || tokens->content[j].type == TOK_OPEN_PARENTHESIS)
        {
            (*i) = j;
            Expr_List_Push(&list, Parse_BinOp(i, tokens));
            j = (*i)-1;
        }
        else
        {
            Expr_List_Push(&list, Expr_Literal(tokens->content[j]));
        }
    }
    (*i) = j;
    return Expr_Group(list);
}

Expr Parse_Function(size_t* i, Token_List* tokens)
{
    String ID = tokens->content[(*i)++].str;
    Expr expr = Parse_Group(i, tokens);
    return Expr_Function(ID, expr);
}

Expr Parse_BinOp(size_t* i, Token_List* tokens)
{
    Expr left;
    if (tokens->content[*i].type == TOK_OPEN_PARENTHESIS)
    {
        (*i)++;
        left = Expr_Parenthesized(Parse_BinOp(i, tokens));
    }
    else
    {
        left = Expr_Literal(tokens->content[*i]);
    }

    (*i)++;

    while (tokens->content[*i].type == TOK_BINOP)
    {
        BinaryOperators operator = tokens->content[*i].str.content[0]; // Get the binary operator
        (*i)++;

        Expr right;
        if (tokens->content[*i].type == TOK_OPEN_PARENTHESIS)
        {
            (*i)++;
            right = Expr_Parenthesized(Parse_BinOp(i, tokens));
        }
        else
        {
            right = Expr_Literal(tokens->content[*i]);
        }

        (*i)++;

        // Check if the next token is a closing parenthesis
        if (tokens->content[*i].type == TOK_CLOSE_PARENTHESIS)
        {
            // Return the current expression if it ends with a closing parenthesis
            return Expr_BinOp(operator, left, right);
        }
        else
        {
            // Continue parsing the next term
            left = Expr_BinOp(operator, left, right);
        }
    }
    (*i)--;

    return left;
}

// `i` should start at `{`
Expr Parse_Code_Block(size_t* i, Token_List* tokens)
{
    if(tokens->content[(*i)].type != TOK_OPEN_CURLY)
    {
        printf("ERROR: You are not opening code block\n");
        exit(1);
    }
    Expr block = Expr_Program();

    size_t j = (*i)+1;
    for(;tokens->content[j].type != TOK_CLOSE_CURLY; ++j)
    {
        if(j >= tokens->size)
        {
            printf("ERROR: You forgot to close code block!!!\n");
            exit(1);
        }
        Expr_List_Push(&block.e.Program.program, Parse_Tokens(&j, tokens));
    }
    (*i) = j;
    return block;
}

Expr Parse_Conditional(size_t* i, Token_List* tokens)
{
    if(tokens->content[(*i)+1].type != TOK_STR)
    {
        printf("ERROR: Trying to pass non-STR to conditional when!!!\n");
        exit(1);
    }
    String ID = tokens->content[++(*i)].str;

    ++(*i);

    return Expr_Conditional(ID, Parse_Code_Block(i, tokens));
}

Expr Parse_Tokens(size_t* i, Token_List* tokens)
{
    switch(tokens->content[*i].type)
    {
        case TOK_WHEN:
        {
            return Parse_Conditional(i, tokens);
        }
        break;
		case TOK_ID:
        {
            switch(tokens->content[(*i)+1].type)
            {
                break;
                case TOK_EQUALS:
                {
                    //ID = 1+1+1
                    String ID = tokens->content[*i].str;
                    Token tok = tokens->content[(*i)+2];
                    
                    (*i) += 2;
                    if(tokens->content[(*i)+1].type == TOK_BINOP || tokens->content[(*i)].type == TOK_OPEN_PARENTHESIS)
                    {
                        return Expr_Set(ID, Parse_BinOp(i, tokens));
                    }
                    else
                    {
                        return Expr_Set(ID, Expr_Literal(tok));
                    }
                }
                break;
                case TOK_OPEN_PARENTHESIS:
                {
                    return Parse_Function(i, tokens);
                }
                break;
                default:
                {
                    printf("%d : %s\n", tokens->content[*i].type, tokens->content[*i].str.content);
                    printf("ERROR: Incorrect Syntax!!! ( %d : %s ) \n", tokens->content[(*i)+1].type, tokens->content[(*i)+1].str.content);
                    exit(1);
                }
                break;
            }
        }
        break;
        default:
        {
            printf("TODO: Think about another error message\n");
            printf("%d : %s", tokens->content[*i].type, tokens->content[*i].str.content);
            exit(1);
        }
        break;
	}
}


void Parse_Program(Expr* program, Token_List* tokens)
{
    for(size_t i = 0; i < tokens->size; ++i)
    {
		Expr_List_Push(&program->e.Program.program, Parse_Tokens(&i, tokens));
	}
}