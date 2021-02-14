#include "color.hpp"

std::string write_color(std::string txt, color c){
    #ifdef __linux__
    // Doesn't care if it's on a VT100 terminal or not
    // will do coloring anyway.
    std::string result = "\033[1;"; // Defaults with bold
    switch(c){
        case color::red:
        result += "31m";
        break;
        case color::blue:
        result += "34m";
        break;
        case color::green:
        result += "32m";
        break;
        case color::yellow:
        result += "33m";
        break;
    }
    result += txt;
    result += "\033[0m";
    return result;
    #else
    return txt;
    #endif
}