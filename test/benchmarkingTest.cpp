#include <celero/Celero.h>
#include <array>

CELERO_MAIN;

class ArrayFixture : public celero::TestFixture
{
	public:
		virtual void SetUp()
		{
			arraySize = 1024;
		}

		std::array<int, 1024> myArray;
		int arraySize;
};

class ClassicFixture : public celero::TestFixture
{
	public:
		virtual void SetUp()
		{
			arraySize = 1024;
			myArray = new int[arraySize];
		}

		virtual void TearDown()
		{
			delete [] myArray;
		}

		int* myArray;
		int arraySize;
};

class ArrayFixture2D : public celero::TestFixture
{
	public:
		virtual void SetUp()
		{
			arraySize = 32;
		}

		std::array<std::array<int, 32>, 32> myArray;
		int arraySize;
};

class ClassicFixture2D : public celero::TestFixture
{
	public:
		virtual void SetUp()
		{
			arraySize = 32;
		}

		virtual void TearDown()
		{
		}

		int myArray[32][32];
		int arraySize;
};

// Run an automatic baseline. 
// Celero will help make sure enough samples are taken to get a reasonable measurement
BASELINE_F(ArrayTest, standardArray, ArrayFixture, 0, 50000)
{
	for(int i = 0; i < this->arraySize; i ++)
	{
		celero::DoNotOptimizeAway(this->myArray[i]);
	}
}

// Run an automatic test.  
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK_F(ArrayTest, classicArray, ClassicFixture, 0, 50000)
{
	for(int i = 0; i < this->arraySize; i ++)
	{
		celero::DoNotOptimizeAway(myArray[i]);
	}
}

BENCHMARK_F(ArrayTest, 2DStandardArray, ArrayFixture2D, 0, 50000)
{
	for(int i = 0; i < this->arraySize; i++)
	{
		for(int j = 0; j < this->arraySize; j++)
		{
			celero::DoNotOptimizeAway(myArray[i][j]);
		}
	}
}

BENCHMARK_F(ArrayTest, 2DClassicArray, ClassicFixture2D, 0, 50000)
{
	for(int i = 0; i < this->arraySize; i++)
	{
		for(int j = 0; j < this->arraySize; j++)
		{
			celero::DoNotOptimizeAway(myArray[i][j]);
		}
	}
}

BENCHMARK_F(ArrayTest, 2DClassicArraySwap, ClassicFixture2D, 0, 50000)
{
	for(int i = 0; i < this->arraySize; i++)
	{
		for(int j = 0; j < this->arraySize; j++)
		{
			celero::DoNotOptimizeAway(myArray[j][i]);
		}
	}
}
