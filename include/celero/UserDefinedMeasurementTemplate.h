#ifndef H_CELERO_USERDEFINEDMEASUREMENTTEMPLATE_H
#define H_CELERO_USERDEFINEDMEASUREMENTTEMPLATE_H

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
	/// \author	Lukas Barth
	///
	template <typename T>
	class UserDefinedMeasurementTemplate : public UserDefinedMeasurement
	{
		static_assert(std::is_arithmetic<T>::value, "UserDefinedMeasurementTemplate requres an arithmetic type.");

	public:
		UserDefinedMeasurementTemplate() = default;

		///
		/// \brief Must be implemented by the user. Must return a specification which aggregations the user wants to be computed.
		///
		virtual UDMAggregationTable getAggregationInfo() const
		{
			UDMAggregationTable table;

			if(this->reportAverage())
			{
				table.push_back({"Avg", UDMAggregationFunction(&UserDefinedMeasurementTemplate<T>::getAverage)});
			}

			if(this->reportMin())
			{
				table.push_back({"Min", UDMAggregationFunction(&UserDefinedMeasurementTemplate<T>::getMin)});
			}

			if(this->reportMax())
			{
				table.push_back({"Max", UDMAggregationFunction(&UserDefinedMeasurementTemplate<T>::getMax)});
			}

			if(this->reportMedian())
			{
				table.push_back({"Median", UDMAggregationFunction(&UserDefinedMeasurementTemplate<T>::getMedian)});
			}

			return table;
		}

		///
		/// \brief You must call this method from your fixture to add a measurement
		///
		void addValue(T x)
		{
			this->values.push_back(x);
		}

	protected:
		///
		/// \brief Override this to return false if you don't want the average to be reported
		///
		virtual bool reportAverage() const
		{
			return true;
		}

		///
		/// \brief Override this to return false if you don't want the minimum to be reported
		///
		virtual bool reportMin() const
		{
			return true;
		}

		///
		/// \brief Override this to return false if you don't want the maximum to be reported
		///
		virtual bool reportMax() const
		{
			return true;
		}

		///
		/// \brief Override this to return false if you don't want the median to be reported
		///
		virtual bool reportMedian() const
		{
			return true;
		}

	private:
		static double getAverage(const std::vector<std::shared_ptr<UserDefinedMeasurement>>& x)
		{
			auto avg = 0.0;
			size_t count = 0;

			for(auto udm : x)
			{
				const auto ddm = dynamic_cast<UserDefinedMeasurementTemplate<T>*>(udm.get());
				avg += std::accumulate(ddm->values.begin(), ddm->values.end(), 0.0);
				count += ddm->values.size();
			}

			return avg / count;
		}

		static double getMin(const std::vector<std::shared_ptr<UserDefinedMeasurement>>& x)
		{
			auto min = std::nan("");

			for(auto udm : x)
			{
				const auto ddm = dynamic_cast<UserDefinedMeasurementTemplate<T>*>(udm.get());

				if(ddm->values.empty())
				{
					continue;
				}

				min = std::fmin(min, *std::min_element(ddm->values.begin(), ddm->values.end()));
			}

			return min;
		}

		static double getMax(const std::vector<std::shared_ptr<UserDefinedMeasurement>>& x)
		{
			auto max = std::nan("");

			for(auto udm : x)
			{
				const auto ddm = dynamic_cast<UserDefinedMeasurementTemplate<T>*>(udm.get());

				if(ddm->values.empty())
				{
					continue;
				}

				max = std::fmax(max, *std::max_element(ddm->values.begin(), ddm->values.end()));
			}

			return max;
		}

		static double getMedian(const std::vector<std::shared_ptr<UserDefinedMeasurement>>& x)
		{
			std::vector<double> allElements;

			for(auto udm : x)
			{
				const auto ddm = dynamic_cast<UserDefinedMeasurementTemplate<T>*>(udm.get());
				for(auto value : ddm->values)
				{
					allElements.push_back(static_cast<double>(value));
				}
			}

			if(allElements.empty())
			{
				return std::nan("");
			}

			std::nth_element(allElements.begin(), allElements.begin() + (allElements.size() / 2), allElements.end());

			if(allElements.size() % 2 == 1)
			{
				return *(allElements.begin() + (allElements.size() / 2 + 1));
			}
			else
			{
				const auto left_of_median = *std::max_element(allElements.begin(), allElements.begin() + (allElements.size() / 2) - 1);
				return (*(allElements.begin() + (allElements.size() / 2 + 1)) + left_of_median) / 2;
			}
		}

		std::vector<T> values;
	};

} // namespace celero

#endif
