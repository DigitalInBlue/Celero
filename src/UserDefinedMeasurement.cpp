///
/// \author	Lukas Barth
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
#include <celero/TestFixture.h>
#include <celero/UserDefinedMeasurement.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

using namespace celero;

UDMCollector::UDMCollector(std::shared_ptr<TestFixture> fixture)
{
	for(auto name : fixture->getUserDefinedMeasurementNames())
	{
		this->collected[name] = std::vector<std::shared_ptr<UserDefinedMeasurement>>();
	}
}

void UDMCollector::collect(std::shared_ptr<TestFixture> fixture)
{
	for(auto udm : fixture->getUserDefinedMeasurements())
	{
		this->collected[udm->getName()].push_back(udm);
	}
}

std::vector<std::string> UDMCollector::getFields(std::shared_ptr<TestFixture> fixture) const
{
	std::vector<std::string> fields;

	for(auto udm : fixture->getUserDefinedMeasurements())
	{
		for(const auto& aggDesc : udm->getAggregationInfo())
		{
			fields.emplace_back(std::string(udm->getName()) +
													std::string(" ") + std::string(aggDesc.first));
		}
	}

	return fields;
}

std::vector<std::pair<std::string, double>> UDMCollector::getAggregateValues() const
{
	std::vector<std::pair<std::string, double>> aggregates;

	for(const auto& collectedEntry : this->collected)
	{
		std::string name = collectedEntry.first;
		std::vector<std::shared_ptr<UserDefinedMeasurement>> collectedUDMs = collectedEntry.second;

		if(collectedUDMs.empty())
		{
			continue;
		}

		for(const auto& aggDesc : collectedUDMs.at(0)->getAggregationInfo())
		{
			std::string fieldName = std::string(name) +
				std::string(" ") + std::string(aggDesc.first); 
			aggregates.emplace_back(fieldName, (aggDesc.second)(collectedUDMs));
		}
	}

	return aggregates;
}

UDMAggregationTable DefaultDoubleMeasurement::getAggregationInfo() const
{
	UDMAggregationTable table;

	if(this->reportAverage())
	{
		table.push_back({"Avg", UDMAggregationFunction(&DefaultDoubleMeasurement::getAverage)});
	}

	if(this->reportMin())
	{
		table.push_back({"Min", UDMAggregationFunction(&DefaultDoubleMeasurement::getMin)});
	}

	if(this->reportMax())
	{
		table.push_back({"Max", UDMAggregationFunction(&DefaultDoubleMeasurement::getMax)});
	}

	if(this->reportMedian())
	{
		table.push_back({"Median", UDMAggregationFunction(&DefaultDoubleMeasurement::getMedian)});
	}

	if(this->reportStddev())
	{
		table.push_back({"StdDev", UDMAggregationFunction(&DefaultDoubleMeasurement::getStddev)});
	}

	return table;
}

double DefaultDoubleMeasurement::getAverage(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms)
{
	double avg = 0;
	size_t count = 0;

	for(auto udm : udms)
	{
		DefaultDoubleMeasurement* ddm = dynamic_cast<DefaultDoubleMeasurement*>(udm.get());
		avg += std::accumulate(ddm->values.begin(), ddm->values.end(), 0.0);
		count += ddm->values.size();
	}

	return avg / count;
}

double DefaultDoubleMeasurement::getMin(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms)
{
	double min = std::nan("");

	for(auto udm : udms)
	{
		DefaultDoubleMeasurement* ddm = dynamic_cast<DefaultDoubleMeasurement*>(udm.get());
		if(ddm->values.empty())
		{
			continue;
		}
		min = std::fmin(min, *std::min_element(ddm->values.begin(), ddm->values.end()));
	}

	return min;
}

double DefaultDoubleMeasurement::getMax(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms)
{
	double max = std::nan("");

	for(auto udm : udms)
	{
		DefaultDoubleMeasurement* ddm = dynamic_cast<DefaultDoubleMeasurement*>(udm.get());
		if(ddm->values.empty())
		{
			continue;
		}
		max = std::fmax(max, *std::max_element(ddm->values.begin(), ddm->values.end()));
	}

	return max;
}

double DefaultDoubleMeasurement::getMedian(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms)
{
	std::vector<double> all_elements;

	for(auto udm : udms)
	{
		DefaultDoubleMeasurement* ddm = dynamic_cast<DefaultDoubleMeasurement*>(udm.get());
		all_elements.insert(all_elements.end(), ddm->values.begin(), ddm->values.end());
	}

	if(all_elements.empty())
	{
		return std::nan("");
	}

	std::nth_element(all_elements.begin(),
									 all_elements.begin() + (all_elements.size() / 2),
									 all_elements.end());

	if(all_elements.size() % 2 == 1)
	{
		return *(all_elements.begin() + (all_elements.size() / 2 + 1));
	}
	else
	{
		double left_of_median = *std::max_element(all_elements.begin(),
																							all_elements.begin() + (all_elements.size() / 2) - 1);
		return (*(all_elements.begin() + (all_elements.size() / 2 + 1)) + left_of_median) / 2;
	}
}

double DefaultDoubleMeasurement::getStddev(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms)
{
	double avg = 0;
	size_t count = 0;

	for(auto udm : udms)
	{
		DefaultDoubleMeasurement* ddm = dynamic_cast<DefaultDoubleMeasurement*>(udm.get());
		avg += std::accumulate(ddm->values.begin(), ddm->values.end(), 0.0);
		count += ddm->values.size();
	}

	avg /= count;

	std::vector<double> diff(count);
	size_t i = 0;
	for(auto udm : udms)
	{
		DefaultDoubleMeasurement* ddm = dynamic_cast<DefaultDoubleMeasurement*>(udm.get());
		std::transform(ddm->values.begin(), ddm->values.end(),
									 diff.begin() + i, [avg](double x) { return x - avg; });
		i += ddm->values.size();
	}

	double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
	return std::sqrt(sq_sum / count);
}

void DefaultDoubleMeasurement::addValue(double value)
{
	this->values.push_back(value);
}
