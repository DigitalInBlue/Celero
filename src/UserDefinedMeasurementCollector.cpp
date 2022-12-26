///
/// \author	Lukas Barth
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
#include <celero/TestFixture.h>
#include <celero/UserDefinedMeasurementCollector.h>

using namespace celero;

UserDefinedMeasurementCollector::UserDefinedMeasurementCollector(std::shared_ptr<TestFixture> fixture)
{
	const auto udm = fixture->getUserDefinedMeasurementNames();

	if(udm.empty() == false)
	{
		for(auto name : fixture->getUserDefinedMeasurementNames())
		{
			this->collected[name] = nullptr;
		}
	}
}

void UserDefinedMeasurementCollector::collect(std::shared_ptr<TestFixture> fixture)
{
	const auto udms = fixture->getUserDefinedMeasurements();

	if(udms.empty() == false)
	{
		for(auto udm : udms)
		{
			if(this->collected[udm->getName()] == nullptr)
			{
				this->collected[udm->getName()] = udm;
			}
			else
			{
				this->collected[udm->getName()]->merge(&*udm);
			}
		}
	}
}

std::vector<std::string> UserDefinedMeasurementCollector::getFields(std::shared_ptr<TestFixture> fixture) const
{
	std::vector<std::string> fields;
	const auto udms = fixture->getUserDefinedMeasurements();

	if(udms.empty() == false)
	{
		for(auto udm : udms)
		{
			for(const auto& aggDesc : udm->getAggregationInfo())
			{
				fields.emplace_back(std::string(udm->getName()) + std::string(" ") + std::string(aggDesc.first));
			}
		}
	}

	return fields;
}

std::vector<std::pair<std::string, double>> UserDefinedMeasurementCollector::getAggregateValues() const
{
	std::vector<std::pair<std::string, double>> aggregates;

	for(const auto& collectedEntry : this->collected)
	{
		const auto name = collectedEntry.first;
		const auto collectedUDMs = collectedEntry.second;

		for(const auto& aggDesc : collectedUDMs->getAggregationInfo())
		{
			const auto fieldName = name + std::string(" ") + aggDesc.first;
			aggregates.emplace_back(fieldName, (aggDesc.second)());
		}
	}

	return aggregates;
}
