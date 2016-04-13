///
/// \author	John Farrier
///
/// \copyright Copyright 2016 John Farrier
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

#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/Benchmark.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Executor.h>
#include <celero/Print.h>
#include <celero/Callbacks.h>

#include <iostream>
#include <cassert>
#include <algorithm>

using namespace celero;

///
/// A local function to figure out how many iterations and samples are required when the user doesn't specify any.
///
void AdjustSampleAndIterationSize(std::shared_ptr<Result> r)
{
	if(r->getExperiment()->getSamples() == 0)
	{
		// The smallest test should take at least twice as long as our timer's resolution.
		// I chose "twice" arbitrarily.
		const auto minTestTime = static_cast<int64_t>(celero::timer::CachePerformanceFrequency(true) * 1e6) * 2;

		// Compute a good number to use for iterations and set the sample size to 30.
		auto test = r->getExperiment()->getFactory()->Create();
		auto testTime = int64_t(0);
		auto testIterations = int64_t(1);
		
		while(testTime < minTestTime)
		{
			testTime = test->run(r->getExperiment()->getThreads(), testIterations, r->getProblemSpaceValue());

			if(testTime < minTestTime)
			{
				testIterations *= 2;
			}
		}

		auto iterations = static_cast<uint64_t>(std::max(static_cast<double>(testIterations), 1000000.0 / testTime));
		auto experiment = r->getExperiment();

		experiment->setIterations(iterations);
		experiment->setSamples(30);

		r->setProblemSpaceValue(r->getProblemSpaceValue(), r->getProblemSpaceValueScale(), iterations);
	}
}

///
/// A local function to support running an individual user-defined function for measurement.
///
void ExecuteProblemSpace(std::shared_ptr<Result> r)
{
	// Define a small internal function object to use to uniformly execute the tests.
	auto testRunner = [r]()
		{
			auto test = r->getExperiment()->getFactory()->Create();
			const auto testTime = test->run(r->getExperiment()->getThreads(), r->getProblemSpaceIterations(), r->getProblemSpaceValue());

			// Save test results
			r->getStatistics()->addSample(testTime);
			r->getExperiment()->incrementTotalRunTime(testTime);
		};

	if(r->getExperiment()->getSamples() > 0)
	{
		for(auto i = r->getExperiment()->getSamples(); i > 0; --i)
		{
			testRunner();
		}

		r->setComplete(true);
	}
	else
	{
		std::cerr << "Celero: Test \"" << r->getExperiment()->getBenchmark()->getName() << "::" << r->getExperiment()->getName() << "\" must have at least 1 sample.\n";
	}
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
			auto testValues = baselineExperiment->getFactory()->Create()->getExperimentValues();
			auto valueResultScale = baselineExperiment->getFactory()->Create()->getExperimentValueResultScale();

			for(auto i : testValues)
			{
				if(i.second != 0)
				{
					baselineExperiment->addProblemSpace(i.first, static_cast<double>(valueResultScale), i.second);
				}
				else
				{
					baselineExperiment->addProblemSpace(i.first, static_cast<double>(valueResultScale), baselineExperiment->getIterations());
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

			// Do a quick sample, if necessary, and adjust sample and iteration sizes, if necessary.
			AdjustSampleAndIterationSize(r);

			// Describe the beginning of the run.
			print::TableRowHeader(r);

			ExecuteProblemSpace(r);

			// Describe the end of the run.
			print::TableResult(r);
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
	auto experimentSize = bmark->getExperimentSize();

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
		executor::RunBaseline(bmark);
	}

	// Populate the problem space with a fake test fixture instantiation.
	{
		auto testValues = e->getFactory()->Create()->getExperimentValues();
		auto valueResultScale = e->getFactory()->Create()->getExperimentValueResultScale();
		for(auto i : testValues)
		{
			if(i.second != 0)
			{
				e->addProblemSpace(i.first, valueResultScale, i.second);
			}
			else
			{
				e->addProblemSpace(i.first, valueResultScale, e->getIterations());
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

		// Do a quick sample, if necessary, and adjust sample and iteration sizes, if necessary.
		AdjustSampleAndIterationSize(r);

		// Describe the beginning of the run.
		print::TableRowHeader(r);

		ExecuteProblemSpace(r);

		// Describe the end of the run.
		print::TableResult(r);
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
