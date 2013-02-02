#include <celero/Celero.h>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

CELERO_MAIN;

// Run an automatic baseline.  
// Celero will help make sure enough samples are taken to get a reasonable measurement
BASELINE(DemoSimple, Baseline, 0, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(3.14159265)));
}

// In reality, all of the "Complex" cases take the same amount of time to run.
// The difference in the results is a product of measurement error.

// Run an automatic test.  
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(DemoSimple, Complex1, 0, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(rand(), 3.14159265))));
}

// Run a manual test consisting of 1 sample of 7100000 operations per measurement.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(DemoSimple, Complex2, 1, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(rand(), 3.14159265))));
}

// Run a manual test consisting of 60 samples of 7100000 operations per measurement.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(DemoSimple, Complex3, 60, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(rand(), 3.14159265))));
}
