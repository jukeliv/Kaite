@echo off

REM Pack the icon resource
windres res\kaite.rc -O coff -o res\kaite.res

set src=src\utils.c src\lexer.c src\parser.c src\interpreter.c src\main.c
set icon=res\kaite.res
REM TODO - use -Wall and -Wextra
set flags=-O2

REM Check if a bin folder already exists
if exist bin\ (
  goto build
) else (
  mkdir bin
)

:build
gcc -o bin\kaite.exe %src% %icon% %flags%
