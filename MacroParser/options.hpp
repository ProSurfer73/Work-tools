#ifndef OPTIONS_HPP
#define OPTIONS_HPP

/// Static options ///
// Options dealt with at compilation time


/// Dynamic options ///
// Options that can be modified and configured by the user at runtime

#include <fstream>
#include <string>

class Options
{
public:
    Options();

    bool loadFromFile(const char* filename);
    bool saveToFile(const char* filename);

private:
    void loadBooleanValue(std::istream& input);

    bool importOnlySourceFileExtension;
    bool importMacroCommented; // should macro that are inside comment such as // and /* be imported ?

};


#endif // OPTIONS_HPP
