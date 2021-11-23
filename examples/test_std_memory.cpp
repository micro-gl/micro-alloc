#define MICRO_ALLOC_DEBUG

#include <micro-alloc/std_memory.h>

using namespace micro_alloc;

void test_std_memory() {

    std_memory alloc;

    void  * p1 = alloc.malloc(512);
    void  * p2 = alloc.malloc(512);
    void  * p3 = alloc.malloc(512);

    alloc.free(p1);
    alloc.free(p2);
    alloc.free(p3);
//    alloc.reset();

    void  * p4 = alloc.malloc(512);
    void  * p5 = alloc.malloc(512);

    //    alloc.free((void *)(memory+256));

    //    alloc.free(p1);
    //    alloc.free(p1);
}


int main() {
    test_std_memory();
}
