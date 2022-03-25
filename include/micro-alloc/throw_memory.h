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

#ifdef MICRO_ALLOC_DEBUG
#include <iostream>
#endif

namespace micro_alloc {

    /**
     * Throw memory, throws upon malloc/free
     *
     */
    class throw_memory : public memory_resource {
    private:
        using base = memory_resource;
        using base::ptr_to_int;
        using typename base::uptr;
        using uintptr_type = memory_resource::uintptr_type;

    public:
        struct throw_memory_exception {};

        throw_memory() : base(6, sizeof(uintptr_type)) {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nHELLO:: throw memory resource\n";
            std::cout << "* final alignment is " << this->alignment << " bytes" << std::endl;
#endif
        }
        ~throw_memory() = default;

        uptr available_size() const override { return 0; }

        void * malloc(uptr size_bytes) override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nMALLOC:: throw memory\n- requested " << size_bytes << "bytes\n";
            std::cout << "- nothing will be fulfilled ";
#endif
            throw throw_memory_exception();
            return nullptr;
        }

        bool free(void *pointer) override {
            auto address = ptr_to_int(pointer);
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nFREE:: throw memory\n- free address @ " << address << "\n";
#endif
            throw throw_memory_exception();
            return true;
        }

        bool is_equal(const memory_resource<> &other) const noexcept override {
            return this->type_id() == other.type_id();
        }
    };
}