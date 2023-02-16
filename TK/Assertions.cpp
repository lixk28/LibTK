#include "Assertions.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

namespace TK {

extern "C" {

void crash(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    vfprintf(stdout, msg, args);
    va_end(args);
    exit(1);
}

}

}
