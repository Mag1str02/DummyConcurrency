include(FetchContent)

# --------------------------------------------------------------------

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wno-missing-template-arg-list-after-template-kw -Wno-cast-function-type-mismatch"
)

# --------------------------------------------------------------------

if(DC_USE_TWIST OR DC_ENABLE_TESTS)
  project_log("FetchContent: twist")

  FetchContent_Declare(
    twist
    GIT_REPOSITORY https://gitlab.com/Lipovsky/twist.git
    GIT_TAG ea5501b4352b62799c019a57755cb0afea9d98c1)
  FetchContent_MakeAvailable(twist)
endif()

if(DC_ENABLE_TESTS)
  project_log("FetchContent: wheels")

  FetchContent_Declare(
    wheels
    GIT_REPOSITORY https://gitlab.com/Lipovsky/wheels.git
    GIT_TAG 2aa145331ba280796cbb6d5f8ee47b8f06d96642)
  FetchContent_MakeAvailable(wheels)
endif()

# --------------------------------------------------------------------
