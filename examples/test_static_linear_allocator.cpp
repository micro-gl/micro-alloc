#define MICRO_ALLOC_DEBUG

#include <iostream>
#include <micro-alloc/static_linear_allocator.h>

using namespace micro_alloc;
using namespace std;

void test_1() {
    static_linear_allocator<char, 1024, 0> allocator_1{};
    allocator_1.allocate(512);
    static_linear_allocator<int, 1024, 0> allocator_2{allocator_1};
    allocator_2.allocate(64);
    allocator_2.allocate(64);

    allocator_2.reset();
    allocator_1.allocate(512);

//    static_linear_allocator<char, 1024, 1> allocator_3{};
//    allocator_3.allocate(1025);

}

int main() {
    test_1();
}

