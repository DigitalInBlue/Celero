///
/// \author	John Farrier
///
/// \copyright Copyright 2015, 2016, 2017, 2018 John Farrier
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
/// http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///

#include <celero/Benchmark.h>
#include <celero/Callbacks.h>
#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/Exceptions.h>
#include <celero/Executor.h>
#include <celero/Print.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace celero;

///
/// A local function to figure out how many iterations and samples are required when the user doesn't specify any.
///
bool AdjustSampleAndIterationSize(std::shared_ptr<celero::ExperimentResult> r)
{
	if(r->getExperiment()->getSamples() == 0)
	{
		// The smallest test should take at least 10x as long as our timer's resolution.
		// I chose "10x" arbitrarily.
		const auto minTestTime = static_cast<int64_t>(celero::timer::CachePerformanceFrequency(true) * 1e6) * 10;

		// Compute a good number to use for iterations and set the sample size to 30.
		auto test = r->getExperiment()->getFactory()->Create();
		auto testTime = int64_t(0);
		auto testIterations = int64_t(1);

		while(testTime < minTestTime)
		{
			const auto runResult = RunAndCatchExc(*test, r->getExperiment()->getThreads(), testIterations, r->getProblemSpaceValue());

			if(runResult.first == false)
			{
				return false; // something bad happened
			}

			testTime = runResult.second;

			if(testTime < minTestTime)
			{
				testIterations *= 2;
			}
		}

		const auto iterations = static_cast<uint64_t>(std::max(static_cast<double>(testIterations), 1000000.0 / testTime));
		auto experiment = r->getExperiment();

		experiment->setIterations(iterations);
		experiment->setSamples(30);

		r->setProblemSpaceValue(r->getProblemSpaceValue(), r->getProblemSpaceValueScale(), iterations);
	}
	return true;
}

///
/// A local function to support running an individual user-defined function for measurement.
///
bool ExecuteProblemSpace(std::shared_ptr<celero::ExperimentResult> r)
{
	// Define a small internal function object to use to uniformly execute the tests.
	auto testRunner = [r](const bool record) {
		auto test = r->getExperiment()->getFactory()->Create();

		const auto runResult = RunAndCatchExc(*test, r->getExperiment()->getThreads(), r->getProblemSpaceIterations(), r->getProblemSpaceValue());

		if(runResult.first == false)
		{
			// something bad happened
			return false;
		}

		const auto testTime = runResult.second;

		// Save test results
		if(record == true)
		{
			r->getTimeStatistics()->addSample(testTime);
			r->getExperiment()->incrementTotalRunTime(testTime);
		}

		return true;
	};

	if(r->getExperiment()->getSamples() > 0)
	{
		// make a first pass to maybe cache instructions/data or other kinds of fist-run-only costs
		if(testRunner(false) == false)
		{
			r->setFailure(true);
			return false;
		}

		for(auto i = r->getExperiment()->getSamples(); i > 0; --i)
		{
			if(testRunner(true) == false)
			{
				r->setFailure(true);
				return false;
			}
		}

		r->setComplete(true);
	}
	else
	{
		std::cerr << "Celero: Test \"" << r->getExperiment()->getBenchmark()->getName() << "::" << r->getExperiment()->getName()
				  << "\" must have at least 1 sample." << std::endl;
		return false;
	}

	return true;
}

void executor::RunAll()
{
	executor::RunAllBaselines();
	executor::RunAllExperiments();
}

void executor::RunAllBaselines()
{
	// Run through all the tests in ascending order.
	for(size_t i = 0; i < celero::TestVector::Instance().size(); i++)
	{
		auto bmark = celero::TestVector::Instance()[i];
		executor::RunBaseline(bmark);
	}
}

void executor::RunAllExperiments()
{
	// Run through all the tests in ascending order.
	for(size_t i = 0; i < celero::TestVector::Instance().size(); i++)
	{
		auto bmark = celero::TestVector::Instance()[i];
		executor::RunExperiments(bmark);
	}
}

void executor::RunBaseline(std::shared_ptr<Benchmark> bmark)
{
	auto baselineExperiment = bmark->getBaseline();

	if(baselineExperiment != nullptr)
	{
		// Populate the problem space with a test fixture instantiation.
		{
			const auto testValues = baselineExperiment->getFactory()->Create()->getExperimentValues();
			const auto valueResultScale = baselineExperiment->getFactory()->Create()->getExperimentValueResultScale();

			for(auto i : testValues)
			{
				if(i.Iterations > 0)
				{
					baselineExperiment->addProblemSpace(i.Value, static_cast<double>(valueResultScale), i.Iterations);
				}
				else
				{
					baselineExperiment->addProblemSpace(i.Value, static_cast<double>(valueResultScale), baselineExperiment->getIterations());
				}
			}

			// Add a single default problem space if none was specified.
			// This is needed to get the result size later.
			if(baselineExperiment->getResultSize() == 0)
			{
				baselineExperiment->addProblemSpace(static_cast<int64_t>(TestFixture::Constants::NoProblemSpaceValue));
			}
		}

		for(size_t i = 0; i < baselineExperiment->getResultSize(); i++)
		{
			auto r = baselineExperiment->getResult(i);
			assert(r != nullptr);

			print::TableRowExperimentHeader(r->getExperiment());

			// Do a quick sample, if necessary, and adjust sample and iteration sizes, if necessary.
			if(AdjustSampleAndIterationSize(r) == true)
			{
				// Describe the beginning of the run.
				print::TableRowProblemSpaceHeader(r);

				if(ExecuteProblemSpace(r))
				{
					// Describe the end of the run.
					print::TableResult(r);
				}
			}
			else
			{
				r->setFailure(true);
			}

			celero::impl::ExperimentResultComplete(r);
		}

		celero::impl::ExperimentComplete(baselineExperiment);
	}
	else
	{
		std::cerr << "No Baseline case defined for \"" + bmark->getName() + "\".  Exiting.";
		std::exit(EXIT_FAILURE);
	}
}

void executor::RunExperiments(std::shared_ptr<Benchmark> bmark)
{
	const auto experimentSize = bmark->getExperimentSize();

	for(size_t i = 0; i < experimentSize; i++)
	{
		auto e = bmark->getExperiment(i);
		assert(e != nullptr);

		executor::Run(e);
	}
}

void executor::Run(std::shared_ptr<Experiment> e)
{
	auto bmark = e->getBenchmark();
	auto baseline = bmark->getBaseline();

	if(baseline->getResultSize() == 0 || baseline->getResult(0)->getComplete() == false)
	{
		if(baseline->getResultSize() != 0 && baseline->getResult(0)->getFailure())
		{
			print::TableRowExperimentHeader(e.get());
			print::TableRowFailure("Baseline failure, skip");

			// Add result output failed result
			e->addProblemSpace(0);

			auto r = e->getResult(0);
			r->setFailure(true);
			celero::impl::ExperimentResultComplete(r);

			return;
		}

		executor::RunBaseline(bmark);
	}

	// Populate the problem space with a fake test fixture instantiation.
	{
		const auto testValues = e->getFactory()->Create()->getExperimentValues();
		const auto valueResultScale = e->getFactory()->Create()->getExperimentValueResultScale();

		for(auto i : testValues)
		{
			if(i.Iterations > 0)
			{
				e->addProblemSpace(i.Value, valueResultScale, i.Iterations);
			}
			else
			{
				e->addProblemSpace(i.Value, valueResultScale, e->getIterations());
			}
		}

		// Add a single default problem space if none was specified.
		// This is needed to get the result size later.
		if(e->getResultSize() == 0)
		{
			e->addProblemSpace(0);
		}
	}

	// Result size will grow based on the problem spaces added above.
	for(size_t i = 0; i < e->getResultSize(); i++)
	{
		auto r = e->getResult(i);

		print::TableRowExperimentHeader(r->getExperiment());

		// Do a quick sample, if necessary, and adjust sample and iteration sizes, if necessary.
		const auto adjustSuccess = AdjustSampleAndIterationSize(r);

		if(adjustSuccess == true)
		{
			// Describe the beginning of the run.
			print::TableRowProblemSpaceHeader(r);

			if(ExecuteProblemSpace(r))
			{
				// Describe the end of the run.
				print::TableResult(r);
			}
		}
		else
		{
			r->setFailure(true);
		}

		celero::impl::ExperimentResultComplete(r);
	}

	celero::impl::ExperimentComplete(e);
}

void executor::Run(std::shared_ptr<Benchmark> bmark)
{
	executor::RunBaseline(bmark);
	executor::RunExperiments(bmark);
}

void executor::Run(const std::string& benchmarkName)
{
	auto bmark = celero::TestVector::Instance()[benchmarkName];

	if(bmark != nullptr)
	{
		executor::Run(bmark);
	}
}

void executor::Run(const std::string& benchmarkName, const std::string& experimentName)
{
	auto bmark = celero::TestVector::Instance()[benchmarkName];

	if(bmark != nullptr)
	{
		auto e = bmark->getExperiment(experimentName);
		assert(e != nullptr);
		executor::Run(e);
	}
}
