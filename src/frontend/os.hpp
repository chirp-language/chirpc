/// \file OS interface

#pragma once

#include "../shared/system.hpp"

#define OS_FILE_CLOSED -1
#define OS_FILE_DEVNULL -2

struct file_open_descriptor
{
    int target_fd;
    int source_fd; // can be OS_FILE_... constant
};

/**
 * @param argv must be a NULL-terminated array of strings
 * @param descs must be an array of descriptors terminated with a descriptor whose target_fd is -1
 * @param use_path whether to use PATH to search for the program (if true), or to use the first argument as a file path directly (if false)
 * @return process exit code
 */
int proc_exec(char const* const argv[], file_open_descriptor const descs[], bool use_path = true);
