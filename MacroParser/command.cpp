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
    cout << "- listre : print the list of the macros that have been redefined" << endl;
    cout << "- list : print the list of non-redefined macros" << endl;
    cout << "- clean : delete all macros registered" << endl;
    cout << "- exit : quit the program" << endl;
}

void dealWithUser(vector< pair<string, string> >& defines, vector<string>& redefinedMacros)
{
    bool running=true;

    while(running)
    {
        cout << "\n > ";

        string userInput;
        getline(cin, userInput);

        running = runCommand(userInput, defines, redefinedMacros);
    }
}


bool runCommand(const string& str, vector< pair<string, string> >& defines, vector<string>& redefinedMacros)
{
    if(str.empty())
        {}
    else if(str == "clean"){
        defines.clear();
        redefinedMacros.clear();
    }
    else if(str == "help")
        printHelp();
    else if(str == "stat"){
        cout << defines.size() << " macros were loaded." << endl;
        cout << "whose " << redefinedMacros.size() << " macros have been redefined." << endl;
    }
    else if(str == "list"){
        for(const auto& p: defines){
            if(std::find(redefinedMacros.begin(), redefinedMacros.end(), p.first) == redefinedMacros.end())
                cout << p.first << " => " << p.second << endl;
        }
    }
    else if(str == "listre"){
        for(const string& str: redefinedMacros){
            cout << " - " << str << endl;
        }
    }
    else if(str.substr(0, 11) == "importfile "){
        readFile(str.substr(11), defines, redefinedMacros);
        runCommand("stat", defines, redefinedMacros);
    }

    else if(str.substr(0, 13) == "importfolder "){
        readDirectory(str.substr(13), defines, redefinedMacros);
        runCommand("stat", defines, redefinedMacros);
    }

    else if(str.substr(0, 5) == "look ")
    {
        if(defines.empty())
            cout << "No macros were imported yet." << endl;
        else
        {
            string userInput = str.substr(5);

            bool found = false;

            for(pair<string,string> p : defines)
            {
                if(p.first == userInput)
                {
                    cout << "first definition: " << p.second << endl;
                    string output = p.second;
                    bool okay = calculateExpression(output, defines, redefinedMacros);
                    cout << "output: " << output << endl;
                    if(!okay)
                        cout << "/!\\ The expression can't be calculated. /!\\\n";
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
        for(auto& p: defines){
            if(p.first == str1){
                p.second = str2;
                found = true;
            }
        }
        if(!found)
            defines.emplace_back(str1, str2);
    }
    else if(str.substr(0,7) == "search " && str.size()>8){
        for(const auto& p: defines){
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

