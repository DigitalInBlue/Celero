#include <celero/Celero.h>

#include <algorithm>
#include <functional>

#ifndef WIN32
#include <cstdlib>
#include <cmath>
#endif

///
/// \class	DemoTransformFixture
///	\autho	John Farrier
/// 
///	\brief	A Celero Test Fixture for array transforms.
///
/// This test fixture will build a problem set of powers of two.  When executed,
/// the selected problem set is used to create an array of the given size, then
/// push random integers into the array.  Each transform function will then
/// modify the randomly generated array for timing.
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
class DemoTransformFixture : public celero::TestFixture
{
	public:
		enum Constants
		{
			Multiple = 2112
		};

		DemoTransformFixture()
		{
			// We will run some total number of sets of tests all together. Each one growing by a power of 2.
			const int totalNumberOfTests = 12;

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
				this->arrayIn.push_back(rand());
			}

			this->arrayOut.resize(this->arraySize);
		}

		/// After each run, clear the vector of random integers.
		virtual void TearDown()
		{
			this->arrayIn.clear();
			this->arrayOut.clear();
		}

		std::vector<int> arrayIn;
		std::vector<int> arrayOut;
		int arraySize;
};

// For a baseline, I'll chose Bubble Sort.
BASELINE_F(DemoTransform, ForLoop, DemoTransformFixture, 0, 10000)
{
	for(int i = 0; i < this->arraySize; i++)
	{
		this->arrayOut[i] = this->arrayIn[i] * DemoTransformFixture::Multiple;
	}
}

BENCHMARK_F(DemoTransform, StdTransform, DemoTransformFixture, 0, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayOut.begin(), std::bind1st(std::multiplies<int>(), DemoTransformFixture::Multiple));
}

BENCHMARK_F(DemoTransform, StdTransformLambda, DemoTransformFixture, 0, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayOut.begin(), 
		[](int in)->int
		{
			return in * DemoTransformFixture::Multiple;
		});
}

BENCHMARK_F(DemoTransform, SelfForLoop, DemoTransformFixture, 0, 10000)
{
	for(int i = 0; i < this->arraySize; i++)
	{
		this->arrayIn[i] *= DemoTransformFixture::Multiple;
	}
}

BENCHMARK_F(DemoTransform, SelfStdTransform, DemoTransformFixture, 0, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayIn.begin(), std::bind1st(std::multiplies<int>(), DemoTransformFixture::Multiple));
}

BENCHMARK_F(DemoTransform, SelfStdTransformLambda, DemoTransformFixture, 0, 10000)
{
	std::transform(this->arrayIn.begin(), this->arrayIn.end(), this->arrayIn.begin(), 
		[](int in)->int
		{
			return in * DemoTransformFixture::Multiple;
		});
}
