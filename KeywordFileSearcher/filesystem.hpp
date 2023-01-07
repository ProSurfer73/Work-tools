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

bool directoryExists(const char* basepath);

bool readFile(const string& path, stringvec& words, bool hasInsensitive);

bool readDirectory(string dir, vector< pair<string, string> >& defineList, stringvec& redefinedMacros);

void explore_directory(std::string dirname, std::vector<std::string>& files);

bool replaceKeywordFile(const string& path, const string& initialKeyword, const string& finalKeyword);

bool readFileWithLine(std::string& path, stringvec& results, const stringvec& words, std::ostream& output, bool containsInsensitive);


// When a file could not be opened, an exception of this type is launched
class FileException
{
    // For optimization reasons, we don't reference the name of the file inside the exception
    const char* what();
};

#endif // FILESYSTEM_HPP

