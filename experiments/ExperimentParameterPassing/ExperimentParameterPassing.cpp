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
CELERO_MAIN

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

	void setNonPODTypeByConstRefWithMove(const std::string& x)
	{
		this->nonPodType = std::move(x);
	}

	void setNonPODTypeByRRef(std::string&& x)
	{
		this->nonPodType = x;
	}

	void setNonPODTypeByRRefWithMove(std::string&& x)
	{
		this->nonPodType = std::move(x);
	}

	void setNonPODTypeByRRefWithSwap(std::string&& x)
	{
		std::swap(this->nonPodType, x);
	}

	void setNonPODTypeByConstRRef(const std::string&& x)
	{
		this->nonPodType = x;
	}

	void setNonPODTypeByConstRRefWithMove(const std::string&& x)
	{
		this->nonPodType = std::move(x);
	}

	void setPODTypeByValue(uint64_t x)
	{
		this->podType = x;
	}

	void setPODTypeByValueWithMove(uint64_t x)
	{
		this->podType = std::move(x);
	}

	void setPODTypeByConstRef(const uint64_t& x)
	{
		this->podType = x;
	}

	void setPODTypeByConstRefWithMove(const uint64_t& x)
	{
		this->podType = std::move(x);
	}

	void setPODTypeByConstRRef(const uint64_t&& x)
	{
		this->podType = x;
	}

	void setPODTypeByConstRRefWithMove(const uint64_t&& x)
	{
		this->podType = std::move(x);
	}

	std::string nonPodType;
	uint64_t podType;
};

#define MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT 3000000
#define MY_NUMBER_OF_SAMPLES 100

BASELINE(NonPODParamPassing, Baseline, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	// While we are not measuring the time to construct a "Foo" or create "bar", since
	// we do this in each of the tests, the time is exactly the same and does not
	// change the baseline measurement.  (The only difference in each of these cases
	// is the part we wish to measure.)

	Foo f;
	auto bar = std::string("bar");
	f.nonPodType = bar;
}

BENCHMARK(NonPODParamPassing, TypeMoveCopy, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.nonPodType = std::move(bar);
}

BENCHMARK(NonPODParamPassing, TypeByValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValue(bar);
}

BENCHMARK(NonPODParamPassing, TypeByMovedValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValue(std::move(bar));
}

BENCHMARK(NonPODParamPassing, TypeByValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValueWithMove(bar);
}

BENCHMARK(NonPODParamPassing, TypeByMoveValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValueWithMove(std::move(bar));
}

BENCHMARK(NonPODParamPassing, TypeByConstRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByConstRef(bar);
}

BENCHMARK(NonPODParamPassing, TypeByConstRRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByConstRRef(std::string("bar"));
}

BENCHMARK(NonPODParamPassing, TypeByConstRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByConstRRefWithMove(std::string("bar"));
}

BENCHMARK(NonPODParamPassing, TypeByMoveConstRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByConstRRefWithMove(std::move(std::string("bar")));
}

BENCHMARK(NonPODParamPassing, TypeByRRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByRRef(std::string("bar"));
}

BENCHMARK(NonPODParamPassing, TypeByRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByRRefWithMove(std::string("bar"));
}

BENCHMARK(NonPODParamPassing, TypeByMoveRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByRRefWithMove(std::move(std::string("bar")));
}

BENCHMARK(NonPODParamPassing, TypeByMoveRRefWithMove2, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByRRefWithMove(std::move(bar));
}

BENCHMARK(NonPODParamPassing, TypeByRRefWithSwap, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByRRefWithSwap(std::string("bar"));
}

BENCHMARK(NonPODParamPassing, TypeByMoveRRefWithSwap, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setNonPODTypeByRRefWithSwap(std::move(std::string("bar")));
}

// ---------------------------------------------------------------------------------------
// Repeat some of the tests above, but use the varible after the use in the "set" function
// ---------------------------------------------------------------------------------------

BASELINE(NonPODParamPassingUseAfter, Baseline, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	// While we are not measuring the time to construct a "Foo" or create "bar", since
	// we do this in each of the tests, the time is exactly the same and does not
	// change the baseline measurement.  (The only difference in each of these cases
	// is the part we wish to measure.)

	Foo f;
	auto bar = std::string("bar");
	f.nonPodType = bar;

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(NonPODParamPassingUseAfter, TypeByValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValue(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(NonPODParamPassingUseAfter, TypeByValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByValueWithMove(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(NonPODParamPassingUseAfter, TypeByConstRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByConstRef(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(NonPODParamPassingUseAfter, TypeByConstRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODTypeByConstRefWithMove(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}


// --------------------------------------------------
// Now repeat all of the tests above with a POD type.
// --------------------------------------------------

BASELINE(PODParamPassing, Baseline, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	// While we are not measuring the time to construct a "Foo" or create "bar", since
	// we do this in each of the tests, the time is exactly the same and does not
	// change the baseline measurement.  (The only difference in each of these cases
	// is the part we wish to measure.)

	Foo f;
	auto bar = uint64_t(2716057);
	f.podType = bar;
}

BENCHMARK(PODParamPassing, TypeMoveCopy, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.podType = std::move(bar);
}

BENCHMARK(PODParamPassing, TypeByValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByValue(bar);
}

BENCHMARK(PODParamPassing, TypeByMovedValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByValue(std::move(bar));
}

BENCHMARK(PODParamPassing, TypeByValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByValueWithMove(bar);
}

BENCHMARK(PODParamPassing, TypeByMoveValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByValueWithMove(std::move(bar));
}

BENCHMARK(PODParamPassing, TypeByConstRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByConstRef(bar);
}

BENCHMARK(PODParamPassing, TypeByConstRRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setPODTypeByConstRRef(uint64_t(2716057));
}

BENCHMARK(PODParamPassing, TypeByMoveConstRRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setPODTypeByConstRRef(std::move(uint64_t(2716057)));
}

BENCHMARK(PODParamPassing, TypeByConstRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setPODTypeByConstRRefWithMove(uint64_t(2716057));
}

BENCHMARK(PODParamPassing, TypeByMoveConstRRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	f.setPODTypeByConstRRefWithMove(std::move(uint64_t(2716057)));
}

// ---------------------------------------------------------------------------------------
// Repeat some of the tests above, but use the varible after the use in the "set" function
// ---------------------------------------------------------------------------------------

BASELINE(PODParamPassingUseAfter, Baseline, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	// While we are not measuring the time to construct a "Foo" or create "bar", since
	// we do this in each of the tests, the time is exactly the same and does not
	// change the baseline measurement.  (The only difference in each of these cases
	// is the part we wish to measure.)

	Foo f;
	auto bar = uint64_t(2716057);
	f.podType = bar;

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassingUseAfter, TypeByValue, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByValue(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassingUseAfter, TypeByValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByValueWithMove(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassingUseAfter, TypeByMoveValueWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByValueWithMove(std::move(bar));

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassingUseAfter, TypeByConstRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByConstRef(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassingUseAfter, TypeByMoveConstRef, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByConstRef(std::move(bar));

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassingUseAfter, TypeByConstRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByConstRefWithMove(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassingUseAfter, TypeByMoveConstRefWithMove, MY_NUMBER_OF_SAMPLES, MY_NUMBER_OF_ITERATIONS_PER_MEASUREMENT)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODTypeByConstRefWithMove(std::move(bar));

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}
