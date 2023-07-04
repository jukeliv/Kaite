#include "..\include\lexer.h"
#include "..\include\parser.h"
#include "..\include\interpreter.h"
#include <stdio.h>
#include <stdlib.h>

void Usage()
{
	printf("Kaite Usage\n\t.\\kaite <INPUT_FILE>\n\n");
	printf("ERROR: No input file passed\n");
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		// This ain't V-lang, i am not going to add that you can use features that are not implemented lol
		Usage();
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
