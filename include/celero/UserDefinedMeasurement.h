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

#include <celero/Export.h>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace celero
{
	class UserDefinedMeasurement;

	///
	/// \brief Describes, which aggregations should be computed on a user-defined measurement.
	///
	/// The string names the aggregation, the UDMAggregateFunction is the function that will be called on the collected vector of user-defined
	/// measurements.
	///
	using UDMAggregationTable = std::vector<std::pair<std::string, std::function<double(void)>>>;

	///
	/// \class UserDefinedMeasurement
	///
	/// Base class that the user must derive user-defined measurements from.
	///
	/// \author	Lukas Barth
	///
	class CELERO_EXPORT UserDefinedMeasurement
	{
	public:
		///
		/// Expose the default destructor, but make it virtual to support inheritance.
		///
		virtual ~UserDefinedMeasurement() = default;

		///
		/// \brief Must be implemented by the user. Must return a specification which aggregations the user wants to be computed.
		///
		virtual UDMAggregationTable getAggregationInfo() const = 0;

		///
		/// \brief Must be implemented by the user. Must return the name of this user-defined measurement.
		///
		virtual std::string getName() const = 0;

		///
		/// \brief Combine the results of two user defined measurements.
		///
		/// As TestFixture classes are created and destroyed, this provides a mechanisim to preserve data.  Internally, this function is used so that
		/// each unique set of (group, experiment, problem space) has its own combined set of user defined measurements.
		///
		virtual void merge(const UserDefinedMeasurement* const x) = 0;

	protected:
		// Class may never be directly instantiated
		UserDefinedMeasurement() = default;
	};

} // namespace celero

