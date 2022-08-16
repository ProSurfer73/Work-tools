#include <iostream>
#include <fstream>

#include "hexa.hpp"
#include "stringeval.hpp"
#include "filesystem.hpp"
#include "command.hpp"

using namespace std;



/** \brief ask the user for the header files to be loaded
 *
 * \param the list of registered macros
 *
 */
void askForInputFiles(vector< pair<string, string> >& defineList, stringvec& redefinedMacros)
{
    string userInput;


    do
    {
        cout << "Type path to file: ";
        getline(cin, userInput);

        if(!userInput.empty() && userInput[0]=='f' && !readDirectory(userInput.substr(1), defineList, redefinedMacros) ){
            std::cout << "!! Couldn't open the directory provided." << endl;
        }
        else if(!userInput.empty() && userInput[0]!='f' && !readFile(userInput, defineList, redefinedMacros)){
            std::cout << "!! Couldn't open the file provided." << endl;
        }

    }
    while(!userInput.empty());
}



int main()
{
    std::cout << "WELCOME TO MACRO PARSER." << endl;
    std::cout << "Type 'help' to see the available commands." << endl;


    vector< pair<string, string> > defines;
    vector<string> redefinedMacros;

    //readFile("C:\\region_defs.h", defines);
/*
    askForInputFiles(defines, redefinedMacros);


    std::cout << endl << defines.size() << " macros were loaded." << endl;
    cout << redefinedMacros.size() << " macros have been redefined." << endl;

    // for(string s: redefinedMacros) cout << s << "'\n";


    string userInput;
    do {
        std::cout << "\nNow please type a macro name to see its value:" << endl;
        //std::cout << "Type \"list\" to print the list of the macros registered." << endl;


        getline(cin, userInput);

        if(userInput == "list")
        {
            for(pair<string,string> p : defines)
                cout << '\'' << p.first << "\'    \'" << p.second << '\'' << endl;
        }
        else
        {
            for(pair<string,string> p : defines)
            {
                if(p.first == userInput)
                {
                    cout << "raw definition: " << p.second << endl;
                    string output = p.second;
                    bool okay = calculateExpression(output, defines, redefinedMacros);
                    cout << "output: " << output << endl;
                    if(!okay)
                        cout << "/!\\ The expression can't be calculated. /!\\\n";
                    break;
                }
            }
        }
    }
    while(!userInput.empty());



*/

    dealWithUser(defines, redefinedMacros);

    return 0;
}


