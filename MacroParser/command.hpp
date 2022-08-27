#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include "filesystem.hpp"
#include "stringeval.hpp"

using namespace std;

struct MacroContainer
{
    // Set large default presize
    // avoid reallocating small amount of memory each time
    MacroContainer()
    {
        //defines.reserve(10000);
        redefinedMacros.reserve(1000);
        incorrectMacros.reserve(1000);
    }

    // Attributes
    std::vector< std::pair< std::string, std::string> > defines;
    //std::map< std::string, std::string > defines;
    std::vector< std::string > redefinedMacros;
    std::vector< std::string > incorrectMacros;
};


bool runCommand(const string& str, MacroContainer& macroContainer);

void dealWithUser(MacroContainer& macroContainer);

#endif // COMMAND_HPP
