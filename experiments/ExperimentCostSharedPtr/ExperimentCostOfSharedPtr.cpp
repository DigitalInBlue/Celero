#include <celero/Celero.h>
#include <celero/Utilities.h>

#include "osg_ref_ptr.h"

#include <array>
#include <chrono>
#include <thread>

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

//
// A class to point to.
//

constexpr int ArraySize{4096};

class Foo
{
public:
	void setData(std::array<int, ArraySize>& x)
	{
		this->data = x;
	}

	std::array<int, ArraySize> getData() const
	{
		return this->data;
	}

	// For OSG's pointer.
	int unref_nodelete() const
	{
		this->osgRefCount--;
		return this->osgRefCount;
	};

	// For OSG's pointer.
	int unref() const
	{
		if(this->unref_nodelete() == 0)
		{
			delete this;
			return 0;
		}

		return this->osgRefCount;
	};

	// For OSG's pointer.
	int ref() const
	{
		this->osgRefCount++;
		return this->osgRefCount;
	};

	// For OSG's pointer.
	inline int referenceCount() const
	{
		return this->osgRefCount;
	}

	std::array<int, ArraySize> data;

	// For OSG's pointer.
	mutable int osgRefCount = {1};
};

class DemoFixture : public celero::TestFixture
{
public:
	Foo* rawPtr = {nullptr};
	std::unique_ptr<Foo> uniquePtr;
	std::shared_ptr<Foo> sharedPtr;
	osg::ref_ptr<Foo> osgPtr;
};

//
// Act on the pointer directly using that pointer type's normal passing paradigm.
// unique_ptr's cannot be passed, so they would be passed in as raw pointers.
// shared_ptr Ptr's are usually passed as shared_ptr's.
//
template <typename T>
void Bar(T x)
{
	if(x != nullptr)
	{
		auto data = x->getData();
		data[celero::Random() % ArraySize] = celero::Random();
		x->setData(data);
	}
}

static const int SamplesCount = 1000;
static const int IterationsCount = 1000;

// ----------------------------------------------------------------------------
// Construct, Destruct

BASELINE_F(CtorDtor, Baseline, DemoFixture, SamplesCount, IterationsCount)
{
	this->rawPtr = new Foo();
	delete this->rawPtr;
}

BENCHMARK_F(CtorDtor, Unique_New, DemoFixture, SamplesCount, IterationsCount)
{
	this->uniquePtr.reset(new Foo());
	this->uniquePtr.reset();
}

BENCHMARK_F(CtorDtor, Unique_Make, DemoFixture, SamplesCount, IterationsCount)
{
	this->uniquePtr = std::make_unique<Foo>();
	this->uniquePtr.reset();
}

BENCHMARK_F(CtorDtor, Shared_New, DemoFixture, SamplesCount, IterationsCount)
{
	this->sharedPtr.reset(new Foo());
	this->sharedPtr.reset();
}

BENCHMARK_F(CtorDtor, Shared_Make, DemoFixture, SamplesCount, IterationsCount)
{
	this->sharedPtr = std::make_shared<Foo>();
	this->sharedPtr.reset();
}

BENCHMARK_F(CtorDtor, OSG_New, DemoFixture, SamplesCount, IterationsCount)
{
	this->osgPtr = new Foo;
	this->osgPtr.release();
}

// ----------------------------------------------------------------------------
// Construct, Access, Destruct

BASELINE_F(CtorAccessDtor, Baseline, DemoFixture, SamplesCount, IterationsCount)
{
	this->rawPtr = new Foo();
	Bar(this->rawPtr);
	delete this->rawPtr;
}

BENCHMARK_F(CtorAccessDtor, Unique_New, DemoFixture, SamplesCount, IterationsCount)
{
	this->uniquePtr.reset(new Foo());
	Bar(this->uniquePtr.get());
	this->uniquePtr.reset();
}

BENCHMARK_F(CtorAccessDtor, Unique_Make, DemoFixture, SamplesCount, IterationsCount)
{
	this->uniquePtr = std::make_unique<Foo>();
	Bar(this->uniquePtr.get());
	this->uniquePtr.reset();
}

BENCHMARK_F(CtorAccessDtor, Shared_New, DemoFixture, SamplesCount, IterationsCount)
{
	this->sharedPtr.reset(new Foo());
	Bar(this->sharedPtr);
	this->sharedPtr.reset();
}

BENCHMARK_F(CtorAccessDtor, Shared_Make, DemoFixture, SamplesCount, IterationsCount)
{
	this->sharedPtr = std::make_shared<Foo>();
	Bar(this->sharedPtr);
	this->sharedPtr.reset();
}

BENCHMARK_F(CtorAccessDtor, OSG, DemoFixture, SamplesCount, IterationsCount)
{
	this->osgPtr = new Foo();
	Bar(this->osgPtr);
	this->osgPtr.release();
}

// ----------------------------------------------------------------------------
// Construct, Copy, Destruct

BASELINE_F(CtorCopyDtor, Baseline, DemoFixture, SamplesCount, IterationsCount)
{
	this->rawPtr = new Foo();
	Bar(this->rawPtr);

	auto copy = this->rawPtr;
	Bar(copy);

	delete this->rawPtr;
}

BENCHMARK_F(CtorCopyDtor, Unique_New, DemoFixture, SamplesCount, IterationsCount)
{
	this->uniquePtr.reset(new Foo());
	Bar(this->uniquePtr.get());

	auto copy = this->uniquePtr.get();
	Bar(copy);

	this->uniquePtr.reset();
}

BENCHMARK_F(CtorCopyDtor, Unique_Make, DemoFixture, SamplesCount, IterationsCount)
{
	this->uniquePtr = std::make_unique<Foo>();
	Bar(this->uniquePtr.get());

	auto copy = this->uniquePtr.get();
	Bar(copy);

	this->uniquePtr.reset();
}

BENCHMARK_F(CtorCopyDtor, Shared_New, DemoFixture, SamplesCount, IterationsCount)
{
	this->sharedPtr.reset(new Foo());
	Bar(this->sharedPtr);

	auto copy = this->sharedPtr;
	Bar(copy);

	this->sharedPtr.reset();
}

BENCHMARK_F(CtorCopyDtor, Shared_Make, DemoFixture, SamplesCount, IterationsCount)
{
	this->sharedPtr = std::make_shared<Foo>();
	Bar(this->sharedPtr);

	auto copy = this->sharedPtr;
	Bar(copy);

	this->sharedPtr.reset();
}

BENCHMARK_F(CtorCopyDtor, OSG, DemoFixture, SamplesCount, IterationsCount)
{
	this->osgPtr = new Foo;
	Bar(this->osgPtr);

	auto copy = this->osgPtr;
	Bar(osgPtr);

	this->osgPtr.release();
}
