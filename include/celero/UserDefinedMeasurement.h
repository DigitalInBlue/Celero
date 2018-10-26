#ifndef H_CELERO_USERDEFINEDMEASUREMENT_H
#define H_CELERO_USERDEFINEDMEASUREMENT_H

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

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <celero/Export.h>

namespace celero
{
	class UserDefinedMeasurement;
	class TestFixture;
	using UDMAggregationFunction = std::function<double(const std::vector<std::shared_ptr<UserDefinedMeasurement>> &)>;
	using UDMAggregationTable = std::vector<std::pair<std::string, UDMAggregationFunction>>;

	///
	/// \class UserDefinedMeasurement
	///
	/// \author	Lukas Barth
	///
	class CELERO_EXPORT UserDefinedMeasurement
	{
	protected:
		///
		/// \brief Class may never be directly instantiated
		///
		UserDefinedMeasurement() = default;

	public:
		virtual UDMAggregationTable getAggregationInfo() const = 0;
		virtual std::string getName() const = 0;
	};

	class CELERO_EXPORT DefaultDoubleMeasurement : public UserDefinedMeasurement {
	public:
		DefaultDoubleMeasurement() = default;

		virtual UDMAggregationTable getAggregationInfo() const override;
		void addValue(double value);
		
	private:
		std::vector<double> values;

		static double getAverage(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
		static double getMin(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
		static double getMax(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
		static double getMedian(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
	};
	
	class UDMCollector
	{
	public:
		UDMCollector(std::shared_ptr<TestFixture> fixture);

		void collect(std::shared_ptr<TestFixture> fixture);
		std::vector<std::string> getFields(std::shared_ptr<TestFixture> fixture) const;
		std::vector<std::pair<std::string, double>> getAggregateValues() const;

	private:
		std::unordered_map<std::string, std::vector<std::shared_ptr<UserDefinedMeasurement>>> collected;
	};
} // namespace celero

#endif
