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
	///
	/// \brief Signature for a function that the user must supply for
	/// every aggregation computed on a user-defined measurement
	///
	using UDMAggregationFunction = std::function<
		double(const std::vector<std::shared_ptr<UserDefinedMeasurement>> &)>;

	///
	/// \brief Describes, which aggregations should be computed on a
	/// user-defined measurement.
	///
	/// The string names the aggregation, the UDMAggregateFunction is
	/// the function that will be called on the collected vector of
	/// user-defined measurements.
	///
	using UDMAggregationTable = std::vector<std::pair<std::string, UDMAggregationFunction>>;

	///
	/// \class UserDefinedMeasurement
	///
	/// Base class that the user must derive user-defined measurements from.
	///
	/// \author	Lukas Barth
	///
	class CELERO_EXPORT UserDefinedMeasurement
	{
	protected:
		// Class may never be directly instantiated
		UserDefinedMeasurement() = default;

	public:
		///
		/// \brief Must be implemented by the user. Must return a
		/// specification which aggregations the user wants to be
		/// computed.
		///
		virtual UDMAggregationTable getAggregationInfo() const = 0;

		///
		/// \brief Must be implemented by the user. Must return the name
		/// of this user-defined measurement.
		/// 
		virtual std::string getName() const = 0; };

	///
	/// \brief Convenience class to derive simple double-based
	/// user-defined measurements from.
	///
	/// For most user-defined measurements, this should be enough. The
	/// user just needs to override getName(), and should possibly set
	/// the according report<Aggregation> flags.
	///
	class CELERO_EXPORT DefaultDoubleMeasurement : public UserDefinedMeasurement {
	public:
		DefaultDoubleMeasurement() = default;

		virtual UDMAggregationTable getAggregationInfo() const override;

		///
		/// \brief You must call this method from your fixture to add a measurement
		///
		void addValue(double value);
		
	protected:
		///
		/// \brief Override this to return false if you don't want the average to be reported
		///
		virtual bool reportAverage() const {return true;}
		///
		/// \brief Override this to return false if you don't want the minimum to be reported
		///
		virtual bool reportMin() const {return true;}
		///
		/// \brief Override this to return false if you don't want the maximum to be reported
		///
		virtual bool reportMax() const {return true;}
		///
		/// \brief Override this to return false if you don't want the median to be reported
		///
		virtual bool reportMedian() const {return true;}
		///
		/// \brief Override this to return false if you don't want the standard deviation to be reported
		///
		virtual bool reportStddev() const {return true;}

	private:
		std::vector<double> values;

		static double getAverage(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
		static double getMin(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
		static double getMax(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
		static double getMedian(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
		static double getStddev(std::vector<std::shared_ptr<UserDefinedMeasurement>> udms);
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
