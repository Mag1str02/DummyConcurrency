#pragma once

#if DC_EXCEPTION_CONTEXT_IMPLEMENTATION_unix 

#include <DummyConcurrency/Context/Exceptions/unix/Context.hpp>

#else

#include <DummyConcurrency/Context/Exceptions/nop/Context.hpp>

#endif
