set_option(DC_FORCE_BUILD_PRESET_OPTIONS "Forces build preset options" ON)
set_option(DC_EXCEPTIONS_CONTEXT "Enables support for exceptions" OFF)
set_option(DC_USE_UCONTEXT "Use ucontext" OFF)
set_option(DC_ENABLE_EXAMPLES "Enable example targets" ON)
set_option(DC_ENABLE_TESTS "Enable test targets" OFF)
set_option(DC_USE_TWIST "Build library on top of twist" OFF)

if(NOT DC_USE_TWIST AND DC_ENABLE_TESTS)
  message(SEND_ERROR "Can not enable tests without twist")
endif()

if(DC_BUILD_TYPE STREQUAL "Debug")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" Debug)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" ON)
  set_build_option(ASAN "Enables address sanitizer" OFF)
  set_build_option(TSAN "Enables thread sanitizer" OFF)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" OFF)
  set_build_option(TWIST_SIM "Enables twist simulation" OFF)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" OFF)
elseif(DC_BUILD_TYPE STREQUAL "DebugASan")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" Debug)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" OFF)
  set_build_option(ASAN "Enables address sanitizer" ON)
  set_build_option(TSAN "Enables thread sanitizer" OFF)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" OFF)
  set_build_option(TWIST_SIM "Enables twist simulation" OFF)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" OFF)
elseif(DC_BUILD_TYPE STREQUAL "Release")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" ReleaseWithDebInfo)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" OFF)
  set_build_option(ASAN "Enables address sanitizer" OFF)
  set_build_option(TSAN "Enables thread sanitizer" OFF)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" OFF)
  set_build_option(TWIST_SIM "Enables twist simulation" OFF)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" OFF)
elseif(DC_BUILD_TYPE STREQUAL "FaultyThreadASan")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" Debug)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" OFF)
  set_build_option(ASAN "Enables address sanitizer" ON)
  set_build_option(TSAN "Enables thread sanitizer" OFF)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" ON)
  set_build_option(TWIST_SIM "Enables twist simulation" OFF)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" OFF)
elseif(DC_BUILD_TYPE STREQUAL "FaultyThreadTSan")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" Debug)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" OFF)
  set_build_option(ASAN "Enables address sanitizer" OFF)
  set_build_option(TSAN "Enables thread sanitizer" ON)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" ON)
  set_build_option(TWIST_SIM "Enables twist simulation" OFF)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" OFF)
elseif(DC_BUILD_TYPE STREQUAL "Fiber")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" ReleaseWithDebInfo)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" OFF)
  set_build_option(ASAN "Enables address sanitizer" OFF)
  set_build_option(TSAN "Enables thread sanitizer" OFF)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" ON)
  set_build_option(TWIST_SIM "Enables twist simulation" ON)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" OFF)
elseif(DC_BUILD_TYPE STREQUAL "FiberASan")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" ReleaseWithDebInfo)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" OFF)
  set_build_option(ASAN "Enables address sanitizer" ON)
  set_build_option(TSAN "Enables thread sanitizer" OFF)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" ON)
  set_build_option(TWIST_SIM "Enables twist simulation" ON)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" OFF)
elseif(DC_BUILD_TYPE STREQUAL "Matrix")
  set_build_option(CMAKE_BUILD_TYPE "CMake build type" ReleaseWithDebInfo)
  set_build_option(UBSAN "Enables undefined behaviour sanitizer" OFF)
  set_build_option(ASAN "Enables address sanitizer" OFF)
  set_build_option(TSAN "Enables thread sanitizer" OFF)
  set_build_option(TWIST_FAULTY "Enables twist fault injection" ON)
  set_build_option(TWIST_SIM "Enables twist simulation" ON)
  set_build_option(TWIST_SIM_ISOLATION "Enables twist simulation isolation" ON)
else()
  message(FATAL_ERROR "Unknown build type: [${DC_BUILD_TYPE}]")
endif()
