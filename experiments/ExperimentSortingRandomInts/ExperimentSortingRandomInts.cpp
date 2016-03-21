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
CELERO_MAIN

///
/// \class	SortFixture
///	\autho	John Farrier
/// 
///	\brief	A Celero Test Fixture for sorting functions.
///
/// This test fixture will build a experiment of powers of two.  When executed,
/// the selected experiment is used to create an array of the given size, then
/// push random integers into the array.  Each sorting function will then
/// sort the randomly generated array for timing.
///
///	This demo highlights how to use the ExperimentValues to build automatic
/// test cases which can scale.  These test cases should ideally be written
/// to a file when executed.  The resulting CSV file can be easily plotted
/// in an application such as Microsoft Excel to show how various
/// tests performed as their experiment scaled.
///
/// \code
/// celeroDemo outfile.csv
/// \endcode
///
class SortFixture : public celero::TestFixture
{
	public:
		SortFixture()
		{
		}

		virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const override
		{
			std::vector<std::pair<int64_t, uint64_t>> problemSpace;

			// We will run some total number of sets of tests all together. 
			// Each one growing by a power of 2.
			const int totalNumberOfTests = 6;

			for(int i = 0; i < totalNumberOfTests; i++)
			{
				// ExperimentValues is part of the base class and allows us to specify
				// some values to control various test runs to end up building a nice graph.
				problemSpace.push_back(std::make_pair(int64_t(pow(2, i+1)), uint64_t(0)));
			}

			return problemSpace;
		}

		/// Before each run, build a vector of random integers.
		virtual void setUp(int64_t experimentValue)
		{
			this->arraySize = experimentValue;
			this->array.reserve(this->arraySize);
		}

		/// Before each iteration. A common utility function to push back random ints to sort.
		void randomize()
		{
			for(int i = 0; i < this->arraySize; i++)
			{
				this->array.push_back(rand());
			}
		}

		/// After each iteration, clear the vector of random integers.
		void clear()
		{
			this->array.clear();
		}

		std::vector<int64_t> array;
		int64_t arraySize;
};

// For a baseline, I'll choose Bubble Sort.
BASELINE_F(SortRandInts, BubbleSort, SortFixture, 30, 10000)
{
	this->randomize();

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

	this->clear();
}

BENCHMARK_F(SortRandInts, SelectionSort, SortFixture, 30, 10000)
{
	this->randomize();

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

	this->clear();
}

// http://www.bfilipek.com/2014/12/top-5-beautiful-c-std-algorithms.html
BENCHMARK_F(SortRandInts, InsertionSort, SortFixture, 30, 10000)
{
	this->randomize();

	for(auto i = std::begin(this->array); i != std::end(this->array); ++i)
	{
		std::rotate(std::upper_bound(std::begin(this->array), i, *i), i, std::next(i));
	}
	
	this->clear();
}

// http://www.bfilipek.com/2014/12/top-5-beautiful-c-std-algorithms.html
template<class FwdIt, typename U>
void quickSort(FwdIt first, FwdIt last, U cmp = U())
{
	auto const N = std::distance(first, last);
	if(N <= 1) return;
	auto const pivot = std::next(first, N / 2);
	std::nth_element(first, pivot, last, cmp);
	quickSort(first, pivot, cmp);
	quickSort(pivot, last, cmp);

}

BENCHMARK_F(SortRandInts, QuickSort, SortFixture, 30, 10000)
{
	this->randomize();

	quickSort(std::begin(this->array), std::end(this->array), std::less<int64_t>());

	this->clear();
}

BENCHMARK_F(SortRandInts, stdSort, SortFixture, 30, 10000)
{
	this->randomize();

	std::sort(this->array.begin(), this->array.end());
	
	this->clear();
}
