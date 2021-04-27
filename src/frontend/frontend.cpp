#include "frontend.hpp"

#include "fs.hpp"

void frontend::make_tmp_folder()
{
    fs::create_folder("tmp");
}

bool frontend::find_compiler()
{
    this->has_gcc = false;
    this->has_clang = false;

    #if defined(__unix__) || defined(__APPLE_CC__)
    // This is utterly terrible
    if (system("which gcc > /dev/null 2>&1") == 0)
    {
        this->has_gcc = true;
        return true;
    }
    else if (system("which clang > /dev/null 2>&1") == 0)
    {
        this->has_clang = true;
        return true;
    }
    #elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // This is utterly terrible
    if (system("where.exe gcc > nul") == 0)
    {
        this->has_gcc = true;
        return true;
    }
    else if (system("where.exe clang > nul") == 0)
    {
        this->has_clang = true;
        return true;
    }
    #endif

    return false;
}

void frontend::write_out(std::string fname, std::string content)
{
    fs::write_file("tmp/" + fname + ".c", content);
}

void frontend::remove_tmp_folder()
{
    fs::remove_folder("tmp");   
}
