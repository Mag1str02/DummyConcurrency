#pragma once

#if __has_feature(address_sanitizer)

#include <DummyConcurrency/Context/Sanitizer/Address/Context.hpp>

#elif __has_feature(thread_sanitizer)

#include <DummyConcurrency/Context/Sanitizer/Thread/Context.hpp>

#else

#include <DummyConcurrency/Context/Sanitizer/Nop/Context.hpp>

#endif
