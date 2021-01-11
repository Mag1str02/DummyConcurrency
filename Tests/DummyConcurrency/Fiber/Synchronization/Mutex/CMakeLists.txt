begin_test_group(FiberSynchronizationMutex)

add_test(Unit.cpp Debug)
add_test(Unit.cpp DebugASan)

add_test(UnitTP.cpp Debug)
add_test(UnitTP.cpp DebugASan)

add_test(Random.cpp Matrix)

add_test(Stress.cpp Release)
add_test(Stress.cpp FaultyThreadASan)
add_test(Stress.cpp FaultyThreadTSan)
add_test(Stress.cpp Fiber)

add_test(Alloc.cpp Matrix)

add_test(StackOverflow.cpp FiberASan)
add_test(StackOverflow.cpp FaultyThreadASan)

add_test(Switch.cpp Matrix)

end_test_group()