#include <cstdio>
#include <memory>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#define popen _popen
#define pclose _pclose
#endif

struct PopenDeleter
{
    void operator()(FILE* p) const
    {
        if (p)
        {
            pclose(p);
        }
    }
};

using PopenPtr = std::unique_ptr<FILE, PopenDeleter>;

PopenPtr popen2(const char* command, const char* type)
{

    FILE* pipe = popen(command, type);
    if (!pipe)
    {
        std::cerr << "popen() failed!" << "\n";
        return nullptr;
    }
    return PopenPtr(pipe);
}