#include "frontend.hpp"

#include "fs.hpp"
#include "os.hpp"

void frontend::make_tmp_folder()
{
    fs::create_folder("tmp");
}

bool frontend::find_compiler()
{
    this->has_gcc = false;
    this->has_clang = false;

    #if defined(CHIRP_PLATFORM_UNIX)
    // This is slightly less utterly terrible
    char const* cmd[] = {"which", /* prog */nullptr, nullptr};
    file_open_descriptor descs[] = {
        {0, OS_FILE_DEVNULL},
        {1, OS_FILE_DEVNULL},
        {2, OS_FILE_DEVNULL},
        {-1, -1}
    };
    cmd[1] = "gcc";
    if (proc_exec(cmd, descs) == 0)
    {
        this->has_gcc = true;
        return true;
    }
    cmd[1] = "clang";
    if (proc_exec(cmd, descs) == 0)
    {
        this->has_clang = true;
        return true;
    }
    #elif defined(CHIRP_PLATFORM_WINNT)
    // This is slightly less utterly terrible
    char const* cmd[] = {"where", /* prog */nullptr, nullptr};
    file_open_descriptor descs[] = {
        {0, OS_FILE_DEVNULL},
        {1, OS_FILE_DEVNULL},
        {2, OS_FILE_DEVNULL},
        {-1, -1}
    };
    cmd[1] = "gcc";
    if (proc_exec(cmd, descs) == 0)
    {
        this->has_gcc = true;
        return true;
    }
    cmd[1] = "clang";
    if (proc_exec(cmd, descs) == 0)
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
