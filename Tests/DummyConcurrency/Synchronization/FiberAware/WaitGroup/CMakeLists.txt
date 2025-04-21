begin_test_group(FiberSynchronizationWaitGroup)

add_test(Unit.cpp Debug)
add_test(Unit.cpp DebugASan)

add_test(Random.cpp Matrix)

add_test(Stress.cpp Release)
add_test(Stress.cpp FaultyThreadASan)
add_test(Stress.cpp FaultyThreadTSan)
add_test(Stress.cpp Fiber)

add_test(StorageRandom.cpp Matrix)

add_test(StorageStress.cpp FaultyThreadASan)

end_test_group()