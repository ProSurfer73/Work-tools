#include "filesystem.hpp"


void replace_all(
    std::string& s,
    std::string const& toReplace,
    std::string const& replaceWith
) {
    std::ostringstream oss;
    std::size_t pos = 0;
    std::size_t prevPos = pos;

    while (true) {
        prevPos = pos;
        pos = s.find(toReplace, pos);
        if (pos == std::string::npos)
            break;
        oss << s.substr(prevPos, pos - prevPos);
        oss << replaceWith;
        pos += toReplace.size();
    }

    oss << s.substr(prevPos);
    s = oss.str();
}


static bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

#if (defined(_WIN32) || defined(_WIN64))

void explore_directory(std::string dirname, std::vector<std::string>& files)
{
    WIN32_FIND_DATA data;
    HANDLE hFind;

    if(dirname.back()!='\\')
        dirname += '\\';

    //std::cout << "dirname: " << dirname << std::endl;

    if ((hFind = FindFirstFile((dirname+'*').c_str(), &data)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                if(strcmp(data.cFileName, ".")!=0 && strcmp(data.cFileName, "..")!= 0){
                    explore_directory(dirname+data.cFileName,files);
                }

            }
            else {
                files.emplace_back(dirname+data.cFileName);
            }
            //Sleep(500);
        }
        while (FindNextFile(hFind, &data) != 0);

        FindClose(hFind);
    }
}


bool directoryExists(const char* szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#else

#include <dirent.h>
#include <sys/types.h>

void explore_directory(std::string basepath, std::vector<std::string>& vec)
{
    struct dirent *dp;
    DIR *dir = opendir(basepath.c_str());
    if(!dir)
        return;

    basepath += '/';


    while ((dp = readdir(dir)) != NULL)
    {


        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {

            if(dp->d_type != DT_DIR){
                vec.emplace_back(basepath+dp->d_name);
            }
            else {
                // Construct new path from our base path
                    explore_directory(basepath+dp->d_name, vec);
            }
        }
    }

    closedir(dir);
}

bool directoryExists(const char* basepath)
{
    DIR *dir = opendir(basepath);
    bool isOpen = (dir != nullptr);

    if(isOpen)
        closedir(dir);

    return isOpen;
}


#endif

bool readFile(const string& path, stringvec& words, bool hasInsensitive)
{
    ifstream file(path);

    // We can't open the file, well, let's throw an exception
    if(!file)
        throw FileException();

    std::string line;

    while(std::getline(file, line))
    {
        for(const std::string& str: words)
        {
            // if it starts with '#', it means that we have to treat it case insensitive
            if(str.front() != '#' && line.find(str) != std::string::npos) {
                return true;
            }
        }

        if(hasInsensitive)
        {

        for(char& c: line) {
            c = toupper(c);
        }

        for(const std::string& str : words)
        {
            // if it starts with '#', it means that we have to treat it case insensitive
            if(str.front() == '#' && line.find(str.substr(1)) != std::string::npos) {
                return true;
            }
        }

        }
    }

    // if we end up here, the word has not been found
    return false;
}


// Function to replace all the occurrences
// of the substring S1 to S2 in string S
void modifyString(string& s, const string& s1, const string& s2)
{
    // Stores the resultant string
    string ans;

    // Traverse the string s
    for (int i = 0; i < s.length(); i++)
    {
        int k = 0;

        // If the first character of
        // string s1 matches with the
        // current character in string s
        if (s[i] == s1[k]
            && i + s1.length()
                   <= s.length())
        {
            int j;

            // If the complete string
            // matches or not
            for (j = i; j < i + s1.length(); j++) {

                if (s[j] != s1[k]) {
                    break;
                }
                else {
                    k = k + 1;
                }
            }

            // If complete string matches
            // then replace it with the
            // string s2
            if (j == i + s1.length()) {
                ans.append(s2);
                i = j - 1;
            }

            // Otherwise
            else {
                ans.push_back(s[i]);
            }
        }

        // Otherwise
        else {
            ans.push_back(s[i]);
        }
    }

    // Print the resultant string
    cout << ans;
}




bool replaceKeywordFile(const string& path, const string& initialKeyword, const string& finalKeyword)
{
    ifstream file(path, ios::in);

    if(!file)
        throw FileException();

    string total;
    string curLine;

    while(getline(file, curLine))
    {
        if(!total.empty())
            total += '\n';

        total += curLine;
    }

    file.close();

    // Replace the word
    replace_all(total, initialKeyword, finalKeyword);

    // Let's check for insensitive words
    if(initialKeyword.front() == '#')
    {
        // Let's uppercase the whole string corresponding to the file

        std::cout << "Not yet implemented!" << std::endl;
        // let's implement here 'replace_all_insensitive'
    }

    // Let's rewrite the modified content into the file
    ofstream file2(path);

    if(!file2){
        cerr << "Can't write file:" << path << endl;
        cerr << strerror(errno) << endl; // displays "Permission denied"
        return false;
    }

    //cout << "content of total: " << total << endl;

    file2 << total;

    file2.close();

    return true;

}

bool readFileWithLine(std::string& path, const stringvec& words, std::ostream& output, bool containsInsensitive)
{
    std::ifstream file(path);

    if(!file)
        throw FileException();

    std::string readLine;

    bool found = false;
    unsigned lineNumber = 1;

    while(std::getline(file, readLine))
    {
        for(const std::string& str : words)
        {
            if(readLine.find(str) != std::string::npos)
            {
                if(!found)
                {
                    output << path << std::endl;
                    found = true;
                }
                output << 'l' << lineNumber << ": " << readLine << std::endl;
            }
        }

        if(containsInsensitive)
        {
            for(const std::string& str : words)
            {
                if(str.front()=='#')
                {
                    unsigned j=1;
                    for(unsigned i=0; i<readLine.size(); ++i){
                        if(str[j] == toupper(readLine[i])){
                            if(++j >= str.size()){
                                if(!found)
                                { // Let's show the filepath first
                                    output << path << std::endl;
                                    found = true;
                                }
                                output << 'l' << lineNumber << ": " << readLine << std::endl;

                            }
                        }
                        else
                            j=1;
                    }
                }
            }
        }

        ++lineNumber;
    }

    if(found)
        output << std::endl;

    return found;
}

const char* FileException::what()
{
    return "some file could not be opened.";
}
