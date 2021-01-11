include(FetchContent)

# --------------------------------------------------------------------

# Offline mode (uncomment next line to enable)
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-template-arg-list-after-template-kw -Wno-cast-function-type-mismatch")

# --------------------------------------------------------------------

project_log("FetchContent: twist")

FetchContent_Declare(
        twist
        GIT_REPOSITORY https://gitlab.com/Lipovsky/twist.git
        GIT_TAG ea5501b4352b62799c019a57755cb0afea9d98c1
)
FetchContent_MakeAvailable(twist)

# --------------------------------------------------------------------
