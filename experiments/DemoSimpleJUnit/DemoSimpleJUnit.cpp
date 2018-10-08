#include <celero/Celero.h>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

/// Run an automatic baseline.
/// In reality, all of the "Complex" cases take the same amount of time to run.
/// The difference in the results is a product of measurement error.
BASELINE(DemoSimple, Baseline, 1, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(3.14159265)));
}

/// Run an automatic test.
/// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK_TEST(DemoSimple, Complex1, 1, 7100000, 1.0)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(celero::Random(), 3.14159265))));
}

/// Run a manual test consisting of 1 sample of 7100000 iterations per measurement.
BENCHMARK_TEST(DemoSimple, Complex2, 1, 7100000, 3.71)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(celero::Random(), 3.14159265))));
}

/// Run a manual test consisting of 60 samples of 7100000 iterations per measurement.
BENCHMARK_TEST(DemoSimple, Complex3, 60, 7100000, 5.0)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(celero::Random(), 3.14159265))));
}
