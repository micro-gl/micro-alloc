#define MICRO_ALLOC_DEBUG

#include <micro-alloc/dynamic_memory.h>
#include <micro-alloc/polymorphic_allocator.h>

using namespace micro_alloc;
using byte = unsigned char;

struct dummy_t {
public:
    int a, b, c;
    explicit dummy_t(int _a=0, int _b=1, int _c=2) :
                        a(_a), b(_b), c(_c) {
        std::cout << "constructed with (" <<a<<","<<b<<","<<c<<")" << std::endl;
    }

    ~dummy_t() { std::cout << "destructed !!" << std::endl; }
};

void test_polymorphic() {
    const int size = 5000;
    byte memory[size];

    dynamic_memory<> mem_resource{memory, size};
    polymorphic_allocator<dummy_t> allocator(&mem_resource);

    // allocate raw memory that can for 5 dummies
    dummy_t * allocated_memory = allocator.allocate(5);

    // construct 5 dummies in memory
    for (int ix = 0; ix < 3; ++ix)
        allocator.construct(allocated_memory+ix, 4, 5, 6);
    for (int ix = 3; ix < 5; ++ix)
        allocator.construct(allocated_memory+ix, 40, 50, 60);

    // test objects
    for (int ix = 0; ix < 5; ++ix) {
        std::cout << "#" << ix
                  << " a :" << allocated_memory[ix].a
                  << " b :" << allocated_memory[ix].b
                  << " c :" << allocated_memory[ix].c << std::endl;
    }

    // destruct dummies
    for (int ix = 0; ix < 5; ++ix)
        allocator.destroy(allocated_memory + ix);

    // deallocate memory
    allocator.deallocate(allocated_memory);
}

int main() {
    test_polymorphic();
}
