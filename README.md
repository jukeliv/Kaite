# Kaite (pronounced "Kite")
Kaite is a simple, open-source language for building projects and executing command-line commands.

# Why Kaite?
I dislike `batch`, which is why Kaite exists.

# Where is it supported?
It has been tested in Windows and Linux, both compiled with GCC.

# Usage
To use Kaite, simply run it in the terminal and input your .kai files. If desired, you can separate your code into files and pass the file names as arguments when calling "Kaite". Here is an example:
```
.\kaite file1.kai file2.kai file3.kai
```

# Syntax
Kaite has a really simple syntax, but it may seem a bit strange to newcomers. Here's how things work:

# Defining Variables
You define variables as you would in any C-like language. First, you specify the name, followed by an equals sign (=) and the value. The value can be either a string or an identifier that copies the value of an existing variable. Here is an example:

```
foo = "bar"
bar = "baz"

print(bar) ; prints out "baz"

bar = foo

print(bar) ; prints out "bar"
```

# When
The `when` keyword is similar to C's `#ifdef`. It internally checks if something is defined. Currently, it supports checking if you are on a Windows or Linux PC and if there is a variable defined in current scopes ( local and global ).
It works by passing a string, like this: `when "WINDOWS"`. A code block works just like in C, here is an example:
```
when "LINUX"
{
    print("Linux")
}
when "WINDOWS"
{
    print("Not Linux")
}
```

# Functions
Functions are not user-defined at the moment, and they are called just like in any other language by specifying the name and the arguments. Arguments don't need to be separated by commas. Here is an example of how functions work:
```
output = "hello_world" ; output file
flags = "-O2"

cmd("gcc main.c -o", output, flags)
```

# Internal Functions (from Kaite STD)
Kaite already provides some predefined functions for you. Here they are:

`cmd` -> Takes a list of parameters and executes them as a shell command.

`print` -> Takes a list of parameters and prints them to the screen, separating them by a newline.

`input` -> Takes a single variable as an argument and prompts the user to input a string, which is then stored in the specified variable.

`global` -> Inside a `when` block, you have a local scope. However, if you want to make a variable accessible globally, you can use the `global` keyword followed by the variable name as an argument. In the global scope, the variable can be accessed from anywhere in the program.

`remove`: The `remove` function takes one argument, which is the variable thats going to be removed. It searches for the variable in both the global and local scopes. If the variable is found, it is removed from the scope.

# Building Kaite
To build Kaite, you can execute the "build.bat" script (yes, I know, it's a bit cringe). However, if you prefer, you can use any C compiler of your choice by running the following command in your terminal: `gcc src\lexer.c src\parser.c src\interpreter.c src\main.c -o kaite.exe -O2`. 

# Good Practices
Here are some good practices to follow when coding in Kaite.
# While working with multiple files
If you are working with multiple Kaite files in a script, consider hiding them within a single Kaite script that executes them. For example, you can combine your three files into one script with the following code:
```
files = "file1.kai file2.kai file3.kai"
cmd("kaite" files)
```

# Minimize Global Scope Variables
When defining variables, avoid placing all of them in the global scope. Instead, try to minimize the number of variables in the global scope. After using a variable, if you no longer need it, remove it using the "remove" command. This command searches for the variable in both the global and local scopes and deletes it.

# Define Variables at the Start
Variables should be defined at the beginning of the file or within

I hope you enjoy using Kaite! Have a great day :)
