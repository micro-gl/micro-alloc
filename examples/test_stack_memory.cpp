#define MICRO_ALLOC_DEBUG

#include <micro-alloc/stack_memory.h>

using namespace micro_alloc;

void test_1() {
    using byte= unsigned char;
    const int size = 5000;
    byte memory[size];

    stack_memory<> alloc{memory, size};

    void * a1 = alloc.malloc(5000);
    void * a2 = alloc.malloc(512);
    void * a3 = alloc.malloc(256);
    void * a4 = alloc.malloc(128);
    void * a5 = alloc.malloc(3);
    alloc.free(a5);
    alloc.free(a4);
    alloc.free(a3);
    alloc.free(a2);
    alloc.free(a2);
    alloc.free(a1);
    alloc.free(a1);

    void * a41 = alloc.malloc(200);
    void * a51 = alloc.malloc(200);
    void * a61 = alloc.malloc(200);
    alloc.free(a41);
    alloc.free(a51);
    alloc.free(a61);
    //    alloc.free(a3);
    //    alloc.free(a3);
    //    alloc.free(a3);

}

int main() {
    test_1();
}
