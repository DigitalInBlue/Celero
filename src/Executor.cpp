#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/BenchmarkInfo.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Executor.h>
#include <celero/Print.h>
#include <celero/ResultTable.h>
#include <celero/JUnit.h>

#include <iostream>

using namespace celero;

void executor::Execute(std::shared_ptr<BenchmarkInfo> x)
{
	// Define a small internal function object to use to uniformly execute the tests.
	auto testRunner = [x](const size_t problemSetValueIndex)
	{
		auto test = x->getFactory()->Create();
		const auto testTime = test->Run(x->getOps(), problemSetValueIndex);

		// Save test results
		x->setRunTime(testTime.first);
		x->incrementTotalRunTime(testTime.first);
	};

	auto temp = x->getFactory()->Create();
	auto problemSetIndex = x->getProblemSetSizeIndex();

	if(x->getSamples() > 0)
	{
		for(auto j = x->getSamples(); j > 0; --j)
		{
			testRunner(problemSetIndex);
		}
	}
	else
	{
		// JEF // Run for at least one second and at least 30 iteratons for good statistical sampling.
		while((celero::timer::ConvertSystemTime(x->getTotalRunTime()) < 1.0) || (x->getSamples() < celero::StatisticalSample))
		{
			x->incrementSamples();
			testRunner(problemSetIndex);
		}

		print::Auto(x);
	}

	ResultTable::Instance().add(x->getGroupName(), x->getTestName(), temp->getProblemSetValue(problemSetIndex), x->getUsPerOp());
	JUnit::Instance().add(*x);
}

void executor::RunAll()
{
	auto moreProblemSetsLeft = true;

	while(moreProblemSetsLeft == true)
	{
		moreProblemSetsLeft = RunAllBaselines();
		moreProblemSetsLeft |= RunAllTests();

		// Reset all baseline data.
		celero::TestVector::Instance().forEachBaseline(
			[](std::shared_ptr<BenchmarkInfo> info)
		{
			info->reset();
		});

		// Reset all benchmark data.
		celero::TestVector::Instance().forEachTest(
			[](std::shared_ptr<BenchmarkInfo> info)
		{
			info->reset();
		});
	}

	ResultTable::Instance().save();
	JUnit::Instance().save();
}

bool executor::RunAllBaselines()
{
	print::StageBanner("Baselining");

	auto moreProblemSetsLeft = false;

	// Run through all the tests in ascending order.
	celero::TestVector::Instance().forEachBaseline(
		[&moreProblemSetsLeft](std::shared_ptr<BenchmarkInfo> info)
	{
		if(info->getProblemSetSizeIndex() < info->getProblemSetSize() || info->getProblemSetSizeIndex() == 0)
		{
			// Describe the beginning of the run.
			print::Run(info);
				
			Execute(info);
				
			// Describe the end of the run.
			print::Done(info);

			info->setBaselineUnit(info->getUsPerOp());

			info->incrementProblemSetSizeIndex();
			moreProblemSetsLeft |= (info->getProblemSetSizeIndex() < info->getProblemSetSize());
		}
	});

	return moreProblemSetsLeft;
}

bool executor::RunAllTests()
{
	print::StageBanner("Benchmarking");

	auto moreProblemSetsLeft = false;

	// Run through all the tests in ascending order.
	celero::TestVector::Instance().forEachTest(
		[&moreProblemSetsLeft](std::shared_ptr<BenchmarkInfo> info)
	{
		if(info->getProblemSetSizeIndex() < info->getProblemSetSize() || info->getProblemSetSizeIndex() == 0)
		{
			// Describe the beginning of the run.
			print::Run(info);
				
			Execute(info);

			// Describe the end of the run.
			print::Done(info);

			print::Baseline(info);

			info->incrementProblemSetSizeIndex();
			moreProblemSetsLeft |= (info->getProblemSetSizeIndex() < info->getProblemSetSize());
		}
	});

	return moreProblemSetsLeft;
}

void executor::RunGroup(const std::string& x)
{
	if(x.empty() == false)
	{
		auto moreProblemSetsLeft = true;

		executor::RunBaseline(x);

		while(moreProblemSetsLeft == true)
		{
			moreProblemSetsLeft = false;
			print::StageBanner("Benchmarking");

			// Run through all the tests in ascending order.
			celero::TestVector::Instance().forEachTest(
				[&moreProblemSetsLeft, &x](std::shared_ptr<BenchmarkInfo> info)
			{
				if(info != nullptr && info->getGroupName() == x)
				{
					if(info->getProblemSetSizeIndex() < info->getProblemSetSize() || info->getProblemSetSizeIndex() == 0)
					{
						// Describe the beginning of the run.
						print::Run(info);
				
						Execute(info);

						// Describe the end of the run.
						print::Done(info);

						print::Baseline(info);

						info->incrementProblemSetSizeIndex();
						moreProblemSetsLeft |= (info->getProblemSetSizeIndex() < info->getProblemSetSize());
					}
				}
			});

			// Reset all baseline data.
			celero::TestVector::Instance().forEachBaseline(
				[](std::shared_ptr<BenchmarkInfo> info)
			{
				info->reset();
			});

			// Reset all benchmark data.
			celero::TestVector::Instance().forEachTest(
				[](std::shared_ptr<BenchmarkInfo> info)
			{
				info->reset();
			});
		}
	}
}

void executor::Run(const std::string&, const std::string&)
{
}

void executor::RunBaseline(const std::string& x)
{
	if(x.empty() == false)
	{
		print::StageBanner("Baselining");

		// Run through all the tests in ascending order.
		auto info = celero::TestVector::Instance().getBaseline(x);

		if(info != nullptr && info->getGroupName() == x)
		{
			if(info->getProblemSetSizeIndex() < info->getProblemSetSize() || info->getProblemSetSizeIndex() == 0)
			{
				// Describe the beginning of the run.
				print::Run(info);
				
				Execute(info);

				// Describe the end of the run.
				print::Done(info);

				info->setBaselineUnit(info->getUsPerOp());

				info->incrementProblemSetSizeIndex();
			}
		}
	}
}

void executor::RunTest(const std::string&, const std::string&)
{
}
