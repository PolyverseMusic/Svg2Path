#pragma once

#include <JuceHeader.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>

class SvgParser
{
public:
    //==============================================================================
    SvgParser() {};
    ~SvgParser() {};
    //! @brief parse the svg file
    //! @arg svgContent: the svg string
    //! @arg path: a reference to the path to draw onto
    String parse(String svgContent, Path& path);
    //! @brief returns a binary representation of the path
    //! @arg path: a reference to the path to read
    //! @arg name: an optional name for the exported path
    String getBinary(Path& path, String name);
    
private:
    bool parseNumber(const String& s, int& index, float& number);
    String parseSVGPathData(const String& pathData, Path& path);
    void collectPaths(XmlElement* element, std::vector<String>& pathDataList);
    
    inline String f(float val) { return String::formatted("%.1ff", val); }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SvgParser)
};
