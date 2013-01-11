#include <celero/Celero.h>
#include <celero/Console.h>
#include <celero/BenchmarkInfo.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Executor.h>
#include <celero/Print.h>

#include <iostream>
#include <algorithm>

using namespace celero;

std::shared_ptr<celero::BenchmarkInfo> celero::RegisterTest(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<celero::Factory> testFactory)
{
	auto info = std::make_shared<celero::BenchmarkInfo>(groupName, benchmarkName, samples, calls, testFactory);
	celero::TestVector::Instance().pushBackTest(info);
	return info;
}

std::shared_ptr<celero::BenchmarkInfo> celero::RegisterBaseline(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<celero::Factory> testFactory)
{
	auto info = std::make_shared<celero::BenchmarkInfo>(groupName, benchmarkName, samples, calls, testFactory);
	celero::TestVector::Instance().pushBackBaseline(info);
	return info;
}

void celero::Run(int, char**)
{
	// Initial output
	print::GreenBar("");
	std::cout << std::fixed;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[  CELERO  ]" << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	executor::RunAll();
	
	// Final output.
	std::string finalOutput = "Completed.  " + std::to_string(celero::TestVector::Instance().getTestSize() + celero::TestVector::Instance().getBaselineSize()) + " tests complete.";
	print::StageBanner(finalOutput);
}

