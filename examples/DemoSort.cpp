#include <celero/Celero.h>
#include <algorithm>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN;

///
/// \class	DemoSortFixture
///	\autho	John Farrier
/// 
///	\brief	A Celero Test Fixture for sorting functions.
///
/// This test fixture will build a problem set of powers of two.  When executed,
/// the selected problem set is used to create an array of the given size, then
/// push random integers into the array.  Each sorting function will then
/// sort the randomly generated array for timing.
///
///	This demo highlights how to use the ProblemSetValues to build automatic
/// test cases which can scale.  These test cases should ideally be written
/// to a file when executed.  The resulting CSV file can be easily plotted
/// in an application such as Microsoft Excel to show how various
/// tests performed as their problem set scaled.
///
/// \code
/// celeroDemo outfile.csv
/// \endcode
///
class DemoSortFixture : public celero::TestFixture
{
	public:
		DemoSortFixture()
		{
			// We will run some total number of sets of tests all together. 
			// Each one growing by a power of 2.
			const int totalNumberOfTests = 6;

			for(int i = 0; i < totalNumberOfTests; i++)
			{
				// ProblemSetValues is part of the base class and allows us to specify
				// some values to control various test runs to end up building a nice graph.
				this->ProblemSetValues.push_back(static_cast<int32_t>(pow(2, i+1)));
			}
		}

		/// Before each run, build a vector of random integers.
		virtual void SetUp(const int32_t problemSetValue)
		{
			this->arraySize = problemSetValue;

			for(int i = 0; i < this->arraySize; i++)
			{
				this->array.push_back(rand());
			}
		}

		/// After each run, clear the vector of random integers.
		virtual void TearDown()
		{
			this->array.clear();
		}

		std::vector<int> array;
		int arraySize;
};

// For a baseline, I'll choose Bubble Sort.
BASELINE_F(DemoSort, BubbleSort, DemoSortFixture, 0, 10000)
{
	for(int x = 0; x < this->arraySize; x++)
	{
		for(int y = 0; y < this->arraySize - 1; y++)
		{
			if(this->array[y] > this->array[y+1])
			{
				std::swap(this->array[y], this->array[y+1]);
			}
		}
	}
}

BENCHMARK_F(DemoSort, SelectionSort, DemoSortFixture, 0, 10000)
{
	for(int x = 0; x < this->arraySize; x++)
	{
		auto minIdx = x;

		for(int y = x; y < this->arraySize; y++)
		{
			if(this->array[minIdx] > this->array[y])
			{
				minIdx = y;
			}
		}

		std::swap(this->array[x], this->array[minIdx]);
	}
}

BENCHMARK_F(DemoSort, stdSort, DemoSortFixture, 0, 10000)
{
	std::sort(this->array.begin(), this->array.end());
}
