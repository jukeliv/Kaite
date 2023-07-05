#!/bin/bash
# this is ugly, but we need to build Kaite in some way while it is running :(

gcc src/utils.c src/lexer.c src/parser.c src/interpreter.c src/main.c -o bin/kaite -O2
objcopy --add-resource bin/kaite=res/kaite.ico,icongroup,0