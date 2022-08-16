#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <vector>
#include <sstream>

#include "filesystem.hpp"
#include "stringeval.hpp"

using namespace std;

void printHelp();

bool runCommand(const string& str, vector< pair<string, string> >& defines, vector<string>& redefinedMacros);

void dealWithUser(vector< pair<string, string> >& defines, vector<string>& redefinedMacros);

#endif // COMMAND_HPP
