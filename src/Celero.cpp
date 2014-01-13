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
#include <algorithm>
#include <map>

using namespace celero;

// Temporary argument parsing.  This can be done better, but OK for now.
std::map<std::string, std::string> MakeArgMap(std::vector<std::string>& argv)
{
	std::map<std::string, std::string> args;

	for(size_t i = 1; i < argv.size() - 1; i += 2)
	{
		args.insert(std::make_pair(argv[i], argv[i+1]));
	}

	return args;
}

std::shared_ptr<celero::BenchmarkInfo> celero::RegisterTest(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<celero::Factory> testFactory, const double target)
{
	auto info = std::make_shared<celero::BenchmarkInfo>(groupName, benchmarkName, samples, calls, testFactory, target);
	celero::TestVector::Instance().pushBackTest(info);
	return info;
}

std::shared_ptr<celero::BenchmarkInfo> celero::RegisterBaseline(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<celero::Factory> testFactory)
{
	auto info = std::make_shared<celero::BenchmarkInfo>(groupName, benchmarkName, samples, calls, testFactory);
	celero::TestVector::Instance().pushBackBaseline(info);
	return info;
}

void celero::Run(int argc, char** argv)
{
	// http://www.helleboreconsulting.com/index.php/blog/105-don-t-argue
	// Convert standard program arguments into a nice STL vector.
	std::vector<std::string> params(argv, argv + argc);
	auto args = MakeArgMap(params);

	// Has help been requested?
	auto helpSpecified = args.find("-h");
	if(helpSpecified != args.end())
	{
		std::cout << "Usage: celero [-g groupNameToRun] [-o outputFileToWriteTo] [-h]\n";
		return;
	}

	// Has a result output file been specified?
	auto outputSpecified = args.find("-o");
	if(outputSpecified != args.end())
	{
		celero::ResultTable::Instance().setFileName(outputSpecified->second);
	}

	// Has a JUnit output file been specified?
	auto junitSpecified = args.find("-xml");
	if(junitSpecified != args.end())
	{
		celero::JUnit::Instance().setFileName(junitSpecified->second);
	}

	// Initial output
	print::GreenBar("");
	std::cout << std::fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[  CELERO  ]" << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	std::string finalOutput;

	// Has a run group been specified?
	auto runGroupSpecified = args.find("-g");
	if(runGroupSpecified != args.end())
	{
		executor::RunGroup(runGroupSpecified->second);
		finalOutput = "Completed.  " + std::to_string(celero::TestVector::Instance().getTestSize(runGroupSpecified->second) + celero::TestVector::Instance().getBaselineSize(runGroupSpecified->second)) + " tests complete.";
	}
	else
	{
		executor::RunAll();
		finalOutput = "Completed.  " + std::to_string(celero::TestVector::Instance().getTestSize() + celero::TestVector::Instance().getBaselineSize()) + " tests complete.";
	}
	
	// Final output.
	print::StageBanner(finalOutput);
}
