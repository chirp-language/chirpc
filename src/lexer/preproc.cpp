#include "preproc.hpp"

#include <iostream>

// Cuts the input into individual words/locations -> will later become tokens
std::vector<location> disjoint(std::vector<std::string> content){
    std::vector<location> result; // <-- what the fuck am I supposed to call this

    size_t l = 0; // lmao good variable names
    for(std::string& line : content){
        for(int i = 0; i < line.size(); i++){
            if(isalnum(line.at(i)) != 0){
                location loc;
                loc.start = i;
                loc.line = l;
                while(i<line.size()&&isalnum(line.at(i))!=0)
                {i++;}
                i--;
                loc.end = i;
                result.push_back(loc);
            }
            else if(isspace(line.at(i)) != 0){
                location loc;
                loc.start = i;
                loc.line = l;
                while(i<line.size()&&isspace(line.at(i))!=0)
                {i++;}
                i--;
                loc.end = i;
                result.push_back(loc);
            }
            else{
                location loc;
                loc.start = i;
                loc.line = l;
                loc.end = i;
                result.push_back(loc);
            }
        }
        l++;
    }
    
    // PREPROCESSOR TOKEN DUMP
    for(location loc : result){
        std::string value;
        for(int i = loc.start; i <= loc.end; i++){
            value += content.at(loc.line).at(i);
        }
        std::cout<<value<<":"<<loc.line<<"<"<<loc.start<<"::"<<loc.end<<">"<<std::endl;
    }

    return result;
}

//TODO: Actual preprocessor 
std::vector<location> preprocess(std::vector<std::string> content){
    std::vector<location> result = disjoint(content);
    return result;
}