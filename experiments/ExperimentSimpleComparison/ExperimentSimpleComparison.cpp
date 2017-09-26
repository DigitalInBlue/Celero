#include <random>

#include <celero/Celero.h>

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

///
/// An additional demo that was created for a Stack Overflow question:
///
/// http://stackoverflow.com/questions/15770049/micro-optimizing-a-c-comparison-function/
///
/// The code here has been updated from the original post to utilize std::random for better sampling.
///
class StackOverflowFixture : public celero::TestFixture
{
public:
	StackOverflowFixture() : Gen(RandomDevice()), RandomBool(0.5), RandomInt(-100, 100)
	{
	}

	inline bool NoOp(bool, int, int)
	{
		return true;
	}

	inline bool Compare(bool greater, int p1, int p2)
	{
		if(greater == true)
		{
			return (p1 >= p2);
		}

		return (p1 <= p2);
	}

	// Code for Compare 2
	inline bool Compare2(bool greater, int p1, int p2)
	{
		bool ret[2] = {p1 <= p2, p1 >= p2};
		return ret[greater];
	}
	// END

	// Code for Compare 3
	inline bool Compare3(bool greater, int p1, int p2)
	{
		return (!greater != !(p1 <= p2)) | (p1 == p2);
	}
	// END

	inline bool Compare4(bool greater, int p1, int p2)
	{
		return (greater ^ (p1 <= p2)) | (p1 == p2);
	}

	// Improved with Bernoulli distribution of booleans.
	std::random_device RandomDevice;
	std::mt19937 Gen;

	// give "true" 1/2 of the time
	std::bernoulli_distribution RandomBool;
	std::uniform_int_distribution<int> RandomInt;
};

static const int SamplesCount = 10000;
static const int IterationsCount = 2000;

// Shortened up the name from "StackOverflowSimpleComparison"
BASELINE_F(SOSimpleComparison, Baseline, StackOverflowFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(NoOp(this->RandomBool(this->Gen), this->RandomInt(this->Gen), this->RandomInt(this->Gen)));
}

BENCHMARK_F(SOSimpleComparison, Compare, StackOverflowFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(Compare(this->RandomBool(this->Gen), this->RandomInt(this->Gen), this->RandomInt(this->Gen)));
}

BENCHMARK_F(SOSimpleComparison, Compare2, StackOverflowFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(Compare2(this->RandomBool(this->Gen), this->RandomInt(this->Gen), this->RandomInt(this->Gen)));
}

BENCHMARK_F(SOSimpleComparison, Compare3, StackOverflowFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(Compare3(this->RandomBool(this->Gen), this->RandomInt(this->Gen), this->RandomInt(this->Gen)));
}

BENCHMARK_F(SOSimpleComparison, Compare4, StackOverflowFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(Compare4(this->RandomBool(this->Gen), this->RandomInt(this->Gen), this->RandomInt(this->Gen)));
}
