include(FetchContent)

# --------------------------------------------------------------------

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wno-missing-template-arg-list-after-template-kw -Wno-cast-function-type-mismatch"
)

# --------------------------------------------------------------------

if(DC_DEVELOPER_MODE)
  project_log("FetchContent: twist_all")

  FetchContent_Declare(
    twist_all
    GIT_REPOSITORY https://github.com/Mag1str02/Twist
    GIT_TAG bf473b6d9b45114558d345bbf73248d0c1a20fd1)
  FetchContent_MakeAvailable(twist_all)
endif()

# --------------------------------------------------------------------
