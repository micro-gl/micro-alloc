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

namespace micro_alloc {

    struct throw_allocator_cannot_allocate_or_deallocate {};

    /**
     * Throw allocator, always throws exception. Does not use any std exceptions.
     * @tparam T the allocated object type
     */
    template<typename T=unsigned char>
    class throw_allocator {
    public:
        using value_type = T;
        using size_t = unsigned long;

        template<class U>
        explicit throw_allocator(const throw_allocator<U> &other) noexcept {};
        explicit throw_allocator() = default;
        template<class U, class... Args> void construct(U *p, Args &&... args) {}
        T *allocate(size_t n) { throw throw_allocator_cannot_allocate_or_deallocate(); }
        void deallocate(T *p, size_t n = 0) { throw throw_allocator_cannot_allocate_or_deallocate(); }
        template<class U> struct rebind {
            typedef throw_allocator<U> other;
        };
    };

    template<class T1, class T2>
    bool operator==(const throw_allocator<T1> &lhs, const throw_allocator<T2> &rhs) noexcept {
        return true;
    }
}