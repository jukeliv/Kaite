#include "../include/interpreter.h"

Variable_List global_variables;
Variable_List* local_variables;

void Variable_List_Init(Variable_List* list, Variable_List* last)
{
    list->heap = 1;
    list->size = 0;
    list->content = malloc(list->heap * sizeof(Variable));
    list->last = last;
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

bool Variable_List_Exist(Variable_List* list, String id)
{
    for(size_t i = 0; i < list->size; ++i)
    {
        if(String_Compare(id, list->content[i].id))
        {
            return true;
        }
    }
    return false;
}
size_t Variable_List_Find(Variable_List* list, String id)
{
    if(Variable_List_Exist(list, id) == false)
        return list->size;
    for(size_t i = 0; i < list->size; ++i)
    {
        if(String_Compare(list->content[i].id, id))
            return i;
    }
    return 0; // just so the compiler doesn't yell at me!!!
}

void Initialize_Global()
{
    Variable_List_Init(&global_variables, NULL);
    Variable_List* local_variables = malloc(sizeof(Variable_List));
    if (local_variables == NULL) {
        printf("ERROR: Failed to allocate memory for local_variables\n");
        exit(1);
    }
    Variable_List_Init(local_variables, NULL);
}

Variable* Get_Variable(String variable, Variable_List* scope)
{
    printf("Get_Variable\n");
    if(!Variable_List_Exist(scope, variable) && !Variable_List_Exist(&global_variables, variable))
    {
        if(scope->last)
        {
            return Get_Variable(variable, scope->last);
        }
        else
        {
            return NULL;
        }
    }

    if(Variable_List_Exist(&global_variables, variable))
        return &global_variables.content[Variable_List_Find(&global_variables, variable)];
    else
        return &scope->content[Variable_List_Find(scope, variable)];
}

Variable_List* Get_Variable_List(Variable_List* variables, String variable)
{
    if(!Variable_List_Exist(variables, variable) && !Variable_List_Exist(&global_variables, variable))
    {
        printf("ERROR: Couldn't find variable \"%s\"\n", variable.content);
        return NULL;
    }
    if(Variable_List_Exist(&global_variables, variable))
        return &global_variables;
    else
        return variables;
}

//TODO: Throw warning when variable is in global scope
void Globalize_Variable(String id)
{
    Variable* var = Get_Variable(id, local_variables);
    if(!var)
    {
        printf("ERROR: Couldn't find variable \"%s\"\n", id);
        exit(1);
    }
    Variable_List_Push(&global_variables, *(var));
    size_t index = Variable_List_Find(Get_Variable_List(local_variables, id), id);
    Variable_List_Remove(local_variables, index);
}

void Interpret_Conditional_If(Expr expr, Variable_List* last)
{
    *expr.e.Conditional.program = Expr_Program();
    
    printf("%d : %s\n", expr.e.Conditional.expr->e.Literal.type, expr.e.Conditional.expr->e.Literal.value.content);

    Variable v = Interpret(*expr.e.Conditional.expr);

    if (v.type == Var_Number && v.value.num != 0) {
        Interpret_Program(*expr.e.Conditional.program, last);
    }
    else if (v.type == Var_String)
    {
        printf("ERROR: Can't interpret if by passing an String\n");
        exit(1);
    }
}
void Interpret_Conditional_While(Expr expr, Variable_List* last)
{
    *expr.e.Conditional.program = Expr_Program();

    Variable v = Interpret(*expr.e.Conditional.expr);

    if (v.type == Var_String)
    {
        printf("ERROR: Can't interpret if by passing an String\n");
        exit(1);
    }

    while(v.value.num != 0)
    {
        Interpret_Program(*expr.e.Conditional.program, last);
        v = Interpret(*expr.e.Conditional.expr);
    }
}

void Interpret_Conditional_When(Expr expr, Variable_List* last)
{
    *expr.e.Conditional.program = Expr_Program();

    if(!strcmp(expr.e.Conditional.ID.content, "WINDOWS"))
    {
        #ifdef _WIN32
        Interpret_Program(*expr.e.Conditional.program, last);
        #endif
    }
    else if(!strcmp(expr.e.Conditional.ID.content, "LINUX"))
    {
        #ifdef __linux__
        Interpret_Program(*expr.e.Conditional.program, last);
        #endif
    }
    else
    {
        if(Get_Variable(expr.e.Conditional.ID, last))
        {
            Interpret_Program(*expr.e.Conditional.program, last);
        }
    }
}

void Interpret_Set(Expr expr)
{
    printf("Interpret_Set: 1\n");
    Variable v = Interpret(*expr.e.Set.literal);
    printf("Interpret_Set: 2\n");
    v.id = expr.e.Set.ID;

    printf("Interpret_Set: 3\n");
    Variable* existingVar = Get_Variable(v.id, local_variables);
    if (!existingVar)
    {
        Variable var = v;
        Variable_List_Push(local_variables, var);
        existingVar = Get_Variable(v.id, local_variables);
    }
    printf("Interpret_Set: 3\n");

    if (existingVar->type != v.type)
    {
        printf("ERROR: Can't set variable %s to a non-matching data-type\n", existingVar->id.content);
        exit(1);
    }
    *existingVar = v;
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
                Variable* var = Get_Variable(literal.e.Literal.value, local_variables);
                if(!var)
                {
                    printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                    exit(1);
                }
                switch(var->type)
                {
                    case Var_String:
                    {
                        char* buf = calloc(255, 1);
                        if(scanf("%s", buf) > 255)
                        {
                            printf("ERROR: To many characters, buffer overflow in \"input\"!\n");
                            exit(1);
                        }

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
                Globalize_Variable(literal.e.Literal.value);
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
                Variable* var = Get_Variable(literal.e.Literal.value, local_variables);
                if(!var)
                {
                    printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                    exit(1);
                }
                size_t index = Variable_List_Find(Get_Variable_List(local_variables, literal.e.Literal.value), literal.e.Literal.value);
                if(index >= local_variables->size) // if variable is not in local scope, search in global
                {
                    index = Variable_List_Find(Get_Variable_List(&global_variables, literal.e.Literal.value), literal.e.Literal.value);
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
    printf("Interpret_Literal: 1\n");
    Variable var = {0};
    printf("Interpret_Literal: 2\n");

    switch(node.e.Literal.type)
    {
        case LITERAL_String:
        {
            printf("Interpret_Literal: 3(String)\n");
            var.type = Var_String;
            var.value.str = node.e.Literal.value;
        }
        break;
        case LITERAL_Number:
        {
            printf("Interpret_Literal: 3(Number)\n");
            var.value.num = String_to_Float(node.e.Literal.value);
            printf("Interpret_Literal: 4(Number : %d)\n", var.value.num);
        }
        break;
        case LITERAL_Id:
        {
            printf("Interpret_Literal: 3(Variable)\n");
            Variable* var2 = Get_Variable(node.e.Literal.value, local_variables);
            if (!var2)
            {
                printf("ERROR: Couldn't find variable \"%s\"\n", node.e.Literal.value.content);
                exit(1);
            }
            var = *var2;
        }
        break;
    }

    printf("%d : %s\n", var.value.num, var.id.content);

    return var;
}


Variable Interpret_Arithmetic(ArithmeticOperators op, Variable left, Variable right)
{
    Variable var;
    var.type = left.type;

    if(op == ARITHMETIC_PLUS)
    {
        if(left.type == Var_String)
        {
            var.value.str = String_Concat(left.value.str, right.value.str);
            return var;
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num + right.value.num;
            return var;
        }
    }
    else if (op == ARITHMETIC_MINUS)
    {
        if(left.type == Var_String)
        {
            printf("ERROR: ARITHMETIC_MINUS not implemented for Strings!!!\n");
            exit(1);
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num - right.value.num;
            return var;
        }
    }
    else if (op == ARITHMETIC_MULTIPLICATION)
    {
        if(left.type == Var_String)
        {
            printf("ERROR: ARITHMETIC_MULTIPLICATION not implemented for Strings!!!\n");
            exit(1);
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num * right.value.num;
            return var;
        }
    }
    else if (op == ARITHMETIC_DIVISION)
    {
        if(left.type == Var_String)
        {
            printf("ERROR: ARITHMETIC_DIVISION not implemented for Strings!!!\n");
            exit(1);
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num / right.value.num;
            return var;
        }
    }
    return var;
}

Variable Interpret_Logic(LogicOperators op, Variable left, Variable right)
{
    Variable var;
    var.type = Var_Number;

    if(op == LOGIC_EQUALS)
    {
        if(left.type == Var_String)
        {
            var.value.num = String_Compare(left.value.str, right.value.str);
            return var;
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num == right.value.num;
            return var;
        }
    }
    else if (op == LOGIC_NOT_EQUALS)
    {
        if(left.type == Var_String)
        {
            var.value.num = !String_Compare(left.value.str, right.value.str);
            return var;
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num != right.value.num;
            return var;
        }
    }
    else if (op == LOGIC_GREATER_THAN)
    {
        if(left.type == Var_String)
        {
            printf("ERROR: LOGIC_GREATER_THAN not implemented for Strings!!!\n");
            exit(1);
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num > right.value.num;
            return var;
        }
    }
    else if (op == LOGIC_GREATER_EQUALS)
    {
        if(left.type == Var_String)
        {
            printf("ERROR: LOGIC_GREATER_EQUALS not implemented for Strings!!!\n");
            exit(1);
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num >= right.value.num;
            return var;
        }
    }
    else if (op == LOGIC_LESS_THAN)
    {
        if(left.type == Var_String)
        {
            printf("ERROR: LOGIC_GREATER_THAN not implemented for Strings!!!\n");
            exit(1);
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num < right.value.num;
            return var;
        }
    }
    else if (op == LOGIC_LESS_EQUALS)
    {
        if(left.type == Var_String)
        {
            printf("ERROR: LOGIC_GREATER_EQUALS not implemented for Strings!!!\n");
            exit(1);
        }
        if(left.type == Var_Number)
        {
            var.value.num = left.value.num <= right.value.num;
            return var;
        }
    }
    return var;
}

Variable Interpret_Binary(Expr node)
{
    Variable left_result = Interpret(*node.e.Binary.left);
    Variable right_result = Interpret(*node.e.Binary.right);

    if(left_result.type != right_result.type)
    {
        printf("ERROR: Can't operate when data-types are not matching!!!\n");
        exit(1);
    }

    switch(node.e.Binary.type)
    {
        case BINARY_Arithmetic:
        {
            return Interpret_Arithmetic(node.e.Binary.operator.arithmetic, left_result, right_result);
        }
        break;
        case BINARY_Logic:
        {
            return Interpret_Logic(node.e.Binary.operator.logic, left_result, right_result);
        }
        break;
    }
}

Variable Interpret(Expr node)
{
    if (node.type == Binary)
    {
        printf("Interpret(Binary)\n");
        return Interpret_Binary(node);
    }
    else if (node.type == Literal)
    {
        printf("Interpret(Literal)\n");
        return Interpret_Literal(node);
    }
    else if(node.type == Parenthesized)
    {
        printf("Interpret(Parenthesized)\n");
        return Interpret(*node.e.Parenthesized.expr);
    }
    else
    {
        printf("ERROR: Node type not suported!!!\n");
        exit(1);
    }
}

void Interpret_Program(Expr program, Variable_List* last)
{
    if(program.type != Program)
    {
        printf("ERROR: Can't parse non-Program AST Node!!!\n");
        exit(1);
    }
    
    Variable_List variables;
    Variable_List_Init(&variables, last);

    local_variables = &variables;

	for(size_t i = 0; i < program.e.Program.program.size; ++i)
	{
		Expr expr = program.e.Program.program.content[i];

		switch(expr.type)
		{
            case Conditional:
            {
                switch(expr.e.Conditional.type)
                {
                    case CONDITIONAL_If:
                    {
                        Interpret_Conditional_If(expr, local_variables);
                    }
                    break;
                    case CONDITIONAL_While:
                    {
                        Interpret_Conditional_While(expr, local_variables);
                    }
                    break;
                    case CONDITIONAL_When:
                        Interpret_Conditional_When(expr, local_variables);
                    break;
                }
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
    local_variables = last;
}