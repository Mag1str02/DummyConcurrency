# Exception context
if(DC_EXCEPTIONS_CONTEXT)
  set(DC_EXCEPTION_CONTEXT_IMPLEMENTATION "unix")
else()
  set(DC_EXCEPTION_CONTEXT_IMPLEMENTATION "nop")
endif()

# Machien context
if((CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64") OR (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm") OR (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64"))
  set(DC_MACHINE_CONTEXT_IMPLEMENTATION "armv8_a_64")
elseif((CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64") OR (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "AMD64"))
  set(DC_MACHINE_CONTEXT_IMPLEMENTATION "x86_64")
elseif(DC_USE_UCONTEXT)
  set(DC_MACHINE_CONTEXT_IMPLEMENTATION "ucontext")
else()
  message(SEND_ERROR "Failed to find machine context implementation for system processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()
