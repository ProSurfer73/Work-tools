# Work-tools
The tools I created and used during my apprenticeship at STMicroelectronics.
They may interest you if you are developping in C/C++, and if you are facing one of the situations these tools are trying to resolve.

Here is the list of the tools :

1. MACRO PARSER (initially called 'define expander')
![MAcroParser1](https://raw.githubusercontent.com/ProSurfer73/Work-tools/main/Screenshots/MacroParser1.png)
This is my most prominent project.
This project import all macros from a file or from a folder and has an option integrated to it to look for the value of the related macro.
It has a nice interface and a help menu.
It reads hexadecimal values, deal with parenthesis, and warns the user when this one uses a macro that has multiple definition.

2. DUPLICATE FILE FINDER (initially called 'folder compare')
Have you ever wondered how you could look for file with the same name, different content, and that are in 2 seperate folders at the same time ?
I have the solution for you in my project : folder compare.
First you have to type the reference folder, which represents the main folder from which the files origin.
Then, you enter the subsidiarie folders.
The file has to be at the same time in the reference folder and in at least one of the subsidiairie folder in order to be printed.
Once you have the results, you can type the filename of the file to see where you can find that specific file.

3. USELESS SEARCHER (initially called 'library searcher')
I meant to create a program that looks for useless files before pushing them.
The program looks at all the files (including their extension), the content of the project files, 
At the end, it offers two options to the user : write the list to a .gitignore file or delete the files.
Unfortunately the project is somewhat unifinished and is not meant to be used by an end-user for now.
But it gives a rough idea of a cool program that could be made.


