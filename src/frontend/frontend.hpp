/*
Manages the frontend tooling, which mean finding/using the backend compiler, creating 
temporary files and stuff like that
*/
#pragma once

class frontend
{
    public:
    void make_tmp_folder();

    // Returns true if a supported compiler is found, otherwise false
    bool find_compiler();

    void remove_tmp_folder();
};