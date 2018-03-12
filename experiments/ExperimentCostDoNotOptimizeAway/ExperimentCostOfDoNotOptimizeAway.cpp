#include <celero/Celero.h>
#include <celero/Utilities.h>

#include <chrono>
#include <thread>

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

// What implementation of "DoNotOptimizeAway" has the smallest cost while still being effective?

static const int SamplesCount = 10000;
static const int IterationsCount = 10000;

// The number of iterations is very large because the operation is very quick.  We want to
// look at a large enough chunk of time to reduce the reliance on the resolution of the
// clock.  Then, do 30 samples of this large number of runs and keep the quickest.
BASELINE(CostOfDNOA, Baseline, SamplesCount, IterationsCount)
{
	// GCC and Clang Optimize to:
	// mov DWORD PTR[rsp - 4], 1024
	// ret
	volatile int x{1024};
	x;
}

BENCHMARK(CostOfDNOA, VarImpl, SamplesCount, IterationsCount)
{
	int x{1024};
	celero::DoNotOptimizeAway(x);
}

BENCHMARK(CostOfDNOA, VarChrono, SamplesCount, IterationsCount)
{
	int x{1024};

	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		// This forces the value to never be optimized away
		// by taking a reference then using it.
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));

		// If we do get here, kick out because something has gone wrong.
		std::abort();
	}
}

BENCHMARK(CostOfDNOA, VarThreadID, SamplesCount, IterationsCount)
{
	int x{1024};

	if(std::this_thread::get_id() != std::this_thread::get_id())
	{
		// This forces the value to never be optimized away
		// by taking a reference then using it.
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));

		// If we do get here, kick out because something has gone wrong.
		std::abort();
	}
}

BENCHMARK(CostOfDNOA, VarVolatile, SamplesCount, IterationsCount)
{
	int x{1024};
	volatile decltype(x) foo = x;
	foo;
}

BENCHMARK(CostOfDNOA, LambdaImpl, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway([]() {
		int x{1024};
		return x;
	});
}

BENCHMARK(CostOfDNOA, LambdaVoidImpl, SamplesCount, IterationsCount)
{
	// Because the lambda does not return, it will be completely optimized away.
	celero::DoNotOptimizeAway([]() {
		int x{1024};
		x;
	});
}

BENCHMARK(CostOfDNOA, LambdaChrono, SamplesCount, IterationsCount)
{
	auto x = []() {
		int x{1024};
		return x;
	};

	x();

	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		// This forces the value to never be optimized away
		// by taking a reference then using it.
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));

		// If we do get here, kick out because something has gone wrong.
		std::abort();
	}
}

BENCHMARK(CostOfDNOA, LambdaThreadID, SamplesCount, IterationsCount)
{
	auto x = []() {
		int x{1024};
		return x;
	};

	x();

	if(std::this_thread::get_id() != std::this_thread::get_id())
	{
		// This forces the value to never be optimized away
		// by taking a reference then using it.
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));

		// If we do get here, kick out because something has gone wrong.
		std::abort();
	}
}

BENCHMARK(CostOfDNOA, LambdaVolatile, SamplesCount, IterationsCount)
{
	// GCC and Clang Optimize to:
	// mov DWORD PTR[rsp - 4], 1024
	//	ret

	auto x = []() {
		int x{1024};
		return x;
	};

	volatile auto foo = x();
	foo;
}
