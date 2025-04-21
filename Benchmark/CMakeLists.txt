add_task_benchmark(Mutex ${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp)
add_task_benchmark(ThreadPoolOverhead ${CMAKE_CURRENT_LIST_DIR}/ThreadPoolOverhead.cpp)
add_task_benchmark(ContextSwitch ${CMAKE_CURRENT_LIST_DIR}/ContextSwitch.cpp)
add_task_benchmark(FiberCreation ${CMAKE_CURRENT_LIST_DIR}/FiberCreation.cpp)