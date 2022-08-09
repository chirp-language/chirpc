#include "os.hpp"

#include <cstdio>
#include <vector>

#if defined(CHIRP_PLATFORM_UNIX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>

int proc_exec(char const* const argv[], file_open_descriptor const descs[], bool use_path)
{
    pid_t child_pid = fork();
    if (child_pid == -1)
    {
        std::perror("Warning: fork failed");
        return -1;
    }
    else if (child_pid == 0)
    {
        std::vector<int> descriptors;
        int nullfd = -1, maxfd = 0, max_used_fd;
        for (auto* descriptor = descs; descriptor->target_fd != -1; ++descriptor)
        {
            if (descriptor->target_fd >= 0)
                maxfd = std::max(maxfd, descriptor->target_fd);
        }
        max_used_fd = maxfd;
        for (auto* descriptor = descs; descriptor->target_fd != -1; ++descriptor)
        {
            if (descriptor->target_fd >= descriptors.size())
                descriptors.resize(descriptor->target_fd + 1, OS_FILE_CLOSED);
            int fd = descriptor->source_fd;
            if (fd == OS_FILE_DEVNULL)
            {
                if (nullfd == -1)
                {
                    nullfd = open("/dev/null", O_RDWR);
                    if (nullfd != -1)
                    {
                        dup2(nullfd, ++maxfd);
                        nullfd = maxfd;
                    }
                }
                fd = nullfd;
            }
            descriptors[descriptor->target_fd] = fd;
        }
        // TODO: Logic for keeping track of fd clobbers is too complicated for now
        int fd_dst = 0;
        for (int& fd_src : descriptors)
        {
            if (fd_src == OS_FILE_CLOSED)
                close(fd_dst);
            else
                dup2(fd_src, fd_dst);
            ++fd_dst;
        }

        // Taken from oracle website
        struct rlimit rl;
        int i;
        getrlimit(RLIMIT_NOFILE, &rl);
        for (i = max_used_fd; i < rl.rlim_max; i++)
            (void) close(i);

        if (use_path)
            execvp(argv[0], const_cast<char**>(argv));
        else
            execv(argv[0], const_cast<char**>(argv));
        chirp_unreachable("Process not executed");
    }

    int status;
    if (waitpid(child_pid, &status, 0) == -1)
    {
        std::perror("Warning: wait failed");
        return -2;
    }
    return WEXITSTATUS(status);
}
#elif defined(CHIRP_PLATFORM_WINNT)
#include <Windows.h>

#include <cstring>
#include <string>

// Adapted from https://stackoverflow.com/questions/2611044/process-start-pass-html-code-to-exe-as-argument/2611075#2611075
static void EscapeBackslashes(std::string& sb, char const* s, char const* begin)
{
    // Backslashes must be escaped if and only if they precede a double quote.
    while (*s == '\\')
    {
        sb += '\\';
        --s;

        if (s == begin)
            break;
    }
}

static std::string ArgvToCommandLine(char const* const args[])
{
    std::string sb;
    for (auto* s = *args; s; ++args)
    {
        auto* const sbeg = s;
        size_t s_len = std::strlen(s);
        auto* const send = s + s_len;
        sb += '"';
        // Escape double quotes (") and backslashes (\).
        while (true)
        {
            // Put this test first to support zero length strings.
            if (s >= send)
                break;

            auto* quote = std::strchr(s, '"');
            if (quote == nullptr)
                break;

            sb.append(s, quote);
            EscapeBackslashes(sb, quote - 1, s);
            sb += "\\\"";
            s = quote + 1;
        }
        sb.append(s, send);
        EscapeBackslashes(sb, send - 1, sbeg);
        sb += "\" ";
    }
    return sb;
}

int proc_exec(char const* const argv[], file_open_descriptor const descs[], bool use_path)
{
    HANDLE handles[3] {};
    for (auto* descriptor = descs; descriptor->target_fd != -1; ++descriptor)
    {
        if (descriptor->target_fd > 2)
        {
            std::fputs("Warning: Windows process exec target fd > 2\n", stderr);
            return -1;
        }

        HANDLE handle = nullptr;
        switch (descriptor->source_fd)
        {
            case OS_FILE_CLOSED:
            case OS_FILE_DEVNULL:
                break;

            case 0:
                handle = GetStdHandle(STD_INPUT_HANDLE);
                break;
            case 1:
                handle = GetStdHandle(STD_OUTPUT_HANDLE);
                break;
            case 2:
                handle = GetStdHandle(STD_ERROR_HANDLE);
                break;
            default:
                std::fputs("Warning: Windows process exec source fd > 2\n", stderr);
                return -1;
        }
        handles[descriptor->target_fd] = handle;
    }
    std::string cmd_line = ArgvToCommandLine(argv);
    STARTUPINFOA startup_info{};
    startup_info.cb = sizeof startup_info;
    startup_info.dwFlags = STARTF_USESTDHANDLES;
    startup_info.hStdInput = handles[0];
    startup_info.hStdOutput = handles[1];
    startup_info.hStdError = handles[2];

    PROCESS_INFORMATION proc_info{};

    if (!CreateProcessA(argv[0], cmd_line.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr, &start_info, &proc_info))
    {
        return -1;
    }

    // Wait until child process exits.
    WaitForSingleObject(proc_info.hProcess, INFINITE);

    int exit_code = -2;
    GetExitCodeProcess(proc_info.hProcess, &exit_code);

    // Close process and thread handles. 
    CloseHandle(proc_info.hProcess);
    CloseHandle(proc_info.hThread);

    return exit_code;
}
#else
#error Unknown platform
#endif
