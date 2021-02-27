/*
Manages the frontend tooling, which mean finding/using the backend compiler, creating 
temporary files and stuff like that
*/
#pragma once

#include <string>

class frontend
{
    public:
    void make_tmp_folder();

    // Returns true if a supported compiler is found, otherwise false
    bool find_compiler();

    // (Filename, Content)
    // Note: The filename must be without filename
    void write_out(std::string,std::string);

    void remove_tmp_folder();
    private:
    bool has_gcc;
    bool has_clang;
};