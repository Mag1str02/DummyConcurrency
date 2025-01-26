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
message("------------------- ${DC_USE_TWIST}")

if(DC_USE_TWIST)
  target_link_libraries(DummyConcurrency PUBLIC twist)
  target_include_directories(
    DummyConcurrency
    PUBLIC ${twist_SOURCE_DIR}/source)
  target_compile_definitions(DummyConcurrency PUBLIC DC_USE_TWIST)
endif()
