#pragma once

#define TK_MAKE_NONMOVABLE(ClassName) \
    private: \
        ClassName(ClassName&&) = delete; \
        ClassName& operator=(ClassName&&) = delete;
