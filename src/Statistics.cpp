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

#include <celero/Statistics.h>

#include <celero/PimplImpl.h>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace celero;

class Statistics::Impl
{
public:
	Impl() :
		sampleSize(0),
		M1(0),
		M2(0),
		M3(0),
		M4(0),
		min(std::numeric_limits<decltype(Impl::min)>::max()),
		max(std::numeric_limits<decltype(Impl::max)>::min())
	{
	}

	explicit Impl(const Statistics& other) :
		sampleSize(other.pimpl->sampleSize),
		M1(other.pimpl->M1),
		M2(other.pimpl->M2),
		M3(other.pimpl->M3),
		M4(other.pimpl->M4),
		min(other.pimpl->min),
		max(other.pimpl->max)
	{
	}

	size_t sampleSize;
	double M1;
	double M2;
	double M3;
	double M4;
	uint64_t min;
	uint64_t max;
};

Statistics::Statistics()
{
}

Statistics::Statistics(const Statistics& other) : pimpl(other)
{
}

Statistics::~Statistics()
{
}

Statistics Statistics::operator+(const Statistics& other)
{
	Statistics combined;

	combined.pimpl->sampleSize = this->pimpl->sampleSize + other.pimpl->sampleSize;

	const auto delta = other.pimpl->M1 - this->pimpl->M1;
	const auto delta2 = delta * delta;
	const auto delta3 = delta * delta2;
	const auto delta4 = delta2 * delta2;

	combined.pimpl->M1 = (this->pimpl->sampleSize * this->pimpl->M1 + other.pimpl->sampleSize * other.pimpl->M1) / combined.pimpl->sampleSize;

	combined.pimpl->M2 = this->pimpl->M2 + other.pimpl->M2 + delta2 * this->pimpl->sampleSize * other.pimpl->sampleSize / combined.pimpl->sampleSize;

	combined.pimpl->M3 = this->pimpl->M3 + other.pimpl->M3
						 + delta3 * this->pimpl->sampleSize * other.pimpl->sampleSize * (this->pimpl->sampleSize - other.pimpl->sampleSize)
							   / (combined.pimpl->sampleSize * combined.pimpl->sampleSize);

	combined.pimpl->M3 +=
		3.0 * delta * (this->pimpl->sampleSize * other.pimpl->M2 - other.pimpl->sampleSize * this->pimpl->M2) / combined.pimpl->sampleSize;

	combined.pimpl->M4 = this->pimpl->M4 + other.pimpl->M4
						 + delta4 * this->pimpl->sampleSize * other.pimpl->sampleSize
							   * (this->pimpl->sampleSize * this->pimpl->sampleSize - this->pimpl->sampleSize * other.pimpl->sampleSize
								  + other.pimpl->sampleSize * other.pimpl->sampleSize)
							   / (combined.pimpl->sampleSize * combined.pimpl->sampleSize * combined.pimpl->sampleSize);

	combined.pimpl->M4 +=
		6.0 * delta2
			* (this->pimpl->sampleSize * this->pimpl->sampleSize * other.pimpl->M2
			   + other.pimpl->sampleSize * other.pimpl->sampleSize * this->pimpl->M2)
			/ (combined.pimpl->sampleSize * combined.pimpl->sampleSize)
		+ 4.0 * delta * (this->pimpl->sampleSize * other.pimpl->M3 - other.pimpl->sampleSize * this->pimpl->M3) / combined.pimpl->sampleSize;

	combined.pimpl->min = std::min(this->pimpl->min, other.pimpl->min);
	combined.pimpl->max = std::max(this->pimpl->max, other.pimpl->max);

	return combined;
}

Statistics& Statistics::operator=(const Statistics& other)
{
	this->pimpl->sampleSize = other.pimpl->sampleSize;
	this->pimpl->M1 = other.pimpl->M1;
	this->pimpl->M2 = other.pimpl->M2;
	this->pimpl->M3 = other.pimpl->M3;
	this->pimpl->M4 = other.pimpl->M4;
	this->pimpl->min = other.pimpl->min;
	this->pimpl->max = other.pimpl->max;

	return *this;
}

Statistics& Statistics::operator+=(const Statistics& other)
{
	const auto combined = *this + other;
	*this = combined;
	return *this;
}

///
/// Resets all accumulated statistics.
///
void Statistics::reset()
{
	this->pimpl->sampleSize = 0;
	this->pimpl->M1 = 0;
	this->pimpl->M2 = 0;
	this->pimpl->M3 = 0;
	this->pimpl->M4 = 0;
	this->pimpl->min = std::numeric_limits<decltype(this->pimpl->min)>::max();
	this->pimpl->max = std::numeric_limits<decltype(this->pimpl->max)>::min();
}

///
/// Adds a statistical sample.
///
void Statistics::addSample(uint64_t x)
{
	const auto n1 = this->pimpl->sampleSize;
	this->pimpl->sampleSize++;

	const auto delta = x - this->pimpl->M1;
	const auto delta_n = delta / this->pimpl->sampleSize;
	const auto delta_n2 = delta_n * delta_n;
	const auto term1 = delta * delta_n * n1;

	this->pimpl->M1 += delta_n;
	this->pimpl->M4 += term1 * delta_n2 * (this->pimpl->sampleSize * this->pimpl->sampleSize - 3 * this->pimpl->sampleSize + 3)
					   + 6 * delta_n2 * this->pimpl->M2 - 4 * delta_n * this->pimpl->M3;
	this->pimpl->M3 += term1 * delta_n * (this->pimpl->sampleSize - 2) - 3 * delta_n * this->pimpl->M2;
	this->pimpl->M2 += term1;

	this->pimpl->min = std::min(this->pimpl->min, x);
	this->pimpl->max = std::max(this->pimpl->max, x);
}

size_t Statistics::getSize() const
{
	return this->pimpl->sampleSize;
}

double Statistics::getMean() const
{
	return this->pimpl->M1;
}

double Statistics::getVariance() const
{
	if(this->pimpl->sampleSize > 1)
	{
		return this->pimpl->M2 / (this->pimpl->sampleSize - 1);
	}

	return 0.0;
}

double Statistics::getStandardDeviation() const
{
	return std::sqrt(this->getVariance());
}

double Statistics::getSkewness() const
{
	if(this->pimpl->sampleSize > 2)
	{
		return sqrt(this->pimpl->sampleSize) * this->pimpl->M3 / pow(this->pimpl->M2, 1.5);
	}

	return 0.0;
}

double Statistics::getKurtosis() const
{
	if(this->pimpl->sampleSize > 3)
	{
		if(this->pimpl->M2 != 0)
		{
			return this->pimpl->sampleSize * this->pimpl->M4 / (this->pimpl->M2 * this->pimpl->M2) - 3.0;
		}
	}

	return 0.0;
}

double Statistics::getZScore() const
{
	const auto sd = this->getStandardDeviation();

	if(sd != 0.0)
	{
		auto mean = this->getMean();
		auto hypothesis = static_cast<double>(this->getMin());
		return (mean - hypothesis) / sd;
	}

	return 0.0;
}

uint64_t Statistics::getMin() const
{
	return this->pimpl->min;
}

uint64_t Statistics::getMax() const
{
	return this->pimpl->max;
}
