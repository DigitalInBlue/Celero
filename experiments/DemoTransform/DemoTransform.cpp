#include <celero/Celero.h>
#include <celero/Benchmark.h>

#include <algorithm>
#include <functional>

#ifndef WIN32
#include <cstdlib>
#include <cmath>
#endif

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

///
/// \class	DemoTransformFixture
///	\autho	John Farrier
/// 
///	\brief	A Celero Test Fixture for array transforms.
///
/// This test fixture will build a experiment of powers of two.  When executed,
/// the selected experiment is used to create an array of the given size, then
/// push random integers into the array.  Each transform function will then
/// modify the randomly generated array for timing.
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
class DemoTransformFixture : public celero::TestFixture
{
	public:
		enum Constants
		{
			Multiple = 2112
		};

		DemoTransformFixture()
		{
		}

		virtual std::vector<int64_t> getExperimentValues() const
		{
			std::vector<int64_t> problemSpaceValues;

			// We will run some total number of sets of tests all together. 
			// Each one growing by a power of 2.
			const int totalNumberOfTests = 12;

			for(int i = 0; i < totalNumberOfTests; i++)
			{
				// ExperimentValues is part of the base class and allows us to specify
				// some values to control various test runs to end up building a nice graph.
				problemSpaceValues.push_back(static_cast<int64_t>(pow(2, i+1)));
			}

			return problemSpaceValues;
		}

		/// Before each run, build a vector of random integers.
		virtual void setUp(int64_t experimentValue)
		{
			this->arraySize = static_cast<int>(experimentValue);

			for(int i = 0; i < this->arraySize; i++)
			{
				this->arrayIn.push_back(rand());
			}

			this->arrayOut.resize(this->arraySize);
		}

		/// After each run, clear the vector of random integers.
		virtual void tearDown()
		{
			this->arrayIn.clear();
			this->arrayOut.clear();
		}

		std::vector<int> arrayIn;
		std::vector<int> arrayOut;
		int arraySize;
};

// For a baseline, I'll chose Bubble Sort.
BASELINE_F(DemoTransform, ForLoop, DemoTransformFixture, 30, 10000)
{
	for(int i = 0; i < this->arraySize; i++)
	{
		this->arrayOut[i] = this->arrayIn[i] * DemoTransformFixture::Multiple;
	}
}

BENCHMARK_F(DemoTransform, StdTransform, DemoTransformFixture, 30, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayOut.begin(), std::bind1st(std::multiplies<int>(), DemoTransformFixture::Multiple));
}

BENCHMARK_F(DemoTransform, StdTransformLambda, DemoTransformFixture, 30, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayOut.begin(), 
		[](int in)->int
		{
			return in * DemoTransformFixture::Multiple;
		});
}

BENCHMARK_F(DemoTransform, SelfForLoop, DemoTransformFixture, 30, 10000)
{
	for(int i = 0; i < this->arraySize; i++)
	{
		this->arrayIn[i] *= DemoTransformFixture::Multiple;
	}
}

BENCHMARK_F(DemoTransform, SelfStdTransform, DemoTransformFixture, 30, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayIn.begin(), std::bind1st(std::multiplies<int>(), DemoTransformFixture::Multiple));
}

BENCHMARK_F(DemoTransform, SelfStdTransformLambda, DemoTransformFixture, 30, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayIn.begin(), 
		[](int in)->int
		{
			return in * DemoTransformFixture::Multiple;
		});
}
