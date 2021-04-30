#include "diagnostic.hpp"
#include "../color.hpp"
#include "location_provider.hpp"
#include <iostream>
#include <charconv>

static inline std::string get_spacing(int l)
{
    std::string og = "    | ";
    std::to_chars(og.data(), og.data() + og.size(), l);
    return og;
}

static bool is_important(std::string const& line)
{
    for(char c : line)
    {
        if(!isspace(c))
            return true;
    }
    return false;
}

std::string diagnostic::show_output(location_provider const& prov, std::vector<std::string> const& content, cmd& options) const
{
    std::string result;
    location const& tloc = prov.get_loc(l.begin);
    
    if (
        type == diagnostic_type::global_warning || 
        type == diagnostic_type::line_warning || 
        type == diagnostic_type::location_warning 
    ) {
        if (options.has_color)
        {
            result = write_color("[WARNING]", color::yellow);
        }
        else
        {
            result = "[WARNING]";
        }

        result += this->msg;
    }
    else if (
        type == diagnostic_type::global_err || 
        type == diagnostic_type::line_err || 
        type == diagnostic_type::location_err )
    {
        if (options.has_color)
        {
            result = write_color("[ERROR]", color::red);
        }
        else
        {
            result = "[ERROR]";
        }

        result += this->msg;
    }
    
    result += "\n";
    
    if (type != diagnostic_type::global_warning && type != diagnostic_type::global_err)
    {
        result += "In ";
        result += prov.print_loc(l);

        result += "\n";
        
        if (tloc.line - 1 >= 0 && is_important(content.at(tloc.line + 1)))
        {
            //result += "    | ";
            result += get_spacing(tloc.line - 1);
            result += content.at(tloc.line - 1);
            result += "\n    | \n";
        }

        result += std::to_string(tloc.line);
        if (options.has_color)
        {
            result += write_color(" --> ", color::yellow);
        }
        else
        {
            result += " --> ";
        }
        result += content.at(tloc.line);
        result += "\n";

        if (type == diagnostic_type::location_warning || type == diagnostic_type::location_err)
        {
            std::string identation;
            result += "    | ";

            for (int i = 0; i < tloc.start; i++)
            {
                identation += " ";
            }
            for (int i = 0; i < tloc.len; i++)
            {
                identation += "^";
            }
            if (options.has_color)
            {
                result += write_color(identation, color::red);
            }
            else
            {
                result += identation;
            }
            result += "\n";
        }
        else
        {
            result += "    |\n";
        }
        if (tloc.line + 1 < content.size() && is_important(content.at(tloc.line + 1)))
        {
            //result += "    | ";
            result += get_spacing(tloc.line + 1);
            result += content.at(tloc.line + 1);
            result += "\n";
        }
    }
    return result;
}
