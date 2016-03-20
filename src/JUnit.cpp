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

#include <celero/JUnit.h>
#include <celero/PimplImpl.h>
#include <celero/Utilities.h>
#include <celero/Benchmark.h>
#include <celero/Timer.h>

#include <assert.h>

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <tuple>

using namespace celero;

///
/// \class Impl
///
class celero::JUnit::Impl
{
	public:
		Impl() : 
			fileName(),
			results(),
			totalTime(0)
		{
		}

		std::string fileName;

		/// Store the test case size, measured baseline, objective baseline, and total run time in seconds.
		std::map<std::string, std::vector<std::shared_ptr<Result>>> results;

		double totalTime;
};

JUnit::JUnit() : pimpl()
{
}

JUnit::~JUnit()
{
}

JUnit& JUnit::Instance()
{
	static JUnit singleton;
	return singleton;
}

void JUnit::setFileName(const std::string& x)
{
	assert(x.empty() == false);
	this->pimpl->fileName = x;
}

void JUnit::add(std::shared_ptr<celero::Result> x)
{
	this->pimpl->results[x->getExperiment()->getBenchmark()->getName()].push_back(x);
	this->save();
}

void JUnit::save()
{
	std::ofstream ofs;
	ofs.open(this->pimpl->fileName);

	if(ofs.is_open() == true)
	{
		const auto os = &ofs;

		*os << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";

		for(auto i : this->pimpl->results)
		{
			uint64_t testSuiteTime = 0;
			size_t testSuiteFailures = 0;

			auto runs = i.second;

			for(auto j : runs)
			{
				if((j->getExperiment()->getBaselineTarget() > 0.0) && (j->getBaselineMeasurement() > j->getExperiment()->getBaselineTarget()))
				{
					testSuiteFailures++;
				}

				testSuiteTime += j->getRunTime();
			}

			*os << "<testsuite errors=\"0\" ";
			*os << "tests=\"" << i.second.size() << "\" ";
			*os << "time=\"" << celero::timer::ConvertSystemTime(testSuiteTime) << "\" ";
			*os << "failures=\"" << testSuiteFailures << "\" ";
			*os << "name=\"" << i.first << "\">\n";
			
			for(auto j : runs)
			{
				*os << "\t<testcase ";
				*os << "time=\"" << celero::timer::ConvertSystemTime(j->getRunTime()) << "\" ";
				*os << "name=\"" << j->getExperiment()->getName() << "#" << j->getProblemSpaceValue() << "\"";
								
				// Compare measured to objective
				if((j->getExperiment()->getBaselineTarget() > 0.0) && (j->getBaselineMeasurement() > j->getExperiment()->getBaselineTarget()))
				{
					// Failure
					*os << ">\n";

					*os << "\t\t<failure ";
					*os << "type=\"Performance objective not met.\" ";
					*os << "message=\"Measurement of " << j->getBaselineMeasurement() << " exceeds objective baseline of " << j->getExperiment()->getBaselineTarget() << "\" ";
					*os << "/>\n";

					*os << "\t</testcase>\n";
				}
				else
				{
					// Success
					*os << "/>\n";
				}
			}
				
			*os << "</testsuite>\n";
		}

		ofs.close();
	}
}
