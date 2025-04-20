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
    GIT_TAG f0884f5adb58c5eb54410af0c4decd42a8742fb0)
  FetchContent_MakeAvailable(twist_all)
endif()

# --------------------------------------------------------------------
