# micro{alloc}
**Fast**, super **slim**, **embeddable**, **headers files** only **`C++11`** memory allocation library.  
**No standard library is required**

Check out our website at [micro-gl.github.io/docs/micro-alloc/](micro-gl.github.io/docs/micro-alloc)
## Introduction
This lib includes several memory resources, that you can configure and can be optionally used with the included   
**polymorphic allocator(included)**, which implements a valid `C++11` allocator.  
It is advised to have a look at the `examples` folder as it is much simple to see  
the following memory resources are implemented:
### **Dynamic memory (heap)**:  
**Best-Fit** free list dynamic memory allocator with blocks coalescing.   
Allocation is **O(free-list-size)**  
Free is:  
- **O(1)** when coalescing with neighbor free blocks (because we know exactly where to insert),  
- **O(free-list-size)** when we can't coalesce.  
**Notes:**  
free blocks are inserted sorted by their address ascending, this is known to reduce fragmentation  
minimal block size 16 bytes for 32 bit pointer types and 32 bytes for 64 bits pointers.  

### **Pool memory**:
Pool block allocator    
Allocations are **O(1)**  
Free is:  
- **O(1)** when {guard_against_double_free==false} (in constructor)
- **O(free-list-size)** when {guard_against_double_free==true} (in constructor)
**Notes:**  
Minimal block size is 4 bytes for 32 bit pointer types and 8 bytes for 64 bits pointers.

### **Stack memory**:
Stack block allocator  
Free is **O(1)**  
Allocations are **O(1)**  

**notes:** 
- Allocations and de-allocations are SAFE. de-allocations will fail if trying 
to free NOT the latest allocated block.
- Each allocation requests a block + a small footer, that holds the size of the block
including the size of aligned footer.
- Upon free operations, the footer is read and compared to the free address in order
to validate the **LIFO** property of the stack.
- At all times, we keep track at the address after the end of the stack
- Minimal block size is 4 bytes for 32 bit pointer types and 8 bytes for 64 bits pointers.
- Blocks print is user_space = block_size - size_of_aligned_footer

### **Linear memory**:
Linear memory allocator

Memory is given progressively without freeing. user can only reset the pointer to the
beginning. this memory is not shrinking.
- Allocations are **O(1)**
- Free does not do anything

### **STD memory**:
Standard memory resource    
Uses the standard default memory allocations operators techniques present in the system

### **Allocators**:
- `Polymorphic_allocator` - goes with memory resources that are written above
- `std_rebind_allocator` - classic allocator that uses the global new/delete operator
- `static_linear_allocator` - self-contained static storage with tagged banks and sizes, allocates linearly, similar
to the linear memory resource.

## Installing `micro{alloc}`
`micro-alloc` is a headers only library, which gives the following install possibilities:
1. Using `cmake` to invoke the `install` target, that will copy everything in your system via
```
$ mkdir cmake-build-release
$ cd cmake-build-release
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --install .
```
2. Copying the `include/micro-alloc` to anywhere you want.

## Consuming `micro{alloc}`
Following options are available:
1. copy the project to a sub folder of your project. inside your **`CMakeLists.txt`** add
```cmake
add_subdirectory(/path/to/micro-alloc)
target_link_libraries(your_app micro-alloc)
```
2. If you installed **`micro{alloc}`** with option 1 (see above) at your system, you can instead
```cmake
find_package(micro-alloc CONFIG REQUIRED)
target_link_libraries(your_app micro-alloc::micro-alloc)
```
3. If you have not installed, you can add in your app's `CMakeLists.txt`
```cmake
target_include_directories(app path/to/micro-alloc/folder/include/)
```
4. If you manually copied the `include/micro-alloc` to the default system include path,  
   you can use `cmake/Findmicro-alloc.cmake` to automatically create the cmake targets
```cmake
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/path/to/Findmicro-alloc/folder)
find_package(micro-alloc REQUIRED)
target_link_libraries(your_app micro-alloc::micro-alloc)
```
5. Just copy the `include/micro-alloc` into a sub folder of your project and include the header  
   files you need with relative path in your source files.

## Running Examples
First make sure you have
- [cmake](https://cmake.org/download/) installed at your system.

There are two ways:
1. Use your favourite IDE to load the root `CMakeLists.txt` file, and then it   
   will pick up all of the targets, including the examples
2. Using the command line:
```bash
$ mkdir cmake-build-release
$ cd cmake-build-release
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build . --target <example_name>
$ ../examples/bin/example_name
```

# How to use
see `examples` folder or website
```text
Author: Tomer Shalev, tomer.shalev@gmail.com, all rights reserved (2021)
```