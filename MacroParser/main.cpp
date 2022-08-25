#include <iostream>
#include <fstream>

#include "hexa.hpp"
#include "stringeval.hpp"
#include "command.hpp"

using namespace std;




int main()
{
    // Welcoming message
    std::cout << "WELCOME TO MACRO PARSER." << endl;
    std::cout << "Type 'help' to see the available commands." << endl;

    // Macro database (stored in volatile memory)
    MacroContainer macroContainer;

    // Let's start the command prompt
    dealWithUser(macroContainer);

    return 0;
}


