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
     * Pool Memory Resource
     *
     * Free is:
     * - O(1) when {guard_against_double_free==false} (in constructor)
     * - O(free-list-size) when {guard_against_double_free==true} (in constructor)
     *
     * Allocations are O(1)
     *
     * Minimal block size is 4 bytes for 32 bit pointer types and 8 bytes for 64 bits pointers.
     *
     * @author Tomer Riko Shalev
     */
    class pool_memory : public memory_resource {
    private:
        using base = memory_resource;
        using typename base::uptr;
        using base::align_up;
        using base::align_down;
        using base::is_aligned;
        using base::ptr_to_int;
        using base::int_to_ptr;
        using base::align_of_uptr;
        using base::max;
        using base::is_alignment_pow_2;
        using base::try_throw;
        using uintptr_type = memory_resource::uintptr_type;

        template<typename T>
        static T int_to(uptr integer) { return reinterpret_cast<T>(integer); }

        struct header_t { header_t *next = nullptr; };
        static constexpr uptr alignment_of_header() { return align_of_uptr(); }

        void *_ptr = nullptr;
        uptr _size = 0;
        uptr _block_size = 0;
        uptr _blocks_count = 0;
        uptr _free_blocks_count = 0;
        header_t *_free_list_root = nullptr;
        bool _guard_against_double_free = false;

        uptr minimal_size_of_any_block() const { return align_up(sizeof(header_t)); }
        uptr correct_block_size(uptr block_size) const {
            block_size = align_up(block_size);
            if (block_size < minimal_size_of_any_block())
                block_size = minimal_size_of_any_block();
            return block_size;
        }
        uptr compute_blocks_count() {
            uptr a = align_up(ptr_to_int(_ptr));
            uptr b = align_down(ptr_to_int(_ptr) + _size);
            uptr diff = b - a;
            return diff / _block_size;
        }

    public:
        uptr block_size() const { return _block_size; }
        uptr blocks_count() const { return _blocks_count; }
        uptr free_blocks_count() const { return _free_blocks_count; }
        uptr start_aligned_address() const { return align_up(ptr_to_int(_ptr)); }
        uptr end_aligned_address() const { return align_down(ptr_to_int(_ptr) + _size); }
        uptr available_size() const override { return free_blocks_count() * _block_size; }

        pool_memory() = delete;

        /**
         *
         *
         * @param ptr start of memory
         * @param size_bytes the memory size in bytes
         * @param block_size the block size
         * @param requested_alignment alignment request, power of 2 integer. Fulfilled alignment might be
         *                              higher than requested because of headers alignment.
         * @param guard_against_double_free if {True}, user will not be able to accidentally
         *          free an already free block at the cost of having free operation at O(free-list-size).
         *          If {False}, free will take O(1) operations like allocations.
         */
        pool_memory(void *ptr, uptr size_bytes, uptr block_size,
                    uptr requested_alignment = sizeof(uintptr_type),
                    bool guard_against_double_free = false) :
                        base(3, max(requested_alignment, sizeof(uintptr_type))), _ptr(ptr),
                        _size(size_bytes), _block_size(0), _guard_against_double_free(guard_against_double_free) {
            const bool is_memory_valid_1 = correct_block_size(block_size) <= size_bytes;
            const bool is_memory_valid_3 = is_alignment_pow_2();
            const bool is_memory_valid = is_memory_valid_1 and is_memory_valid_3;
            if (is_memory_valid) reset(block_size);
            this->_is_valid = is_memory_valid;

#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nHELLO:: pool memory resource\n";
            std::cout << "* requested alignment is " << requested_alignment << " bytes\n";
            std::cout << "* final alignment is " << this->alignment << " bytes\n";
            std::cout << "* correct block size due to headers and final alignment is "
                      << correct_block_size(block_size) << " bytes\n";
            std::cout << "* number of blocks is " << _blocks_count << "\n";
            if (is_memory_valid)
                std::cout << "* first block @ " << ptr_to_int(_free_list_root) << std::endl;
            if (!is_memory_valid_1)
                std::cout << "* memory does not satisfy minimal size requirements !!!\n";
            if (!is_memory_valid_3)
                std::cout << "* error:: final alignment should be a power of 2\n";
            if(!is_memory_valid) try_throw();
            // I invoke a virtual method from a constructor, BUT it will invoke the local copy,
            // which is OK
            print(false);
#endif
        }

        ~pool_memory() override {
            _free_list_root = nullptr;
            _ptr = nullptr;
            _blocks_count = _block_size = _size = 0;
        }

        void reset(const uptr block_size) {
            _block_size = correct_block_size(block_size);
            const uptr blocks = _free_blocks_count = _blocks_count
                    = compute_blocks_count();
            void *ptr = _ptr;
            uptr current = align_up(ptr_to_int(ptr));
            uptr next = current + _block_size;
            _free_list_root = int_to<header_t *>(current);
            for (int ix = 0; ix < blocks - 1; ++ix) {
                auto *header_current = int_to<header_t *>(current);
                auto *header_next = int_to<header_t *>(next);
                header_current->next = header_next;
                current += _block_size;
                next += _block_size;
            }
            int_to<header_t *>(current)->next = nullptr;
        }

        void *malloc() { return malloc(0); }
        void *malloc(uptr size_bytes_dont_matter) override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nMALLOC:: pool memory resource\n";
#endif

            if (_free_list_root == nullptr) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- no free blocks are available\n";
#endif
                try_throw();
                return nullptr;
            }
            auto *current_node = _free_list_root;
            _free_list_root = _free_list_root->next;
            _free_blocks_count -= 1;

#ifdef MICRO_ALLOC_DEBUG
            std::cout << "- handed a free block @" << ptr_to_int(current_node) << "\n";
            std::cout << "- free blocks in pool [" << _free_blocks_count << "/"
                      << _blocks_count << "]\n";
#endif

            return current_node;
        }

        bool free(void *pointer) override {
            auto address = ptr_to_int(pointer);
            const uptr min_range = align_up(ptr_to_int(_ptr));
            const uptr max_range = align_down(ptr_to_int(_ptr) + _size);
            const bool is_in_range = address >= min_range && address < max_range;

#ifdef MICRO_ALLOC_DEBUG
            std::cout << std::endl << "FREE:: pool allocator \n- free a block address @ " << address << std::endl;
#endif
            if (!is_in_range) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- error: address is not in range [" << min_range << " -- " << max_range << std::endl;
#endif
                try_throw();
                return false;
            }

            bool is_address_block_aligned = (address - min_range) % _block_size == 0;
            if (!is_address_block_aligned) {
#ifdef MICRO_ALLOC_DEBUG
                std::cout << "- error: address is not aligned to " << _block_size << " bytes block sizes\n";
#endif
                try_throw();
                return false;
            }

            if (_guard_against_double_free) {
                bool is_freeing_an_already_free_block = false;
                auto *current = _free_list_root;
                while (current) {
                    if (ptr_to_int(current) == address) {
                        is_freeing_an_already_free_block = true;
                        break;
                    }
                    current = current->next;
                }
                if (is_freeing_an_already_free_block) {
#ifdef MICRO_ALLOC_DEBUG
                    std::cout << "- error: tried to free an already Free block\n";
#endif
                    try_throw();
                    return false;
                }
            }

            auto *block = int_to<header_t *>(address);
            block->next = _free_list_root;
            _free_list_root = block;
            _free_blocks_count += 1;

#ifdef MICRO_ALLOC_DEBUG
            std::cout << "- free blocks in pool [" << _free_blocks_count << "/"
                      << _blocks_count << "]\n";
#endif
            return true;
        }

        void print(bool dummy) const override {
#ifdef MICRO_ALLOC_DEBUG
            std::cout << "\nPRINT:: pool allocator \n";
            std::cout << "- free list is [" << _free_blocks_count << "/" << _blocks_count << "]\n\n";
#endif
        }

        bool is_equal(const memory_resource &other) const noexcept override {
            bool equals = this->type_id() == other.type_id();
            if (!equals) return false;
            const auto *casted_other = reinterpret_cast<const pool_memory *>(&other);
            equals = this->_ptr == casted_other->_ptr;
            return equals;
        }
    };
}