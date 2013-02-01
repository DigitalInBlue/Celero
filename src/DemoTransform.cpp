#include <celero/Celero.h>

#include <algorithm>
#include <functional>

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
				this->arrayIn.push_back(rand());
			}

			this->arrayOut.reserve(this->arraySize);
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
