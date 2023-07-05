#include "..\include\interpreter.h"

Variable_List global_variables;
Variable_List* local_variables;

void Variable_List_Init(Variable_List* list)
{
    list->heap = 1;
    list->size = 0;
    list->content = malloc(list->heap * sizeof(Variable));
}

void Variable_List_Free(Variable_List* list)
{
    list->heap = 0;
    list->size = 0;
    if(list->content)
        free(list->content);
}

void Variable_List_Push(Variable_List* list, Variable var)
{
    if (list->size >= list->heap) {
        list->heap *= 2;
        list->content = realloc(list->content, list->heap * sizeof(Variable));
        if (!list->content) {
            printf("ERROR: Couldn't allocate memory for Variable_List\n");
            exit(1);
        }
    }
    list->content[list->size++] = var;
}

void Variable_List_Remove(Variable_List* list, int index)
{
    if (index < 0 || index >= list->size) {
        printf("ERROR: Invalid index for removal\n");
        return;
    }
    // Shift elements to the left starting from the index to remove
    for (int i = index; i < list->size - 1; i++) {
        list->content[i] = list->content[i + 1];
    }

    // Update the size and reallocate memory if necessary
    list->size--;
}

bool Variable_List_Exist(Variable_List* list, const char* id)
{
    for(size_t i = 0; i < list->size; ++i)
    {
        if(!strcmp(list->content[i].id, id))
        {
            return true;
        }
    }
    return false;
}
size_t Variable_List_Find(Variable_List* list, const char* id)
{
    if(!Variable_List_Exist(list, id))
        return list->size;
    for(size_t i = 0; i < list->size; ++i)
    {
        if(!strcmp(list->content[i].id, id))
            return i;
    }
    return 0; // just so the compiler doesn't yell at me!!!
}

void Initialize_Global()
{
    Variable_List_Init(&global_variables);
}

Variable* Get_Variable(const char* variable)
{
    if(!Variable_List_Exist(local_variables, variable) && !Variable_List_Exist(&global_variables, variable))
    {
        return NULL;
    }
    Variable* var;
    if(Variable_List_Exist(&global_variables, variable))
        var = &global_variables.content[Variable_List_Find(&global_variables, variable)];
    else
        var = &local_variables->content[Variable_List_Find(local_variables, variable)];
    return var;
}

Variable_List* Get_Variable_List(Variable_List* variables, const char* variable)
{
    if(!Variable_List_Exist(variables, variable) && !Variable_List_Exist(&global_variables, variable))
    {
        printf("ERROR: Couldn't find variable \"%s\"\n", variable);
        return NULL;
    }
    if(Variable_List_Exist(&global_variables, variable))
        return &global_variables;
    else
        return variables;
}

//TODO: Throw warning when variable is in global scope
void Globalize_Variable(const char* id)
{
    Variable* var = Get_Variable(id);
    if(!var)
    {
        printf("ERROR: Couldn't find variable \"%s\"\n", id);
        exit(1);
    }
    Variable_List_Push(&global_variables, *(var));
    size_t index = Variable_List_Find(Get_Variable_List(local_variables, id), id);
    Variable_List_Remove(local_variables, index);
}

void Interpret_Conditional(Expr expr)
{
    expr.e.Conditional.program->type = Program;
    if(!strcmp(expr.e.Conditional.ID.content, "WINDOWS"))
    {
        #ifdef _WIN32
        Interpret_Program(*expr.e.Conditional.program);
        #endif
    }
    else if(!strcmp(expr.e.Conditional.ID.content, "LINUX"))
    {
        #ifdef __linux__
        Interpret_Program(*expr.e.Conditional.program);
        #endif
    }
    else
    {
        if(Get_Variable(expr.e.Conditional.ID.content))
        {
            Interpret_Program(*expr.e.Conditional.program);
        }
    }
}

void Interpret_Set(Expr expr)
{
    if(!Get_Variable(expr.e.Set.ID.content))
    {
        Variable var;
        var.id = strdup(expr.e.Set.ID.content);
        var.type = Interpret(*expr.e.Set.literal).type;
        Variable_List_Push(local_variables, var);
    }
    Variable* var = Get_Variable(expr.e.Set.ID.content);

    Expr literal = *expr.e.Set.literal;

    Variable v = Interpret(literal);

    if(var->type != v.type)
    {
        printf("ERROR: Can't set variable %s to a non-matching data-type\n", var->id);
        exit(1);
    }

    v.id = var->id;
    *var = v;
}

void Interpret_Function(Expr expr)
{
	Expr group = *(expr.e.Function.group);
	String functionName = expr.e.Function.ID;

    if(!strcmp(functionName.content, "cmd"))
    {
        if (!group.e.Group.literals.size) {
            printf("ERROR: No arguments passed to \"cmd\"\n");
            exit(1);
        }

        size_t str_size = 0;
        for (size_t i = 0; i < group.e.Group.literals.size; ++i) {
            Expr literal = group.e.Group.literals.content[i];
            Variable v = Interpret(literal);
            switch(v.type)
            {
                case Var_String:
                {
                    str_size += v.value.str.size;
                }
                break;
                case Var_Number:
                    str_size += snprintf(NULL, 0, "%f", v.value.num);
                break;
            }
            str_size++; // Account for space separator
        }

        char* buffer = calloc((str_size + 1), sizeof(char)); // Allocate space for null terminator
        if (!buffer) {
            printf("ERROR: Couldn't allocate memory for Command-Buffer\n");
            return;
        }

        size_t buffer_index = 0;
        for (size_t i = 0; i < group.e.Group.literals.size; ++i) {
            Expr literal = group.e.Group.literals.content[i];
            Variable v = Interpret(literal);
            if (i != 0) {
                buffer[buffer_index++] = ' ';
            }

            switch(v.type)
            {
                case Var_String:
                    memcpy(buffer + buffer_index, v.value.str.content, v.value.str.size);
                    buffer_index += v.value.str.size;
                break; 
                case Var_Number:
                    buffer_index += sprintf(buffer + buffer_index, "%f", Interpret(literal).value.num);
                break;
            }
        }

        system(buffer);
        free(buffer);
    }
    else if(!strcmp(functionName.content, "print"))
    {
        if(!group.e.Group.literals.size)
        {
            printf("ERROR: No arguments passed to \"print\"\n");
            return;
        }
        for(size_t i = 0; i < group.e.Group.literals.size; ++i)
        {
            Expr literal = group.e.Group.literals.content[i];
            Variable v = Interpret(literal);
            
            switch(v.type)
            {
                case Var_String:
                    printf("%s\n", v.value.str.content);
                break;
                case Var_Number:
                    printf("%f\n", v.value.num);
                break;
            }
        }
    }
    else if(!strcmp(functionName.content, "input"))
    {
        if(!group.e.Group.literals.size)
        {
            printf("ERROR: No arguments passed to \"input\"\n");
            return;
        }
        else if(group.e.Group.literals.size > 1)
        {
            printf("ERROR: To many arguments passed to \"input\"\n");
            return;
        }

        Expr literal = group.e.Group.literals.content[0];

        switch(literal.e.Literal.type)
        {
            case LITERAL_String:
            {
                printf("ERROR: Wrong data-type passed to function \"input\"!!!\n");
                exit(1);
            }
            break;
            case LITERAL_Id:
            {
                Variable* var = Get_Variable(literal.e.Literal.value.content);
                if(!var)
                {
                    printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                    exit(1);
                }
                switch(var->type)
                {
                    case Var_String:
                    {
                        char buf[256] = {0};
                        scanf("%256s", &buf);
                        var->value.str = String_CStr(buf);
                    }
                    break;
                    case Var_Number:
                        scanf("%f", &var->value.num);
                    break;
                }
            }
            break;
        }
    }
    else if(!strcmp(functionName.content, "global"))
    {
        if(!group.e.Group.literals.size)
        {
            printf("ERROR: No arguments passed to \"input\"\n");
            exit(1);
        }
        else if(group.e.Group.literals.size > 1)
        {
            printf("ERROR: To many arguments passed to \"input\"\n");
            exit(1);
        }

        Expr literal = group.e.Group.literals.content[0];

        switch(literal.e.Literal.type)
        {
            case LITERAL_String:
            {
                printf("ERROR: Can't use non-Variable data-type in function \"global\"");
                exit(1);
            }
            break;
            case LITERAL_Id:
            {
                Globalize_Variable(literal.e.Literal.value.content);
            }
            break;
        }
    }
    else if(!strcmp(functionName.content, "remove"))
    {
        if(!group.e.Group.literals.size)
        {
            printf("ERROR: No arguments passed to \"input\"\n");
            exit(1);
        }
        else if(group.e.Group.literals.size > 1)
        {
            printf("ERROR: To many arguments passed to \"input\"\n");
            exit(1);
        }

        Expr literal = group.e.Group.literals.content[0];

        switch(literal.e.Literal.type)
        {
            case LITERAL_String:
            {
                printf("ERROR: Can't use non-Variable data-type in function \"remove\"");
                exit(1);
            }
            break;
            case LITERAL_Id:
            {
                Variable* var = Get_Variable(literal.e.Literal.value.content);
                if(!var)
                {
                    printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                    exit(1);
                }
                size_t index = Variable_List_Find(Get_Variable_List(local_variables, literal.e.Literal.value.content), literal.e.Literal.value.content);
                if(index >= local_variables->size) // if variable is not in local scope, search in global
                {
                    index = Variable_List_Find(Get_Variable_List(&global_variables, literal.e.Literal.value.content), literal.e.Literal.value.content);
                    Variable_List_Remove(&global_variables, index);
                    return;
                }
                Variable_List_Remove(local_variables, index);
            }
            break;
        }
    }
    else
    {
        printf("TODO: Implement user-defined functions!!!\n");
        exit(1);
    }
}

Variable Interpret_Literal(Expr node)
{
    Variable var;
    switch(node.e.Literal.type)
    {
        case LITERAL_String:
            var.value.str = node.e.Literal.value;
            var.type = Var_String;
        break;
        case LITERAL_Number:
            var.value.num = strtod(node.e.Literal.value.content, NULL);
            var.type = Var_Number;
        break;
        case LITERAL_Id:
        {
            Variable* var2 = Get_Variable(node.e.Literal.value.content);
            if(!var2)
            {
                printf("ERROR: Couldn't find variable \"%s\"\n", node.e.Literal.value.content);
                exit(1);
            }
            var = *var2;
        }
        break;
    }
    return var;
}

Variable Interpret_BinOp(Expr node)
{
    Variable left_result = Interpret(*node.e.BinOp.left);
    Variable right_result = Interpret(*node.e.BinOp.right);

    if(left_result.type != right_result.type)
    {
        printf("ERROR: Can't operate when data-types are not matching!!!\n");
        exit(1);
    }

    Variable var;
    var.type = left_result.type;

    if (node.e.BinOp.op == BINOP_PLUS)
    {
        if(left_result.type == Var_String)
        {
            var.value.str = String_Concat(left_result.value.str, right_result.value.str);
            return var;
        }
        if(left_result.type == Var_Number)
        {
            var.value.num = left_result.value.num + right_result.value.num;
            return var;
        }
    }
    else if (node.e.BinOp.op == BINOP_MINUS)
    {
        if(left_result.type == Var_String)
        {
            printf("ERROR: BINOP_MINUS not implemented for Strings!!!\n");
            exit(1);
        }
        if(left_result.type == Var_Number)
        {
            var.value.num = left_result.value.num - right_result.value.num;
            return var;
        }
    }
    else if (node.e.BinOp.op == BINOP_MULTIPLICATION)
    {
        if(left_result.type == Var_String)
        {
            printf("ERROR: BINOP_MULTIPLICATION not implemented for Strings!!!\n");
            exit(1);
        }
        if(left_result.type == Var_Number)
        {
            var.value.num = left_result.value.num * right_result.value.num;
            return var;
        }
    }
    else if (node.e.BinOp.op == BINOP_DIVISION)
    {
        if(left_result.type == Var_String)
        {
            printf("ERROR: BINOP_DIVISION not implemented for Strings!!!\n");
            exit(1);
        }
        if(left_result.type == Var_Number)
        {
            var.value.num = left_result.value.num / right_result.value.num;
            return var;
        }
    }

    return var;
}

Variable Interpret(Expr node)
{
    Variable var;
    if (node.type == BinOp)
    {
        var = Interpret_BinOp(node);
    }
    else if (node.type == Literal)
    {
        var = Interpret_Literal(node);
    }
    else if(node.type == Parenthesized)
    {
        var = Interpret(*node.e.Parenthesized.expr);
    }

    return var;
}

Variable_List Interpret_Program(Expr program)
{
    if(program.type != Program)
    {
        printf("ERROR: Can't parse non-Program AST Node!!!\n");
        exit(1);
    }
    
    Variable_List variables;
    Variable_List_Init(&variables);

    local_variables = &variables;

	for(size_t i = 0; i < program.e.Program.program.size; ++i)
	{
		Expr expr = program.e.Program.program.content[i];

		switch(expr.type)
		{
            case Conditional:
            {
                Interpret_Conditional(expr);
                local_variables = &variables; // we need to reset the local scope after going to another scope
            }
            break;
			case Set:
			{
                Interpret_Set(expr);
			}
			break;
			case Function:
			{
                Interpret_Function(expr);
			}
			break;
		}
	}
    return variables;
}