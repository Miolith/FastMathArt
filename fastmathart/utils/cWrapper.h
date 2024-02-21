#include <cstdio>
#include <memory>
#include <iostream>

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