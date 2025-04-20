begin_test_group(Future)

add_test(Unit.cpp DebugASan)

add_test(Stress.cpp Release)
add_test(Stress.cpp FaultyThreadASan)
add_test(Stress.cpp FaultyThreadTSan)

end_test_group()