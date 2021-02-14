#include "fs.hpp"

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