project_log("FetchContent: wheels")

FetchContent_Declare(
        wheels
        GIT_REPOSITORY https://gitlab.com/Lipovsky/wheels.git
        GIT_TAG 2aa145331ba280796cbb6d5f8ee47b8f06d96642
)
FetchContent_MakeAvailable(wheels)

include(Test.cmake)

setup_tests()

add_subdirectory(TestingLibrary)
add_subdirectory(DummyConcurrency)