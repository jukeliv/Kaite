#include "..\include\interpreter.h"

Variable_List global_variables;

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

Variable* Get_Variable(Variable_List* variables, const char* variable)
{
    if(!Variable_List_Exist(variables, variable) && !Variable_List_Exist(&global_variables, variable))
    {
        return NULL;
    }
    Variable* var;
    if(Variable_List_Exist(&global_variables, variable))
        var = &global_variables.content[Variable_List_Find(&global_variables, variable)];
    else
        var = &variables->content[Variable_List_Find(variables, variable)];
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

        printf("ERROR: Unknown check in conditional \"when\"");
        return;
    }
}

void Interpret_Set(Variable_List* variables, Expr expr)
{
    if(!Get_Variable(variables, expr.e.Set.ID.content))
    {
        Variable var;
        var.id = strdup(expr.e.Set.ID.content);
        Variable_List_Push(variables, var);
    }
    Variable* var = Get_Variable(variables, expr.e.Set.ID.content);

    Expr literal = *(expr.e.Set.literal);

    switch(literal.e.Literal.type)
    {
        case LITERAL_Id:
        {
            Variable* var2 = Get_Variable(variables, literal.e.Literal.value.content);
            if(!var2)
            {
                printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                return;
            }
            var->str = var2->str;
        }
        break;
        case LITERAL_String:
        {
            var->str = literal.e.Literal.value;
        }
        break;
    }
}

void Interpret_Function(Variable_List* variables, Expr expr)
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
            switch (literal.e.Literal.type) {
                case LITERAL_String:
                    str_size += literal.e.Literal.value.size;
                    break;

                case LITERAL_Id: {
                    Variable* var = Get_Variable(variables, literal.e.Literal.value.content);
                    if (!var) {
                        printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                        exit(1);
                    }
                    str_size += Get_Variable(variables, literal.e.Literal.value.content)->str.size;
                    break;
                }
            }
            str_size++; // Account for space separator
        }

        char* buffer = malloc((str_size + 1) * sizeof(char)); // Allocate space for null terminator
        if (!buffer) {
            printf("ERROR: Couldn't allocate memory for Command-Buffer\n");
            return;
        }

        size_t buffer_index = 0;
        for (size_t i = 0; i < group.e.Group.literals.size; ++i) {
            Expr literal = group.e.Group.literals.content[i];
            switch (literal.e.Literal.type) {
                case LITERAL_String:
                {
                    if (i != 0) {
                        buffer[buffer_index++] = ' ';
                    }
                    memcpy(buffer + buffer_index, literal.e.Literal.value.content, literal.e.Literal.value.size);
                    buffer_index += literal.e.Literal.value.size;
                }
                break;
                case LITERAL_Id:
                {
                    Variable* var = Get_Variable(variables, literal.e.Literal.value.content);
                    if (!var) {
                        printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                        exit(1);
                    }
                    if (i != 0) {
                        buffer[buffer_index++] = ' ';
                    }
                    memcpy(buffer + buffer_index, var->str.content, var->str.size);
                    buffer_index += var->str.size;
                }
                break;
            }
        }

        buffer[buffer_index] = '\0'; // Add null terminator
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
            switch(literal.e.Literal.type)
            {
                case LITERAL_String:
                {
                    printf("%s\n", literal.e.Literal.value.content);
                }
                break;
                case LITERAL_Id:
                {
                    Variable* var = Get_Variable(variables, literal.e.Literal.value.content);
                    if(!var)
                    {
                        printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                        return;
                    }
                    printf("%s\n", var->str.content);
                }
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
                return;
            }
            break;
            case LITERAL_Id:
            {
                Variable* var = Get_Variable(variables, literal.e.Literal.value.content);
                if(!var)
                {
                    printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                    return;
                }
                char buf[256] = {0};
                scanf("%256s", &buf);
                var->str = String_CStr(buf);
            }
            break;
        }
    }
    else if(!strcmp(functionName.content, "global"))
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
                printf("ERROR: Can't use non-Variable data-type in function \"global\"");
                return;
            }
            break;
            case LITERAL_Id:
            {
                Variable* var = Get_Variable(variables, literal.e.Literal.value.content);
                if(!var)
                {
                    printf("ERROR: Couldn't find variable \"%s\"\n", literal.e.Literal.value.content);
                    return;
                }
                Variable_List_Push(&global_variables, *(var));
                size_t index = Variable_List_Find(Get_Variable_List(variables, literal.e.Literal.value.content), literal.e.Literal.value.content);
                Variable_List_Remove(variables, index);
            }
            break;
        }
    }
    else
    {
        printf("TODO: Implement user-defined functions!!!\n");
        return;
    }
}

void Interpret_Program(Expr program)
{
    if(program.type != Program)
    {
        printf("ERROR: Can't parse non-Program AST Node!!!\n");
        return;
    }
    
    Variable_List variables;
    Variable_List_Init(&variables);

	for(size_t i = 0; i < program.e.Program.program.size; ++i)
	{
		Expr expr = program.e.Program.program.content[i];

		switch(expr.type)
		{
            case Conditional:
            {
                Interpret_Conditional(expr);
            }
            break;
			case Set:
			{
                Interpret_Set(&variables, expr);
			}
			break;
			case Function:
			{
                Interpret_Function(&variables, expr);
			}
			break;
		}
	}
    Variable_List_Free(&variables);
}