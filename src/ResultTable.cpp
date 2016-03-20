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

#include <celero/ResultTable.h>
#include <celero/PimplImpl.h>
#include <celero/Benchmark.h>

#include <assert.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace celero;

///
/// \class Impl
///
class celero::ResultTable::Impl
{
	public:
		Impl() :
			precision(5)
		{
		}

		void setFileName(const std::string& x)
		{
			if(this->ofs.is_open() == true)
			{
				this->ofs.close();
			}

			this->ofs.open(x);

			// Print the header for the table.
			if(this->ofs.is_open() == true)
			{
				this->ofs << "Group,Experiment,Problem Space,Samples,Iterations,Baseline,";
				this->ofs << "us/Iteration,Iterations/sec,Min (us),Mean (us),Max (us),Variance,Standard Deviation,Skewness,Kurtosis,Z Score\n";
			}
		}

		std::string format(double x)
		{
			std::stringstream ss;
			ss << std::fixed;
			ss.precision(this->precision);
			ss << x;
			return ss.str();
		}

		std::ofstream ofs;
		const size_t precision;
};

ResultTable::ResultTable() : pimpl()
{
}

ResultTable::~ResultTable()
{
}

ResultTable& ResultTable::Instance()
{
	static ResultTable singleton;
	return singleton;
}

void ResultTable::setFileName(const std::string& x)
{
	assert(x.empty() == false);
	this->pimpl->setFileName(x);
}

void ResultTable::add(std::shared_ptr<Result> x)
{
	if(this->pimpl->ofs.is_open() == true)
	{
		this->pimpl->ofs << x->getExperiment()->getBenchmark()->getName() << ","
			<< x->getExperiment()->getName() << ","
			<< x->getProblemSpaceValue() << ","
			<< x->getExperiment()->getSamples() << ","
			<< x->getProblemSpaceIterations() << ",";

		this->pimpl->ofs << x->getBaselineMeasurement() << ","
			<< x->getUsPerCall() << ","
			<< x->getCallsPerSecond() << ","
			<< x->getStatistics()->getMin() << ","
			<< x->getStatistics()->getMean() << ","
			<< x->getStatistics()->getMax() << ","
			<< x->getStatistics()->getVariance() << ","
			<< x->getStatistics()->getStandardDeviation() << ","
			<< x->getStatistics()->getSkewness() << ","
			<< x->getStatistics()->getKurtosis() << ","
			<< x->getStatistics()->getZScore()
			<< "\n";
	}
}

