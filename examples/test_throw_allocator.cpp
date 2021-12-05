#define MICRO_ALLOC_DEBUG

#include <micro-alloc/throw_allocator.h>

using namespace micro_alloc;

void test_1() {
    throw_allocator<int> allocator;
    allocator.allocate(1);
}

int main() {
    test_1();
}

