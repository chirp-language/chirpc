#include "fs.hpp"
#include "../shared/system.hpp"

#include <fstream>
#include <ftw.h>
#include <sys/stat.h>

namespace fs
{
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    bool create_folder(std::string name)
    {
        // On any system with <sys/stat.h> this should be good
        #if defined(CHIRP_PLATFORM_UNIX)
        int st = mkdir(name.c_str(), 0777);
        #else
        int st = mkdir(name.c_str());
        #endif

        return !st; // a status of 0 from st is success
    }

    bool write_file(std::string fn, std::string c)
    {
        std::ofstream f(fn);
        f << c;
        f.close();
        return true;
    }

    // Delete a single file
    int delete_file(char const* path, struct stat const*, int, struct FTW*)
    {
        int st = remove(path);
        return !st; // same story as create_folder
    }

    bool remove_folder(std::string name)
    {
        // Clears all files in the folder.
        bool success = !nftw(name.c_str(), delete_file, 64, FTW_DEPTH | FTW_PHYS);
        // Delete the folder itself
        success |= remove(name.c_str());
        return success;
    }
}
