///
/// \author	John Farrier
///
/// \copyright Copyright 2015 John Farrier 
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

using namespace celero;

///
/// A local function to support running an individual user-defined function for measurement.
///
void ExecuteProblemSpace(std::shared_ptr<Result> r)
{
	// Define a small internal function object to use to uniformly execute the tests.
	auto testRunner = [r]()
		{
			auto test = r->getExperiment()->getFactory()->Create();
			const auto testTime = test->run(r->getExperiment()->getCalls(), r->getProblemSpaceValue());

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
	print::StageBanner("Baselining");

	// Run through all the tests in ascending order.
	for(size_t i = 0; i < celero::TestVector::Instance().size(); i++)
	{
		auto bmark = celero::TestVector::Instance()[i];
		executor::RunBaseline(bmark);
	}
}

void executor::RunAllExperiments()
{
	print::StageBanner("Benchmarking");

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
		// Populate the problem space with a fake test fixture instantiation.
		{
			auto testValues = baselineExperiment->getFactory()->Create()->getExperimentValues();
			for(auto i : testValues)
			{
				baselineExperiment->addProblemSpace(i);
			}
		}

		for(size_t i = 0; i < baselineExperiment->getResultSize(); i++)
		{
			auto r = baselineExperiment->getResult(i);
			assert(r != nullptr);

			// Describe the beginning of the run.
			print::Run(r);

			ExecuteProblemSpace(r);
				
			// Describe the end of the run.
			print::Done(r);
			print::Baseline(r);
			celero::impl::ExperimentResultComplete(r);
		}
	
		celero::impl::ExperimentComplete(baselineExperiment);
	}
	else
	{
		print::Failure("No Baseline case defined for \"" + bmark->getName() + "\".  Exiting.");
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

	if(bmark->getBaseline()->getResult(0)->getComplete() == false)
	{
		executor::RunBaseline(bmark);
	}

	// Populate the problem space with a fake test fixture instantiation.
	{
		auto testValues = e->getFactory()->Create()->getExperimentValues();
		for(auto i : testValues)
		{
			e->addProblemSpace(i);
		}
	}

	// Result size will grow based on the problem spaces added above.
	for(size_t i = 0; i < e->getResultSize(); i++)
	{
		auto r = e->getResult(i);

		// Describe the beginning of the run.
		print::Run(r);

		ExecuteProblemSpace(r);
				
		// Describe the end of the run.
		print::Done(r);
		print::Baseline(r);

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