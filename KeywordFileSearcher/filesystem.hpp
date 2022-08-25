#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>


using namespace std;

typedef std::vector<std::string> stringvec;

// Fucntions available

BOOL DirectoryExists(LPCTSTR szPath);

bool readFile(const string& path, const stringvec& words);

bool readDirectory(string dir, vector< pair<string, string> >& defineList, stringvec& redefinedMacros);

void explore_directory(std::string directory_name, stringvec& fileCollection);

bool replaceKeywordFile(const string& path, const string& initialKeyword, const string& finalKeyword);

#endif // FILESYSTEM_HPP

