#include "SvgParser.h"

bool SvgParser::parseNumber(const String& s, int& index, float& number)
{
    while (index < s.length() && CharacterFunctions::isWhitespace(s[index]))
        ++index;
    
    if (index >= s.length())
        return false;
    
    int start = index;
    bool hasDecimal = false;
    bool hasExp = false;
    
    if (s[index] == '+' || s[index] == '-')
        ++index;
    
    while (index < s.length())
    {
        if (CharacterFunctions::isDigit(s[index]))
        {
            ++index;
        }
        else if (s[index] == '.' && !hasDecimal)
        {
            hasDecimal = true;
            ++index;
        }
        else if ((s[index] == 'e' || s[index] == 'E') && !hasExp)
        {
            hasExp = true;
            ++index;
            if (s[index] == '+' || s[index] == '-')
                ++index;
        }
        else
        {
            break;
        }
    }
    
    if (start == index)
        return false;
    
    number = s.substring(start, index).getFloatValue();
    
    while (index < s.length() && (CharacterFunctions::isWhitespace(s[index]) || s[index] == ','))
        ++index;
    
    return true;
}

String SvgParser::parseSVGPathData(const String& pathData, Path& path)
{
    String juceCode;
    int index = 0;
    juce_wchar command = 0;
    juce_wchar prevCommand = 0;
    float x = 0, y = 0;
    float startX = 0, startY = 0;
    float prevCtrlX = 0, prevCtrlY = 0;
    
    auto skipWhitespace = [&](int& idx)
    {
        while (idx < pathData.length() && CharacterFunctions::isWhitespace(pathData[idx]))
            ++idx;
    };
    
    while (index < pathData.length())
    {
        skipWhitespace(index);
        
        if (index >= pathData.length())
            break;
        
        if (CharacterFunctions::isLetter(pathData[index]))
        {
            command = pathData[index++];
        }
        else if (prevCommand)
        {
            command = prevCommand;
        }
        else
        {
            std::cerr << "Invalid path data at position " << index << std::endl;
            return {};
        }
        
        bool isRelative = CharacterFunctions::isLowerCase(command);
        
        switch (CharacterFunctions::toUpperCase(command))
        {
            case 'M':
            {
                float x1, y1;
                if (!parseNumber(pathData, index, x1) || !parseNumber(pathData, index, y1))
                {
                    std::cerr << "Invalid 'M' command at position " << index << std::endl;
                    return {};
                }
                if (isRelative)
                {
                    x += x1;
                    y += y1;
                }
                else
                {
                    x = x1;
                    y = y1;
                }
                juceCode << "    path.startNewSubPath(" << f(x) << ", " << f(y) << ");\n";
                path.startNewSubPath(x, y);
                startX = x;
                startY = y;
                prevCommand = isRelative ? 'l' : 'L';
                break;
            }
            case 'L':
            {
                while (true)
                {
                    float x1, y1;
                    if (!parseNumber(pathData, index, x1) || !parseNumber(pathData, index, y1))
                        break;
                    if (isRelative)
                    {
                        x += x1;
                        y += y1;
                    }
                    else
                    {
                        x = x1;
                        y = y1;
                    }
                    juceCode << "    path.lineTo(" << f(x) << ", " << f(y) << ");\n";
                    path.lineTo(x, y);
                }
                prevCommand = command;
                break;
            }
            case 'H':
            {
                while (true)
                {
                    float x1;
                    if (!parseNumber(pathData, index, x1))
                        break;
                    if (isRelative)
                    {
                        x += x1;
                    }
                    else
                    {
                        x = x1;
                    }
                    juceCode << "    path.lineTo(" << f(x) << ", " << f(y) << ");\n";
                    path.lineTo(x, y);
                }
                prevCommand = command;
                break;
            }
            case 'V':
            {
                while (true)
                {
                    float y1;
                    if (!parseNumber(pathData, index, y1))
                        break;
                    if (isRelative)
                    {
                        y += y1;
                    }
                    else
                    {
                        y = y1;
                    }
                    juceCode << "    path.lineTo(" << f(x) << ", " << f(y) << ");\n";
                    path.lineTo(x, y);
                }
                prevCommand = command;
                break;
            }
            case 'C':
            {
                while (true)
                {
                    float x1, y1, x2, y2, x3, y3;
                    if (!parseNumber(pathData, index, x1) || !parseNumber(pathData, index, y1)
                        || !parseNumber(pathData, index, x2) || !parseNumber(pathData, index, y2)
                        || !parseNumber(pathData, index, x3) || !parseNumber(pathData, index, y3))
                        break;
                    float cx1 = isRelative ? x + x1 : x1;
                    float cy1 = isRelative ? y + y1 : y1;
                    float cx2 = isRelative ? x + x2 : x2;
                    float cy2 = isRelative ? y + y2 : y2;
                    x = isRelative ? x + x3 : x3;
                    y = isRelative ? y + y3 : y3;
                    juceCode << "    path.cubicTo(" << f(cx1) << ", " << f(cy1) << ", " << f(cx2) << ", " << f(cy2)
                    << ", " << f(x) << ", " << f(y) << ");\n";
                    path.cubicTo(cx1, cy1, cx2, cy2, x, y);
                    prevCtrlX = cx2;
                    prevCtrlY = cy2;
                }
                prevCommand = command;
                break;
            }
            case 'S':
            {
                while (true)
                {
                    float x2, y2, x3, y3;
                    if (!parseNumber(pathData, index, x2) || !parseNumber(pathData, index, y2)
                        || !parseNumber(pathData, index, x3) || !parseNumber(pathData, index, y3))
                        break;
                    float cx1 = x * 2 - prevCtrlX;
                    float cy1 = y * 2 - prevCtrlY;
                    float cx2 = isRelative ? x + x2 : x2;
                    float cy2 = isRelative ? y + y2 : y2;
                    x = isRelative ? x + x3 : x3;
                    y = isRelative ? y + y3 : y3;
                    juceCode << "    path.cubicTo(" << f(cx1) << ", " << f(cy1) << ", " << f(cx2) << ", " << f(cy2)
                    << ", " << f(x) << ", " << f(y) << ");\n";
                    path.cubicTo(cx1, cy1, cx2, cy2, x, y);
                    prevCtrlX = cx2;
                    prevCtrlY = cy2;
                }
                prevCommand = command;
                break;
            }
            case 'Q':
            {
                while (true)
                {
                    float x1, y1, x2, y2;
                    if (!parseNumber(pathData, index, x1) || !parseNumber(pathData, index, y1)
                        || !parseNumber(pathData, index, x2) || !parseNumber(pathData, index, y2))
                        break;
                    float cx1 = isRelative ? x + x1 : x1;
                    float cy1 = isRelative ? y + y1 : y1;
                    x = isRelative ? x + x2 : x2;
                    y = isRelative ? y + y2 : y2;
                    juceCode << "    path.quadraticTo(" << f(cx1) << ", " << f(cy1) << ", " << f(x) << ", " << f(y)
                    << ");\n";
                    path.quadraticTo(cx1, cy1, x, y);
                    prevCtrlX = cx1;
                    prevCtrlY = cy1;
                }
                prevCommand = command;
                break;
            }
            case 'T':
            {
                while (true)
                {
                    float x2, y2;
                    if (!parseNumber(pathData, index, x2) || !parseNumber(pathData, index, y2))
                        break;
                    float cx1 = x * 2 - prevCtrlX;
                    float cy1 = y * 2 - prevCtrlY;
                    x = isRelative ? x + x2 : x2;
                    y = isRelative ? y + y2 : y2;
                    juceCode << "    path.quadraticTo(" << f(cx1) << ", " << f(cy1) << ", " << f(x) << ", " << f(y)
                    << ");\n";
                    path.quadraticTo(cx1, cy1, x, y);
                    prevCtrlX = cx1;
                    prevCtrlY = cy1;
                }
                prevCommand = command;
                break;
            }
            case 'A':
            {
                // Arc commands are complex and require more elaborate handling.
                // For simplicity, you can approximate arcs with cubic Bézier curves or skip them.
                std::cerr << "Arc commands are not supported in this parser.\n";
                return {};
            }
            case 'Z':
            {
                juceCode << "    path.closeSubPath();\n";
                path.closeSubPath();
                x = startX;
                y = startY;
                prevCommand = command;
                break;
            }
            default:
                std::cerr << "Unknown command '" << (char) command << "' at position " << index << std::endl;
                return {};
        }
    }
    
    return juceCode;
}

void SvgParser::collectPaths(XmlElement* element, std::vector<String>& pathDataList)
{
    if (element == nullptr)
        return;
    
    if (element->hasTagName("path"))
    {
        auto pathData = element->getStringAttribute("d");
        if (!pathData.isEmpty())
        {
            pathDataList.push_back(pathData);
        }
    }
    
    // Recursively check all child elements
    for (auto* child = element->getFirstChildElement(); child != nullptr; child = child->getNextElement())
    {
        collectPaths(child, pathDataList);
    }
}

String SvgParser::parse(String svgContent, Path& path)
{
    // Parse the SVG content using JUCE's XML parsing
    std::unique_ptr<XmlElement> svg(XmlDocument::parse(svgContent));
    
    if (svg == nullptr)
    {
        return "Could not parse SVG content.";
    }
    
    // Get viewBox dimensions
    auto viewBoxAttr = svg->getStringAttribute("viewBox");
    float vbX = 0.0f, vbY = 0.0f, vbWidth = 1.0f, vbHeight = 1.0f;
    
    if (!viewBoxAttr.isEmpty())
    {
        StringArray tokens;
        tokens.addTokens(viewBoxAttr, " ,", "");
        tokens.removeEmptyStrings();
        
        if (tokens.size() == 4)
        {
            vbX = tokens[0].getFloatValue();
            vbY = tokens[1].getFloatValue();
            vbWidth = tokens[2].getFloatValue();
            vbHeight = tokens[3].getFloatValue();
        }
    }
    
    // Use the recursive function to collect all <path> elements
    std::vector<String> pathDataList;
    collectPaths(svg.get(), pathDataList);
    
    if (pathDataList.empty())
    {
        return "No path data found in SVG content.";
    }
    
    // Generate JUCE code for each path
    String fullJuceCode;
    fullJuceCode << "Path createPath()\n";
    fullJuceCode << "{\n";
    fullJuceCode << "    Path path;\n";
    path.clear();
    for (const auto& pathData: pathDataList)
    {
        auto juceCode = parseSVGPathData(pathData, path);
        
        if (juceCode.isEmpty())
        {
            return "Error parsing path data.";
        }
        
        fullJuceCode << juceCode;
    }
    
    fullJuceCode << "    return path;\n";
    fullJuceCode << "}\n";
    
    return fullJuceCode;
}

String SvgParser::getBinary(Path& path, String name)
{
    if (!path.isEmpty())
    {
        MemoryOutputStream data;
        path.writePathToStream(data);
        
        MemoryOutputStream out;
        if (name.isNotEmpty())
        {
            out << "static const unsigned char " << name << "PathData[] = ";
        }
        else
        {
            out << "static const unsigned char pathData[] = ";
        }
        build_tools::writeDataAsCppLiteral(data.getMemoryBlock(), out, false, true);
        out << newLine;
        
        return out.toString();
    }
    return "path empty";
}
