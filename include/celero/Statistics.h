#pragma once

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

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace celero
{
	///
	/// \class Statistics
	///
	/// \author	John Farrier
	///
	/// Sources:
	/// http://www.johndcook.com/skewness_kurtosis.html
	/// http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
	/// http://prod.sandia.gov/techlib/access-control.cgi/2008/086212.pdf
	/// http://en.wikipedia.org/wiki/Kurtosis
	///
	template <typename T = int64_t>
	class Statistics
	{
		static_assert(std::is_arithmetic<T>::value, "Statistics requres an arithmetic type.");

	public:
		///
		/// \brief	Default constructor
		///
		Statistics() = default;

		///
		///
		///
		Statistics(const Statistics& other)
			: sampleSize(other.sampleSize), M1(other.M1), M2(other.M2), M3(other.M3), M4(other.M4), min(other.min), max(other.max)
		{
		}

		///
		///
		///
		~Statistics() = default;

		///
		///
		///
		Statistics operator+(const Statistics<T>& other)
		{
			Statistics<T> combined;

			combined.sampleSize = this->sampleSize + other.sampleSize;

			const auto delta = other.M1 - this->M1;
			const auto delta2 = delta * delta;
			const auto delta3 = delta * delta2;
			const auto delta4 = delta2 * delta2;

			combined.M1 = (this->sampleSize * this->M1 + other.sampleSize * other.M1) / combined.sampleSize;

			combined.M2 = this->M2 + other.M2 + delta2 * this->sampleSize * other.sampleSize / combined.sampleSize;

			combined.M3 =
				this->M3 + other.M3
				+ delta3 * this->sampleSize * other.sampleSize * (this->sampleSize - other.sampleSize) / (combined.sampleSize * combined.sampleSize);

			combined.M3 += 3.0 * delta * (this->sampleSize * other.M2 - other.sampleSize * this->M2) / combined.sampleSize;

			combined.M4 = this->M4 + other.M4
						  + delta4 * this->sampleSize * other.sampleSize
								* (this->sampleSize * this->sampleSize - this->sampleSize * other.sampleSize + other.sampleSize * other.sampleSize)
								/ (combined.sampleSize * combined.sampleSize * combined.sampleSize);

			combined.M4 += 6.0 * delta2 * (this->sampleSize * this->sampleSize * other.M2 + other.sampleSize * other.sampleSize * this->M2)
							   / (combined.sampleSize * combined.sampleSize)
						   + 4.0 * delta * (this->sampleSize * other.M3 - other.sampleSize * this->M3) / combined.sampleSize;

			combined.min = std::min(this->min, other.min);
			combined.max = std::max(this->max, other.max);

			return combined;
		}

		Statistics& operator+=(const Statistics& other)
		{
			const auto combined = *this + other;
			*this = combined;
			return *this;
		}

		Statistics& operator=(const Statistics& other)
		{
			this->sampleSize = other.sampleSize;
			this->M1 = other.M1;
			this->M2 = other.M2;
			this->M3 = other.M3;
			this->M4 = other.M4;
			this->min = other.min;
			this->max = other.max;

			return *this;
		}

		///
		/// Resets all accumulated statistics.
		///
		void reset()
		{
			this->sampleSize = 0;
			this->M1 = 0;
			this->M2 = 0;
			this->M3 = 0;
			this->M4 = 0;
			this->min = std::numeric_limits<decltype(this->min)>::max();
			this->max = std::numeric_limits<decltype(this->max)>::min();
		}

		///
		/// Adds a statistical sample.
		///
		void addSample(T x)
		{
			const auto n1 = this->sampleSize;
			this->sampleSize++;

			const auto delta = x - this->M1;
			const auto delta_n = delta / static_cast<decltype(delta)>(this->sampleSize);
			const auto delta_n2 = delta_n * delta_n;
			const auto term1 = delta * delta_n * n1;

			this->M1 += delta_n;
			this->M4 += term1 * delta_n2 * (this->sampleSize * this->sampleSize - 3 * this->sampleSize + 3) + 6 * delta_n2 * this->M2
						- 4 * delta_n * this->M3;
			this->M3 += term1 * delta_n * (this->sampleSize - 2) - 3 * delta_n * this->M2;
			this->M2 += term1;

			this->min = std::min(this->min, x);
			this->max = std::max(this->max, x);
		}

		size_t getSize() const
		{
			return this->sampleSize;
		}

		double getMean() const
		{
			return this->M1;
		}

		double getVariance() const
		{
			if(this->sampleSize > 1)
			{
				return this->M2 / (this->sampleSize - 1);
			}

			return 0.0;
		}

		double getStandardDeviation() const
		{
			return std::sqrt(this->getVariance());
		}

		double getSkewness() const
		{
			if(this->sampleSize > 2)
			{
				return sqrt(this->sampleSize) * this->M3 / pow(this->M2, 1.5);
			}

			return 0.0;
		}

		double getKurtosis() const
		{
			if(this->sampleSize > 3)
			{
				if(this->M2 != 0)
				{
					return this->sampleSize * this->M4 / (this->M2 * this->M2) - 3.0;
				}
			}

			return 0.0;
		}

		///
		/// Computed as (mean - hypothesis)/standard_deviation
		///
		/// Here, the hypothesis is our minimum value.
		///
		double getZScore() const
		{
			const auto sd = this->getStandardDeviation();

			if(sd != 0.0)
			{
				return (this->getMean() - static_cast<double>(this->getMin())) / sd;
			}

			return 0.0;
		}

		T getMin() const
		{
			return this->min;
		}

		T getMax() const
		{
			return this->max;
		}

	private:
		size_t sampleSize{0};
		double M1{0.0};
		double M2{0.0};
		double M3{0.0};
		double M4{0.0};
		T min{std::numeric_limits<T>::max()};
		T max{std::numeric_limits<T>::min()};
	};
} // namespace celero

