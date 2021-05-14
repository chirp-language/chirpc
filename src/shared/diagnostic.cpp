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

static inline std::string replace_tabs(std::string s)
{
    for (size_t i = 0; i != s.size(); )
    {
        if (s[i] == '\t')
        {
            s.replace(i, 1, "    ");
            i += 4;
        }
        else
            ++i;
    }
    return s;
}

static bool is_important(std::string const& line)
{
    for (char c : line)
    {
        if(!isspace(c))
            return true;
    }
    return false;
}

void diagnostic::show_output(location_provider const& prov, std::vector<std::string> const& content, cmd& options, std::ostream& os) const
{
    location const& tloc = prov.get_loc(l.begin);
    
    if (
        type == diagnostic_type::global_warning || 
        type == diagnostic_type::line_warning || 
        type == diagnostic_type::location_warning 
    ) {
        if (options.has_color)
        {
            os << apply_color("[WARNING]", color::red | color::green | color::bright | color::bold);
        }
        else
        {
            os << "[WARNING]";
        }

        os << this->msg;
    }
    else if (
        type == diagnostic_type::global_err || 
        type == diagnostic_type::line_err || 
        type == diagnostic_type::location_err )
    {
        if (options.has_color)
        {
            os << apply_color("[ERROR]", color::red | color::bright | color::bold);
        }
        else
        {
            os << "[ERROR]";
        }

        os << this->msg;
    }
    
    os << '\n';
    
    if (type != diagnostic_type::global_warning && type != diagnostic_type::global_err)
    {
        os << "In ";
        os << prov.print_loc(l);

        os << '\n';
        
        if (tloc.line - 1 >= 0 && is_important(content.at(tloc.line + 1)))
        {
            //os << "    | ";
            os << get_spacing(tloc.line - 1);
            os << replace_tabs(content.at(tloc.line - 1));
            os << "\n    | \n";
        }

        os << tloc.line;
        if (options.has_color)
        {
            os << apply_color(" --> ", color::red | color::green | color::bright | color::bold);
        }
        else
        {
            os << " --> ";
        }
        os << replace_tabs(content.at(tloc.line));
        os << '\n';

        if (type == diagnostic_type::location_warning || type == diagnostic_type::location_err)
        {
            auto const& line = content.at(tloc.line);
            std::string indentation;
            os << "    | ";

            for (int i = 0; i < tloc.start; i++)
            {
                if (line[i] == '\t')
                    indentation += "    ";
                else
                    indentation += ' ';
            }
            for (int i = 0; i < tloc.len; i++)
            {
                if (line[tloc.start + i] == '\t')
                    indentation += "^^^^";
                else
                    indentation += '^';
            }
            if (options.has_color)
            {
                os << apply_color(std::move(indentation), color::red);
            }
            else
            {
                os << indentation;
            }
            os << '\n';
        }
        else
        {
            os << "    |\n";
        }
        if (tloc.line + 1 < content.size() && is_important(content.at(tloc.line + 1)))
        {
            //os << "    | ";
            os << get_spacing(tloc.line + 1);
            os << replace_tabs(content.at(tloc.line + 1));
            os << '\n';
        }
    }
}
