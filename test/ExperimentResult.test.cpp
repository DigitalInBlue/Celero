#include <celero/Experiment.h>
#include <celero/ExperimentResult.h>
#include <celero/TestFixture.h>
#include <gtest/gtest.h>

// Tests for null problemSpace (the bug) — these should not crash

TEST(ExperimentResult, getProblemSpaceValueNullSafe)
{
	celero::ExperimentResult result(nullptr);
	EXPECT_EQ(0, result.getProblemSpaceValue());
}

TEST(ExperimentResult, getProblemSpaceIterationsNullSafe)
{
	celero::ExperimentResult result(nullptr);
	EXPECT_EQ(uint64_t{0}, result.getProblemSpaceIterations());
}

TEST(ExperimentResult, getUsPerCallNullSafe)
{
	celero::ExperimentResult result(nullptr);
	EXPECT_DOUBLE_EQ(0.0, result.getUsPerCall());
}

TEST(ExperimentResult, getUnitsPerSecondNullSafe)
{
	celero::ExperimentResult result(nullptr);
	EXPECT_DOUBLE_EQ(0.0, result.getUnitsPerSecond());
}

// Tests for valid problemSpace — ensure behavior is preserved after fix

TEST(ExperimentResult, getProblemSpaceValueWithValidSpace)
{
	celero::ExperimentResult result(nullptr);
	auto ev = std::make_shared<celero::TestFixture::ExperimentValue>(42, 100);
	result.setProblemSpaceValue(ev);
	EXPECT_EQ(42, result.getProblemSpaceValue());
}

TEST(ExperimentResult, getProblemSpaceIterationsWithValidSpace)
{
	celero::ExperimentResult result(nullptr);
	auto ev = std::make_shared<celero::TestFixture::ExperimentValue>(42, 100);
	result.setProblemSpaceValue(ev);
	EXPECT_EQ(uint64_t{100}, result.getProblemSpaceIterations());
}

TEST(ExperimentResult, getUsPerCallWithValidSpace)
{
	celero::ExperimentResult result(nullptr);
	auto ev = std::make_shared<celero::TestFixture::ExperimentValue>(0, 10);
	result.setProblemSpaceValue(ev);
	result.addRunTimeSample(1000); // 1000 us total, 10 iterations = 100 us/call
	EXPECT_GT(result.getUsPerCall(), 0.0);
}

TEST(ExperimentResult, getUnitsPerSecondWithValidSpace)
{
	celero::ExperimentResult result(nullptr);
	auto ev = std::make_shared<celero::TestFixture::ExperimentValue>(100, 10);
	result.setProblemSpaceValue(ev, 1.0);
	result.addRunTimeSample(1000);
	EXPECT_GT(result.getUnitsPerSecond(), 0.0);
}
