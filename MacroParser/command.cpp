#include "command.hpp"

void printHelp()
{
    cout << "Here are the available commands:" << endl;
    cout << "- help : print this menu" << endl;
    cout << "- importfile [file] : import macros from a file to the program" << endl;
    cout << "- importfolder [folder] : import all macros from all header files from a folder" << endl;
    cout << "- stat : print the number of macros imported" << endl;
    cout << "- look [macro] : calculate the value of a macro given in input" << endl;
    cout << "- define [macro] [value] : add/replace a specific macro" << endl;
    cout << "- search [name] : print all macros containing the string name" << endl;
    cout << "- listok/listre/listin : print the list of okay/redefined/incorrect macros" << endl;
    cout << "- clean : delete all macros registered" << endl;
    cout << "- exit : quit the program" << endl;
}

void dealWithUser(MacroContainer& macroContainer)
{
    bool running=true;

    while(running)
    {
        cout << "\n > ";

        string userInput;
        getline(cin, userInput);

        running = runCommand(userInput, macroContainer);
    }
}


bool runCommand(const string& str, MacroContainer& macroContainer)
{
    if(str.empty())
        {}
    else if(str == "clean"){
        macroContainer.defines.clear();
        macroContainer.redefinedMacros.clear();
        macroContainer.incorrectMacros.clear();
    }
    else if(str == "help")
        printHelp();
    else if(str == "stat"){
        cout << macroContainer.defines.size() << " macros were loaded." << endl;
        cout << "|-> " << macroContainer.redefinedMacros.size() << " macros have been redefined." << endl;
        cout << "|-> " << macroContainer.incorrectMacros.size() << " macros seem incorrect." << endl;
    }
    else if(str == "list"){
        cout << "The command 'list' does not exist." << endl;
        cout << "Did you mean listok ? listre ? listin ?" << endl;
    }
    else if(str == "listok"){
        for(const auto& p: macroContainer.defines){
            if(std::find(macroContainer.redefinedMacros.begin(), macroContainer.redefinedMacros.end(), p.first) == macroContainer.redefinedMacros.end())
                cout << p.first << " => " << p.second << endl;
        }
    }
    else if(str == "listre"){
        for(const string& str: macroContainer.redefinedMacros){
            cout << " - " << str << endl;
        }
    }
    else if(str == "listin"){
        for(const string& str: macroContainer.incorrectMacros){
            cout << " - " << str << endl;
        }
    }
    else if(str.substr(0, 11) == "importfile "){
        readFile(str.substr(11), macroContainer);
        runCommand("stat", macroContainer);
    }

    else if(str.substr(0, 13) == "importfolder "){
        readDirectory(str.substr(13), macroContainer);
        runCommand("stat", macroContainer);
    }

    else if(str.substr(0, 5) == "look ")
    {
        if(macroContainer.defines.empty())
            cout << "No macros were imported yet." << endl;
        else
        {
            string userInput = str.substr(5);

            bool found = false;

            for(pair<string,string>& p : macroContainer.defines)
            {
                if(p.first == userInput)
                {
                    cout << "first definition: " << p.second << endl;
                    string output = p.second;
                    bool displayPbInfo = false;
                    bool okay = calculateExpression(output, macroContainer.defines, macroContainer.redefinedMacros, macroContainer.incorrectMacros, displayPbInfo);
                    cout << "output: " << output << endl;
                    if(!okay)
                        cout << "/!\\ The expression can't be calculated. /!\\\n";
                    if(displayPbInfo){
                        cout << "\nIt seems that you are using macros that seem incorrect or have been redefined." << endl;
                        cout << "You can look for the values available for that specific macro by typing 'search [macro]'." << endl;
                        cout << "And finally you address the issue by correcting its value by typing 'define [macro] [value]." << endl;
                    }
                    found=true;
                    break;
                }
            }

            if(!found)
                cout << "No macro was found with this name." << endl;
        }

    }
    else if(str.substr(0,7) == "define " && str.size()>=10){
        std::stringstream ss(str.substr(6));
        string str1;
        ss >> str1;
        string str2;
        if(ss.tellg() != (-1))
            str2=str.substr(6+ss.tellg());

        bool found = false;
        for(auto& p: macroContainer.defines){
            if(p.first == str1){
                p.second = str2;
                found = true;
            }
        }
        if(!found)
            macroContainer.defines.emplace_back(str1, str2);
        std::remove(macroContainer.incorrectMacros.begin(), macroContainer.incorrectMacros.end(), str1);
        if(!doesExprLookOk(str2))
            cout << "/!\\ Warning: the expression of the macro doesn't look correct. /!\\" << endl;
    }
    else if(str.substr(0,7) == "search " && str.size()>8){
        for(const auto& p: macroContainer.defines){
            if(p.first.find(str.substr(7)) != string::npos)
                cout << " - " << p.first << " => " << p.second << endl;
        }
    }
    else if(str == "exit")
        return false;
    else {
        cout << "This command is unknown." << endl;
    }

    return true;
}

