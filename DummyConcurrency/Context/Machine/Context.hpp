#pragma once

#if defined(DC_MACHINE_CONTEXT_IMPLEMENTATION_x86_64_win)

#include <DummyConcurrency/Context/Machine/x86_64_win/Context.hpp>

#elif defined(DC_MACHINE_CONTEXT_IMPLEMENTATION_x86_64_unix)

#include <DummyConcurrency/Context/Machine/x86_64_unix/Context.hpp>

#elif defined(DC_MACHINE_CONTEXT_IMPLEMENTATION_armv8_a_64)

#include <DummyConcurrency/Context/Machine/armv8_a_64/Context.hpp>

#elif defined(DC_MACHINE_CONTEXT_IMPLEMENTATION_ucontext)

#include <DummyConcurrency/Context/Machine/ucontext/Context.hpp>

#else

#include <DummyConcurrency/Context/Machine/nop/Context.hpp>

#endif
