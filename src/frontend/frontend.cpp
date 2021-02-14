#include "frontend.hpp"

#include "fs.hpp"

void frontend::make_tmp_folder()
{
    fs::create_folder("tmp");
}

bool frontend::find_compiler()
{
    return false;
}

void frontend::remove_tmp_folder()
{
    fs::remove_folder("tmp");   
}