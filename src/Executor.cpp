///
/// \author	John Farrier
///
/// \copyright Copyright 2015-2023 John Farrier
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
#include <celero/Memory.h>
#include <celero/Print.h>
#include <celero/TestVector.h>
#include <celero/UserDefinedMeasurementCollector.h>
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
	if((r->getExperiment()->getSamples() == 0) || (r->getExperiment()->getIterations() == 0))
	{
		// The smallest test should take at least 10x as long as our timer's resolution.
		// I chose "2x" arbitrarily.
		const auto minTestTime = static_cast<int64_t>(celero::timer::CachePerformanceFrequency(true) * 1e6) * 2;

		// Compute a good number to use for iterations and set the sample size to 30.
		auto test = r->getExperiment()->getFactory()->Create();
		auto testTime = int64_t(0);
		auto testIterations = int64_t(1);

		while(testTime < minTestTime)
		{
			const auto runResult = RunAndCatchExc(*test, r->getExperiment()->getThreads(), testIterations, r->getProblemSpace());

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

		if(experiment->getIterations() == 0)
		{
			experiment->setIterations(std::max(iterations, uint64_t(30)));
		}

		if(experiment->getSamples() == 0)
		{
			experiment->setSamples(30);
		}

		auto ps = r->getProblemSpace();
		ps->Iterations = iterations;
		r->setProblemSpaceValue(ps, r->getProblemSpaceValueScale());
	}

	return true;
}

///
/// A local function to support running an individual user-defined function for measurement.
///
bool ExecuteProblemSpace(std::shared_ptr<celero::ExperimentResult> r)
{
	// Define a small internal function object to use to uniformly execute the tests.
	auto testRunner = [r](const bool record, std::shared_ptr<UserDefinedMeasurementCollector> udmCollector)
	{
		auto test = r->getExperiment()->getFactory()->Create();

		const auto runResult = RunAndCatchExc(*test, r->getExperiment()->getThreads(), r->getProblemSpaceIterations(), r->getProblemSpace());

		if(runResult.first == false)
		{
			// something bad happened
			return false;
		}

		const auto testTime = runResult.second;

		// Save test results
		if(record == true)
		{
			r->addRunTimeSample(testTime);
			r->addMemorySample(celero::GetRAMVirtualUsedByCurrentProcess());
			r->getExperiment()->incrementTotalRunTime(testTime);

			if(udmCollector != nullptr)
			{
				udmCollector->collect(test);
			}
		}

		return true;
	};

	if(r->getExperiment()->getSamples() > 0)
	{
		// make a first pass to maybe cache instructions/data or other kinds of fist-run-only costs
		if(testRunner(false, nullptr) == false)
		{
			r->setFailure(true);
			return false;
		}

		auto udmCollector = std::make_shared<UserDefinedMeasurementCollector>(r->getExperiment()->getFactory()->Create());

		for(auto i = r->getExperiment()->getSamples(); i > 0; --i)
		{
			if(testRunner(true, udmCollector) == false)
			{
				r->setFailure(true);
				return false;
			}
		}

		r->setUserDefinedMeasurements(udmCollector);
		r->setComplete(true);
	}
	else
	{
		std::cerr << "ERROR: Celero Test \"" << r->getExperiment()->getBenchmark()->getName() << "::" << r->getExperiment()->getName()
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

		if(bmark != nullptr)
		{
			executor::RunBaseline(bmark);
		}
	}
}

void executor::RunAllExperiments()
{
	// Run through all the tests in ascending order.
	for(size_t i = 0; i < celero::TestVector::Instance().size(); i++)
	{
		auto bmark = celero::TestVector::Instance()[i];
		if(bmark != nullptr)
		{
			executor::RunExperiments(bmark);
		}
	}
}

bool executor::RunBaseline(std::shared_ptr<Benchmark> bmark)
{
	if(bmark == nullptr)
	{
		return false;
	}

	auto baselineExperiment = bmark->getBaseline();

	if(baselineExperiment != nullptr)
	{
		// Populate the problem space with a test fixture instantiation.
		{
			const auto testValues = baselineExperiment->getFactory()->Create()->getExperimentValues();
			const auto valueResultScale = baselineExperiment->getFactory()->Create()->getExperimentValueResultScale();

			for(auto i : testValues)
			{
				if(i->Iterations > 0)
				{
					baselineExperiment->addProblemSpace(i, static_cast<double>(valueResultScale));
				}
				else
				{
					i->Iterations = baselineExperiment->getIterations();
					baselineExperiment->addProblemSpace(i, static_cast<double>(valueResultScale));
				}
			}

			// Add a single default problem space if none was specified.
			// This is needed to get the result size later.
			if(baselineExperiment->getResultSize() == 0)
			{
				baselineExperiment->addProblemSpace(
					std::make_shared<celero::TestFixture::ExperimentValue>(static_cast<int64_t>(TestFixture::Constants::NoProblemSpaceValue)));
			}
		}

		for(size_t i = 0; i < baselineExperiment->getResultSize(); i++)
		{
			auto r = baselineExperiment->getResult(i);
			assert(r != nullptr);

			Printer::get().TableRowExperimentHeader(r->getExperiment());

			// Do a quick sample, if necessary, and adjust sample and iteration sizes, if necessary.
			if(AdjustSampleAndIterationSize(r) == true)
			{
				// Describe the beginning of the run.
				Printer::get().TableRowProblemSpaceHeader(r);

				if(ExecuteProblemSpace(r))
				{
					// Describe the end of the run.
					Printer::get().TableResult(r);
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
		std::cerr << "ERROR: No Baseline case defined for \"" + bmark->getName() + "\".\n";
		return false;
	}

	return true;
}

void executor::RunExperiments(std::shared_ptr<Benchmark> bmark)
{
	if(bmark == nullptr)
	{
		return;
	}

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
	if(e == nullptr)
	{
		return;
	}

	auto bmark = e->getBenchmark();
	auto baseline = bmark->getBaseline();

	if(baseline->getResultSize() == 0 || baseline->getResult(0)->getComplete() == false)
	{
		if(baseline->getResultSize() != 0 && baseline->getResult(0)->getFailure())
		{
			Printer::get().TableRowExperimentHeader(e.get());
			Printer::get().TableRowFailure("Baseline failure, skip");

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
		auto factory = e->getFactory();

		if(factory == nullptr)
		{
			return;
		}

		auto factoryCreate = factory->Create();

		if(factoryCreate == nullptr)
		{
			return;
		}

		const auto testValues = factoryCreate->getExperimentValues();

		factoryCreate = factory->Create();
		const auto valueResultScale = factoryCreate->getExperimentValueResultScale();

		for(auto i : testValues)
		{
			if(i->Iterations > 0)
			{
				e->addProblemSpace(i, valueResultScale);
			}
			else
			{
				i->Iterations = e->getIterations();
				e->addProblemSpace(i, valueResultScale);
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

		Printer::get().TableRowExperimentHeader(r->getExperiment());

		// Do a quick sample, if necessary, and adjust sample and iteration sizes, if necessary.
		const auto adjustSuccess = AdjustSampleAndIterationSize(r);

		if(adjustSuccess == true)
		{
			// Describe the beginning of the run.
			Printer::get().TableRowProblemSpaceHeader(r);

			if(ExecuteProblemSpace(r))
			{
				// Describe the end of the run.
				Printer::get().TableResult(r);
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
	if(executor::RunBaseline(bmark) == true)
	{
		executor::RunExperiments(bmark);
	}
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
