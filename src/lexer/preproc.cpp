#include "preproc.hpp"

#include <iostream>

// Cuts the input into individual words/locations -> will later become tokens
std::vector<location> disjoint(std::vector<std::string> content)
{
    std::vector<location> result;

    size_t l = 0; // line counter
    for (std::string& line : content)
    {
        for (size_t i = 0; i < line.size(); i++)
        {
            if (line.at(i) == '"')
            {
                location loc;
                loc.start = i;
                loc.line = l;
                // Doesn't handle \"
                i++;
                while (i < line.size() && line.at(i) != '"')
                {
                    // Skips escape characters
                    if (line.at(i) == '\\')
                    {
                        i++;
                    }
                    i++;
                }
                //i--;
                loc.end = i;
                result.push_back(loc);
            }
            else if (line.at(i) == '\'')
            {
                location loc;
                loc.start = i;
                loc.line = l;
                // Doesn't handle \'
                i++;
                while (i < line.size() && line.at(i) != '\'')
                {
                    // Skips escape characters
                    if (line.at(i) == '\\')
                    {
                        i++;
                    }
                    i++;
                }
                //i--;
                loc.end = i;
                result.push_back(loc);
            }
            else if (isalnum(line.at(i)) != 0)
            {
                location loc;
                loc.start = i;
                loc.line = l;
                while (i < line.size() && isalnum(line.at(i)) != 0)
                {
                    if (i < line.size() - 2)
                    {
                        if (line.at(i + 1) == '.' && isdigit(line.at(i + 2)))
                        {
                            i += 2;
                        }
                    }
                    i++;
                }
                i--;
                loc.end = i;
                result.push_back(loc);
            }
            else if (isspace(line.at(i)) != 0)
            {
                location loc;
                loc.start = i;
                loc.line = l;
                while (i < line.size() && isspace(line.at(i)) != 0)
                {
                    i++;
                }
                i--;
                loc.end = i;
                //result.push_back(loc);
            }
            else
            {
                location loc;
                loc.start = i;
                loc.line = l;
                
                if(i + 1 < line.size() && line.at(i+1) == '=')
                {
                    i++;
                }

                loc.end = i;
                result.push_back(loc);
            }
        }
        l++;
    }
    return result;
}

// This function should have no side-effects
std::vector<location> preprocess(std::string fname, std::vector<std::string> content)
{
    enum class platform
    {
        WINDOWS,
        LINUX,
        BSD,
        OSX,
        UNIX,
        UNKNOWN
    };

    // Currently it only takes into account the system where it was compiled
    platform target_platform;

// Note: Linux should always be before the Unix macro
#if defined(_WIN64) || defined(_WIN32) || defined(__WINDOWS__)
    target_platform = platform::WINDOWS;
#elif defined(__linux) || defined(linux) || defined(__linux__)
    target_platform = platform::LINUX;
#elif defined(__DragonFly__) || defined(__FreeBSD)
    target_platform = platform::BSD;
#elif defined(__APPLE__) || defined(macintosh) || defined(__MACH__)
    target_platform = platform::OSX;
#elif defined(__unix) || defined(unix)
    target_platform = platform::UNIX;
#else
    target_platform = platform::UNKOWN;
#endif

    using namespace std::string_literals;
    std::vector<location> src = disjoint(content);
    std::vector<location> result;

    for (size_t i = 0; i < src.size(); i++)
    {
        location loc = src.at(i);

        if (content.at(loc.line).at(loc.start) == '#')
        {
            size_t first_line = loc.line;
            while (i < src.size() && first_line == src.at(i).line)
            {
                i++;
            }
            i--;
        }
        else if (content.at(loc.line).at(loc.start) == '@')
        {
            // Check if it's the first non-whitespace character on line
            // If first tracked location, it is certain to be whitespace
            if (
                (i == 0 || src.at(i - 1).line != src.at(i).line) &&
                (i + 1 < src.size() && src.at(i).line == src.at(i + 1).line))
            {
                std::string cmd;
                for (int n = src.at(i + 1).start; n <= src.at(i + 1).end; n++)
                {
                    cmd += content.at(src.at(i + 1).line).at(n);
                }

                if (cmd == "platform"s)
                {
                    // Doesn't check  if on same line tho
                    if (i + 3 < src.size())
                    {
                        location pname = src.at(i + 3); // platform name
                        std::string pval;
                        for (int n = pname.start; n <= pname.end; n++)
                        {
                            pval += content.at(pname.line).at(n);
                        }
                        i += 3;
                        platform p;
                        if (pval == "windows"s)
                        {
                            p = platform::WINDOWS;
                        }
                        else if (pval == "linux"s)
                        {
                            p = platform::LINUX;
                        }
                        else if (pval == "bsd"s)
                        {
                            p = platform::BSD;
                        }
                        else if (pval == "apple"s || pval == "osx"s || pval == "mac"s)
                        {
                            p = platform::OSX;
                        }
                        else if (pval == "unix"s)
                        {
                            p = platform::UNIX;
                        }
                        else
                        {
                            p = platform::UNKNOWN;
                        }

                        if (target_platform != p)
                        {
                            // Doesn't check for @end, just checks for @
                            // which should be improved
                            while (i < src.size() && content.at(src.at(i).line).at(src.at(i).start) != '@')
                            {
                                i++;
                            }
                            i--;
                        }
                    }
                }
                else if (cmd == "end"s)
                {
                    // It's normal behaviour now
                    //std::cout<<"PREPROCESSOR WARN: Uncatched end at ("<<src.at(i+1).line<<":"<<src.at(i+1).start<<")"<<std::endl;
                    i++;
                }
                else
                {
                    std::cout << "PREPROCESSOR WARN: Unknown at (" << 
                    src.at(i + 1).line << ":" << src.at(i + 1).start << ")\n";
                    i++;
                }
            }
        }
        else
        {
            src.at(i).filename = fname;
            result.push_back(src.at(i));
        }
    }

    return result;
}