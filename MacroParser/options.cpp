#include "options.hpp"

Options::Options()
{
    resetToDefault();
    loadFromFile(OPTIONS_FILENAME);
}

void Options::resetToDefault()
{
    // Set default parameters for the option
    importOnlySourceFileExtension = true;
    importMacroCommented = false;
    printReplacements = false;
    printExprAtEveryStep = false;
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
                loadBooleanValue(line.substr(30), importOnlySourceFileExtension);
            }
            else if(line.substr(0,21)=="importMacroCommented=")
            {
                loadBooleanValue(line.substr(21), importMacroCommented);
            }
            else if(line.substr(0,18)=="printReplacements=")
            {
                loadBooleanValue(line.substr(18), printReplacements);
            }
            else if(line.substr(0,21)=="printExprAtEveryStep=")
            {
                loadBooleanValue(line.substr(21), printExprAtEveryStep);
            }
        }




        return true;
    }


    return false;
}

 bool Options::loadBooleanValue(const std::string& input, bool& booleanValue)
{
    if(input=="1" || input=="true"){
        booleanValue = true;
        return true;
    }
    else if(input == "0" || input=="false"){
        booleanValue = false;
        return true;
    }

    return false;
}

void Options::toStream(std::ostream& stream) const
{
    stream << "importOnlySourceFileExtension=" << importOnlySourceFileExtension << std::endl;
    stream << "importMacroCommented=" << importMacroCommented << std::endl;
    stream << "printReplacements=" << printReplacements << std::endl;
    stream << "printExprAtEveryStep=" << printExprAtEveryStep << std::endl;
}



bool Options::saveToFile(const char* filename) const
{
    std::ofstream file(filename);

    if(!file)
        return false;

    toStream(file);
    return true;
}

// Getters implementation

bool Options::doesImportOnlySourceFileExtension() const
{
    return importOnlySourceFileExtension;
}

bool Options::doesImportMacroCommented() const
{
    return importMacroCommented;
}

bool Options::doesPrintReplacements() const
{
    return printReplacements;
}

bool Options::doesPrintExprAtEveryStep() const
{
    return printExprAtEveryStep;
}
