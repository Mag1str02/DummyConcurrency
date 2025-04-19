cmake_minimum_required(VERSION 3.20)

project(DummyConcurrency)

include(CMake/Utils.cmake)
include(CMake/Options.cmake)
include(CMake/Logging.cmake)
include(CMake/Ccache.cmake)
include(CMake/Helpers.cmake)
include(CMake/CheckCompiler.cmake)
include(CMake/CompileOptions.cmake)
include(CMake/Sanitize.cmake)
include(CMake/Platform.cmake)
include(CMake/Development.cmake)
include(CMake/Context.cmake)

add_subdirectory(Dependencies)

# This must be included after library so that third party code is not affected
include(CMake/Werror.cmake)
include(CMake/Task.cmake)

# add_subdirectory(tasks)
add_subdirectory(DummyConcurrency)

if(DC_DEVELOPER_MODE)
  add_subdirectory(Benchmark)
  add_subdirectory(Tests)
endif()

if(DC_ENABLE_EXAMPLES)
  add_subdirectory(Examples)
endif()
