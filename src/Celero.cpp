#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/BenchmarkInfo.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Executor.h>
#include <celero/Print.h>
#include <celero/ResultTable.h>
#include <celero/JUnit.h>
#include <celero/CommandLine.h>

#include <iostream>
#include <algorithm>
#include <map>

using namespace celero;

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
	celero::timer::CachePerformanceFrequency();

	cmdline::parser args;
	args.add<std::string>("group", 'g', "Runs a specific group of benchmarks.", false, "");
	args.add<std::string>("outputFile", 'o', "Saves a results table to the named file.", false, "");
	args.add<std::string>("xml", 'x', "Saves a JUnit XML-formatted file to the named file.", false, "");
	args.parse_check(argc, argv);

	// Has a result output file been specified?
	auto argument = args.get<std::string>("outputFile");
	if(argument.empty() == false)
	{
		celero::ResultTable::Instance().setFileName(argument);
	}

	// Has a JUnit output file been specified?
	argument = args.get<std::string>("xml");
	if(argument.empty() == false)
	{
		celero::JUnit::Instance().setFileName(argument);
	}

	// Initial output
	print::GreenBar("");
	std::cout << std::fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[  CELERO  ]" << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	std::string finalOutput;

	// Has a run group been specified?
	argument = args.get<std::string>("group");
	if(argument.empty() == false)
	{
		executor::RunGroup(argument);
		finalOutput = "Completed.  " + std::to_string(celero::TestVector::Instance().getTestSize(argument) + celero::TestVector::Instance().getBaselineSize(argument)) + " tests complete.";
	}
	else
	{
		executor::RunAll();
		finalOutput = "Completed.  " + std::to_string(celero::TestVector::Instance().getTestSize() + celero::TestVector::Instance().getBaselineSize()) + " tests complete.";
	}
	
	// Final output.
	print::StageBanner(finalOutput);
}
