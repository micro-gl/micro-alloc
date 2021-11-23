cmake_minimum_required(VERSION 3.12)
project(micro-alloc-examples)
message(\n===========\n${PROJECT_NAME} \n===========\n)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG  ${CMAKE_CXX_FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS_RELEASE " -fno-rtti -fno-exceptions -fno-unwind-tables \
                            -fstrict-aliasing -O3  -flto \
                            -Wstrict-aliasing -fno-math-errno \
                            -ffast-math -fno-threadsafe-statics \
                            -fdata-sections -ffunction-sections \
                            " CACHE INTERNAL "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(libs micro-alloc)

set(SOURCES
        test_allocators.cpp
        test_stack_memory.cpp
        test_dynamic_memory.cpp
        test_pool_memory.cpp
        test_linear_memory.cpp
        test_std_memory.cpp
        test_polymorphic_allocator.cpp
        test_utils.cpp
        )

set(SOURCES_SHARED "")

foreach( testsourcefile ${SOURCES} )
    # I used a simple string replace, to cut off .cpp.
    string( REPLACE ".cpp" "" testname ${testsourcefile} )
    include_directories("${PROJECT_BINARY_DIR}")

    add_executable( ${testname} ${testsourcefile} ${SOURCES_SHARED} )
    # Make sure YourLib is linked to each app
    target_link_libraries( ${testname} ${libs} )
endforeach( testsourcefile ${SOURCES} )

