#include "frontend.hpp"

#include "fs.hpp"

void frontend::make_tmp_folder()
{
    fs::create_folder("tmp");
}

bool frontend::find_compiler()
{
    #ifdef __unix__
    this->has_gcc = false;
    this->has_clang = false;
    
    // This is uterly terrible
    if(system("which gcc > /dev/null 2>&1") == 0)
    {
        this->has_gcc = true;
    }
    if(system("which clang > /dev/null 2>&1") == 0)
    {
        this->has_clang = true;
    }

    if(this->has_gcc || this->has_clang)
    {
        return true;
    }
    else{
        return false;
    }
    #else
    return false;
    #endif 
}

void frontend::remove_tmp_folder()
{
    fs::remove_folder("tmp");   
}