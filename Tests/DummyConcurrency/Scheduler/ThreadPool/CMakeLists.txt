begin_test_group(SchedulerThreadPool)

add_test(Unit.cpp Debug)

add_test(Random.cpp Matrix)

add_test(Stress.cpp Fiber)
add_test(Stress.cpp FaultyThreadASan)
add_test(Stress.cpp FaultyThreadTSan)

end_test_group()