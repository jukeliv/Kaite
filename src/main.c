#include "..\include\lexer.h"
#include "..\include\parser.h"
#include "..\include\interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Usage()
{
	printf("Kaite Usage\n\t.\\kaite <INPUT_FILES>\n\n");
	printf("ERROR: No input file passed\n");
}

const char* concat_files(int argc, char** argv)
{
	size_t size = 0;
	for(int i = 1; i < argc; ++i)
	{
		size += strlen(read_file(argv[i]));
	}
	char* buf = malloc(size+1);
	memset(buf, 0, size);
	for(int i = 1; i < argc; ++i)
	{
		strcat(buf, read_file(argv[i]));
	}
	return buf;
}

Variable_List Execute_Code(const char* source)
{
	Token_List tokens;
	Token_List_Init(&tokens);

	Tokenize_File(&tokens, source);
	//printf("Tokenizer: Success\n");

	Expr program = Expr_Program();

	Parse_Program(&program, &tokens);
	Token_List_Free(&tokens);
	//printf("Parser: Success\n");

	Initialize_Global(); // initialize global scope
	Interpret_Program(program);
    
	//printf("Interpreter: Success\n");
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		// This ain't V-lang, i am not going to add that you can use features that are not implemented lol
		Usage();
		return 1;
	}

	Execute_Code(concat_files(argc, argv));

	return 0;
}
