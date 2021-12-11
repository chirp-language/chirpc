#include "diagnostic.hpp"
#include "color.hpp"
#include "system.hpp"
#include "location_provider.hpp"
#include <iostream>
#include <charconv>

static inline std::string get_spacing(int l, char s = '|')
{
    std::string og = "    | ";
    og.at(4) = s;
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

void diagnostic_manager::show(diagnostic const& d)
{
    if (d.type.is_error())
        error = true;

    if (d.type.is_warning())
    {
        print_color("[WARNING] ", has_color, os, color::red | color::green | color::bright | color::bold);
    }
    else if (d.type.is_error())
    {
        print_color("[ERROR] ", has_color, os, color::red | color::bright | color::bold);
    }
    else if (d.type.is_note())
    {
        print_color("[NOTE] ", has_color, os, color::green | color::blue | color::bright | color::bold);
    }

    os << d.msg;
    os << '\n';
    
    if (d.type.has_location())
    {
        if (loc_prov && current_source)
        {
            location const& tloc = loc_prov->get_loc(d.loc.begin);
            location tloce = loc_prov->get_loc(d.loc.end);
            if (tloce.line != tloc.line)
            {
                tloce = tloc;
                tloce.len = current_source->at(tloce.line).size() - tloce.start;
            }
            os << "In ";
            os << loc_prov->print_loc(d.loc);

            os << '\n';
            
            if (tloc.line - 1 >= 0 && is_important(current_source->at(tloc.line + 1)))
            {
                //os << "    | ";
                os << get_spacing(tloc.line);
                os << replace_tabs(current_source->at(tloc.line - 1));
                os << "\n    | \n";
            }

            print_color(get_spacing(tloc.line + 1, '>'), has_color, os, color::red | color::green | color::bright | color::bold);
            os << replace_tabs(current_source->at(tloc.line));
            os << '\n';

            {
                auto const& line = current_source->at(tloc.line);
                std::string indentation;
                os << "    | ";

                for (int i = 0; i < tloc.start; i++)
                {
                    if (line[i] == '\t')
                        indentation += "    ";
                    else
                        indentation += ' ';
                }
                for (int i = 0; i < tloce.len + tloce.start - tloc.start; i++)
                {
                    if (line[tloc.start + i] == '\t')
                        indentation += "^^^^";
                    else
                        indentation += '^';
                }
                print_color(indentation, has_color, os, color::green | color::bright);
                os << '\n';
            }
            if (tloc.line + 1 < current_source->size() && is_important(current_source->at(tloc.line + 1)))
            {
                //os << "    | ";
                os << get_spacing(tloc.line + 2);
                os << replace_tabs(current_source->at(tloc.line + 1));
                os << '\n';
            }
        }
        else
        {
            os << "Location unavailable\n";
        }
    }
}

#ifdef __CHIRP_UNREACHABLE_AVAILABLE
[[noreturn]]void __chirp_unreachable(char const* message)
{
    std::cerr << "\nUnreachable code has been reached: " << message << '\n';
    std::abort();
}
#endif
