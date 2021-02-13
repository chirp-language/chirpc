#include "helper.hpp"

#include <iostream>

std::string helper::write_helper(std::vector<std::string> content)
{
    std::string result;
    if(type==global_warning||type==line_warning||type==location_warning){
        result = "[WARNING] ";
        result += this->msg;
    }
    else if(type==global_err||type==line_err||type==location_err){
        result = "[ERROR] ";
        result += this->msg;
    }
    result += "\n";
    if(type!=global_warning&&type!=global_err){
        result += "At ";
        result += l.filename;
        result += ":";
        result += std::to_string(l.line);
        if(type==location_warning||type==location_err){
            result += ":";
            result += std::to_string(l.start);
        }
        result += "\n";
        if(l.line-1>=0){
            result += "    | ";
            result += content.at(l.line-1);
            result += "\n";
        }
        result += std::to_string(l.line);
        result += " --> ";
        result += content.at(l.line);
        result +="\n";
        if(type==location_warning||type==location_err){
            std::string identation;
            for(int i=0;i<l.start+6;i++){
                identation += " ";
            }
            for(int i=0;i<=l.end-l.start;i++){
                identation += "^";
            }
            result += identation;
            result += "\n";
        }
        if(l.line+1<content.size()){
            result += "    | ";
            result += content.at(l.line+1);
            result += "\n";
        }
    }
    return result;
}