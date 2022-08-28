#include "options.hpp"

Options::Options()
{
    // Set default parameters for the option
    importOnlySourceFileExtension = true;
    importMacroCommented = true;
}


bool Options::loadFromFile(const char* filename)
{
    std::ifstream file(filename);

    if(file.is_open())
    {
        std::string line;

        while(getline(file, line))
        {
            if(line.substr(0,30)=="importOnlySourceFileExtension=")
            {
                if(line.substr(30)=="true")
                    importMacroCommented=true;
                //else if(line.substr(30)=="false")

            }


        }




        return true;
    }


    return false;
}

void loadBooleanValue(std::istream& input, bool& booleanValue)
{

}



bool Options::saveToFile(const char* filename)
{

}
