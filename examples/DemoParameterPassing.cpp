#include <random>

#include <celero/Celero.h>

///
/// This demo was created to understand the cost with various falvors of "setters" where
/// a function in a class or struct is provided solely to set the value of an internal member.
/// There was some guidance being provided on the internet that if you intend to simply copy
/// the value passed into the function, to incur the cost of the copy when passing the argument.
/// Other guidance suggested to always pass non-pod types in by const ref.  When in doubt, measure!
///
/// http://stackoverflow.com/questions/18673658/should-i-always-move-on-sink-constructor-or-setter-arguments
///

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN;

struct Foo
{
	Foo() : 
		nonPodType()
	{
	}

	void setNonPODTypeByValue(std::string x)
	{
		this->nonPodType = x;
	}

	void setNonPODTypeByValueWithMove(std::string x)
	{
		this->nonPodType = std::move(x);
	}

	void setNonPODTypeByConstRef(const std::string& x)
	{
		this->nonPodType = x;
	}

	void setNonPODTypeByConstRRef(const std::string&& x)
	{
		this->nonPodType = x;
	}

	void setNonPODTypeByConstRRefWithMove(const std::string&& x)
	{
		this->nonPodType = std::move(x);
	}

	std::string nonPodType;
};

#define MY_NUMBER_OF_CALLS_PER_MEASUREMENT 3000000
#define MY_NUMBER_OF_SAMPLES 100

BASELINE(ParamPassing, Baseline, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	// While we are not measuring the time to construct a "Foo" or create "bar", since
	// we do this in each of the tests, the time is exactly the same and does not
	// change the baseline measurement.  (The only difference in each of these cases
	// is the part we wish to measure.)

	Foo f;
	auto bar = std::string("bar");
	f.nonPodType = bar;
}

BENCHMARK(ParamPassing, MoveCopy, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.nonPodType = std::move(bar);
}

BENCHMARK(ParamPassing, NonPODTypeByValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValue(bar);
}

BENCHMARK(ParamPassing, NonPODTypeByMovedValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValue(std::move(bar));
}

BENCHMARK(ParamPassing, NonPODTypeByValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValue(bar);
}

BENCHMARK(ParamPassing, NonPODTypeByMoveValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValue(std::move(bar));
}

BENCHMARK(ParamPassing, NonPODTypeByConstRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByConstRef(bar);
}

BENCHMARK(ParamPassing, NonPODTypeByConstRRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByConstRef(bar);
}

BENCHMARK(ParamPassing, NonPODTypeByConstRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByConstRef(bar);
}

BENCHMARK(ParamPassing, NonPODTypeByMoveConstRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_CALLS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByConstRef(std::move(bar));
}
