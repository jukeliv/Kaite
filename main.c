#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("Usage - kaite <INPUT> <DEFINITION>...<DEFINITION>\n");
		printf("ERROR: No input file passed\n");
		return 1;
	}

    Token_List tokens;
	Token_List_Init(&tokens);
	Tokenize_File(&tokens, argv[1]);

	Expr program = Expr_Program();

	Parse_Program(&program, &tokens);

	Token_List_Free(&tokens);

	Initialize_Global(); // initialize global scope
	Interpret_Program(program);

	return 0;
}
