#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

namespace fs = std::filesystem;

void check_status(const fs::path& p, fs::file_status s)
{
    std::cout << p;
    switch (s.type())
    {
    case fs::file_type::none:
        std::cout << " has `not-evaluated-yet` type";
        break;
    case fs::file_type::not_found:
        std::cout << " does not exist";
        break;
    case fs::file_type::regular:
        std::cout << " is a regular file";
        break;
    case fs::file_type::directory:
        std::cout << " is a directory";
        break;
    case fs::file_type::symlink:
        std::cout << " is a symlink";
        break;
    case fs::file_type::block:
        std::cout << " is a block device";
        break;
    case fs::file_type::character:
        std::cout << " is a character device";
        break;
    case fs::file_type::fifo:
        std::cout << " is a named IPC pipe";
        break;
    case fs::file_type::socket:
        std::cout << " is a named IPC socket";
        break;
    case fs::file_type::unknown:
        std::cout << " has `unknown` type";
        break;
    default:
        std::cout << " has `implementation-defined` type";
        break;
    }
    std::cout << '\n';
}

int main(int argc, char** argv)
{
    std::string file_name;
    if (argc > 1 && argc < 3)
        file_name = argv[1];
    else if (argc < 2)
    {
        std::cout << "Error. Too little arguments" << std::endl;
        return -1;
    }

    if (argc > 3)
    {
        std::cout << "Error. Too many arguments" << std::endl;
        return -1;
    }

    check_status(file_name, fs::status(file_name));

    return EXIT_SUCCESS;
}
