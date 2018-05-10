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

BASELINE(DNOA, Baseline, SamplesCount, IterationsCount)
{
	volatile std::vector<int> x(1024);
	x;
}

BENCHMARK(DNOA, VarImpl, SamplesCount, IterationsCount)
{
	std::vector<int> x(1024);
	celero::DoNotOptimizeAway(x);
}

BENCHMARK(DNOA, VarChrono, SamplesCount, IterationsCount)
{
	std::vector<int> x(1024);

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

BENCHMARK(DNOA, VarThreadID, SamplesCount, IterationsCount)
{
	std::vector<int> x(1024);

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

BENCHMARK(DNOA, VarThreadID2, SamplesCount, IterationsCount)
{
	std::vector<int> x(1024);

	static auto ttid = std::this_thread::get_id();
	if(ttid == std::thread::id())
	{
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));
		std::abort();
	}
}

BENCHMARK(DNOA, VarVolatile, SamplesCount, IterationsCount)
{
	std::vector<int> x(1024);
	volatile decltype(x) foo = x;
	foo;
}

BENCHMARK(DNOA, LambdaImpl, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway([]() {
		std::vector<int> x(1024);
		return x;
	});
}

BENCHMARK(DNOA, LambdaVoidImpl, SamplesCount, IterationsCount)
{
	// Because the lambda does not return, it will be completely optimized away.
	celero::DoNotOptimizeAway([]() {
		std::vector<int> x(1024);
		x;
	});
}

BENCHMARK(DNOA, LambdaChrono, SamplesCount, IterationsCount)
{
	auto x = []() {
		std::vector<int> x(1024);
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

BENCHMARK(DNOA, LambdaThreadID, SamplesCount, IterationsCount)
{
	auto x = []() {
		std::vector<int> x(1024);
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

BENCHMARK(DNOA, LambdaThreadID2, SamplesCount, IterationsCount)
{
	auto x = []() {
		std::vector<int> x(1024);
		return x;
	};

	x();

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

BENCHMARK(DNOA, LambdaVolatile, SamplesCount, IterationsCount)
{
	// GCC and Clang Optimize to:
	// mov DWORD PTR[rsp - 4], 1024
	//	ret

	auto x = []() {
		std::vector<int> x(1024);
		return x;
	};

	volatile auto foo = x();
	foo;
}
