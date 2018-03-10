#ifndef H_CELERO_STATISTICS_H
#define H_CELERO_STATISTICS_H

///
/// \author	John Farrier
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

#include <celero/Export.h>
#include <celero/Pimpl.h>
#include <cstdint>

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
	class Statistics
	{
	public:
		///
		/// \brief	Default constructor
		///
		Statistics();

		Statistics(const Statistics& other);

		~Statistics();

		Statistics operator+(const Statistics& other);

		Statistics& operator+=(const Statistics& other);

		Statistics& operator=(const Statistics& other);

		///
		/// Resets all accumulated statistics.
		///
		void reset();

		///
		/// Adds a statistical sample.
		///
		void addSample(uint64_t x);

		size_t getSize() const;

		double getMean() const;

		double getVariance() const;

		double getStandardDeviation() const;

		double getSkewness() const;

		double getKurtosis() const;

		///
		/// Computed as (mean - hypothesis)/standard_deviation
		///
		/// Here, the hypothesis is our minimum value.
		///
		double getZScore() const;

		uint64_t getMin() const;

		uint64_t getMax() const;

	private:
		class Impl;
		Pimpl<Impl> pimpl;
	};
}

#endif
