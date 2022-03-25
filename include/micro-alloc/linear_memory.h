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
 * Linear Memory Resource:
 *
 * Memory is given progressively without freeing. user can only reset the pointer to the
 * beginning. This memory is not shrinking.
 *
 * - Allocations are O(1)
 * - Free does not do anything
 *
 * @author Tomer Riko Shalev
 */
    class linear_memory : public memory_resource {
    private:
        using base = memory_resource;
        using typename base::uptr;
        using base::align_up;
        using base::align_down;
        using base::is_aligned;
        using base::ptr_to_int;
        using base::int_to_ptr;
        using base::int_to;
        using base::try_throw;
        using base::is_alignment_pow_2;
        using uintptr_type = uptr;

        void *_ptr;
        void *_current_ptr;
        uint _size;

    public:

        linear_memory() = delete;

        /**
         *
         * @param ptr start of memory
         * @param size_bytes the memory size in bytes
         * @param alignment power of 2 alignment
         */
        linear_memory(void *ptr, uptr size_bytes, uptr alignment = sizeof(uintptr_type)) :
                base{1, alignment}, _ptr(ptr), _current_ptr(nullptr), _size(size_bytes) {
            const bool is_memory_valid = is_alignment_pow_2();
            this->_is_valid = is_memory_valid;

#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nHELLO:: linear memory resource\n";
            std::cout << "* requested alignment is " << alignment << " bytes" << std::endl;
            std::cout << "* size is " << size_bytes << " bytes" << std::endl;
            if (!is_alignment_pow_2())
                std::cout << "* error:: alignment should be a power of 2\n";
#endif
            if (is_memory_valid) reset();
            else try_throw();
        }

        ~linear_memory() override {
            _current_ptr = _ptr = nullptr;
            _size = 0;
        }

        void reset() {
            _current_ptr = base::template int_to<void *>(align_up(ptr_to_int(_ptr)));
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nRESET:: linear memory\n- reset memory to start @ "
                      << ptr_to_int(_current_ptr) << " (aligned up)\n";
#endif
        }

        uptr available_size() const override {
            const uptr min = align_up(ptr_to_int(_current_ptr));
            const uptr delta = end_aligned_address() - min;
            return delta;
        }
        uptr start_aligned_address() const { return align_up(ptr_to_int(_ptr)); }
        uptr end_aligned_address() const { return align_down(ptr_to_int(_ptr) + _size); }

        void *malloc(uptr size_bytes) override {
            size_bytes = align_up(size_bytes);
            const uptr available_space = available_size();
            const bool has_available_size = size_bytes <= available_space;
            const bool has_requested_size_zero = size_bytes == 0;

#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nMALLOC:: linear allocator\n"
                      << "- request a block of size " << size_bytes << " (aligned up)\n";
#endif

            if (has_requested_size_zero) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- error, cannot fulfill a size 0 bytes block !!\n";
#endif
                try_throw();
                return nullptr;
            }

            if (!has_available_size) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- error, could not fulfill this size\n- available size is " << available_space << "\n";
#endif
                try_throw();
                return nullptr;
            }
            auto *pointer = _current_ptr;
            _current_ptr = base::template int_to<void *>(ptr_to_int(pointer) + size_bytes);
            return pointer;
        }

        bool free(void *pointer) override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nFREE:: linear allocator \n"
                      << "- linear allocator does not free space, use reset() instead \n"
                      << "- available size is " << available_size() << std::endl;
#endif
            return false;
        }

        void print(bool embed) const override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nPRINT:: linear allocator \n- available size is " << available_size() << "\n";
#endif
        }

        bool is_equal(const memory_resource &other) const noexcept override {
            bool equals = this->type_id() == other.type_id();
            if (!equals) return false;
            const auto *casted_other = static_cast<const linear_memory *>(&other);
            equals = this->_ptr == casted_other->_ptr;
            return equals;
        }

    };
}