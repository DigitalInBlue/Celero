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

#include <celero/Print.h>
#include <celero/Console.h>
#include <celero/Benchmark.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>
#include <celero/Timer.h>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

using namespace celero;

enum PrintConstants : size_t
{
	ColumnSeperatorWidth = 3,
	DoubleDecimals = 5,
	NumberOfColumns = 8,
	ColumnWidth = 15
};

///
/// http://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
/// Center-aligns string within a field of width w. Pads with blank spaces to enforce alignment.
///
std::string PrintCenter(const std::string s, const size_t w = PrintConstants::ColumnWidth)
{
	std::stringstream ss;
	std::stringstream spaces;

	// count excess room to pad
	auto padding = w - s.size();

	for(size_t i = 0; i < padding/2; ++i)
	{
		spaces << " ";
	}

	// format with padding
	ss << spaces.str() << s << spaces.str();

	// if odd #, add 1 space
	if(padding > 0 && padding%2 != 0)
	{
		ss << " ";
	}

	ss << " | ";
	return ss.str();
}

///
/// http://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
/// Convert double to string with specified number of places after the decimal and left padding.
///
std::string PrintColumn(const double x, const size_t decDigits = PrintConstants::DoubleDecimals, const size_t width = PrintConstants::ColumnWidth)
{
	std::stringstream ss;
	ss << std::fixed << std::right;

	// fill space around displayed #
	ss.fill(' ');

	// set  width around displayed #
	ss.width(width);

	// set # places after decimal
	ss.precision(decDigits);
	ss << x << " | ";

	return ss.str();
}

///
/// http://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
/// Convert double to string with specified number of places after the decimal.
///
std::string PrintColumn(const int64_t x, const size_t width = PrintConstants::ColumnWidth)
{
	std::stringstream ss;
	ss << std::fixed;

	// fill space around displayed #
	ss.fill(' ');

	// set  width around displayed #
	ss.width(width);

	ss << x << " | ";
	return ss.str();
}

///
/// http://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
/// Convert double to string with specified number of places after the decimal.
///
std::string PrintColumn(const uint64_t x, const size_t width = PrintConstants::ColumnWidth)
{
	std::stringstream ss;
	ss << std::fixed;

	// fill space around displayed #
	ss.fill(' ');

	// set  width around displayed #
	ss.width(width);

	ss << x << " | ";
	return ss.str();
}

///
/// http://stackoverflow.com/questions/14765155/how-can-i-easily-format-my-data-table-in-c
/// Convert double to string with specified number of places after the decimal.
///
std::string PrintColumn(const std::string& x, const size_t width = PrintConstants::ColumnWidth)
{
	std::stringstream ss;
	ss << std::fixed << std::left;

	// fill space around displayed #
	ss.fill(' ');

	// set width around displayed #
	ss.width(width);

	if(x.length() > width)
	{
		// Truncate
		std::string xTrunc = x;
		xTrunc = xTrunc.substr(0, width);
		ss << xTrunc << " | ";
	}
	else
	{
		ss << x << " | ";
	}

	return ss.str();
}

std::string PrintHRule()
{
	std::stringstream ss;
	ss << std::string(PrintConstants::ColumnWidth*PrintConstants::NumberOfColumns + PrintConstants::ColumnSeperatorWidth*(PrintConstants::NumberOfColumns - 1) + 2, '-') << "\n";
	return ss.str();
}

void celero::print::Console(const std::string& x)
{
	std::cout << "Celero: " << x << std::endl;
}

void celero::print::TableBanner()
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default); 
	std::cout << PrintHRule();
	std::cout	<< PrintCenter("Group")
				<< PrintCenter("Experiment")
				<< PrintCenter("Prob. Space")
				<< PrintCenter("Samples")
				<< PrintCenter("Iterations")

				<< PrintCenter("Baseline")
				<< PrintCenter("us/Iteration")
				<< PrintCenter("Iterations/sec")
				<< "\n";
	std::cout << PrintHRule();
}

void celero::print::TableRowHeader(std::shared_ptr<Result> x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
	std::cout << PrintColumn(x->getExperiment()->getBenchmark()->getName())
				<< PrintColumn(x->getExperiment()->getName());

	if(x->getProblemSpaceValue() == static_cast<int64_t>(TestFixture::Constants::NoProblemSpaceValue))
	{
		std::cout << std::fixed << std::right << PrintColumn("Null");
	}
	else
	{
		std::cout << PrintColumn(x->getProblemSpaceValue());
	}

	std::cout << PrintColumn(x->getExperiment()->getSamples())
				<< PrintColumn(x->getProblemSpaceIterations());
}

void celero::print::TableResult(std::shared_ptr<Result> x)
{
	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);

	// Slower than Baseline
	if(x->getBaselineMeasurement() > 1.0)
	{
		celero::console::SetConsoleColor(celero::console::ConsoleColor_Yellow);
	}
	else if(x->getBaselineMeasurement() < 1.0)
	{
		celero::console::SetConsoleColor(celero::console::ConsoleColor_Green);
	}
	else
	{
		celero::console::SetConsoleColor(celero::console::ConsoleColor_Cyan);
	}

	std::cout	<< PrintColumn(x->getBaselineMeasurement())
				<< PrintColumn(x->getUsPerCall())
				<< PrintColumn(x->getCallsPerSecond(), 2)
				<< "\n";

	celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
}
