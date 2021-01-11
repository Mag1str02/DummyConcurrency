begin_test_group(FiberScheduling)

add_test(UnitTP.cpp Debug)
add_test(UnitTP.cpp DebugASan)

add_test(UnitRL.cpp Debug)
add_test(UnitRL.cpp DebugASan)

add_test(SyncRL.cpp Matrix)

end_test_group()