#pragma once

#include <iostream>  // IWYU pragma: keep

#define DC_PANIC(msg)                  \
    do {                               \
        std::cerr << msg << std::endl; \
        std::abort();                  \
    } while (false)

#define DC_ASSERT(expr, msg) \
    do {                     \
        if (!(expr)) {       \
            DC_PANIC(msg);   \
        }                    \
    } while (false);