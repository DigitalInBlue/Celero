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

#include <celero/Benchmark.h>
#include <celero/Experiment.h>
#include <celero/ExperimentResult.h>
#include <celero/PimplImpl.h>
#include <celero/Statistics.h>
#include <celero/Timer.h>
#include <celero/UserDefinedMeasurementCollector.h>
#include <celero/Utilities.h>

#include <algorithm>
#include <cassert>

using namespace celero;

class ExperimentResult::Impl
{
public:
	Impl()
	{
	}

	explicit Impl(Experiment* const p) : parent(p)
	{
	}

	/// Track statistics related to execution time about this specific experiment.
	Statistics<int64_t> statsTime;
	Statistics<int64_t> statsRAM;

	std::shared_ptr<UserDefinedMeasurementCollector> udmCollector;

	std::shared_ptr<celero::TestFixture::ExperimentValue> problemSpace;
	double problemSpaceValueScale{1.0};

	/// A pointer back to our owning Experiment parent.
	Experiment* parent{nullptr};

	/// A "completed" flag.
	bool complete{false};

	/// A "failure" flag.
	bool failure{false};
};

ExperimentResult::ExperimentResult()
{
}

ExperimentResult::~ExperimentResult()
{
}

ExperimentResult::ExperimentResult(Experiment* x) : pimpl(x)
{
}

Experiment* ExperimentResult::getExperiment() const
{
	return this->pimpl->parent;
}

void ExperimentResult::setProblemSpaceValue(std::shared_ptr<celero::TestFixture::ExperimentValue> x, double scale)
{
	this->pimpl->problemSpace = x;
	this->pimpl->problemSpaceValueScale = scale;
}

std::shared_ptr<celero::TestFixture::ExperimentValue> ExperimentResult::getProblemSpace() const
{
	return this->pimpl->problemSpace;
}

int64_t ExperimentResult::getProblemSpaceValue() const
{
	return this->pimpl->problemSpace->Value;
}

double ExperimentResult::getProblemSpaceValueScale() const
{
	return this->pimpl->problemSpaceValueScale;
}

uint64_t ExperimentResult::getProblemSpaceIterations() const
{
	return this->pimpl->problemSpace->Iterations;
}

const Statistics<int64_t>& ExperimentResult::getTimeStatistics() const
{
	return this->pimpl->statsTime;
}

const Statistics<int64_t>& ExperimentResult::getRAMStatistics() const
{
	return this->pimpl->statsRAM;
}

void ExperimentResult::addRunTimeSample(const uint64_t x)
{
	this->pimpl->statsTime.addSample(static_cast<int64_t>(x));
}

void ExperimentResult::addMemorySample(const int64_t x)
{
	this->pimpl->statsRAM.addSample(static_cast<int64_t>(x));
}

uint64_t ExperimentResult::getRunTime() const
{
	return static_cast<uint64_t>(this->pimpl->statsTime.getMin());
}

int64_t ExperimentResult::getRAM() const
{
	return static_cast<uint64_t>(this->pimpl->statsRAM.getMin());
}

double ExperimentResult::getUsPerCall() const
{
	if(this->pimpl->failure == false)
	{
		return static_cast<double>(this->pimpl->statsTime.getMin()) / static_cast<double>(this->pimpl->problemSpace->Iterations);
	}

	return 0.0;
}

double ExperimentResult::getCallsPerSecond() const
{
	if(this->pimpl->failure == false)
	{
		return 1.0 / (this->getUsPerCall() * celero::UsToSec);
	}

	return 0.0;
}

double ExperimentResult::getUnitsPerSecond() const
{
	return (this->pimpl->problemSpaceValueScale > 0.0)
			   ? ((this->pimpl->problemSpace->Value * this->pimpl->problemSpace->Iterations / this->pimpl->problemSpaceValueScale)
				  / (this->pimpl->statsTime.getMin() * celero::UsToSec))
			   : 0.0;
}

double ExperimentResult::getBaselineMeasurement() const
{
	if(this->pimpl->parent->getIsBaselineCase() == false)
	{
		const auto bm = this->pimpl->parent->getBenchmark();

		if(bm != nullptr)
		{
			const auto baselineExperiment = bm->getBaseline();

			if(baselineExperiment != nullptr)
			{
				const auto baselineResult = baselineExperiment->getResultByValue(this->getProblemSpaceValue());

				if(baselineResult != nullptr)
				{
					const auto baselineResultUs = baselineResult->getUsPerCall();

					// Prevent possible divide by zero.
					if(baselineResultUs > 0)
					{
						return this->getUsPerCall() / baselineResult->getUsPerCall();
					}
				}
			}
		}

		return -1.0;
	}

	return 1.0;
}

void ExperimentResult::setComplete(bool x)
{
	this->pimpl->complete = x;
}

bool ExperimentResult::getComplete() const
{
	return this->pimpl->complete;
}

void ExperimentResult::setFailure(bool x)
{
	this->pimpl->failure = x;
}

bool ExperimentResult::getFailure() const
{
	return this->pimpl->failure;
}

void ExperimentResult::setUserDefinedMeasurements(std::shared_ptr<UserDefinedMeasurementCollector> x)
{
	this->pimpl->udmCollector = x;
}

std::shared_ptr<UserDefinedMeasurementCollector> ExperimentResult::getUserDefinedMeasurements() const
{
	return this->pimpl->udmCollector;
}
