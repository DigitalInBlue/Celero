#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/BenchmarkInfo.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Executor.h>
#include <celero/Print.h>

#include <iostream>

using namespace celero;

void executor::Execute(std::shared_ptr<BenchmarkInfo> x)
{
	// Define a small internal function object to use to uniformly execute the tests.
	std::function<void(void)> testRunner = [x]()
	{
		auto test = x->getFactory()->Create();
		const auto testTime = test->Run(x->getOps());

		// Save test results
		x->setRunTime(testTime);
		x->incrementTotalRunTime(testTime);
	};

	if(x->getSamples() > 0)
	{
		for(auto i = x->getSamples(); i > 0; --i)
		{
			testRunner();
		}
	}
	else
	{
		// JEF // Run for at least one second and at least 30 iteratons for good statistical sampling.
		while((x->getTotalRunTime() < celero::UsPerSec) || (x->getSamples() < celero::StatisticalSample))
		{
			x->incrementSamples();
			testRunner();
		}

		print::Auto(x);
	}
}

void executor::RunAll()
{
	RunAllBaselines();
	RunAllTests();
}

void executor::RunAllBaselines()
{
	print::StageBanner("Baselining");

	// Run through all the tests in ascending order.
	celero::TestVector::Instance().forEachBaseline(
		[](std::shared_ptr<BenchmarkInfo> info)
	{
		// Describe the beginning of the run.
		print::Run(info);
				
		Execute(info);
				
		// Describe the end of the run.
		print::Done(info);

		info->setBaselineUnit(info->getRunTime());
	});
}

void executor::RunAllTests()
{
	print::StageBanner("Benchmarking");

	// Run through all the tests in ascending order.
	celero::TestVector::Instance().forEachTest(
		[](std::shared_ptr<BenchmarkInfo> info)
	{
		// Describe the beginning of the run.
		print::Run(info);
				
		Execute(info);
				
		// Describe the end of the run.
		print::Done(info);
		print::Baseline(info);
	});
}

void executor::RunGroup(const std::string& x)
{
	executor::RunBaseline(x);
	
	// Run tests...
}

void executor::Run(const std::string&, const std::string&)
{
}

void executor::RunBaseline(const std::string&)
{
}

void executor::RunTest(const std::string&, const std::string&)
{
}