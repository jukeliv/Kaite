@echo off
REM this is ugly, but we need to build Kaite in some way while it is runing :(

windres res\kaite.rc -O coff -o res\kaite.res
gcc src\utils.c src\lexer.c src\parser.c src\interpreter.c src\main.c res\kaite.res -o bin\kaite.exe -O2 -ffast-math