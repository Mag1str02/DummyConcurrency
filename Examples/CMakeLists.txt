macro(add_example EXAMPLE_NAME)
    add_executable(Example${EXAMPLE_NAME} ${EXAMPLE_NAME}.cpp)
    target_link_libraries(Example${EXAMPLE_NAME} PUBLIC DummyConcurrency)
endmacro(add_example)

add_example(BasicFibers)
add_example(FiberAwareSynchornization)
add_example(FutexWait)
add_example(ThreadPool)
