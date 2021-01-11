begin_test_group(ThreadSynchronizationWaitGroup)

add_test(Unit.cpp Debug)

add_test(Random.cpp Matrix)
add_test(Random.cpp FaultyThreadASan)
add_test(Random.cpp FaultyThreadTSan)

add_test(Model.cpp Matrix)

add_test(Futex.cpp Matrix)

add_test(Storage.cpp Matrix)
add_test(Storage.cpp FaultyThreadASan)

end_test_group()