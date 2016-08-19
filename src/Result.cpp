///
/// \author	John Farrier
///
/// \copyright Copyright 2016 John Farrier
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

#include <celero/Result.h>
#include <celero/PimplImpl.h>

#include <celero/Benchmark.h>
#include <celero/Statistics.h>
#include <celero/Timer.h>
#include <celero/Utilities.h>
#include <celero/Experiment.h>

#include <algorithm>
#include <cassert>

using namespace celero;

class Result::Impl
{
	public:
		Impl() :
			stats(),
			problemSpaceValue(0),
			problemSpaceValueScale(1.0),
			problemSpaceIterations(0),
			parent(nullptr),
			complete(false)
		{
		}

		explicit Impl(Experiment* const p) :
			stats(),
			problemSpaceValue(0),
			problemSpaceValueScale(1.0),
			problemSpaceIterations(1),
			parent(p),
			complete(false)
		{
		}

		/// Track statistics about this specific experiment.
		Statistics stats;

		int64_t problemSpaceValue;
		double problemSpaceValueScale;
		uint64_t problemSpaceIterations;

		/// A pointer back to our owning Experiment parent.
		Experiment* parent;

		/// A "completed" flag.
		bool complete;
};

Result::Result()
{
}

Result::Result(Experiment* x) : pimpl(x)
{
}

Result::~Result()
{
}

Experiment* Result::getExperiment() const
{
	return this->pimpl->parent;
}

void Result::setProblemSpaceValue(int64_t x, double scale, uint64_t iterations)
{
	this->pimpl->problemSpaceValue = x;
	this->pimpl->problemSpaceValueScale = scale;
	this->pimpl->problemSpaceIterations = iterations;
}

int64_t Result::getProblemSpaceValue() const
{
	return this->pimpl->problemSpaceValue;
}

double Result::getProblemSpaceValueScale() const
{
	return this->pimpl->problemSpaceValueScale;
}

uint64_t Result::getProblemSpaceIterations() const
{
	return this->pimpl->problemSpaceIterations;
}

Statistics* Result::getStatistics()
{
	return &this->pimpl->stats;
}

void Result::addRunTimeSample(const uint64_t runTime)
{
	this->pimpl->stats.addSample(runTime);
}

uint64_t Result::getRunTime() const
{
	return this->pimpl->stats.getMin();
}

double Result::getUsPerCall() const
{
	return static_cast<double>(this->pimpl->stats.getMin()) / static_cast<double>(this->pimpl->problemSpaceIterations);
}

double Result::getCallsPerSecond() const
{
	return 1.0 / (this->getUsPerCall() * 1.0e-6);
}

double Result::getUnitsPerSecond() const
{
	return (this->pimpl->problemSpaceValueScale > 0) ? ((this->pimpl->problemSpaceValue * this->pimpl->problemSpaceIterations / this->pimpl->problemSpaceValueScale) / (this->pimpl->stats.getMin() * 1.0e-6)) : 0;
}

double Result::getBaselineMeasurement()
{
	if(this->pimpl->parent->getIsBaselineCase() == false)
	{
		auto bm = this->pimpl->parent->getBenchmark();

		if(bm != nullptr)
		{
			auto baselineExperiment = bm->getBaseline();

			if(baselineExperiment != nullptr)
			{
				auto baselineResult = baselineExperiment->getResultByValue(this->getProblemSpaceValue());
				if(baselineResult){
						return this->getUsPerCall() / baselineResult->getUsPerCall();
				}
			}
		}

		return -1.0;
	}

	return 1.0;
}

void Result::setComplete(bool x)
{
	this->pimpl->complete = x;
}

bool Result::getComplete() const
{
	return this->pimpl->complete;
}
