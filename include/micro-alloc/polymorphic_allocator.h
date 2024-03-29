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
#include "traits.h"

namespace micro_alloc {

    /**
    * Polymorphic Allocator, that uses exchangeable memory resources
     *
    * @tparam T the allocated object type
    */
    template<typename T=char>
    class polymorphic_allocator {
    public:
        using value_type = T;
        using memory = memory_resource;
        using uintptr_type = memory::uintptr_type;
        using size_t = uintptr_type;
        static const uintptr_type default_align = sizeof(uintptr_type);

    private:
        memory *_mem;

    public:
        polymorphic_allocator() = delete;

        template<class U>
        explicit polymorphic_allocator(const polymorphic_allocator<U> &other) noexcept
                : polymorphic_allocator{other.resource()} {}

        explicit polymorphic_allocator(memory_resource *r) : _mem{r} {}

        memory *resource() const { return _mem; }

        template<class U, class... Args> void construct(U *p, Args &&... args) {
            new(p) U(micro_alloc::traits::forward<Args>(args)...);
        }

        template<class U> void destroy( U* p ) { p->~U(); }

        T *allocate(size_t n) { return (T *) _mem->malloc(n * sizeof(T)); }
        void deallocate(T *p, size_t n = 0) { _mem->free(p); }
        void *allocate_bytes(size_t nbytes, size_t alignment = default_align) {
            return _mem->malloc(nbytes);
        }
        void deallocate_bytes(void *p, size_t nbytes, size_t alignment = default_align) {
            _mem->free(p);
        }
        template<class U> U *allocate_object(size_t n = 1) {
            return allocate_bytes(n * sizeof(U), alignof(U));
        }
        template<class U> void deallocate_object(U *p, size_t n = 1) {
            deallocate_bytes(p, sizeof(U), alignof(U));
        }

        template<class U, class... CtorArgs> U *new_object(CtorArgs &&... ctor_args) {
            U *p = allocate_object<U>();
            construct(p, micro_alloc::traits::forward<CtorArgs>(ctor_args)...);
            return p;
        }

        template<class U> void delete_object(U *p) {
            p->~U();
            deallocate_object(p);
        }

        polymorphic_allocator &operator=(polymorphic_allocator) = delete;

        polymorphic_allocator select_on_container_copy_construction(const polymorphic_allocator &dummy) const {
            return polymorphic_allocator<T>(*this);
        }

        template<class U> struct rebind {
            typedef polymorphic_allocator<U> other;
        };
    };

    template<class T1, class T2, typename uintptr_type>
    bool operator==(const polymorphic_allocator<T1> &lhs,
                    const polymorphic_allocator<T2> &rhs) noexcept {
        return *(lhs.resource()) == *(rhs.resource());
    }
}