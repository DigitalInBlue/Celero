#include <celero/Celero.h>
#include <celero/Benchmark.h>

#include <thread>
#include <chrono>

#ifndef WIN32
#include <cstdlib>
#include <cmath>
#endif

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

///
/// What we want to find out is what is the "fastest" way to disable compiler optimization of trivial operations.
/// The basic form of the functions being tested is this:
///
/// \verbatim
/// 
///	#ifndef WIN32
///	#include <unistd.h>
///	#endif
///	
///	#include <cstdlib>
///	#include <thread>
///	#include <functional>
///	#include <chrono>
///	#include <atomic>
///	#include <stdint.h>
///	
///	template<class T> void DoNotOptimizeAway1(T&& x)
///	{
///	  x();
///	  
///	  if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
///	  {
///	     	const auto* p = &x;
///	      putchar(*reinterpret_cast<const char*>(p));
///	      std::abort();
///	  }
///	}
///	
///	void Benchmark()
///	{
///	  int x = 0;
///	  
///	  for(int i = 0; i < 512; i++)
///	  {
///	    DoNotOptimizeAway1([&x](){x += 1;});
///	  }
///	}
///	
///
/// \endverbatim
///
/// Generated Assembly is shown before each of the benchmarks based on gcc.godbolt output of gcc 4.8
///
BASELINE(DemoDNOA, Optimized, 30, 67108864)
{
	int x = 0;
	x++;
}

///
/// \verbatim
///	Benchmark():
///		pushq	%rbx
///		xorl	%eax, %eax
///		movl	$512, %ebx
///		subq	$16, %rsp
///		jmp	.L4
///	.L8:
///		movl	12(%rsp), %eax
///	.L4:
///		addl	$1, %eax
///		movl	%eax, 12(%rsp)
///		call	std::chrono::_V2::system_clock::now()
///		testq	%rax, %rax
///		je	.L7
///		subl	$1, %ebx
///		jne	.L8
///		addq	$16, %rsp
///		popq	%rbx
///		ret
///	.L7:
///		leaq	12(%rsp), %rdi
///		movq	stdout(%rip), %rsi
///		movsbl	%dil, %edi
///		call	_IO_putc
///		call	abort
/// \endverbatim
///	
BENCHMARK(DemoDNOA, Chrono, 30, 67108864)
{
	int x = 0;

	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));
		std::abort();
	}
}

///
/// \verbatim
///	Benchmark():
///		pushq	%rbx
///		xorl	%eax, %eax
///		movl	$512, %ebx
///		subq	$16, %rsp
///		jmp	.L4
///	.L8:
///		movl	12(%rsp), %eax
///	.L4:
///		addl	$1, %eax
///		movl	%eax, 12(%rsp)
///		call	__gthrw_pthread_self()
///		testq	%rax, %rax
///		je	.L7
///		subl	$1, %ebx
///		jne	.L8
///		addq	$16, %rsp
///		popq	%rbx
///		ret
///	.L7:
///		leaq	12(%rsp), %rdi
///		movq	stdout(%rip), %rsi
///		movsbl	%dil, %edi
///		call	_IO_putc
///		call	abort
/// \endverbatim
///	
BENCHMARK(DemoDNOA, Thread, 30, 100000)
{
	int x = 0;

	if(std::this_thread::get_id() == std::thread::id())
	{
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));
		std::abort();
	}
}

///
/// \verbatim
///	Benchmark():
///		pushq	%rbx
///		xorl	%eax, %eax
///		movl	$512, %ebx
///		subq	$16, %rsp
///		jmp	.L7
///	.L3:
///		cmpq	$0, void DoNotOptimizeAway1<Benchmark()::{lambda()#1}>(Benchmark()::{lambda()#1}&&)::ttid(%rip)
///		je	.L13
///		subl	$1, %ebx
///		je	.L1
///		movl	12(%rsp), %eax
///	.L7:
///		addl	$1, %eax
///		cmpb	$0, guard variable for void DoNotOptimizeAway1<Benchmark()::{lambda()#1}>(Benchmark()::{lambda()#1}&&)::ttid(%rip)
///		movl	%eax, 12(%rsp)
///		jne	.L3
///		movl	guard variable for void DoNotOptimizeAway1<Benchmark()::{lambda()#1}>(Benchmark()::{lambda()#1}&&)::ttid, %edi
///		call	__cxa_guard_acquire
///		testl	%eax, %eax
///		je	.L3
///		call	__gthrw_pthread_self()
///		movl	guard variable for void DoNotOptimizeAway1<Benchmark()::{lambda()#1}>(Benchmark()::{lambda()#1}&&)::ttid, %edi
///		movq	%rax, void DoNotOptimizeAway1<Benchmark()::{lambda()#1}>(Benchmark()::{lambda()#1}&&)::ttid(%rip)
///		call	__cxa_guard_release
///		jmp	.L3
///	.L1:
///		addq	$16, %rsp
///		popq	%rbx
///		ret
///	.L13:
///		leaq	12(%rsp), %rdi
///		movq	stdout(%rip), %rsi
///		movsbl	%dil, %edi
///		call	_IO_putc
///		call	abort
/// \endverbatim
///	
BENCHMARK(DemoDNOA, StaticThread, 30, 67108864)
{
	int x = 0;

	static auto ttid = std::this_thread::get_id();
	if(ttid == std::thread::id())
	{
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));
		std::abort();
	}
}

BENCHMARK(DemoDNOA, DNOA, 30, 67108864)
{
	int x = 0;
	celero::DoNotOptimizeAway([&](){x++;});
}