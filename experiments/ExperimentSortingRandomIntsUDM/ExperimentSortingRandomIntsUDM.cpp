#include <celero/Celero.h>

#include <algorithm>
#include <iostream>
#include <string>

#ifndef _WIN32
#include <cmath>
#include <cstdlib>
#endif

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

///
/// Keep track of how many times an integer is copied during sorting.
///
class CopyCountingInt
{
public:
	explicit CopyCountingInt(int64_t v) : val(v)
	{
	}

	CopyCountingInt() : val(0)
	{
	}

	CopyCountingInt(const CopyCountingInt& other) : val(other.val)
	{
		CopyCountingInt::copiesCounted++;
	}

	~CopyCountingInt()
	{
	}

	operator int64_t()
	{
		return val;
	}

	CopyCountingInt& operator=(const CopyCountingInt& other)
	{
		this->val = other.val;
		CopyCountingInt::copiesCounted++;
		return *this;
	}

	static void resetCount()
	{
		CopyCountingInt::copiesCounted = 0;
	}

	static size_t getCount()
	{
		return CopyCountingInt::copiesCounted;
	}

	int64_t val{0};

private:
	static size_t copiesCounted;
};

size_t CopyCountingInt::copiesCounted{0};

bool operator<(const CopyCountingInt& lhs, const CopyCountingInt& rhs)
{
	return lhs.val < rhs.val;
}

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
/// celeroExperimentSortingRandomIntsUDM --outputTable udm.csv
/// \endcode
///
class SortFixture : public celero::TestFixture
{
public:
	class CopyCountUDM : public celero::UserDefinedMeasurementTemplate<size_t>
	{
		std::string getName() const override
		{
			return "Copies";
		}

		// Turn off some of the output reporting.
		bool reportStandardDeviation() const override
		{
			return false;
		}

		bool reportSkewness() const override
		{
			return false;
		}

		bool reportKurtosis() const override
		{
			return false;
		}

		bool reportZScore() const override
		{
			return false;
		}
	};

	/// Used for debugging multiple UDM's
	class RandomUDM : public celero::UserDefinedMeasurementTemplate<size_t>
	{
		std::string getName() const override
		{
			return "Random";
		}

		// Turn off some of the output reporting.
		bool reportStandardDeviation() const override
		{
			return false;
		}

		bool reportSkewness() const override
		{
			return false;
		}

		bool reportKurtosis() const override
		{
			return false;
		}

		bool reportZScore() const override
		{
			return false;
		}
	};

	SortFixture()
	{
	}

	std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
	{
		std::vector<celero::TestFixture::ExperimentValue> problemSpace;

		// ExperimentValues is part of the base class and allows us to specify
		// some values to control various test runs to end up building a nice graph.
		for(int64_t elements = 64; elements <= int64_t(2048); elements *= 2)
		{
			problemSpace.push_back(elements);
		}

		return problemSpace;
	}

	/// Before each sample, build a vector of random integers.
	void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		this->arraySize = experimentValue.Value;
	}

	// Before each iteration
	void onExperimentStart(const celero::TestFixture::ExperimentValue&) override
	{
		this->array.resize(this->arraySize);
		CopyCountingInt::resetCount();

		for(int i = 0; i < this->arraySize; i++)
		{
			this->array[i] = CopyCountingInt(celero::Random());
		}
	}

	void tearDown() override
	{
	}

	void onExperimentEnd() override
	{
		/// After each iteration, clear the vector of random integers.
		this->array.clear();

		this->copyCountUDM->addValue(CopyCountingInt::getCount());
		CopyCountingInt::resetCount();

		this->randomUDM->addValue(static_cast<size_t>(rand()));
	}

	std::vector<std::shared_ptr<celero::UserDefinedMeasurement>> getUserDefinedMeasurements() const override
	{
		return {this->copyCountUDM, this->randomUDM};
	}

	std::vector<CopyCountingInt> array;
	int64_t arraySize{0};

	std::shared_ptr<CopyCountUDM> copyCountUDM{new CopyCountUDM};
	std::shared_ptr<RandomUDM> randomUDM{new RandomUDM};
};

static const int SamplesCount = 2000;
static const int IterationsCount = 4;

// For a baseline, I'll choose Bubble Sort.
BASELINE_F(SortRandInts, BubbleSort, SortFixture, SamplesCount, IterationsCount)
{
	for(int x = 0; x < this->arraySize; x++)
	{
		for(int y = 0; y < this->arraySize - 1; y++)
		{
			if(this->array[y] > this->array[y + 1])
			{
				std::swap(this->array[y], this->array[y + 1]);
			}
		}
	}
}

BENCHMARK_F(SortRandInts, SelectionSort, SortFixture, SamplesCount, IterationsCount)
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

// http://www.bfilipek.com/2014/12/top-5-beautiful-c-std-algorithms.html
BENCHMARK_F(SortRandInts, InsertionSort, SortFixture, SamplesCount, IterationsCount)
{
	for(auto i = std::begin(this->array); i != std::end(this->array); ++i)
	{
		std::rotate(std::upper_bound(std::begin(this->array), i, *i), i, std::next(i));
	}
}

// http://www.bfilipek.com/2014/12/top-5-beautiful-c-std-algorithms.html
template <class FwdIt, typename U>
void QuickSort(FwdIt first, FwdIt last, U cmp = U())
{
	auto const N = std::distance(first, last);

	if(N <= 1)
	{
		return;
	}

	auto const pivot = std::next(first, N / 2);
	std::nth_element(first, pivot, last, cmp);
	QuickSort(first, pivot, cmp);
	QuickSort(pivot, last, cmp);
}

BENCHMARK_F(SortRandInts, QuickSort, SortFixture, SamplesCount, IterationsCount)
{
	QuickSort(std::begin(this->array), std::end(this->array), std::less<int64_t>());
}

BENCHMARK_F(SortRandInts, stdSort, SortFixture, SamplesCount, IterationsCount)
{
	std::sort(std::begin(this->array), std::end(this->array));
}
