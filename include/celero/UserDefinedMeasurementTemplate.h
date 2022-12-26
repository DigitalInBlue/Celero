#pragma once

///
/// \author	Lukas Barth, John Farrier
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

#include <celero/Statistics.h>
#include <celero/UserDefinedMeasurement.h>

#include <numeric>
#include <type_traits>

namespace celero
{
	///
	/// \class UserDefinedMeasurementTemplate
	///
	/// Base class that the user must derive user-defined measurements from.
	///
	/// \author	Lukas Barth, John Farrier
	///
	template <typename T>
	class UserDefinedMeasurementTemplate : public UserDefinedMeasurement
	{
		static_assert(std::is_arithmetic<T>::value, "UserDefinedMeasurementTemplate requres an arithmetic type.");

	public:
		///
		/// Default constructor
		///
		UserDefinedMeasurementTemplate() = default;

		///
		/// Default destructor
		///
		virtual ~UserDefinedMeasurementTemplate() = default;

		///
		/// \brief Must be implemented by the user. Must return a specification which aggregations the user wants to be computed.
		///
		UDMAggregationTable getAggregationInfo() const override
		{
			UDMAggregationTable table;

			if(this->reportSize() == true)
			{
				table.push_back({"# Samp", [this]() { return static_cast<double>(this->getStatistics().getSize()); }});
			}

			if(this->reportMean() == true)
			{
				table.push_back({"Mean", [this]() { return this->getStatistics().getMean(); }});
			}

			if(this->reportVariance() == true)
			{
				table.push_back({"Var", [this]() { return this->getStatistics().getVariance(); }});
			}

			if(this->reportStandardDeviation() == true)
			{
				table.push_back({"StdDev", [this]() { return this->getStatistics().getStandardDeviation(); }});
			}

			if(this->reportSkewness() == true)
			{
				table.push_back({"Skew", [this]() { return this->getStatistics().getSkewness(); }});
			}

			if(this->reportKurtosis() == true)
			{
				table.push_back({"Kurtosis", [this]() { return this->getStatistics().getKurtosis(); }});
			}

			if(this->reportZScore() == true)
			{
				table.push_back({"ZScore", [this]() { return this->getStatistics().getZScore(); }});
			}

			if(this->reportMin() == true)
			{
				table.push_back({"Min", [this]() { return static_cast<double>(this->getStatistics().getMin()); }});
			}

			if(this->reportMax() == true)
			{
				table.push_back({"Max", [this]() { return static_cast<double>(this->getStatistics().getMax()); }});
			}

			return table;
		}

		///
		/// \brief You must call this method from your fixture to add a measurement
		///
		void addValue(T x)
		{
			this->stats.addSample(x);
		}

		///
		/// Preserve measurements within a group/experiment/problem space set.
		///
		void merge(const UserDefinedMeasurement* const x) override
		{
			const auto toMerge = dynamic_cast<const UserDefinedMeasurementTemplate<T>* const>(x);
			this->stats += toMerge->stats;
		}

	protected:
		virtual bool reportSize() const
		{
			return true;
		}

		virtual bool reportMean() const
		{
			return true;
		}

		virtual bool reportVariance() const
		{
			return true;
		}

		virtual bool reportStandardDeviation() const
		{
			return true;
		}

		virtual bool reportSkewness() const
		{
			return true;
		}

		virtual bool reportKurtosis() const
		{
			return true;
		}

		virtual bool reportZScore() const
		{
			return true;
		}

		virtual bool reportMin() const
		{
			return true;
		}

		virtual bool reportMax() const
		{
			return true;
		}

		const Statistics<T>& getStatistics() const
		{
			return this->stats;
		}

	private:
		/// Continuously gathers statistics without having to retain data history.
		Statistics<T> stats;
	};

} // namespace celero

