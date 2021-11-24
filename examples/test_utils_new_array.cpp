#define MICRO_ALLOC_DEBUG

#include <micro-alloc/dynamic_memory.h>
#include <micro-alloc/polymorphic_allocator.h>
#include <micro-alloc/utils.h>

using namespace micro_alloc;
using byte = unsigned char;

struct dummy_t {
public:
    int a, b, c;
    explicit dummy_t(int _a=0, int _b=1, int _c=2) :
                        a(_a), b(_b), c(_c) {
        std::cout << "constructed with (" <<a<<","<<b<<","<<c<<")"
                  << std::endl;
    }
    ~dummy_t() { std::cout << "destructed !!" << std::endl; }
};

void test_1() {
    const int size = 5000;
    byte memory[size];

    dynamic_memory<> mem_resource{memory, size};
    polymorphic_allocator<dummy_t> allocator(&mem_resource);

    // allocate raw memory that can for 5 dummies
    auto * array = micro_alloc::new_array<dummy_t>(5, allocator, 40, 50, 60);
    micro_alloc::delete_array(array, allocator);
}

void test_2() {
    const int size = 5000;
    byte memory[size];

    dynamic_memory<> mem_resource{memory, size};
    polymorphic_allocator<dummy_t> allocator(&mem_resource);

    // allocate raw memory that can for 5 dummies
    auto ** array = micro_alloc::new_array<dummy_t *>(5, allocator, new dummy_t);
    micro_alloc::delete_array(array, allocator);
}

int main() {
    test_1();
//    test_2();
}
