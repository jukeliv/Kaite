@echo off
REM this is ugly, but we need to build Kaite in some way while it is runing :(
gcc src\utils.c src\lexer.c src\parser.c src\interpreter.c src\main.c -o bin\kaite.exe -O2