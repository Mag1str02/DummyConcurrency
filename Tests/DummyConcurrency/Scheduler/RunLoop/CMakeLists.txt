begin_test_group(SchedulerRunLoop)

add_test(Unit.cpp Debug)
add_test(Unit.cpp DebugASan)

add_test(Sane.cpp Matrix)

end_test_group()