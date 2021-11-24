# This file has to be included by a parent, this is not standalone
cmake_minimum_required(VERSION 3.12)
project(micro-alloc-examples)
message(\n===========\n${PROJECT_NAME} \n===========\n)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(libs micro-alloc)

set(SOURCES
        test_stack_memory.cpp
        test_dynamic_memory.cpp
        test_pool_memory.cpp
        test_linear_memory.cpp
        test_std_memory.cpp
        test_polymorphic_allocator.cpp
        test_throw_allocator.cpp
        test_utils_new_array.cpp
        test_utils_new_object.cpp
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

