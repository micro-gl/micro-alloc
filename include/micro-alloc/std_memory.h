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

#include "memory_resource.h"

//#define MICRO_ALLOC_DEBUG

#ifdef MICRO_ALLOC_DEBUG
#include <iostream>
#endif

namespace micro_alloc {

    /**
     * Standard Memory Resource:
     *
     * uses the standard default memory allocations operators techniques present in the system
     *
     * @author Tomer Riko Shalev
     */
    class std_memory : public memory_resource<micro_alloc::uintptr_type> {
    private:
        using base = memory_resource<micro_alloc::uintptr_type>;

    public:

        /**
         * ctor
         *
         * @param alignment alignment requirement
         */
        explicit std_memory(uptr alignment = sizeof(unsigned long)) :
                base{0, alignment} {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << std::endl << "HELLO:: standard memory resource" << std::endl;
            std::cout << "* requested alignment is " << alignment << " bytes" << std::endl;
            std::cout << "* BUT, alignment does not matter, because we proxy to operator new" << std::endl;
#endif
        }

        uptr available_size() const override { return ~uptr(0); }

        void *malloc(uptr size_bytes) override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nMALLOC:: standard memory\n"
                      << "- request a block of size " << size_bytes << std::endl;
#endif
            return ::operator new(size_bytes);
        }

        bool free(void *pointer) override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nFREE:: standard memory \n";
#endif
            ::operator delete(pointer);
            return true;
        }

        void print(bool embed) const override {
        }

        bool is_equal(const memory_resource<> &other) const noexcept override {
            bool equals = this->type_id() == other.type_id();
            return equals;
        }
    };
}