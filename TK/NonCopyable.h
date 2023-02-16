#pragma once

#define TK_MAKE_NONCOPYABLE(ClassName) \
    private: \
        ClassName(const ClassName&) = delete; \
        ClassName& operator=(const ClassName&) = delete;
