/*========================================================================================
 Copyright (2021), Tomer Shalev (tomer.shalev@gmail.com, https://github.com/HendrixString).
 All Rights Reserved.
 License is a custom open source semi-permissive license with the following guidelines:
 1. unless otherwise stated, derivative work and usage of this file is permitted and
    should be credited to the project and the author of this project.
 2. Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
========================================================================================*/
#pragma once

#include "traits.h"

namespace micro_alloc {

    /**
     * Allocate and Construct new array with allocator.
     * NOTE:
     * - Header info with size of array is extra allocated and written
     * - You can only release this array with delete_array function
     *
     * @tparam U value type
     * @tparam allocator_type allocator type
     * @tparam Args args for constructor
     * @param count how many objects in array
     * @param allocator allocator instance
     * @param args variadic args for constructor
     * @return a pointer to initialized array
     */
    template<class U, class allocator_type, class... Args>
    U * new_array(const unsigned count,
                  const allocator_type & allocator,
                  Args &&... args) {
        // rebind the allocator to byte allocator
        using rebind_allocator_t = typename allocator_type::template rebind<char>::other;
        rebind_allocator_t rebind_allocator(allocator);

        // calculate info header size, note:
        // offset has to be a number that is divisible by the allocation
        // alignment (most allocators can only handle static/one alignment value).
        // 32 is an absolute max that is divisible by all alignments. (we can settle for 8 even).
        const auto header_size = 16;

        // allocate memory + info header
        char * raw_memory = rebind_allocator.allocate(header_size + count*sizeof(U));
        U * object_memory = reinterpret_cast<U *>(raw_memory+header_size);

        // write info header, uintptr_type is big enough to store any pointer,
        // therefore it is a legit holder for count limits
        using uintptr_type = micro_alloc::uintptr_type;
        auto * header = reinterpret_cast<uintptr_type *>(raw_memory);
        *header = count;

        // construct the array objects
        for (int ix = 0; ix < count; ++ix)
            new(object_memory+ix) U(micro_alloc::traits::forward<Args>(args)...);

        return object_memory;
    }

    /**
     * Destruct and Deallocates an array, that was created using new_array function
     * @tparam U array item type
     * @tparam allocator_type allocator type
     * @param pointer pointer to array
     * @param allocator allocator, that was used to allocate memory for this array
     */
    template<class U, class allocator_type>
    void delete_array(U * pointer,
                      const allocator_type & allocator = allocator_type()) {
        // rebind the allocator to byte allocator
        using rebind_allocator_t = typename allocator_type::template rebind<char>::other;
        rebind_allocator_t rebind_allocator(allocator);

        // calculate info header size
        using uintptr_type = micro_alloc::uintptr_type;
        const auto header_size = 16;

        // get original raw memory with header
        char * raw_memory = reinterpret_cast<char *>(pointer)-header_size;

        // read count
        const auto count = *reinterpret_cast<uintptr_type *>(raw_memory);

        // destruct objects in array
        for (int ix = 0; ix < count; ++ix) (pointer+ix)->~U();

        // de-allocate memory
        rebind_allocator.deallocate(raw_memory);
    }
}