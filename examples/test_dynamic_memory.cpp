#define MICRO_ALLOC_DEBUG

#include <micro-alloc/dynamic_memory.h>

using namespace micro_alloc;

void test_polymorphic() {
    using byte= unsigned char;
    const int size = 5000;
    byte memory[size];

    dynamic_memory<> alloc{memory, size};

    void * a1 = alloc.malloc(200);
    void * a2 = alloc.malloc(200);
    void * a3 = alloc.malloc(200);
    alloc.free(a3);
    alloc.free(a1);
    alloc.free(a2);
//    alloc.free(a2);
//    void * a4 = alloc.allocate(200);
//    void * a5 = alloc.allocate(200);
//    void * a6 = alloc.allocate(200);
//    alloc.free(a2);
//    alloc.free(a4);
//    alloc.free(a6);
//    alloc.free(a3);
//    alloc.free(a3);
//    alloc.free(a3);

}

int main() {
    test_polymorphic();
}
