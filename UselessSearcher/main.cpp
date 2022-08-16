#include <iostream>
#include <vector>
#include <windows.h>
#include <fstream>
#include <algorithm>

using namespace std;

typedef std::vector<std::string> stringvec;

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
            v.push_back(data.cFileName);
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
        if(sv[i].find('.') != std::string::npos && sv[i]!="." && sv[i]!="..")
            fileCollection.push_back(directory_name+'\\'+sv[i]);

        // Else it's a folder, and you have to reexecute the function recursilvely
        else if(sv[i]!="." && sv[i]!=".."){
            explore_directory(directory_name+'\\'+sv[i], fileCollection);
        }
    }
}


/*
Test if the line start with #include
*/
bool detectLine(std::string line)
{
    if (line.rfind("#include", 0) == 0) { // pos=0 limits the search to the prefix
         return true;
    }
    return false;
}


/*
We suppose the line is with an include preprocessor.
We want to extrat the library name from that line.
*/

std::string extractLine(std::string line)
{
    // to do : far from being perfect
    return &line[8];
}

/*

*/
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


void scanFile(std::string filePath, stringvec& libraryCollection)
{
    std::ifstream file(filePath);

    std::string str;

    // We read the file line by line
    while(getline(file, str))
    {
        if(detectLine(str)){
            libraryCollection.push_back(extractLine(str));

            /*if(str == "#include MBEDTLS_CONFIG_FILE")
                std::cout << "FILEPATH: " << filePath << std::endl;
            else if(str == "#include MBEDTLS_USER_CONFIG_FILE")
                std::cout << "FILEPATH: " << filePath << std::endl;*/

        }

    }

}


stringvec extractLocalLibraries(const stringvec& totalLibraries)
{
    std::vector<std::string> localLibraries;

    for(int i=0; i<totalLibraries.size(); ++i)
    {
        if( (totalLibraries[i]).find('"') != string::npos){


            std::string sss = totalLibraries[i].substr(totalLibraries[i].find('"')+1 , totalLibraries[i].find_last_of('"')-2);

            // If we detect the character '/%' then things are gonna be different
            if(sss.find('/') != std::string::npos){
                sss = sss.substr(totalLibraries[i].find_last_of('/')-1);

                if(sss[0] == '/')
                    sss = string(&sss[1]);
            }

            localLibraries.push_back(sss);

        }
    }

    return localLibraries;
}

stringvec extractErrorFromTotalLibraries(const stringvec& totalLibraries)
{
    std::vector<std::string> errorLibraries;

    for(int i=0; i<totalLibraries.size(); ++i)
    {
        if( totalLibraries[i].find('"') != string::npos){
            // Then it is probably a local library
        }
        else if( totalLibraries[i].find('<') != string::npos ) {
            // Then it is probably a global library
        }
        else {
            //
            errorLibraries.push_back(totalLibraries[i]);
        }
    }

    return errorLibraries;
}


void replaceFileByFilePath(stringvec& localLibraries, const std::string& filename, const std::string& filepath)
{
    for(int i=0; i<localLibraries.size(); ++i)
    {
        if(localLibraries[i] == filename){
            localLibraries[i] = filepath+'\\'+filename;
            //return;
        }
    }
}



void reexplore_directory(std::string directory_name, stringvec& fileCollection)
{
   stringvec sv;

    // We look for all the files and folders that are in that directory
    read_directory(directory_name, sv);


    // We explore all teh different inputs
    for(size_t i=0; i < sv.size(); ++i){




        // IF there is a point then, it's a file
        // Also it's important to note that filenames "." and ".." aren't files
        if(sv[i].find('.') != std::string::npos && sv[i]!="." && sv[i]!="..")
            replaceFileByFilePath(fileCollection, sv[i], directory_name);

        // Else it's a folder, and you have to reexecute the function recursilvely
        else if(sv[i]!="." && sv[i]!=".."){
            reexplore_directory(directory_name+'\\'+sv[i], fileCollection);
        }
    }
}

void retirer_liste(stringvec& bigOne, const stringvec& smallOne)
{

    for(std::string str : smallOne){

        // Simplification
        stringvec& vec = bigOne;

        // Delete the element (if the element exists)
        vec.erase(std::remove(vec.begin(), vec.end(), str), vec.end());
    }
}

void extractUnspecifiedLibraries(stringvec& libraries, stringvec& unspecifiedList)
{
    // We are looking for names that do not refer to an actual path

    for(std::string str : libraries)
    {
        if(str[1] != ':' && str[2] != '//'){
            unspecifiedList.push_back(str);
        }
    }


}

/*
Remove the first path of the path C:/ in order to be used by .gitignore file
*/

void simplifyPaths(stringvec& sv, std::string initialPath)
{
    for(int i=0; i<sv.size(); ++i){
        sv[i] = sv[i].substr(initialPath.size());
    }
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void scanShFile(string location, stringvec& libraryCollection)
{
    std::cout << "Scanning " << location << endl;

    std::ifstream file(location);

    char byte;

    while(file.get(byte))
    {

        if(byte == '"')
        {
            std::string str;

            // We get the name of the file from the string
            while(file.get(byte) && byte != '"')
                str += byte;

            //std::cout << "string:" << str << endl;

            // For it to be a file, it must contain a point
            // it musn't contain 2 points
            // It musn't also already exist in the library collection
            if(str.find('.') != std::string::npos
            && str.find('.') == str.rfind('.')
            && std::find(libraryCollection.begin(), libraryCollection.end(), str) == libraryCollection.end())
            {
                //std::cout << "added:" << str << endl;

                libraryCollection.push_back( string("\"")+str+string(" \"") );
            }
        }

    }

}

void scanProjectFile(string location, stringvec& libraryCollection)
{
    // Here we want to extract the linked ressources from the CubeIDE project file

    std::ifstream file(location);

    // First step, we look for the word <linkedResources>
    string str;
    while(file >> str && str != "<linkedResources>");

    // Second step we look for <name>, but this time, th eword is not seperated blank spaces
    while(getline(file,str))
    {
        int k=0;
        string goal = "<name>";

        for(int i=0; i<str.size(); ++i){

            if(str[i]==goal[k])
            {
                ++k;

                if(k==str.size())
                {
                    // In this cas, we passed the goal
                    goal = "";

                    while(++i < str.size() && str[i]!='<')
                        goal += str[i];

                    std::cout << "Added: " << goal << std::endl;

                    // We add this file to the library collection
                    libraryCollection.push_back(goal);

                    // We quit the first loop, and we enter the second one
                    break;
                }
            }
        }

    }


}


int main()
{
    std::cout << "Please enter the directory of the git repository :" << std::endl;
    std::cout << "ex: " << "C:\\Users\\divouxje\\Desktop\\jeanleprodegit" << std::endl;

    // We get the line of the path of the git repository
    std::string pathGitRepository;
    getline(std::cin, pathGitRepository);

    if(pathGitRepository=="t")
        pathGitRepository="C:\\Users\\divouxje\\Desktop\\STM32Cube_FW_U5_V1.0.0";

    std::cout << "Please enter the path to the project from that directory : " << std::endl;
    std::cout << "ex: " << "\\Projects\\B-U585I-IOT02A\\Applications\\TFM" << std::endl;

    std::string pathProject;
    getline(std::cin, pathProject);

    if(pathProject=="t")
        pathProject="\\Projects\\B-U585I-IOT02A\\Applications\\TFM";



    // Step 1: We list all the files tha are in that directory
    std::vector<std::string> fileCollection;
    explore_directory(pathGitRepository+pathProject, fileCollection);


    /* Debugging step: we print the results */
    //for(int i=0; i<fileCollection.size(); ++i) std::cout << fileCollection[i] << endl;


    // Step 2: We scan all the files for potential library inputs
    std::vector<std::string> libraryCollection;

    for(size_t i=0; i < fileCollection.size(); ++i)
    {
        // Here we scan sh file
        if(hasEnding(fileCollection[i], ".sh"))
            scanShFile(fileCollection[i], libraryCollection);

        // Here we scan project files
        else if(hasEnding(fileCollection[i], ".project"))
            scanProjectFile(fileCollection[i], libraryCollection);

        // Here we scan source and header files
        else
            scanFile(fileCollection[i], libraryCollection);
    }
    supprimerDoublons(libraryCollection);


    // Debugging step: we print the results
    // for(auto str: libraryCollection) std::cout << str << std::endl;



    // Step 3: We extract from it the local librairies

    stringvec localLibraries = extractLocalLibraries(libraryCollection);
    stringvec errorLibraries = extractErrorFromTotalLibraries(libraryCollection);

    // Debugging step: we print the results
    // std::cout << "\n\nExtracted local libraries" << std::endl;
    // for(auto str: localLibraries) std::cout << str << std::endl;


    // Step 4: We look for the file path corresponding to these local libraries

    reexplore_directory(pathGitRepository, localLibraries);

    // Step 5: We determine the files hat have to be deleted
    stringvec toBeDeleted;

    std::vector<std::string> allTheFiles;
    explore_directory(pathGitRepository, allTheFiles);

    retirer_liste(allTheFiles, fileCollection);
    retirer_liste(allTheFiles, localLibraries);

    // From now, allTheFiles variable contains the list of all the files to be deleted

    stringvec errorLibraries2;
    extractUnspecifiedLibraries(localLibraries, errorLibraries2);





    // Printing part: we exchange the informations with the user
    std::cout << "Error with these libraries (=> not considered local nor global) :" << std::endl;

    for(auto str: errorLibraries) std::cout << str << std::endl;

    std::cout << "\n\nError with these libraries (=> they do not refer to a file in the folder) :" << std::endl;

    for(auto str: errorLibraries2) std::cout << str << std::endl;

    std::cout << "\n\n" << allTheFiles.size() << " files to be ignored" << std::endl;


    std::cout << "Now please type the files you want to ignore (okay=>compute the changes, exit=>cancel):" << std::endl;

    string inputLine;
    while(getline(std::cin, inputLine))
    {
        if(inputLine=="exit")
            return 0; // We normally exit the program

        else if(inputLine == "okay")
            break; // We save the changes

        else {
            // We try to remove the input from the list of files to be deleted
            stringvec& vec = allTheFiles;
            vec.erase(std::remove(vec.begin(), vec.end(), inputLine), vec.end());

            std::cout << "still " << allTheFiles.size() << " files to go" << std::endl;
        }

    }

    std::cout << "Type if you want the 'gitignore solution' or type 2 for the 'delete solution' or type 3 to exit" << std::endl;

    while(getline(std::cin, inputLine) && inputLine != "1" && inputLine != "2" && inputLine != "3");

    if(inputLine == "1")
    {

    // Let's save the change

    simplifyPaths(allTheFiles, pathGitRepository);

    // Let's add this to the gitignore file



    std::ofstream gitignoreFile(pathGitRepository+"\\.gitignore");

    for(auto str : allTheFiles){
        gitignoreFile << str << std::endl;
    }

    std::cout << "Looks good !" << std::endl;

    }

    else if(inputLine == "2")
    {
        for(auto str : allTheFiles)
        {
            if(remove(str.c_str()) != 0)
            {



                std::cout << "Error: couldn't remove '" << str << "'\n";
            }


        }

        std::cout << "Looks good" << std::endl;



    }



    return 0;
}
