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
    GIT_TAG 423895e348f296b8f60db6be83b21d1b08d183d7)
  FetchContent_MakeAvailable(twist_all)
endif()

# --------------------------------------------------------------------
