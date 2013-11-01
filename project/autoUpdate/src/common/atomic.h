#ifndef ATOMIC_HPP_
#define ATOMIC_HPP_

#if defined(WIN32) || defined(WIN64)

#include <intrin.h>

#define atomic_inc(a, b) ((b) + _InterlockedExchangeAdd(a, b))
#define atomic_dec(a, b) (-(b) + _InterlockedExchangeAdd(a, -(b)))
#define atomic_compare_and_swap(a, b, c) ((b) == _InterlockedCompareExchange(a, c, b))
//#define atomic_and  __sync_and_and_fetch
//#define atomic_or  __sync_or_and_fetch
//#define atomic_xor __sync_xor_and_fetch
//#define atomic_nand __sync_nand_and_fetch

#else

#define atomic_inc __sync_add_and_fetch
#define atomic_dec __sync_sub_and_fetch
#define atomic_compare_and_swap __sync_bool_compare_and_swap
//#define atomic_and  __sync_and_and_fetch
//#define atomic_or  __sync_or_and_fetch
//#define atomic_xor __sync_xor_and_fetch
//#define atomic_nand __sync_nand_and_fetch

#endif // defined(WIN32) || defined(WIN64)

#endif  // ATOMIC_HPP_

