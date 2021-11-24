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

    namespace traits {

        template<class T> struct remove_reference      {typedef T type;};
        template<class T> struct remove_reference<T&>  {typedef T type;};
        template<class T> struct remove_reference<T&&> {typedef T type;};

        template <class _Tp> inline typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept
        {
            typedef typename remove_reference<_Tp>::type _Up;
            return static_cast<_Up&&>(__t);
        }

        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type& __t) noexcept
        { return static_cast<_Tp&&>(__t); }

        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type&& __t) noexcept
        { return static_cast<_Tp&&>(__t); }

        template<bool B, class T, class F> struct conditional { typedef T type; };
        template<class T, class F> struct conditional<false, T, F> { typedef F type; };

        template <class Tp, Tp _v>
        struct integral_constant
        {
            static constexpr const Tp      value = _v;
            typedef Tp               value_type;
            typedef integral_constant type;
            constexpr operator value_type() const noexcept {return value;}
        };

        typedef integral_constant<bool, true> true_type;
        typedef integral_constant<bool, false> false_type;

    }

    template<bool B, class T, class F>
    using cond = micro_alloc::traits::conditional<B, T, F>;

    static constexpr unsigned int PS = sizeof (void *);

    /**
     * An integral type, that is suitable to hold a pointer address
     */
    using uintptr_type = typename cond<
            PS==sizeof(unsigned short), unsigned short ,
            typename cond<
                    PS==sizeof(unsigned int), unsigned int,
                    typename cond<
                            PS==sizeof(unsigned long), unsigned long, unsigned long long>::type>::type>::type;

}