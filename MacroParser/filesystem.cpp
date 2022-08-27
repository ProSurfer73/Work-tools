#include "filesystem.hpp"

bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

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


BOOL DirectoryExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#ifdef LOWLEVEL_FILE_IMPORT

bool readFile(const string& pathToFile, MacroContainer& macroContainer)
{
    ifstream file(pathToFile);

    if(file.fail())
        return false;

    #ifdef DEBUG_LOG_FILE_IMPORT
        cout << "Opened " << pathToFile << endl;
    #endif

    char defineStr[] = "#define ";
    int posDefineStr = 0;

    char characterRead;

    int posLineComment=0;

    while(file.get(characterRead))
    {
        /// avoid to laod defines that are commented
        if(characterRead == '/')
        {
            posLineComment++;

            if(posLineComment == 2)
            {
                // we skip the all line
                while(file.get(characterRead) && characterRead != '\n');

                // we reset the slash counter
                posLineComment=0;
            }
        }



        else if(characterRead == defineStr[posDefineStr])
        {
            posDefineStr++;

            //
            if(posDefineStr == 7)
            {
                posDefineStr = 0;

                string str1;

                /// We get the identifier

                // We skip spaces first
                while(file.get(characterRead) && characterRead != '\n'){
                    if(characterRead != ' '){
                        str1 += characterRead;
                        break;
                    }
                }
                // Then we laod the identifier (the complete word)
                while(file.get(characterRead) && characterRead != '\n' && characterRead != ' '){
                        str1 += characterRead;
                }


                // skip spaces
                //while(file >> characterRead && characterRead == ' ')

                string str2;

                //int posComment=0;

                // We get the value
                while(file.get(characterRead) && characterRead != '\n')
                {
                    str2 += characterRead;
                }

                // Deal with comment that could appear on str2
                auto look = str2.find("//");
                if(look == string::npos) look = str2.find("/*");
                if(look != string::npos){
                    str2 = str2.substr(0,look);
                }

                #ifdef DEBUG_LOG_FILE_IMPORT
                cout << str1 << " => " << str2 << endl;
                #endif

                clearSpaces(str1);
                clearSpaces(str2);

                // Ignore single letter macro and empty ones
                if(str2.empty() || str1.size()==1)
                    continue;


                // Do not import macros containing errors
                if(str2.find("#define") == string::npos)
                {
                    // If it is a multiple line macro
                    while(str2.back() == '\\')
                    {
                        str2.pop_back();
                        string inpLine;

                        // we get the next line
                        getline(file, inpLine);
                        clearSpaces(inpLine);
                        str2 += inpLine;

                        auto look = str2.find("//");
                        if(look == string::npos){
                            look = str2.find("/*");
                            if(look != string::npos){
                                char k='a';
                                while(file.get(characterRead)){
                                    if(k=='*' && characterRead=='/')
                                        break;
                                    k=characterRead;
                                }
                            }
                        }
                        if(look != string::npos){
                            str2 = str2.substr(0,look);
                        }
                    }


                    for(const pair<string,string>& p: macroContainer.defines)
                    {
                        if(p.first == str1)
                        {
                            if(p.second != str2)
                                macroContainer.redefinedMacros.emplace_back(str1);
                            while(file.get(characterRead) && characterRead != '\n');
                            continue;
                        }
                    }


                    // Add the couple to the define list
                    // If the couple does not already exist
                    //if(std::find(macroContainer.defines.begin(), macroContainer.defines.end(), std::make_pair(str1, str2)) == macroContainer.defines.end())
                    macroContainer.defines.emplace_back( str1, str2 );

                    if(!doesExprLookOk(str2)){
                        macroContainer.incorrectMacros.emplace_back(str1);
                    }

                }





            }

        }
        else
            posDefineStr = 0;
    }



    return true;
}

#else


bool readFile(const string& pathToFile, MacroContainer& mc)
{
    ifstream file(pathToFile);

    if(!file.is_open())
        return false;

    #ifdef DEBUG_LOG_FILE_IMPORT
        cout << "Opened " << pathToFile << endl;
    #endif

    string inputLine;

    while(getline(file, inputLine))
    {
        // If the line starts with the prefix "#define"
        if (inputLine.size()>10 && inputLine[0]=='#' && inputLine.substr(0,8) == "#define ")
        {
            // Delete the define part from the line
            inputLine = inputLine.substr(7);

            std::stringstream ss(inputLine);

            string str1;
            ss >> str1;

            inputLine += "   ";
            //string str2 = inputLine.substr(inputLine.find_last_of(str1)+2);
            //string str2 = inputLine.substr(inputLine.find_last_of(str1)+1);
            string str2 = std::string(&inputLine[ss.tellg()]);

            // Deal with comment that could appear on str2
            auto look = str2.find("//");
            if(look == string::npos) look = str2.find("/*");
            if(look != string::npos){
                str2 = str2.substr(0,look);
            }

            #ifdef DEBUG_LOG_FILE_IMPORT
                cout << str1 << " => " << str2 << endl;
            #endif

            clearSpaces(str1);
            clearSpaces(str2);

            for(pair<string,string>& p: mc.defines)
            {
                if(p.first == str1 && p.second != str2){
                    mc.redefinedMacros.emplace_back(str1);
                    continue;
                }
            }



            // Add the couple to the define list
            mc.defines.emplace_back( str1, str2 );

        }


    }

    #ifdef DEBUG_LOG_FILE_IMPORT
                cout << endl;
    #endif


    return true;
}

#endif


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
            fileCollection.emplace_back(directory_name+'\\'+sv[i]);

        // Else it's a folder, and you have to reexecute the function recursilvely
        else if(sv[i]!="." && sv[i]!=".."){
            explore_directory
            (directory_name+'\\'+sv[i], fileCollection);
        }
    }
}

#ifdef ENABLE_FILE_LOADING_BAR


static void printNbFilesLoaded(std::mutex& mymutex, bool& ended, unsigned& nbFiles, const unsigned maxNbFiles)
{
    // First initial delay before starting to diplay loading status
    const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    int delayFirstSleep = 4000;

    while(delayFirstSleep > 0)
    {
        Sleep(120);
        delayFirstSleep -= 120;

        mymutex.lock();
        bool myend = ended;
        mymutex.unlock();

        if(myend)
            break;
    }

    // We display the loading status, yeah

    int notEverytime=0;

    while(true)
    {
        ++notEverytime;

        if(notEverytime == 10)
        {
            #if (defined DEBUG_LOG_FILE_IMPORT && !(defined DEBUG_LOG_FILE_IMPORT)) && defined ENABLE_MUTEX_LOADINGBAR
                mymutex.lock();
                unsigned currentNbFiles = nbFiles;
                mymutex.unlock();
            #else
                unsigned currentNbFiles = nbFiles;
            #endif

            #if defined DEBUG_LOG_FILE_IMPORT && defined ENABLE_MUTEX_LOADINGBAR
            mymutex.lock();
            #endif // DEBUG_LOG_FILE_IMPORT

            cout << '[' << currentNbFiles*100/static_cast<float>(maxNbFiles) << "%] " << currentNbFiles << " files over " << maxNbFiles << " are loaded. ~"
            << maxNbFiles*(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()-start)/(currentNbFiles+1)/60000 << "min left\n" ;

            #if defined DEBUG_LOG_FILE_IMPORT && defined ENABLE_MUTEX_LOADINGBAR
            mymutex.unlock();
            #endif // DEBUG_LOG_FILE_IMPORT

            notEverytime = 0;
        }


        Sleep(400);

        mymutex.lock();
        bool myend = ended;
        mymutex.unlock();

        if(myend)
            break;
    }
}

#endif

bool readDirectory(string dir, MacroContainer& macroContainer)
{
    stringvec fileCollection;

    explore_directory(dir, fileCollection);

    if(fileCollection.empty())
        return false;

    #ifdef ENABLE_FILE_LOADING_BAR
    std::cout << std::setprecision(3);
    bool ended = false;
    unsigned localNbFile = 0;
    unsigned nbFiles = 0;
    std::mutex mymutex;
    std::thread tr = std::thread(printNbFilesLoaded, std::ref(mymutex), std::ref(ended), std::ref(nbFiles), fileCollection.size());
    #endif
    #ifdef DISPLAY_FOLDER_IMPORT_TIME
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    #endif // DISPLAY_FOLDER_IMPORT_TIME

    for(const string& str: fileCollection)
    {
        if(hasEnding(str, ".h") || hasEnding(str, ".c") || hasEnding(str, ".cpp") || hasEnding(str, ".hpp"))
        {
            if(!readFile(str, macroContainer)){
                std::cerr << "Couldn't read/open file : " << str << endl;
            }


        }

        #ifdef ENABLE_FILE_LOADING_BAR
        localNbFile++;
        #endif

        #ifdef ENABLE_FILE_LOADING_BAR
            #ifdef ENABLE_MUTEX_LOADINGBAR
            mymutex.lock();
            #endif
            nbFiles = localNbFile;
            #ifdef ENABLE_MUTEX_LOADINGBAR
            mymutex.unlock();
            #endif
        #endif
    }

    #ifdef ENABLE_FILE_LOADING_BAR
    mymutex.lock();
    ended = true;
    mymutex.unlock();
    tr.join();
    #endif
    #ifdef DISPLAY_FOLDER_IMPORT_TIME
    auto importTime = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()-now);
    cout << "Import time: " << importTime << " ms.\n";
    #endif // DISPLAY_FOLDER_IMPORT_TIME


    return true;
}










