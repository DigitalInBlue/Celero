///
/// \author	John Farrier
///
/// \copyright Copyright 2014 John Farrier 
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
#include <celero/Callbacks.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cmath>

using namespace celero;

std::shared_ptr<celero::Benchmark> celero::RegisterTest(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<celero::Factory> experimentFactory, const double target)
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
	p->setFactory(experimentFactory);
	p->setBaselineTarget(target);

	bm->addExperiment(p);

	return bm;
}

std::shared_ptr<celero::Benchmark> celero::RegisterBaseline(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<celero::Factory> experimentFactory)
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
	p->setFactory(experimentFactory);
	p->setBaselineTarget(1);

	bm->setBaseline(p);

	return bm;
}

std::vector<uint64_t> celero::BuildDistribution(uint64_t numberOfSamples, uint64_t callsPerSample)
{
	std::vector<uint64_t> measurements;
	
	while(numberOfSamples--)
	{
		// Dummy variable
		uint64_t dummy = 0;
		uint64_t cps = callsPerSample;

		// Get the starting time.
		auto startTime = celero::timer::GetSystemTime();

		while(cps--)
		{
			celero::DoNotOptimizeAway(dummy++);
		}
			
		auto endTime = celero::timer::GetSystemTime();

		measurements.push_back(endTime - startTime);
	}

	return measurements;
}

void RunDistribution(int64_t intArgument)
{
	auto genFunc = [intArgument](size_t calls)->std::map<double, uint64_t>
		{
			auto vec = celero::BuildDistribution(intArgument, calls);
			std::sort(std::begin(vec), std::end(vec));

			std::map<double, uint64_t> histogram;
			uint64_t minVal = std::numeric_limits<uint64_t>::max();
			uint64_t maxVal = 0;

			for(auto i : vec)
			{
				minVal = std::min(minVal, i);
				maxVal = std::max(maxVal, i);
			}

			auto normalize = [](uint64_t min, uint64_t max, uint64_t val)->double
				{
					max -= min;
					val -= min;
					return static_cast<double>(val)/static_cast<double>(max);
				};

			std::vector<double> normalVector;

			for(auto& i : vec)
			{
				normalVector.push_back(normalize(minVal, maxVal, i));
			}

			for(auto& i : normalVector)
			{
				// Create a historgram of no more than 1024 values.
				i = static_cast<int>(i*1024.0);
				histogram[i] = 0;
			}

			for(auto i : normalVector)
			{
				// Accumulate values into the up to 1024 bins.
				histogram[i] = histogram[i]+1;
			}

			return histogram;
		};

	std::vector<std::map<double, uint64_t>> histograms;

	print::StageBanner("Building Distributions Output");
	print::Run("Distributions 64");
	histograms.push_back(genFunc(64));
	print::Run("Distributions 256");
	histograms.push_back(genFunc(256));
	print::Run("Distributions 1024");
	histograms.push_back(genFunc(1024));
	print::Run("Distributions 4096");
	histograms.push_back(genFunc(4096));

	auto maxLen = size_t(0);
	maxLen = std::max(maxLen, histograms[0].size());
	maxLen = std::max(maxLen, histograms[1].size());
	maxLen = std::max(maxLen, histograms[2].size());
	maxLen = std::max(maxLen, histograms[3].size());

	std::ofstream os;
	os.open("celeroDistribution.csv");

	os << "64, , 256, , 1024, , 4096, , " << std::endl;

	for(size_t i = 0; i < maxLen; ++i)
	{
		for(size_t j = 0; j < histograms.size(); j++)
		{
			if(i < histograms[j].size())
			{
				auto element = std::begin(histograms[j]);
				for(size_t k = 0; k < i; k++)
				{
					++element;
				}

				os << element->first << ", " << element->second << ", ";
			}
			else
			{
				os << " , , ";
			}
		}

		os << "\n";
	}

	os.close();
}

void celero::Run(int argc, char** argv)
{
	celero::timer::CachePerformanceFrequency();

	cmdline::parser args;
	args.add<std::string>("group", 'g', "Runs a specific group of benchmarks.", false, "");
	args.add<std::string>("outputTable", 't', "Saves a results table to the named file.", false, "");
	args.add<std::string>("junit", 'j', "Saves a JUnit XML-formatted file to the named file.", false, "");
	args.add<std::string>("archive", 'a', "Saves or updates a result archive file.", false, "");
	args.add<uint64_t>("distribution", 'd', "Builds a file to help characterize the distribution of measurements and exits.", false, 0);
	args.parse_check(argc, argv);

	// Initial output
	print::GreenBar("");
	std::cout << std::fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[  CELERO  ]" << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

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
			[](std::shared_ptr<celero::Experiment::Result> p)
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
			[](std::shared_ptr<celero::Experiment::Result> p)
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
			[](std::shared_ptr<celero::Experiment::Result> p)
			{
				celero::JUnit::Instance().add(p);
			});
	}

	std::string finalOutput;

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
	
	// Final output.
	print::StageBanner(finalOutput);
}
