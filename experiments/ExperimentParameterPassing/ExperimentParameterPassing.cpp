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
	Foo() : nonPodType()
	{
	}

	void setNonPODByValue(std::string x)
	{
		this->nonPodType = x;
	}

	inline void setNonPODByValueInline(std::string x)
	{
		this->nonPodType = x;
	}

	void setNonPODByValueWithMove(std::string x)
	{
		this->nonPodType = std::move(x);
	}

	void setNonPODByConstRef(const std::string& x)
	{
		this->nonPodType = x;
	}

	inline void setNonPODByConstRefInline(const std::string& x)
	{
		this->nonPodType = x;
	}

	void setNonPODByConstRefWithMove(const std::string& x)
	{
		this->nonPodType = std::move(x);
	}

	void setNonPODByRRef(std::string&& x)
	{
		this->nonPodType = x;
	}

	void setNonPODByRRefWithMove(std::string&& x)
	{
		this->nonPodType = std::move(x);
	}

	void setNonPODByRRefWithSwap(std::string&& x)
	{
		std::swap(this->nonPodType, x);
	}

	void setNonPODByConstRRef(const std::string&& x)
	{
		this->nonPodType = x;
	}

	void setNonPODByConstRRefWithMove(const std::string&& x)
	{
		this->nonPodType = std::move(x);
	}

	void setPODByValue(uint64_t x)
	{
		this->podType = x;
	}

	void setPODByValueWithMove(uint64_t x)
	{
		this->podType = std::move(x);
	}

	void setPODByConstRef(const uint64_t& x)
	{
		this->podType = x;
	}

	void setPODByConstRefWithMove(const uint64_t& x)
	{
		this->podType = std::move(x);
	}

	void setPODByConstRRef(const uint64_t&& x)
	{
		this->podType = x;
	}

	void setPODByConstRRefWithMove(const uint64_t&& x)
	{
		this->podType = std::move(x);
	}

	std::string nonPodType;
	uint64_t podType;
};

static const int SamplesCount = 2000;
static const int IterationsCount = 150000;

// BASELINE(NonPODPass, Baseline, SamplesCount, IterationsCount)
BASELINE(NonPODPass, Baseline, SamplesCount, IterationsCount)
{
	// While we are not measuring the time to construct a "Foo" or create "bar", since
	// we do this in each of the tests, the time is exactly the same and does not
	// change the baseline measurement.  (The only difference in each of these cases
	// is the part we wish to measure.)

	Foo f;
	auto bar = std::string("bar");
	f.nonPodType = bar;
}

BENCHMARK(NonPODPass, MoveCopy, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.nonPodType = std::move(bar);
}

BENCHMARK(NonPODPass, ByValue, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByValue(bar);
}

BENCHMARK(NonPODPass, ByValueInline, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByValueInline(bar);
}

BENCHMARK(NonPODPass, ByMovedValue, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByValue(std::move(bar));
}

BENCHMARK(NonPODPass, ByValueWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByValueWithMove(bar);
}

BENCHMARK(NonPODPass, ByMoveValueWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByValueWithMove(std::move(bar));
}

BENCHMARK(NonPODPass, ByConstRef, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByConstRef(bar);
}

BENCHMARK(NonPODPass, ByConstRefInline, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByConstRefInline(bar);
}

BENCHMARK(NonPODPass, ByConstRRef, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByConstRRef(std::string("bar"));
}

BENCHMARK(NonPODPass, ByConstRRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByConstRRefWithMove(std::string("bar"));
}

BENCHMARK(NonPODPass, ByMoveConstRRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByConstRRefWithMove(std::move(std::string("bar")));
}

BENCHMARK(NonPODPass, ByRRef, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByRRef(std::string("bar"));
}

BENCHMARK(NonPODPass, ByRRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByRRefWithMove(std::string("bar"));
}

BENCHMARK(NonPODPass, ByMoveRRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByRRefWithMove(std::move(std::string("bar")));
}

BENCHMARK(NonPODPass, ByMoveRRefWithMove2, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByRRefWithMove(std::move(bar));
}

BENCHMARK(NonPODPass, ByRRefWithSwap, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByRRefWithSwap(std::string("bar"));
}

BENCHMARK(NonPODPass, ByMoveRRefWithSwap, SamplesCount, IterationsCount)
{
	Foo f;
	f.setNonPODByRRefWithSwap(std::move(std::string("bar")));
}

// ---------------------------------------------------------------------------------------
// Repeat some of the tests above, but use the varible after the use in the "set" function
// ---------------------------------------------------------------------------------------

BASELINE(NonPODPassUseAfter, Baseline, SamplesCount, IterationsCount)
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

BENCHMARK(NonPODPassUseAfter, ByValue, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByValue(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(NonPODPassUseAfter, ByValueWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByValueWithMove(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(NonPODPassUseAfter, ByConstRef, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByConstRef(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(NonPODPassUseAfter, ByConstRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = std::string("bar");
	f.setNonPODByConstRefWithMove(bar);

	bar += "food";
	celero::DoNotOptimizeAway(&bar);
}

// --------------------------------------------------
// Now repeat all of the tests above with a POD type.
// --------------------------------------------------

BASELINE(PODParamPass, Baseline, SamplesCount, IterationsCount)
{
	// While we are not measuring the time to construct a "Foo" or create "bar", since
	// we do this in each of the tests, the time is exactly the same and does not
	// change the baseline measurement.  (The only difference in each of these cases
	// is the part we wish to measure.)

	Foo f;
	auto bar = uint64_t(2716057);
	f.podType = bar;
}

BENCHMARK(PODParamPass, MoveCopy, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.podType = std::move(bar);
}

BENCHMARK(PODParamPass, ByValue, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByValue(bar);
}

BENCHMARK(PODParamPass, ByMovedValue, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByValue(std::move(bar));
}

BENCHMARK(PODParamPass, ByValueWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByValueWithMove(bar);
}

BENCHMARK(PODParamPass, ByMoveValueWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByValueWithMove(std::move(bar));
}

BENCHMARK(PODParamPass, ByConstRef, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByConstRef(bar);
}

BENCHMARK(PODParamPass, ByConstRRef, SamplesCount, IterationsCount)
{
	Foo f;
	f.setPODByConstRRef(uint64_t(2716057));
}

BENCHMARK(PODParamPass, ByMoveConstRRef, SamplesCount, IterationsCount)
{
	Foo f;
	f.setPODByConstRRef(std::move(uint64_t(2716057)));
}

BENCHMARK(PODParamPass, ByConstRRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	f.setPODByConstRRefWithMove(uint64_t(2716057));
}

BENCHMARK(PODParamPass, ByMoveConstRRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	f.setPODByConstRRefWithMove(std::move(uint64_t(2716057)));
}

// ---------------------------------------------------------------------------------------
// Repeat some of the tests above, but use the varible after the use in the "set" function
// ---------------------------------------------------------------------------------------

BASELINE(PODParamPassUseAfter, Baseline, SamplesCount, IterationsCount)
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

BENCHMARK(PODParamPassUseAfter, ByValue, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByValue(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassUseAfter, ByValueWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByValueWithMove(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassUseAfter, ByMoveValueWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByValueWithMove(std::move(bar));

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassUseAfter, ByConstRef, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByConstRef(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassUseAfter, ByMoveConstRef, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByConstRef(std::move(bar));

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassUseAfter, ByConstRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByConstRefWithMove(bar);

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}

BENCHMARK(PODParamPassUseAfter, ByMoveConstRefWithMove, SamplesCount, IterationsCount)
{
	Foo f;
	auto bar = uint64_t(2716057);
	f.setPODByConstRefWithMove(std::move(bar));

	bar += uint64_t(3370318);
	celero::DoNotOptimizeAway(&bar);
}
