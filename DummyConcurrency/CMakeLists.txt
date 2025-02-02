include(Sources.cmake)

add_library(DummyConcurrency ${DummyConcurrencySources})

target_compile_definitions(
  DummyConcurrency
  PUBLIC DC_MACHINE_CONTEXT_IMPLEMENTATION_${DC_MACHINE_CONTEXT_IMPLEMENTATION})
target_compile_definitions(
  DummyConcurrency
  PUBLIC
    DC_EXCEPTION_CONTEXT_IMPLEMENTATION_${DC_EXCEPTION_CONTEXT_IMPLEMENTATION})

target_include_directories(DummyConcurrency PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)

if(DC_ENABLE_TESTS)
  target_link_libraries(DummyConcurrency PUBLIC twist)
  target_include_directories(
    DummyConcurrency
    PUBLIC ${twist_SOURCE_DIR}/source)
  target_compile_definitions(DummyConcurrency PUBLIC DC_IMPLEMENTATION_LAYER_TWIST)
endif()
