#include "fs.hpp"

#include <fstream>

namespace fs
{
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    bool create_folder(std::string name){
        #ifdef __linux__
        std::string cmd = "mkdir ";
        cmd += name;
        system(cmd.c_str());
        #elif 
        system("echo Cannot create folder on this platform.");
        #endif 
        return true;
    }

    bool write_file(std::string fn, std::string c)
    {
        std::ofstream f(fn);
        f << c;
        f.close();
        return true;
    }

    bool remove_folder(std::string name){
        #ifdef __linux__
        std::string cmd = "rm -r ";
        cmd += name;
        system(cmd.c_str());
        #elif
        system("echo Error idk it's almost 3AM and I can't remove that folder");
        #endif
        return true;
    }
}