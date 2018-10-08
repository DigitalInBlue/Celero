#include <celero/Celero.h>

#include <random>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

std::random_device RandomDevice;
std::uniform_int_distribution<int> UniformDistribution(0, 1024);

/// In reality, all of the "Complex" cases take the same amount of time to run.
/// The difference in the results is a product of measurement error.
///
/// Interestingly, taking the sin of a constant number here resulted in a
/// great deal of optimization in clang and gcc.
BASELINE(DemoSimple, Baseline, 30, 1000000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}

/// Run a test consisting of 0 samples of 0 iterations per measurement.
/// Since the sample size is equal to 0, celero will compute a number to use for both samples and iterations.
BENCHMARK(DemoSimple, Complex1, 0, 0)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}

/// Run a test consisting of 30 samples of 710000 iterations per measurement.
/// There are not enough samples here to get a reasonable measurement
/// It should get a Basline number lower than the previous test.
BENCHMARK(DemoSimple, Complex2, 30, 710000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}

/// Run a test consisting of 60 samples of 710000 iterations per measurement.
/// There are not enough samples here to get a reasonable measurement
/// It should get a Basline number lower than the previous test.
BENCHMARK(DemoSimple, Complex3, 60, 710000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(UniformDistribution(RandomDevice), 3.14159265))));
}

/// Run a test consisting of 60 samples of 7100000 iterations per measurement.
/// Samples here are much large and there are more of them.  Expect a better answer here.
/*
BENCHMARK(DemoSimple, Complex4, 600, 20000000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(celero::Random(), 3.14159265))));
}
*/
