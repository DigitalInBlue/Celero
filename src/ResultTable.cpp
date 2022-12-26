///
/// \author	John Farrier
///
/// \copyright Copyright 2015-2023 John Farrier
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
#include <celero/PimplImpl.h>
#include <celero/ResultTable.h>
#include <celero/UserDefinedMeasurementCollector.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace celero;

///
/// \class Impl
///
class celero::ResultTable::Impl
{
public:
	Impl()
	{
	}

	~Impl()
	{
		closeFile();
	}

	void closeFile()
	{
		if(this->ofs.is_open() == true)
		{
			this->ofs.close();
		}
	}

	void setFileName(const std::string& x)
	{
		if(this->ofs.is_open() == true)
		{
			this->ofs.close();
		}

		this->ofs.open(x);

		this->hasWrittenHeader = false;
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
	const size_t precision{5};
	bool hasWrittenHeader{false};
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

void ResultTable::closeFile()
{
	this->pimpl->closeFile();
}

void ResultTable::add(std::shared_ptr<celero::ExperimentResult> x)
{
	if(this->pimpl->ofs.is_open() == true)
	{
		if(this->pimpl->hasWrittenHeader == false)
		{
			// Print the header for the table.
			this->pimpl->ofs << "Group,Experiment,Problem Space,Samples,Iterations,Failure,";

			this->pimpl->ofs << "Baseline,us/Iteration,Iterations/sec,";

			this->pimpl->ofs << "T Min (us),T Mean (us),T Max (us),T Variance,T Standard Deviation,T Skewness,T Kurtosis,T Z Score,";
			this->pimpl->ofs << "R Min (us),R Mean (us),R Max (us),R Variance,R Standard Deviation,R Skewness,R Kurtosis,R Z Score,";

			// User Defined Metrics
			const auto udmCollector = x->getUserDefinedMeasurements();
			for(const auto& entry : udmCollector->getAggregateValues())
			{
				this->pimpl->ofs << entry.first << ",";
			}

			// Purposefully flushing the buffer here.
			this->pimpl->ofs << std::endl;

			this->pimpl->hasWrittenHeader = true;
		}

		// Description
		this->pimpl->ofs << x->getExperiment()->getBenchmark()->getName() << "," << x->getExperiment()->getName() << "," << x->getProblemSpaceValue()
						 << "," << x->getExperiment()->getSamples() << "," << x->getProblemSpaceIterations() << "," << x->getFailure() << ",";

		// Measurements
		this->pimpl->ofs << x->getBaselineMeasurement() << "," << x->getUsPerCall() << "," << x->getCallsPerSecond() << ",";

		// Statistics
		this->pimpl->ofs << x->getTimeStatistics().getMin() << "," << x->getTimeStatistics().getMean() << "," << x->getTimeStatistics().getMax()
						 << "," << x->getTimeStatistics().getVariance() << "," << x->getTimeStatistics().getStandardDeviation() << ","
						 << x->getTimeStatistics().getSkewness() << "," << x->getTimeStatistics().getKurtosis() << ","
						 << x->getTimeStatistics().getZScore() << ",";

		// Statistics
		this->pimpl->ofs << x->getRAMStatistics().getMin() << "," << x->getRAMStatistics().getMean() << "," << x->getRAMStatistics().getMax() << ","
						 << x->getRAMStatistics().getVariance() << "," << x->getRAMStatistics().getStandardDeviation() << ","
						 << x->getRAMStatistics().getSkewness() << "," << x->getRAMStatistics().getKurtosis() << ","
						 << x->getRAMStatistics().getZScore() << ",";

		// User Defined Metrics
		const auto udmCollector = x->getUserDefinedMeasurements();
		for(const auto& entry : udmCollector->getAggregateValues())
		{
			this->pimpl->ofs << entry.second << ",";
		}

		// Purposefully flushing the buffer here.
		this->pimpl->ofs << std::endl;
	}
}
