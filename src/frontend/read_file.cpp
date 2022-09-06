#include "fs.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include "../shared/system.hpp"

#ifdef CHIRP_PLATFORM_UNIX
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

using native_handle = int;
#else
using native_handle = std::FILE*;
#endif

static int open_file(const char* fname, native_handle& file);
static int file_size(native_handle file, size_t& size);
static int read_file_stream(native_handle file, size_t known_size, std::string& out);
static int read_file_buffer(native_handle file, size_t size, std::string& out);

int read_file_to_string(const char* fname, std::string& out)
{
    int err;
    native_handle file;
    if ((err = open_file(fname, file)))
        return err;
    size_t size;
    err = file_size(file, size);
    if (err)
    {
        err = read_file_stream(file, 0, out);
    }
    else
    {
        out.resize(size);
        err = read_file_buffer(file, size, out);
    }
    return err;
}

static int read_file_stream(native_handle file, size_t known_size, std::string& out)
{
    if (!known_size)
        known_size = 128;
    out.resize(known_size);
    size_t total_bytes_read = 0;
    while (true)
    {
        #ifdef CHIRP_PLATFORM_UNIX
        auto bytes_read = ::read(file, out.data() + total_bytes_read, known_size);
        if (bytes_read < 0)
            return -errno;
        #else
        auto bytes_read = std::fread(out.data() + total_bytes_read, 1, known_size, file);
        if (!b and std::ferror(file))
            return -errno
        #endif
        if (bytes_read == 0)
            break;
        known_size -= bytes_read;
        total_bytes_read += bytes_read;
        if (known_size < 16)
            known_size += 64;
        out.resize(total_bytes_read + known_size);
    }
    out.resize(total_bytes_read);
    return 0;
}

#ifdef CHIRP_PLATFORM_UNIX
static int open_file(const char* fname, native_handle& file)
{
    int fd = open(fname, O_RDONLY);
    if (fd < 0)
        return -errno;

    file = fd;
    return 0;
}

static int file_size(native_handle file, size_t& size)
{
    struct stat res;
    if (fstat(file, &res) < 0)
        return -errno;

    if (res.st_size == 0)
        return -EPIPE;

    size = res.st_size;
    return 0;
}

static int read_file_buffer(native_handle file, size_t size, std::string& out)
{
    // Gotta go fast!
    void* mem = ::mmap(nullptr, size, PROT_READ, MAP_PRIVATE, file, 0);
    if (mem == MAP_FAILED)
        return read_file_stream(file, size, out);
    std::memcpy(out.data(), mem, size);
    ::munmap(mem, size);
    return 0;
}
#else
static int open_file(const char* fname, native_handle& file)
{
    std::FILE* f = std::fopen(fname, "r");
    if (!f)
        return -errno;

    file = f;
    return 0;
}

static int file_size(native_handle file, size_t& size)
{
    int err = std::fseek(file, 0, SEEK_END);
    if (err)
        return err;
    size = std::ftell();
    std::rewind(file);
    return 0;
}

static int read_file_buffer(native_handle file, size_t size, std::string& out)
{
    return read_file_stream(file, size, out);
}
#endif
