#include "bonk.hpp"

#include <fstream>
#include <vector>
#include <iostream>

bonk::value parse_list(int& i, std::vector<std::string>& words);

bonk::value parse_field(int& i, std::vector<std::string>& words, std::string& name)
{
    int t = 0;
    if(words.at(i) == "int")
    {
        t = 3;
    }
    else if(words.at(i) == "str")
    {
        t = 4;
    }
    else if(words.at(i) == "bool")
    {
        t = 5;
    }
    else if(words.at(i) == "list")
    {
        t = 2;
    }
    else if(words.at(i) == "end")
    {
        // not supposed to happen
        t = 1;
    }

    i++;

    if(t != 1)
    {
        name = words.at(i);
    }

    i++;

    if(t ==  2)
    {
        return parse_list(i, words);
    }
    else
    {
        void* value;

        if(t == 3)
        {
            value = new int;
            *static_cast<int*>(value) = std::stoi(words.at(i));
        }
        else if(t == 4)
        {
            value = new std::string;
            *static_cast<std::string*>(value) = words.at(i);
        }
        else if(t == 5)
        {
            value = new bool;

            if(words.at(i) == "true")
            {
                *static_cast<bool*>(value) = true;
            }
            else
            {
                *static_cast<bool*>(value) = false;
            }
        }
        return bonk::value(t, value);
    }
}

bonk::value parse_list(int& i, std::vector<std::string>& words)
{
    std::map<std::string, bonk::value>* value = new std::map<std::string, bonk::value>;
    bonk::value list(2, value);
    
    while(i < words.size())
    {
        while(i < words.size() && words.at(i).at(0) == '\n')
        {
            i++;
        }

        if(i >= words.size())
        {
            break;
        }

        if(words.at(i) == "end")
        {
            break;
        }
        else
        {
            std::string name;
            bonk::value v = parse_field(i, words, name);
            value->insert(std::pair<std::string,bonk::value>(name,v));
        }
        i++;
    }
    return list;
}

std::map<std::string, bonk::value> bonk::parse_file(std::string filename)
{
    std::map<std::string, bonk::value> map;

    std::ifstream file(filename);

    if(!file)
    {
        std::cout<<"BONK CANT OPEN FILE "<<filename<<std::endl;
    }

    std::string line;
    std::vector<std::string> words;
    while (std::getline(file, line))
    {

        std::string word;
        for(char c : line)
        {
            if(line.at(0) == '#'){}
            else if(!isspace(c)  && c != '\n')
            {
                word += c;
            }
            else
            {
                if(!word.empty())
                {
                    words.push_back(word);
                }
                word.clear();
            }
        }

        if(!word.empty())
        {
            words.push_back(word);
            word.clear();
        }

        words.push_back("\n");
    }
    file.close();

    int i = 0;
    int lcount = 0;
    while(i < words.size())
    {
        while(i < words.size() && words.at(i).at(0) == '\n')
        {
            lcount++;
            i++;
        }

        if(i >= words.size())
        {
            break;
        }

        std::string name;
        bonk::value v = parse_field(i, words, name);
        //std::cout<<name<<std::endl;
        map[name] = v;

        i++;
    }

    return map;
}

std::string bonk::to_string(const std::string& name,const bonk::value& v)
{
    std::string txt;

    if(v.type == 0)
    {
        txt = "# error";
    }
    else if(v.type == 1)

    {
        txt = "# none";
    }
    else if(v.type == 2)
    {
        std::map<std::string, bonk::value>* m = static_cast<std::map<std::string, bonk::value>*>(v.data);

        txt = "list " +  name + "\n";

        for(const std::pair<std::string, bonk::value>& x : *m)
        {
            txt += bonk::to_string(x.first, x.second) + "\n";
        }

        txt += "end";
    }
    else if(v.type == 3)
    {
        txt += "int " +  name + " " + std::to_string(*static_cast<int*>(v.data));
    }
    else if(v.type == 4)
    {
        txt += "str " +  name + " " + *static_cast<std::string*>(v.data);
    }
    else if(v.type == 5)
    {
        txt += "bool " +  name + " " + std::to_string(*static_cast<bool*>(v.data));
    }

    return txt;
}