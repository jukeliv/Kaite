#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lexer.h>
#include <parser.h>
#include <interpreter.h>

static void Usage()
{
	printf("Kaite Usage\n\t.\\kaite <INPUT_FILES>\n\n");
	printf("ERROR: No input file passed\n");
}

static const char* Concat_Files(int argc, char** argv)
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

static Variable_List Execute_Code(const char* source)
{
	Token_List tokens;
	Token_List_Init(&tokens);

	Tokenize_File(&tokens, source);

	Expr program = Expr_Program();

	Parse_Program(&program, &tokens);
	Token_List_Free(&tokens);

	Initialize_Global();
	Interpret_Program(program);
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		// This ain't V-lang, i am not going to add that you can use features that are not implemented lol
		Usage();
		return 1;
	}

	Execute_Code(Concat_Files(argc, argv));

	return 0;
}
