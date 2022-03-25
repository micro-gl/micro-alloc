#define MICRO_ALLOC_DEBUG
#define MICRO_ALLOC_ENABLE_THROW

#include <micro-alloc/pool_memory.h>

using namespace micro_alloc;

void test_1() {
    using byte= unsigned char;
    const int size = 1024;
    byte memory[size];

    pool_memory alloc{memory, size, 32, 4, true};

    void  * p1 = alloc.malloc();
    void  * p2 = alloc.malloc();
    void  * p3 = alloc.malloc();
    void  * p4 = alloc.malloc();
    void  * p5 = alloc.malloc();

    alloc.free(p1);
    alloc.free(p2);
    alloc.free(p4);
    alloc.free(p3);
    alloc.free(p3);
    //    alloc.free((void *)(memory+256));

    //    alloc.free(p1);
    //    alloc.free(p1);
}

int main() {
    test_1();
}
