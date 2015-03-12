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

#include <celero/Archive.h>
#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/Benchmark.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Executor.h>
#include <celero/Print.h>
#include <celero/ResultTable.h>
#include <celero/JUnit.h>
#include <celero/CommandLine.h>
#include <celero/Distribution.h>
#include <celero/Callbacks.h>

#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

using namespace celero;

std::shared_ptr<celero::Benchmark> celero::RegisterTest(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, const uint64_t threads, std::shared_ptr<celero::Factory> experimentFactory, const double target)
{
	auto bm = celero::TestVector::Instance()[groupName];

	if(bm == nullptr)
	{
		bm = std::make_shared<Benchmark>(groupName);
		celero::TestVector::Instance().push_back(bm);
	}

	auto p = std::make_shared<Experiment>(bm);
	p->setIsBaselineCase(false);
	p->setName(benchmarkName);
	p->setSamples(samples);
	p->setCalls(calls);
	p->setThreads(threads);
	p->setFactory(experimentFactory);
	p->setBaselineTarget(target);

	bm->addExperiment(p);

	return bm;
}

std::shared_ptr<celero::Benchmark> celero::RegisterBaseline(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, const uint64_t threads, std::shared_ptr<celero::Factory> experimentFactory)
{
	auto bm = celero::TestVector::Instance()[groupName];

	if(bm == nullptr)
	{
		bm = std::make_shared<Benchmark>(groupName);
		celero::TestVector::Instance().push_back(bm);
	}

	auto p = std::make_shared<Experiment>(bm);
	p->setIsBaselineCase(true);
	p->setName(benchmarkName);
	p->setSamples(samples);
	p->setCalls(calls);
	p->setThreads(threads);
	p->setFactory(experimentFactory);
	p->setBaselineTarget(1.0);

	bm->setBaseline(p);

	return bm;
}

void celero::Run(int argc, char** argv)
{
	cmdline::parser args;
	args.add("list", 'l', "Prints a list of all available benchmarks.");
	args.add<std::string>("group", 'g', "Runs a specific group of benchmarks.", false, "");
	args.add<std::string>("outputTable", 't', "Saves a results table to the named file.", false, "");
	args.add<std::string>("junit", 'j', "Saves a JUnit XML-formatted file to the named file.", false, "");
	args.add<std::string>("archive", 'a', "Saves or updates a result archive file.", false, "");
	args.add<uint64_t>("distribution", 'd', "Builds a file to help characterize the distribution of measurements and exits.", false, 0);
	args.parse_check(argc, argv);
	
	if(args.exist("list") == true)
	{
		auto& tests = celero::TestVector::Instance();
		std::vector<std::string> testNames;
		
		for(auto i = size_t(0); i < tests.size(); i++)
		{
			auto bm = celero::TestVector::Instance()[i];
			testNames.push_back(bm->getName());
		}

		std::sort(std::begin(testNames), std::end(testNames));

		std::cout << "Avaliable tests:" << std::endl;
		for(auto i : testNames)
		{
			std::cout << "\t" << i << std::endl;
		}

		return;
	}

	// Initial output
	print::GreenBar("");
	std::cout << std::fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[  CELERO  ]" << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	celero::print::GreenBar("");
	celero::timer::CachePerformanceFrequency();
	
	// Shall we build a distribution?
	auto intArgument = args.get<uint64_t>("distribution");
	if(intArgument > 0)
	{
		RunDistribution(intArgument);
	}

	// Has a result output file been specified?
	auto argument = args.get<std::string>("outputTable");
	if(argument.empty() == false)
	{
		celero::ResultTable::Instance().setFileName(argument);

		celero::AddExperimentResultCompleteFunction(
			[](std::shared_ptr<celero::Result> p)
			{
				celero::ResultTable::Instance().add(p);
			});
	}

	// Has a result output file been specified?
	argument = args.get<std::string>("archive");
	if(argument.empty() == false)
	{
		celero::Archive::Instance().setFileName(argument);
		
		celero::AddExperimentResultCompleteFunction(
			[](std::shared_ptr<celero::Result> p)
			{
				celero::Archive::Instance().add(p);
			});
	}

	// Has a JUnit output file been specified?
	argument = args.get<std::string>("junit");
	if(argument.empty() == false)
	{
		celero::JUnit::Instance().setFileName(argument);

		celero::AddExperimentResultCompleteFunction(
			[](std::shared_ptr<celero::Result> p)
			{
				celero::JUnit::Instance().add(p);
			});
	}

	// Has a run group been specified?
	argument = args.get<std::string>("group");
	if(argument.empty() == false)
	{
		executor::Run(argument);
	}
	else
	{
		executor::RunAll();
	}

	// Results Sumamry
	const auto summaryOutput = std::string("Results Summary");
	print::StageBanner(summaryOutput);

	// Print a summary of run results.
	for(auto i = size_t(0); i < celero::TestVector::Instance().size(); ++i)
	{
		auto test = celero::TestVector::Instance()[i];

		celero::print::SummaryTest(test->getName());

		// Print baseline
		for(auto j = size_t(0); j < test->getBaseline()->getResultSize(); ++j)
		{
			celero::print::Summary(test->getBaseline()->getResult(j));

			// Print test cases corresponding to that baseline.
			for(auto k = size_t(0); k < test->getExperimentSize(); k++)
			{
				// Experiments should have the same number of results as the baseline.
				assert(test->getExperiment(k)->getResultSize() == test->getBaseline()->getResultSize());

				celero::print::Summary(test->getExperiment(k)->getResult(j));
			}
		}
	}
	
	// Final output.
	const auto finalOutput = std::string("Complete");
	print::StageBanner(finalOutput);
}
