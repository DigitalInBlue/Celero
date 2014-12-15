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

#include <celero/Print.h>
#include <celero/Console.h>
#include <celero/Benchmark.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Timer.h>

#include <iostream>
#include <iomanip>
#include <chrono>

using namespace celero;

void print::StageBanner(const std::string& x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[==========] " << std::endl;
	std::cout << "[ STAGE    ] ";
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << x << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[==========] " << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
}

void print::GreenBar(const std::string& x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
	std::cout << "[==========] ";
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << x << std::endl;
}

void print::Run(std::shared_ptr<Result> x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << "[ RUN      ] ";

	if(x->getExperiment()->getResultSize() > 1)
	{
		std::cout << x->getExperiment()->getShort() << " @ " << x->getProblemSpaceValue();
	}
	else
	{
		std::cout << x->getExperiment()->getShort();
	}

	std::cout << " [" << x->getExperiment()->getSamples() << " samples of " << x->getExperiment()->getCalls() << " calls each.]" << std::endl;
}

void print::Run(const std::string& x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << "[ RUN      ] " << x << std::endl;
}

void print::Status(const std::string& x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << "[ STATUS   ] " << x << std::endl;
}

void print::Failure(const std::string& x)
{	
	celero::console::SetConsoleColor(celero::console::ConsoleColor_WhiteOnRed_Bold);
	std::cout << "[==========] " << std::endl;
	std::cout << "[ FAILURE  ] ";
	std::cout << x << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_WhiteOnRed_Bold);
	std::cout << "[==========] " << std::endl;
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
}

void print::Done(std::shared_ptr<Result> x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << "[     DONE ] ";
	std::cout << x->getExperiment()->getShort() << " ";
	std::cout << celero::timer::ConvertSystemTime(x->getExperiment()->getTotalRunTime()) << " sec.";
	std::cout << " [" << x->getRunTime() << " us] [" << x->getUsPerCall() << " us/call]" << std::fixed << 
		" [" << x->getOpsPerSecond() << " calls/sec]" << std::endl;
}

void print::Baseline(std::shared_ptr<Result> x)
{	
	auto baselineGroupName = x->getExperiment()->getBenchmark()->getName();
	if(baselineGroupName.empty() == false)
	{
		celero::console::SetConsoleColor(celero::console::ConsoleColor_Cyan);
		std::cout << "[ BASELINE ] ";
		std::cout << x->getExperiment()->getShort() << " ";
		celero::console::SetConsoleColor(celero::console::ConsoleColor_Green_Bold);
		std::cout << x->getBaselineMeasurement();
		celero::console::SetConsoleColor(celero::console::ConsoleColor_Cyan);
		std::cout << " [SD: " << x->getStatistics()->getStandardDeviation() <<
			", V: " << x->getStatistics()->getVariance() <<
			", K: " << x->getStatistics()->getKurtosis() << "]" << std::endl;

		celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	}
}

void print::SummaryTest(const std::string& x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << "[ TEST     ] " << x << std::endl;
}

void print::Summary(std::shared_ptr<Result> x)
{	
	auto baselineGroupName = x->getExperiment()->getBenchmark()->getName();
	if(baselineGroupName.empty() == false)
	{
		celero::console::SetConsoleColor(celero::console::ConsoleColor_White);
		std::cout << "[          ] ";
		std::cout << std::left << std::setw(32) << x->getExperiment()->getName() << ", ";
		std::cout << std::left << std::setw(8) << x->getProblemSpaceValue() << ", ";
		std::cout << x->getBaselineMeasurement() << std::endl;

		celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	}
}