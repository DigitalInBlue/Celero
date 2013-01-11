#include <celero/Celero.h>

CELERO_MAIN;

class TestFixture : public celero::TestFixture
{
	public:
		virtual void SetUp()
		{
		}

		virtual void TearDown()
		{
		}
};

// Run an automatic baseline.  
// Celero will help make sure enough samples are taken to get a reasonable measurement
BASELINE(CeleroBenchTest, Baseline, 0, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(3.14159265)));
}

// Run an automatic test.  
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(CeleroBenchTest, Complex1, 0, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(rand(), 3.14159265))));
}

// Run a manual test consisting of 1 sample of 7100000 operations per measurement.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(CeleroBenchTest, Complex2, 1, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(rand(), 3.14159265))));
}

// Run a manual test consisting of 60 samples of 7100000 operations per measurement.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK(CeleroBenchTest, Complex3, 60, 7100000)
{
	celero::DoNotOptimizeAway(static_cast<float>(sin(fmod(rand(), 3.14159265))));
}
