#include <celero/Celero.h>
#include <algorithm>

class DemoSortFixture : public celero::TestFixture
{
	public:
		DemoSortFixture()
		{
			// We will run some total number of sets of tests all together. Each one growing by a power of 2.
			const int totalNumberOfTests = 9;

			for(int i = 0; i < totalNumberOfTests; i++)
			{
				// ProblemSetValues is part of the base class and allows us to specify
				// some values to control various test runs to end up building a nice graph.
				this->ProblemSetValues.push_back(pow(2, i));
			}
		}

		/// Before each run, build a vector of random integers.
		virtual void SetUp(const uint32_t problemSetValue)
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

// For a baseline, I'll chose Bubble Sort.
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
