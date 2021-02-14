#include "helper.hpp"
#include "../color.hpp"
#include <iostream>

std::string helper::write_helper(std::vector<std::string> content,cmd& options)
{
    std::string result;
    
    if (
        type == helper_type::global_warning || 
        type == helper_type::line_warning || 
        type == helper_type::location_warning )
    {
        if(options.has_color)
        {
            result = write_color("[WARNING]",color::yellow);
        }
        else
        {
            result = "[WARNING]";
        }

        result += this->msg;
    }
    else if (
        type == helper_type::global_err || 
        type == helper_type::line_err || 
        type == helper_type::location_err )
    {
        if(options.has_color)
        {
            result = write_color("[ERROR]",color::red);//"[ERROR] ";
        }
        else
        {
            result = "[ERROR]";
        }

        result += this->msg;
    }
    
    result += "\n";
    
    if (type != helper_type::global_warning && type != helper_type::global_err)
    {
        result += "At ";
        result += l.filename;
        result += ":";
        result += std::to_string(l.line);
        
        if (type == helper_type::location_warning || type == helper_type::location_err)
        {
            result += ":";
            result += std::to_string(l.start);
        }
        
        result += "\n";
        
        if (l.line - 1 >= 0)
        {
            result += "    | ";
            result += content.at(l.line - 1);
            result += "\n\n";
        }

        result += std::to_string(l.line);
        result += " --> ";
        result += content.at(l.line);
        result += "\n";

        if (type == helper_type::location_warning || type == helper_type::location_err)
        {
            std::string identation;
            for (int i = 0; i < l.start + 6; i++)
            {
                identation += " ";
            }
            for (int i = 0; i <= l.end - l.start; i++)
            {
                identation += "^";
            }
            if(options.has_color){
                result += write_color(identation,color::red);
            }
            else{
                result += identation;
            }
            result += "\n";
        }
        if (l.line + 1 < content.size())
        {
            result += "    | ";
            result += content.at(l.line + 1);
            result += "\n";
        }
    }
    return result;
}