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
     * Stack Memory Resource
     *
     * Free is O(1)
     * Allocations are O(1)
     *
     * Notes:
     * - Allocations and de-allocations are SAFE. de-allocations will fail if trying
     *   to free NOT the latest allocated block.
     * - Each allocation requests a block + a small footer, that holds the size of the block
     *   including the size of aligned footer.
     * - Upon free operations, the footer is read and compared to the free address in order
     *   to validate the LIFO property of the stack.
     * - At all times, we keep track at the address after the end of the stack
     * - Minimal block size is 4 bytes for 32 bit pointer types and 8 bytes for 64 bits pointers.
     * - Blocks print is user_space = block_size - size_of_aligned_footer
     *
     * Block is:
     *  [..aligned data.. | distance to prev block end]
     *
     * @author Tomer Riko Shalev
     */
    class stack_memory : public memory_resource {
    private:
        using base = memory_resource;
        using typename base::uptr;
        using base::align_of_uptr;
        using base::align_up;
        using base::align_down;
        using base::is_aligned;
        using base::ptr_to_int;
        using base::int_to_ptr;
        using base::int_to;
        using base::is_alignment_pow_2;
        using base::try_throw;
        using uintptr_type = memory_resource::uintptr_type;

        template<typename T>
        static T int_to(uptr integer) { return reinterpret_cast<T>(integer); }

        struct footer_t { uptr distance_to_prev_block_end = 0; /* distance to last block end */ };
        static constexpr uptr alignment_of_footer() { return align_of_uptr(); }

        void *_ptr ;
        uptr _current_block_end;
        uptr _size;


    public:
        uptr start_aligned_address() const { return align_up(ptr_to_int(_ptr)); }
        uptr end_aligned_address() const { return align_down(ptr_to_int(_ptr) + _size); }

        stack_memory() = delete;

        /**
         * @param ptr start of memory
         * @param size_bytes the memory size in bytes
         * @param alignment power of 2 alignment
         */
        stack_memory(void *ptr, uptr size_bytes, uptr alignment = sizeof(uintptr_type)) :
                base{4, max(sizeof (uintptr_type), alignment)}, _ptr(ptr), _size(size_bytes), _current_block_end(0) {
            const bool is_memory_valid_1 = alignment_of_footer() <= size_bytes;
            const bool is_memory_valid_2 = alignment_of_footer() <= size_bytes;
            const bool is_memory_valid_3 = is_alignment_pow_2();
            const bool is_memory_valid = is_memory_valid_1 and is_memory_valid_3;
            if (is_memory_valid) _current_block_end = align_up(ptr_to_int(_ptr));
            this->_is_valid = is_memory_valid;

#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nHELLO:: stack memory resource\n";
            std::cout << "* requested alignment is " << alignment << " bytes" << std::endl;
            std::cout << "* final alignment is " << this->alignment << " bytes" << std::endl;
            std::cout << "* minimal block size due to footer and alignment is " << alignment_of_footer() << " bytes\n";
            std::cout << "* principal mem due to alignment " << end_aligned_address()-start_aligned_address() << " bytes\n";
            if (!is_memory_valid_1)
                std::cout << "* error:: memory does not satisfy minimal size requirements !!!\n";
            if (!is_memory_valid_3)
                std::cout << "* error:: final alignment should be a power of 2\n";
#endif
            if(!is_memory_valid) try_throw();
        }

        ~stack_memory() override {
            _ptr = nullptr; _current_block_end = _size = 0;
        }

        uptr available_size() const override {
            const uptr min = align_up(_current_block_end);
            const uptr max = end_aligned_address();
            const uptr delta = max - min;
            return delta;
        }

        void * malloc(uptr size_bytes) override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nMALLOC:: stack memory\n- requested " << size_bytes << "bytes\n";
#endif
            if (size_bytes == 0) return nullptr;

            const uptr prev_block_end = _current_block_end;
            const uptr new_block_start = align_up(prev_block_end);
            const uptr aligned_size_bytes = align_up(size_bytes);
            const uptr start_of_footer = new_block_start + align_up(aligned_size_bytes, alignment_of_footer());
            const uptr new_block_end = start_of_footer + sizeof(footer_t);
            // distance in bytes from end of new block to end of last block
            const uptr distance_to_prev_block_end = new_block_end - prev_block_end;

            bool has_space = distance_to_prev_block_end <= available_size();
            if (!has_space) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- no free space available " << available_size() << std::endl;
                std::cout << "- tried to allocate " << distance_to_prev_block_end << "bytes\n";
#endif
                try_throw();
                return nullptr;
            }

            _current_block_end += distance_to_prev_block_end;
            footer_t *footer = int_to<footer_t *>(start_of_footer);
            footer->distance_to_prev_block_end = distance_to_prev_block_end;
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "- handed a free block @" << new_block_start << std::endl;
            std::cout << "- allocated " << distance_to_prev_block_end << "bytes\n";
            print(true);
#endif
            return int_to<void *>(new_block_start);
        }

        bool free(void *pointer) override {
            auto address = ptr_to_int(pointer);

#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nFREE:: stack allocator\n- free a block address @ " << address << "\n";
#endif
            const bool is_empty = _current_block_end == start_aligned_address();
            if (is_empty) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- error: nothing was allocated, nothing to free\n";
#endif
                try_throw();
                return false;
            }

            const uptr current_block_end = _current_block_end;
            const uptr current_footer_start = current_block_end - sizeof(footer_t);
            footer_t *footer = int_to<footer_t *>(current_footer_start);
            const uptr last_block_end = current_block_end - footer->distance_to_prev_block_end;
            const uptr current_block_start = align_up(last_block_end);
            bool is_lifo = address == current_block_start;

            if (!is_lifo) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- error: proposed free block is not the latest allocated, "
                             "and thus violating the LIFO property !!!" << std::endl;
#endif
                try_throw();
                return false;
            }

            _current_block_end -= footer->distance_to_prev_block_end;
#ifdef MICRO_ALLOC_DEBUG
            print(true);
#endif
            return true;
        }

        void print(bool embed) const override {
#ifdef MICRO_ALLOC_DEBUG
            if (!embed)
                std::cout << std::endl << "PRINT:: stack allocator " << std::endl;
            std::cout << "- blocks (LIFO order) [";
            uptr root = align_up(ptr_to_int(_ptr));
            uptr head = _current_block_end;
            bool is_last = true;
            while (head > root) {
                footer_t *footer = int_to<footer_t *>(head - sizeof (footer_t));
                head -= footer->distance_to_prev_block_end;
                bool is_first = head <= root;
                std::cout << (!is_last ? "<-" : "") << footer->distance_to_prev_block_end
                          << (is_first ? "(ROOT)" : "");
                if (is_last) is_last = false;
            }
            std::cout << "]" << std::endl;
#endif
        }

        bool is_equal(const memory_resource &other) const noexcept override {
            bool equals = this->type_id() == other.type_id();
            if (!equals) return false;
            const auto *casted_other = reinterpret_cast<const stack_memory *>(&other);
            equals = this->_ptr == casted_other->_ptr;
            return equals;
        }
    };
}