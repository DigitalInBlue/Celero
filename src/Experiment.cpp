///
/// \author	John Farrier
///
/// \copyright Copyright 2014 John Farrier 
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

#include <celero/Experiment.h>
#include <celero/TestFixture.h>
#include <celero/Factory.h>
#include <celero/PimplImpl.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>

#include <algorithm>
#include <cassert>

using namespace celero;

// ----------------------------------------------------------------------------
//
// Define the Experiment::Result::Impl class.
// (I hate putting more than one class in a file.)
//
// ----------------------------------------------------------------------------

class Experiment::Result::Impl
{
	public:
		Impl()
		{
		}

		Impl(Experiment* p) : 
			stats(),
			problemSpaceValue(0),
			parent(p),
			complete(false)
		{
		}

		/// Track statistics about this specific experiment.
		Statistics stats;
		
		int64_t problemSpaceValue;

		/// A pointer back to our owning Experiment parent.
		Experiment* parent;

		/// A "completed" flag.
		bool complete;
};

// ----------------------------------------------------------------------------
//
// Define the Experiment::Result class.
// (I hate putting more than one class in a file.)
//
// ----------------------------------------------------------------------------

Experiment::Result::Result()
{
}

Experiment::Result::Result(Experiment* x) : pimpl(x)
{
}

Experiment* Experiment::Result::getExperiment() const
{
	return this->pimpl->parent;
}

void Experiment::Result::setProblemSpaceValue(int64_t x)
{
	this->pimpl->problemSpaceValue = x;
}

int64_t Experiment::Result::getProblemSpaceValue() const
{
	return this->pimpl->problemSpaceValue;
}

Statistics* Experiment::Result::getStatistics()
{
	return &this->pimpl->stats;
}

void Experiment::Result::addRunTimeSample(const uint64_t runTime)
{
	this->pimpl->stats.addSample(runTime);
}

uint64_t Experiment::Result::getRunTime() const
{
	return this->pimpl->stats.getMin();
}

double Experiment::Result::getUsPerCall() const
{
	return celero::timer::ConvertSystemTime(this->pimpl->stats.getMin()) / static_cast<double>(this->pimpl->parent->getCalls());
}

double Experiment::Result::getOpsPerSecond() const
{
	return 100000.0 / (this->getUsPerCall() * celero::UsPerSec);
}

double Experiment::Result::getBaselineMeasurement()
{
	if(this->pimpl->parent->getIsBaselineCase() == false)
	{
		auto bm = this->pimpl->parent->getBenchmark();

		if(bm != nullptr)
		{
			auto baselineExperiment = bm->getBaseline();

			if(baselineExperiment != nullptr)
			{
				return this->getUsPerCall() / baselineExperiment->getResultByValue(this->getProblemSpaceValue())->getUsPerCall();
			}
		}

		return -1.0;
	}

	return 1.0;
}

void Experiment::Result::setComplete(bool x)
{
	this->pimpl->complete = x;
}

bool Experiment::Result::getComplete() const
{
	return this->pimpl->complete;
}

// ----------------------------------------------------------------------------
//
// Define the Experiment::Impl class.
// (I hate putting more than one class in a file.)
//
// ----------------------------------------------------------------------------

class Experiment::Impl
{
	public:
		Impl() : 
			benchmark(),
			name(),
			baselineUnit(0),
			baselineTarget(0),
			samples(0),
			calls(0),
			totalRunTime(0),
			isBaselineCase(false)
		{
		}

		Impl(std::weak_ptr<Benchmark> bm, const std::string& name, uint64_t samples, uint64_t calls, double pBaselineTarget) :
			benchmark(bm),
			name(name),
			baselineUnit(0),
			baselineTarget(pBaselineTarget),
			samples(samples),
			calls(calls),
			totalRunTime(0),
			isBaselineCase(false)
		{
		}

		Impl(std::weak_ptr<Benchmark> bm) :
			benchmark(bm),
			name(),
			baselineUnit(0),
			baselineTarget(0),
			samples(0),
			calls(0),
			totalRunTime(0),
			isBaselineCase(false)
		{
		}

		Impl(const Experiment& other) : 
			benchmark(other.pimpl->benchmark),
			name(other.pimpl->name),
			baselineUnit(other.pimpl->baselineUnit),
			baselineTarget(other.pimpl->baselineTarget),
			samples(other.pimpl->samples),
			calls(other.pimpl->calls),
			totalRunTime(other.pimpl->totalRunTime),
			isBaselineCase(other.pimpl->isBaselineCase)
		{
		}

		/// There is one result for each problem space value.
		/// In the event there are not any problem spaces, there shal be a single result.
		std::vector<std::shared_ptr<Experiment::Result>> results;

		/// The owning benchmark object which groups together all experiments.
		std::weak_ptr<Benchmark> benchmark;

		/// The factory to associate with this benchmark.
		std::shared_ptr<Factory> factory;

		/// The name of this experiment.
		std::string name;

		/// The number of microseconds per test (which makes up one baseline unit).
		double baselineUnit;

		/// Used to pass/fail benchmarks when outputting JUnit.
		double baselineTarget;

		/// Test samples to complete.
		uint64_t samples;
		
		/// Calls per test run.  (Size of each sample.)
		uint64_t calls;

		/// The best run time for this test
		uint64_t totalRunTime;

		bool isBaselineCase;
};

// ----------------------------------------------------------------------------
//
// Define the Experiment class.
// (I hate putting more than one class in a file.)
//
// ----------------------------------------------------------------------------

Experiment::Experiment() : 
	pimpl()
{
}

Experiment::Experiment(std::weak_ptr<Benchmark> benchmark) :
	pimpl(benchmark)
{
}

Experiment::Experiment(std::weak_ptr<Benchmark> benchmark, const std::string& name, uint64_t samples, uint64_t calls, double baselineTarget) :
	pimpl(benchmark, name, samples, calls, baselineTarget)
{
}

Experiment::Experiment(const Experiment&)
{
}

Experiment::~Experiment()
{
}

std::shared_ptr<Benchmark> Experiment::getBenchmark()
{
	return this->pimpl->benchmark.lock();
}

void Experiment::setName(const std::string& x)
{
	this->pimpl->name = x;
}

std::string Experiment::getName() const
{
	return this->pimpl->name;
}

void Experiment::setSamples(uint64_t x)
{
	this->pimpl->samples = x;
}

uint64_t Experiment::getSamples() const
{
	return this->pimpl->samples;
}

void Experiment::setCalls(uint64_t x)
{
	this->pimpl->calls = x;
}

uint64_t Experiment::getCalls() const
{
	return this->pimpl->calls;
}

Experiment::operator std::string() const
{
	auto output = this->getShort();

	if(this->getSamples() > 0)
	{
		output += " -- " + std::to_string(this->getSamples());

		if(this->getSamples() == 1)
		{
			output += " run, ";
		}
		else
		{
			output += " samples, ";
		}
	}
	else
	{
		output += " -- Auto Run, ";
	}

	output += std::to_string(this->getCalls());

	if(this->getCalls() == 1)
	{
		output += " call per run.";
	}
	else
	{
		output += " calls per run.";
	}

	return output;
}

std::string Experiment::getShort() const
{
	auto bm = this->pimpl->benchmark.lock();

	if(bm != nullptr)
	{
		return bm->getName() + "." + this->getName();
	}

	return this->getName();
}

void Experiment::setBaselineTarget(double x)
{
	this->pimpl->baselineTarget = x;
}

double Experiment::getBaselineTarget() const
{
	return this->pimpl->baselineTarget;
}

void Experiment::incrementTotalRunTime(const uint64_t x)
{
	this->pimpl->totalRunTime += x;
}

uint64_t Experiment::getTotalRunTime() const
{
	return this->pimpl->totalRunTime;
}

void Experiment::setIsBaselineCase(bool x)
{
	this->pimpl->isBaselineCase = x;
}

bool Experiment::getIsBaselineCase() const
{
	return this->pimpl->isBaselineCase;
}

void Experiment::setFactory(std::shared_ptr<Factory> x)
{
	this->pimpl->factory = x;
}

std::shared_ptr<Factory> Experiment::getFactory() const
{
	return this->pimpl->factory;
}

void Experiment::addProblemSpace(int64_t x)
{
	auto r = std::make_shared<Experiment::Result>(this);
	r->setProblemSpaceValue(x);
	this->pimpl->results.push_back(r);
}

size_t Experiment::getResultSize()
{
	if(this->pimpl->results.empty() == true)
	{
		this->pimpl->results.push_back(std::make_shared<Experiment::Result>(this));
	}

	return this->pimpl->results.size();
}

std::shared_ptr<Experiment::Result> Experiment::getResult(size_t x)
{
	if((x == 0) && (this->pimpl->results.empty() == true))
	{
		this->pimpl->results.push_back(std::make_shared<Experiment::Result>(this));
	}

	return this->pimpl->results[x];
}

std::shared_ptr<Experiment::Result> Experiment::getResultByValue(int64_t x)
{
	std::shared_ptr<Experiment::Result> r;

	auto found = std::find_if(std::begin(this->pimpl->results), std::end(this->pimpl->results), 
		[x](std::shared_ptr<Experiment::Result> i)->bool
		{
			return (i->getProblemSpaceValue() == x);
		});

	if(found != std::end(this->pimpl->results))
	{
		r = *found;
	}

	return r;
}
