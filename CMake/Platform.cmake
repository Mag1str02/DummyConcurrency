if(NOT DEFINED UNIX)
    set(UNIX false)
endif()
if(NOT DEFINED APPLE)
    set(APPLE false)
endif()
if(NOT DEFINED WINDOWS)
    set(WINDOWS false)
endif()

set(LINUX false)
if(DEFINED UNIX AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX true)
endif()
if(WIN32)
    set(WINDOWS true)
endif()


