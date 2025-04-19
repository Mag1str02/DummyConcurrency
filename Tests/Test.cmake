macro(setup_tests)
    add_executable(RunAllTests DummyConcurrency/RunTests.cpp)
    message (STATUS "Setting up tests for [${DC_BUILD_TYPE}] build type")
    target_link_libraries(RunAllTests 
        PUBLIC wheels 
        PUBLIC DummyConcurrency
        PUBLIC Testing
    )
    target_include_directories(RunAllTests 
        PUBLIC ${wheels_SOURCE_DIR}/source
    )
endmacro(setup_tests)

macro(begin_test_group GROUP_NAME)
    set(CURRENT_TEST_GROUP Test${GROUP_NAME})
    add_executable(${CURRENT_TEST_GROUP} ${CMAKE_SOURCE_DIR}/Tests/DummyConcurrency/RunTests.cpp)
    target_link_libraries(${CURRENT_TEST_GROUP} 
        PUBLIC wheels 
        PUBLIC DummyConcurrency
        PUBLIC Testing
    )
    target_include_directories(${CURRENT_TEST_GROUP} 
        PUBLIC ${wheels_SOURCE_DIR}/source
    )
endmacro(begin_test_group)
macro(end_test_group)
    unset(CURRENT_TEST_GROUP)
endmacro(end_test_group)


macro(add_test FILE BUILD_TYPE)
    if(NOT DEFINED CURRENT_TEST_GROUP)
        message(FATAL_ERROR "No current test group")
    endif()

    if ("${BUILD_TYPE}" STREQUAL DC_BUILD_TYPE)
        target_sources(RunAllTests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/${FILE})
        target_sources(${CURRENT_TEST_GROUP} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/${FILE})
    endif()
endmacro(add_test)