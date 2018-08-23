///
/// \author	John Farrier
///
/// \copyright Copyright 2015, 2016, 2017, 2018 John Farrier
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

#include <assert.h>
#include <celero/Benchmark.h>
#include <celero/JUnit.h>
#include <celero/PimplImpl.h>
#include <celero/Timer.h>
#include <celero/Utilities.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

using namespace celero;

///
/// \struct celero::JUnit::Impl
///
class celero::JUnit::Impl
{
public:
	std::string fileName;

	/// Store the test case size, measured baseline, objective baseline, and total run time in seconds.
	std::map<std::string, std::vector<std::shared_ptr<celero::ExperimentResult>>> results;

	double totalTime = {0.0};
};

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

void JUnit::add(std::shared_ptr<celero::ExperimentResult> x)
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

		*os << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;

		for(auto i : this->pimpl->results)
		{
			uint64_t testSuiteTime = 0;
			size_t testSuiteFailures = 0;
			size_t testSuiteErrors = 0;

			const auto runs = i.second;

			for(auto j : runs)
			{
				if(j->getFailure())
				{
					testSuiteErrors++;
					continue;
				}
				else if((j->getExperiment()->getBaselineTarget() > 0.0) && (j->getBaselineMeasurement() > j->getExperiment()->getBaselineTarget()))
				{
					testSuiteFailures++;
				}

				testSuiteTime += j->getRunTime();
			}

			*os << "<testsuite errors=\"" << testSuiteErrors << "\" ";
			*os << "tests=\"" << i.second.size() << "\" ";
			*os << "time=\"" << celero::timer::ConvertSystemTime(testSuiteTime) << "\" ";
			*os << "failures=\"" << testSuiteFailures << "\" ";
			*os << "name=\"" << i.first << "\">" << std::endl;

			for(auto j : runs)
			{
				*os << "\t<testcase ";
				*os << "time=\"" << celero::timer::ConvertSystemTime(j->getFailure() ? 0 : j->getRunTime()) << "\" ";
				*os << "name=\"" << j->getExperiment()->getName() << "#" << j->getProblemSpaceValue() << "\"";

				// Compare measured to objective
				if(j->getFailure())
				{
					// Error
					*os << ">" << std::endl;

					*os << "\t\t<error ";
					*os << "type=\"exception\"";
					*os << "/>" << std::endl;

					*os << "\t</testcase>" << std::endl;
				}
				else if((j->getExperiment()->getBaselineTarget() > 0.0) && (j->getBaselineMeasurement() > j->getExperiment()->getBaselineTarget()))
				{
					// Failure
					*os << ">" << std::endl;

					*os << "\t\t<failure ";
					*os << "type=\"Performance objective not met.\" ";
					*os << "message=\"Measurement of " << j->getBaselineMeasurement() << " exceeds objective baseline of "
						<< j->getExperiment()->getBaselineTarget() << "\" ";
					*os << "/>" << std::endl;

					*os << "\t</testcase>" << std::endl;
				}
				else
				{
					// Success
					*os << "/>" << std::endl;
				}
			}

			*os << "</testsuite>" << std::endl;
		}

		ofs.close();
	}
}
