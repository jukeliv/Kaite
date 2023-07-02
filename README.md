# Kaite ( pronounced Kite )
 Kaite is a simple, open-source language to build projects and execute command-line commands

# Why Kaite?
 I hate `batch`, that is why it is here

# Syntax
 Kaite has a really simple syntax lol, but it is a bit strange to new people,
 so here is how things work

# Defining variables
 You define variables like in any C-like language, you first put the name, then equals, and
 the value, it can either be a string, or an id, that copies the value of an already
 existing variable. Here is an example:

 ```
 foo = "bar"
 bar = "baz 
 print(bar) ; prints out "baz"
 
 bar = foo
 
 print(bar) ; prints out "bar"
 ```

# When
 The `when` keyword is like C `#ifdef`, it checks internally if, something is defined.
 For now it only has a check if you are in a Windows or Linux pc. it works by passing an
 string, like this `when "WINDOWS"`. Also, to define where the code block ends, you need to
 add at the end of the code block, the `end` keyword.

# Functions
 Functions are not user-defined for now, and they are called as in any other language, by
 putting the name and the arguments, also arguments don't need to be separated by a comma,
 here is an example of how functions work:
 ```
 output = "hello_world" ; output file
 flags = "-O2"
 
 cmd("gcc main.c -o" output flags )
 ```
# Intern Functions ( from Kaite STD )
 Kaite has already some functions defined for you, here are them:
 
 `cmd` -> takes a list of parameters and executes them as a shell command.
 
 `print` -> takes a list of parameters and prints them to the screen ( separating them by a newline ).

 `input` -> takes a single variable for the input, then it lets the user input a string and saves it
 in the variable passed to the function.
 
 `global` -> inside a `when`, you have a local scope, also outside, so if you want to put your variable
 in a global scope, you call global and pass it as the argument, in the global scope it can be accessed
 anywhere in the program.