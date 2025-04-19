begin_test_group(DataStructuresAtomicSharedPtr)

add_test(Unit.cpp DebugASan)

add_test(Random.cpp Matrix)

add_test(Stress.cpp Release)
add_test(Stress.cpp FaultyThreadASan)
add_test(Stress.cpp FaultyThreadTSan)

end_test_group()
