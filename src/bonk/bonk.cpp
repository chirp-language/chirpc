#include "bonk.hpp"

#include <vector>
#include <iostream>
#include <cstring>
#include <charconv>

#include "../frontend/fs.hpp"
#include "../lexer/lexer.hpp"

using token_iterator = std::vector<std::string_view>::const_iterator;

static bonk::list parse_list(token_iterator& words, token_iterator const words_end);

static bonk::handle parse_field(token_iterator& words, token_iterator const words_end, std::string& name)
{
    bonk::value_type t = bonk::error_t;

    if (words + 2 >= words_end)
        return nullptr;

    if (*words == "int")
    {
        t = bonk::int_t;
    }
    else if (*words == "str")
    {
        t = bonk::string_t;
    }
    else if (*words == "bool")
    {
        t = bonk::bool_t;
    }
    else if (*words == "list")
    {
        t = bonk::list_t;
    }
    else
    {
        return nullptr;
    }
    ++words;

    name = *words;
    ++words;

    if (t == bonk::list_t)
        return parse_list(words, words_end);

    if (t == bonk::int_t)
    {
        long val;
        auto res = std::from_chars(words->begin(), words->end(), val, 10);
        if (res.ec != std::errc())
            return nullptr;
        return bonk::new_int(val);
    }

    if (t == bonk::string_t)
    {
        auto string_start = words;
        while (words != words_end and *words != "\n")
            ++words;
        if (words == words_end)
            return nullptr;
        --words;
        return bonk::new_string(std::string_view(string_start->data(), words->data() + words->size() - string_start->data()));
    }

    if (t == bonk::bool_t)
    {
        if (*words == "true")
            return bonk::get_true();
        else if (*words == "false")
            return bonk::get_false();
        else
            return nullptr;
    }

    return nullptr;
}

bonk::list parse_list(token_iterator& words, token_iterator const words_end)
{
    std::map<std::string, bonk::handle> map;
    
    while (words < words_end)
    {
        while (words < words_end && *words == "\n")
        {
            ++words;
        }

        if (words >= words_end)
            break;

        if (*words == "end")
        {
            break;
        }
        else
        {
            std::cout << ">" << *words << "<" << std::endl;

            std::string name;
            bonk::handle v = parse_field(words, words_end, name);
            if (!v)
                chirp_unreachable("Parse error");
            map.insert_or_assign(std::move(name), std::move(v));
        }
        ++words;
    }

    return bonk::new_list_from_map(std::move(map));
}

bonk::list bonk::parse_file(std::string filename)
{
    std::string source;
    if (int res = read_file_to_string(filename.c_str(), source); res < 0)
    {
        std::cerr << "BONK CANT OPEN FILE \"" << filename << "\" Reason: " << std::strerror(res) << std::endl;
        return nullptr;
    }

    std::vector<std::string_view> tokens;
    size_t tok_beg = 0;
    for (size_t pos = 0; pos != source.size(); )
    {
        static const char nltok = '\n';

        char const* ch = source.data() + pos;
        if (*ch == '#')
        {
            if (tok_beg != pos)
            {
                tokens.push_back(std::string_view(source.data() + tok_beg, pos - tok_beg));
            }
            ++pos;
            ++ch;
            while (pos < source.size() and *ch != '\n' and *ch != '\r')
            {
                ++pos;
                ++ch;
            }
            // Skip "\r\n"
            if (ch[-1] == '\r' and ch[0] == '\n')
                ++pos;

            tokens.push_back(std::string_view(&nltok, 1));
            tok_beg = pos;
            continue;
        }

        if (chirp_isspace(*ch))
        {
            if (tok_beg != pos)
            {
                tokens.push_back(std::string_view(source.data() + tok_beg, pos - tok_beg));
            }

            do
            {
                if (*ch == '\n')
                   tokens.push_back(std::string_view(&nltok, 1));
                ++pos;
                ++ch;
            } while (pos < source.size() and chirp_isspace(*ch));
            tok_beg = pos;
            continue;
        }

        ++pos;
    }

    int lcount = 0;
    std::map<std::string, bonk::handle> map;
    for (auto it = tokens.cbegin(), end = tokens.cend(); it != end; )
    {
        if (*it == "\n")
        {
            ++lcount;
            ++it;
            continue;
        }

        std::string name;
        bonk::handle v = parse_field(it, end, name);
        if (!v)
            chirp_unreachable("Parse error");
        //std::cout << name << std::endl;
        map.insert_or_assign(std::move(name), std::move(v));
        ++it;
    }

    return bonk::new_list_from_map(std::move(map));
}

static void serialize_value(std::string& txt, std::string_view name, const bonk::value& v)
{
    switch (v.type)
    {
        case bonk::error_t:
        default:
            txt += "# error\n";
            break;
        case bonk::none_t:
            txt += "# none\n";
            break;
        case bonk::list_t:
        {
            ((txt += "list ") += name) += "\n";

            auto& lst = static_cast<const bonk::list_value&>(v);
            auto values = lst.elements();
            auto names = lst.names();

            name = "-";
            for (size_t index = 0; index != lst.length; ++index)
            {
                if (names.data)
                    name = names.data[index]->data();
                serialize_value(txt, name, *values.data[index]);
            }

            txt += "end\n";
            break;
        }
        case bonk::int_t:
            ((((txt += "int ") += name) += " ") += std::to_string(static_cast<const bonk::int_value&>(v).value)) += "\n";
            break;
        case bonk::string_t:
            ((((txt += "str ") += name) += " ") += static_cast<const bonk::string_value&>(v).data()) += "\n";
            break;
        case bonk::bool_t:
            (((txt += "bool ") += name) += (static_cast<const bonk::bool_value&>(v).value ? " true" : " false")) += "\n";
            break;
    }
}

std::string bonk::serialize(const std::string& name, const bonk::value& v)
{
    std::string txt;
    txt.reserve(32);
    serialize_value(txt, name, v);
    return txt;
}
