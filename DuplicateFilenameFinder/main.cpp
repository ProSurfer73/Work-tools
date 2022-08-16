#include <iostream>
#include <vector>
#include <windows.h>
#include <fstream>
#include <algorithm>
#include <cassert>

typedef std::vector<std::string> stringvec;

using namespace std;


BOOL DirectoryExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool compareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

  if (f1.fail() || f2.fail()) {
    return false; //file problem
  }

  if (f1.tellg() != f2.tellg()) {
    return false; //size mismatch
  }

  //seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}


/*
*/
void read_directory(const std::string& name, stringvec& v)
{
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.emplace_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}

void explore_directory(std::string directory_name, stringvec& fileCollection)
{
   stringvec sv;

    // We look for all the files and folders that are in that directory
    read_directory(directory_name, sv);


    // We explore all teh different inputs
    for(size_t i=0; i < sv.size(); ++i){

        // IF there is a point then, it's a file
        // Also it's important to note that filenames "." and ".." aren't files
        // oldimplemntation: if(sv[i].find('.') != std::string::npos && sv[i]!="." && sv[i]!="..")
        if(!DirectoryExists((directory_name+'\\'+sv[i]).c_str()) && sv[i]!="." && sv[i]!="..")
            fileCollection.push_back(directory_name+'\\'+sv[i]);

        // Else it's a folder, and you have to reexecute the function recursilvely
        else if(sv[i]!="." && sv[i]!=".."){
            explore_directory
            (directory_name+'\\'+sv[i], fileCollection);
        }
    }
}

const char* extractFilename(const string& path)
{
    return &(path[path.find_last_of('\\')+1]);

}

void supprimerDoublons(stringvec& sv)
{

    // Sorting the array
    std::sort(sv.begin(), sv.end());
    // Now v becomes 1 1 2 2 3 3 3 3 7 7 8 10

    // Using std::unique
    auto ip = std::unique(sv.begin(), sv.end());
    // Now v becomes {1 2 3 7 8 10 * * * * * *}
    // * means undefined

    // Resizing the vector so as to remove the undefined terms
    sv.resize(std::distance(sv.begin(), ip));
}


long GetFileSize(const TCHAR *fileName)
{
    BOOL                        fOk;
    WIN32_FILE_ATTRIBUTE_DATA   fileInfo;

    if (NULL == fileName)
        return -1;

    fOk = GetFileAttributesEx(fileName, GetFileExInfoStandard, (void*)&fileInfo);
    if (!fOk)
        exit(-1);
    assert(0 == fileInfo.nFileSizeHigh);
    return (long)fileInfo.nFileSizeLow;
}


bool compareFilesContent(const string& file1, const string& file2)
{
    // First implementation based on the size of the file
    //return GetFileSize(file1.c_str()) == GetFileSize(file2.c_str());

    // Second implementation based on the content of the file (recommended)
    return compareFiles(file1, file2);
}



int main()
{
    std::cout << "WELCOME TO DUPLICATE FILENAMES SEARCHER !\n";
    std::cout << "The first path entered is the reference folder, only files that are present in this folder AND another folder will be printed.\n\n";

    string entry;
    stringvec paths;


    do
    {
        if(entry.size() == 0)
            std::cout << "Please enter the path of the reference folder:\n";
        else
            std::cout << "Please enter the path of the compared folder (or nothing to exit):\n";
        getline(std::cin, entry);

        if(!entry.empty())
        {
            if(!DirectoryExists(entry.c_str()))
                cout << "/!\\ It seems like this directory doesn't exists /!\\" << endl;

            paths.emplace_back(entry);
        }

    }
    while(!entry.empty());


    if(paths.size() < 2){
        cout << "You must enter at least two different paths.\n";
        return 0;
    }

    cout << endl;



    while(entry != "no" && entry != "yes")
    {
        cout << "Only look for files with different content (yes or no) ? ";
        getline(cin, entry);
    }

    cout << endl << endl;

    bool keepOnlyFilesWithDifferentContent = (entry == "yes");







    vector<stringvec> fileCollection;

    // We list all the files that are in those directories
    for(auto str: paths){
        stringvec vec;
        explore_directory(str, vec);
        fileCollection.emplace_back(vec);
    }

    vector<stringvec> filenameCollection;

    // We extract the filenames for each folder
    for(unsigned i=0; i<paths.size(); ++i)
    {
        //cout << "Inside the folder: " << paths[i] << endl;

        filenameCollection.emplace_back();

        for(auto str: fileCollection[i]){
            filenameCollection[i].emplace_back(extractFilename(str));
        }

        //cout << endl << endl;
    }


    /*
    // DEBUGGING: Print the filenames obtained

    for(unsigned i=0; i<paths.size(); ++i){

        cout << "Inside the folder: " << paths[i] << endl;

        for(auto str : filenameCollection[i])
            cout << str << endl;
    }
    */



    // Compare the filenames

    stringvec matchingFilenames;

    for(unsigned i=1; i<paths.size(); ++i)
    {
        for(string str: filenameCollection[0])
        {
            stringvec& v = filenameCollection[i];

            if(std::find(v.begin(), v.end(), str) != v.end())
            {
                // The user asks that we only keep files with the different content
                if(keepOnlyFilesWithDifferentContent)
                {
                    bool okay = true;

                    for(unsigned k=1; k<paths.size(); ++k)
                    {
                        for(string str1: fileCollection[0])
                        {
                            if(extractFilename(str1) == str)
                            {

                            for(string str2: fileCollection[k])
                            {
                                if(extractFilename(str2) == str)
                                {
                                    if(compareFilesContent(str1, str2)){
                                        okay = false;
                                        goto end;
                                    }

                                }
                            }

                            }
                        }
                    }

                    end:

                    if(okay)
                        matchingFilenames.emplace_back(str);
                }
                else
                {
                    matchingFilenames.emplace_back(str);
                }

            }
        }
    }

    supprimerDoublons(matchingFilenames);





    // Print matching filenames

    cout << matchingFilenames.size() << " results found.\n";

    for(auto str: matchingFilenames)
        cout << str << endl;


    //

    cout << "\n\n";

    do {

        cout << "\nNow please enter the filename to see its location in the folders (or nothing to exit): " << endl;
        getline(cin, entry);

        if(!entry.empty())
        {
            stringvec results;

            for(stringvec& vec: fileCollection)
            {
                for(string& str: vec)
                {
                    if(extractFilename(str) == entry){
                        results.emplace_back(str);
                    }
                }

            }

            if(results.empty())
                cout << "The file you are looking for doesn't seem to exist.\n";
            else {
                cout << results.size() << " common files found.\n";

                for(string str: results){
                    cout << str << endl;
                }
            }
        }

    }
    while(!entry.empty());








    return 0;
}


