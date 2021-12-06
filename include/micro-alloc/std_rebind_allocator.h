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
     * Standard Allocator:
     *
     * Uses the present new and delete operators
     *
     * @tparam T the allocated object type
     */
    template<typename T=unsigned char>
    class std_rebind_allocator {
    public:
        using value_type = T;
        using size_t = unsigned long;

        template<class U>
        explicit std_rebind_allocator(const std_rebind_allocator<U> &other) noexcept {};
        explicit std_rebind_allocator() = default;

        template<class U, class... Args>
        void construct(U *p, Args &&... args) {
            ::new(p) U(micro_alloc::traits::forward<Args>(args)...);
        }

        T *allocate(size_t n) { return (T *) operator new(n * sizeof(T)); }
        void deallocate(T *p, size_t n = 0) { operator delete(p); }

        template<class U> struct rebind {
            typedef std_rebind_allocator<U> other;
        };
    };

    template<class T1, class T2>
    bool operator==(const std_rebind_allocator<T1> &lhs, const std_rebind_allocator<T2> &rhs) noexcept {
        return true;
    }
}