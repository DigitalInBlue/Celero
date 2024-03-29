#pragma once

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

#include <celero/UserDefinedMeasurement.h>
#include <celero/TestFixture.h>
#include <unordered_map>

namespace celero
{
	///
	/// \class UserDefinedMeasurementCollector
	///
	/// \author	Lukas Barth
	///
	class CELERO_EXPORT UserDefinedMeasurementCollector
	{
	public:
		UserDefinedMeasurementCollector(std::shared_ptr<TestFixture> fixture);

		void collect(std::shared_ptr<TestFixture> fixture);
		std::vector<std::string> getFields(std::shared_ptr<TestFixture> fixture) const;
		std::vector<std::pair<std::string, double>> getAggregateValues() const;

	private:
		std::unordered_map<std::string, std::shared_ptr<celero::UserDefinedMeasurement>> collected;
	};
} // namespace celero

