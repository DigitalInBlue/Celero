#include <celero/Celero.h>
#include <celero/Utilities.h>

#include <chrono>
#include <thread>

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

//
// Baseline with a non-pimpl'd class.
//

class NoImpl
{
public:
	NoImpl() : value(0)
	{
	}

	int getValue() const
	{
		return this->value;
	}

	void setValue(int x)
	{
		this->value = x;
	}

	int value;
};

//
// Define an Impl class to use in several of our tests.
//

class GenericImpl
{
public:
	GenericImpl() : value(0)
	{
	}

	int value;
};

//
// Raw Pointer implementation
//

class RawPointer
{
public:
	RawPointer() : pimpl(new GenericImpl)
	{
	}

	RawPointer(const RawPointer&) = delete;

	RawPointer(RawPointer&& other)
	{
		this->pimpl = other.pimpl;
		other.pimpl = nullptr;
	}

	~RawPointer()
	{
		delete pimpl;
	}

	int getValue() const
	{
		return this->pimpl->value;
	}

	void setValue(int x)
	{
		this->pimpl->value = x;
	}

private:
	GenericImpl* pimpl;
};

//
// std::unique_ptr<> implementation
//

class UniquePointer
{
public:
	UniquePointer() : pimpl(std::make_unique<GenericImpl>())
	{
	}

	int getValue() const
	{
		return this->pimpl->value;
	}

	void setValue(int x)
	{
		this->pimpl->value = x;
	}

private:
	std::unique_ptr<GenericImpl> pimpl;
};

//
// Create a Herb Sutter'esque Pimpl Class
//

#include <celero/Pimpl.h>

class SutterPointer
{
public:
	int getValue() const
	{
		return this->pimpl->value;
	}

	void setValue(int x)
	{
		this->pimpl->value = x;
	}

private:
	class Impl
	{
	public:
		Impl() : value(0)
		{
		}

		int value;
	};

	celero::Pimpl<SutterPointer::Impl> pimpl;
};

#include <celero/PimplImpl.h>

//
// A basic test fixture for our tests to share.
//

class DemoFixture : public celero::TestFixture
{
public:
	NoImpl noPtr;
	RawPointer rawPtr;
	UniquePointer uniquePtr;
	SutterPointer sutterPtr;
};

static const int SamplesCount = 10000;
static const int IterationsCount = 100000;

// The number of iterations is very large because the operation is very quick.  We want to
// look at a large enough chunk of time to reduce the reliance on the resolution of the
// clock.  Then, do 30 samples of this large number of runs and keep the quickest.
BASELINE_F(CostOfPimplGet, Baseline, DemoFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(noPtr.getValue());
}

BENCHMARK_F(CostOfPimplGet, RawPtr, DemoFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(rawPtr.getValue());
}

BENCHMARK_F(CostOfPimplGet, UniquePtr, DemoFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(uniquePtr.getValue());
}

BENCHMARK_F(CostOfPimplGet, SutterPtr, DemoFixture, SamplesCount, IterationsCount)
{
	celero::DoNotOptimizeAway(sutterPtr.getValue());
}

// Set

BASELINE_F(CostOfPimplSet, Baseline, DemoFixture, SamplesCount, IterationsCount)
{
	noPtr.setValue(5);
}

BENCHMARK_F(CostOfPimplSet, RawPtr, DemoFixture, SamplesCount, IterationsCount)
{
	rawPtr.setValue(6);
}

BENCHMARK_F(CostOfPimplSet, UniquePtr, DemoFixture, SamplesCount, IterationsCount)
{
	uniquePtr.setValue(7);
}

BENCHMARK_F(CostOfPimplSet, SutterPtr, DemoFixture, SamplesCount, IterationsCount)
{
	sutterPtr.setValue(8);
}
