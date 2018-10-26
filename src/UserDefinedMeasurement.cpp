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
	for (auto udm : fixture->getUserDefinedMeasurements()) {
		this->collected[udm->getName()].push_back(udm);
	}
}

std::vector<std::string> UDMCollector::getFields(std::shared_ptr<TestFixture> fixture) const
{
	std::vector<std::string> fields;

	for (auto udm : fixture->getUserDefinedMeasurements()) {
		for(const auto& aggDesc : udm->getAggregationInfo())
		{
			fields.emplace_back(std::string(udm->getName()) + std::string(" ") + std::string(aggDesc.first));
		}
	}

	return fields;
}

std::vector<std::pair<std::string, double>>
UDMCollector::getAggregateValues() const
{
	std::vector<std::pair<std::string, double>> aggregates;
	
	for (const auto & collectedEntry : this->collected) {
		std::string name = collectedEntry.first;
		std::vector<std::shared_ptr<UserDefinedMeasurement>> collectedUDMs = collectedEntry.second;

		if (collectedUDMs.empty()) {
			continue;
		}

		for(const auto& aggDesc : collectedUDMs.at(0)->getAggregationInfo())
		{
			std::string fieldName = std::string(name) + std::string(" ") + std::string(aggDesc.first); // TODO factor out
			aggregates.emplace_back(fieldName, (aggDesc.second)(collectedUDMs));
		}
	}

	return aggregates;
}

UDMAggregationTable
DefaultDoubleMeasurement::getAggregationInfo() const
{
	return {
		{"Avg", UDMAggregationFunction(&DefaultDoubleMeasurement::getAverage)}
	};
}

double
DefaultDoubleMeasurement::getAverage(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms)
{
	double avg = 0;
	size_t count = 0;
	
	for (auto udm : udms) {
		DefaultDoubleMeasurement * ddm = dynamic_cast<DefaultDoubleMeasurement *>(udm.get());
		avg += std::accumulate(ddm->values.begin(), ddm->values.end(), 0.0);
		count += ddm->values.size();
	}

	return avg / count;
}

void
DefaultDoubleMeasurement::addValue(double value)
{
	this->values.push_back(value);
}
