set(SOURCES
        Testing/Twist/Stress.cpp
        Testing/Twist/Randomize.cpp)

if (TWIST_SIM)
    list(APPEND SOURCES
        Testing/Twist/Simulation/Result.cpp)

    if (TWIST_SIM_ISOLATION)
        list(APPEND SOURCES
                Testing/Twist/Model.cpp)
    endif()
endif()

add_library(Testing STATIC ${SOURCES})

target_include_directories(Testing PUBLIC .)
target_link_libraries(Testing twist wheels)
