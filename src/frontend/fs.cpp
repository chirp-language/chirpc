#include "fs.hpp"

#if defined(_WIN64) || defined(_WIN32) || defined(__WINDOWS__)
#define CHIRP_PLATFORM_WINDOWS
#elif defined(__linux) || defined(linux) || defined(__linux__)
#define CHIRP_PLATFORM_LINUX
#elif defined(__DragonFly__) || defined(__FreeBSD__) || \
    defined(__NETBSD__) || defined(__OpenBSD__)
#define CHIRP_PLATFORM_BSD
#elif defined(__APPLE__) || defined(macintosh) || defined(__MACH__)
#define CHIRP_PLATFORM_OSX
#elif defined(__unix) || defined(unix)
#define CHIRP_PLATFORM_UNIX
#else
#define CHIRP_PLATFORM_UNKNOWN
#endif

#if defined(CHIRP_PLATFORM_LINUX) || defined(CHIRP_PLATFORM_BSD) || \
    defined(CHIRP_PLATFORM_OSX) || defined(CHIRP_PLATFORM_UNIX)
#define CHIRP_PLATFORM_POSIX
#endif

#include <fstream>
#include <ftw.h>
#include <sys/stat.h>

namespace fs
{
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    bool create_folder(std::string name)
    {
        // On any system with <sys/stat.h> this should be good
        #if defined(CHIRP_PLATFORM_POSIX)
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
    int delete_file(char const* path, struct stat const* stat, int flag, struct FTW* ftw)
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