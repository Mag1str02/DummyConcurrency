#pragma once

#if UNIX
#include "DummyConcurrency/ImplementationLayer/STD/Futex/Unix.hpp"  // IWYU pragma: export
#else
#include "DummyConcurrency/ImplementationLayer/STD/Futex/Windows.hpp"  // IWYU pragma: export
#endif