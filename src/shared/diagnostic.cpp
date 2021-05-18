#include "diagnostic.hpp"
#include "../color.hpp"
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
        if (has_color)
        {
            os << apply_color("[WARNING] ", color::red | color::green | color::bright | color::bold);
        }
        else
        {
            os << "[WARNING] ";
        }
    }
    else if (d.type.is_error())
    {
        if (has_color)
        {
            os << apply_color("[ERROR] ", color::red | color::bright | color::bold);
        }
        else
        {
            os << "[ERROR] ";
        }
    }
    else if (d.type.is_note())
    {
        if (has_color)
        {
            os << apply_color("[NOTE] ", color::green | color::blue | color::bright | color::bold);
        }
        else
        {
            os << "[NOTE] ";
        }
    }

    os << d.msg;
    os << '\n';
    
    if (d.type.has_location())
    {
        if (loc_prov && current_source)
        {
            location const& tloc = loc_prov->get_loc(d.l.begin);
            location tloce = loc_prov->get_loc(d.l.end);
            if (tloce.line != tloc.line)
            {
                tloce = tloc;
                tloce.len = current_source->at(tloce.line).size() - tloce.start;
            }
            os << "In ";
            os << loc_prov->print_loc(d.l);

            os << '\n';
            
            if (tloc.line - 1 >= 0 && is_important(current_source->at(tloc.line + 1)))
            {
                //os << "    | ";
                os << get_spacing(tloc.line);
                os << replace_tabs(current_source->at(tloc.line - 1));
                os << "\n    | \n";
            }

            if (has_color)
            {
                os << apply_color(get_spacing(tloc.line + 1, '>'), color::red | color::green | color::bright | color::bold);
            }
            else
            {
                os << get_spacing(tloc.line + 1, '>');
            }
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
                if (has_color)
                {
                    os << apply_color(std::move(indentation), color::green | color::bright);
                }
                else
                {
                    os << indentation;
                }
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
