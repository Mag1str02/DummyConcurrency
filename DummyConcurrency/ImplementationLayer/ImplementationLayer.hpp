#pragma once

#ifdef DC_IMPLEMENTATION_LAYER_TWIST
#include "DummyConcurrency/ImplementationLayer/Twist/Twist.hpp"  // IWYU pragma: export
#else
#include "DummyConcurrency/ImplementationLayer/STD/STD.hpp"  // IWYU pragma: export
#endif
