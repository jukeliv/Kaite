# Kaite (pronounced "Kite")
Kaite is a simple, open-source language for building projects and executing command-line commands.

# Why Kaite?
I dislike `batch`, which is why Kaite exists.

# Where is it supported?
It has been tested in Windows and Linux, both compiled with GCC.

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
The `when` keyword is similar to C's `#ifdef`. It internally checks if something is defined. Currently, it only supports checking if you are on a Windows or Linux PC. It works by passing a string, like this: `when "WINDOWS"`. A code block works just like in C, here is an example:
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

# Building Kaite
To build Kaite, you can execute the "build.bat" script (yes, I know, it's a bit cringe). However, if you prefer, you can use any C compiler of your choice by running the following command in your terminal: `gcc main.c -o kaite.exe -O2`. 

I hope you enjoy using Kaite! Have a great day :)