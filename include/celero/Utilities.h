#ifndef H_CELERO_UTILITIES_H
#define H_CELERO_UTILITIES_H

///
/// \author	John Farrier
///
/// \copyright Copyright 2015, 2016, 2017, 2018 John Farrier
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
/// http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef __FreeBSD__
#include <stdio.h>
#endif

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <thread>

#include <celero/Export.h>

namespace celero
{
	///
	/// \func DoNotOptimizeAway
	///
	/// Used to prevent compiler optimization of a variable
	/// that performs no real purpose other than to participate
	/// in a benchmark
	///
	/// Consider the following trivial benchmark:
	///
	/// \code
	/// BASELINE(...)
	/// {
	///     int x = 0;
	///
	///     for(int i = 0; i < 64; i++)
	///     {
	///         x += i;
	///     }
	/// }
	/// \endcode
	///
	/// Using Ubuntu clang v3.0, the resultant assembly is highly optimized
	/// as one might expect, but not terribly useful for baselining:
	///
	/// \verbatim
	/// movl	$2016, %eax             # imm = 0x7E0
	/// ret
	/// \endverbatim
	///
	/// Now, replace the inner loop with a call to DoNotOptimizeAway:
	///
	/// \code
	/// DoNotOptimizeAway(x += i);
	/// \endcode
	///
	/// The result is now a loop which is meaningful for establishing a
	/// baseline.
	///
	/// \verbatim
	/// xorl	%ecx, %ecx
	/// xorl	%eax, %eax
	/// .LBB0_1:                                # =>This Inner Loop Header: Depth=1
	/// addl	%ecx, %eax
	/// incl	%ecx
	/// cmpl	$64, %ecx
	/// jne	.LBB0_1
	/// ret
	/// \endverbatim
	///
	/// GCC 4.8 gives similar results.
	///
	/// gcc.godbolt.org permalink: http://goo.gl/lsngwX
	///
	/// Folly uses a simple bit of inline assembly:
	/// > template <class T>
	/// > void doNotOptimizeAway(T&& datum) {
	///	> asm volatile("" : "+r" (datum));
	/// >}
	///
	/// It would be great if that were portable with respect to both compilers and 32/64-bit targets.
	///
	template <class T>
	void DoNotOptimizeAway(T&& x)
	{
		static auto ttid = std::this_thread::get_id();
		if(ttid == std::thread::id())
		{
			// This forces the value to never be optimized away
			// by taking a reference then using it.
			const auto* p = &x;
			putchar(*reinterpret_cast<const char*>(p));

			// If we do get here, kick out because something has gone wrong.
			std::abort();
		}
	}

	/// Specialization for std::function objects which return a value.
	template <class T>
	void DoNotOptimizeAway(std::function<T(void)>&& x)
	{
		volatile auto foo = x();

		static auto ttid = std::this_thread::get_id();
		if(ttid == std::thread::id())
		{
			// This forces the value to never be optimized away
			// by taking a reference then using it.
			const auto* p = &foo + &x;
			putchar(*reinterpret_cast<const char*>(p));

			// If we do get here, kick out because something has gone wrong.
			std::abort();
		}
	}

	/// Specialization for std::function objects which return void.
	template <>
	CELERO_EXPORT void DoNotOptimizeAway(std::function<void(void)>&& x);

	///
	/// Quick definition of the number of microseconds per second.
	///
	constexpr uint64_t UsPerSec(1000000);

	///
	/// Conversion from Microseconds to Seconds.
	///
	constexpr double UsToSec{1.0e-6};

	///
	/// Drop-in replacement for std::rand();
	///
	CELERO_EXPORT int Random();
} // namespace celero

#endif
