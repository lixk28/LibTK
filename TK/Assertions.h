#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#define __stringify_inner(x) #x
#define __stringify(x) __stringify_inner(x)

#define ASSERT(assertion) \
    do { \
        if (!(assertion)) { \
            crash("[Assert] " __FILE__ ":" __stringify(__LINE__) " " #assertion "\n"); \
        } \
    } while (0)

#define ASSERT_WITH_MSG(assertion, msg, ...) \
    do { \
        if (!(assertion)) { \
            crash("[Assert] " __FILE__ ":" __stringify(__LINE__) " " #assertion "\n" msg "\n", ##__VA_ARGS__); \
        } \
    } while (0)

#define VEIRFY(expr) \
    do { \
        if (!(expr)) { \
            crash("[Verify] " __FILE__ ":" __stringify(__LINE__) " " #expr "\n"); \
        } \
    } while (0)


#define VEIRFY_WITH_MSG(expr, msg, ...) \
    do { \
        if (!(expr)) { \
            crash("[Verify] " __FILE__ ":" __stringify(__LINE__) " " #expr "\n" msg "\n", ##__VA_ARGS__); \
        } \
    } while (0)

extern "C" {

__attribute__((noreturn)) void crash(const char* msg, ...) __attribute__((format(printf, 1, 2)));

void crash(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    vfprintf(stdout, msg, args);
    va_end(args);
    exit(1);
}

}
