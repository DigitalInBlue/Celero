#include <celero/BenchmarkInfo.h>
#include <celero/TestFixture.h>
#include <celero/Factory.h>
#include <celero/PimplImpl.h>
#include <celero/Utilities.h>

using namespace celero;

class BenchmarkInfo::Impl
{
	public:
		Impl() : 
			groupName(),
			benchmarkName(),
			samples(0),
			calls(0),
			baselineUnit(1),
			bestRunTime(UINT64_MAX),
			totalRunTime(0),
			factory()
		{
		}

		Impl(const std::string& groupName, const std::string& benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<Factory> testFactory) :
			groupName(groupName),
			benchmarkName(benchmarkName),
			samples(samples),
			calls(calls),
			baselineUnit(1),
			bestRunTime(UINT64_MAX),
			totalRunTime(0),
			factory(testFactory)
		{
		}

		/// Group name
		std::string groupName;
		
		/// Test name
		std::string benchmarkName;

		/// Test samples completed
		uint64_t samples;
		
		/// Calls per test run
		uint64_t calls;

		/// The number of microseconds making up one baseline unit
		uint64_t baselineUnit;

		/// The best run time for this test
		uint64_t bestRunTime;

		/// The best run time for this test
		uint64_t totalRunTime;
		
		/// The factory to associate with this benchmark.
		std::shared_ptr<Factory> factory;

};

BenchmarkInfo::BenchmarkInfo(const std::string& groupName, const std::string& benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<Factory> testFactory) :
	pimpl(groupName, benchmarkName, samples, calls, testFactory)
{
}

BenchmarkInfo::~BenchmarkInfo()
{
}

std::string BenchmarkInfo::getGroupName() const
{
	return this->pimpl->groupName;
}

std::string BenchmarkInfo::getTestName() const
{
	return this->pimpl->benchmarkName;
}

void BenchmarkInfo::incrementSamples()
{
	this->pimpl->samples++;
}

uint64_t BenchmarkInfo::getSamples() const
{
	return this->pimpl->samples;
}

uint64_t BenchmarkInfo::getOps() const
{
	return this->pimpl->calls;
}

std::shared_ptr<Factory> BenchmarkInfo::getFactory() const
{
	return this->pimpl->factory;
}

std::string BenchmarkInfo::get() const
{
	std::string output;

	output += this->getGroupName() + "." + this->getTestName();

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

	output += std::to_string(this->getOps());

	if(this->getOps() == 1)
	{
		output += " operation per run.";
	}
	else
	{
		output += " calls per run.";
	}

	return std::move(output);
}

std::string BenchmarkInfo::getShort() const
{
	std::string output;
	output += this->getGroupName() + "." + this->getTestName();
	return std::move(output);
}

void BenchmarkInfo::setBaselineUnit(const uint64_t x)
{
	this->pimpl->baselineUnit = x;
}

uint64_t BenchmarkInfo::getBaselineUnit() const
{
	return this->pimpl->baselineUnit;
}

void BenchmarkInfo::setRunTime(const uint64_t runTime)
{
	this->pimpl->bestRunTime = std::min(runTime, this->pimpl->bestRunTime);
}

uint64_t BenchmarkInfo::getRunTime() const
{
	return this->pimpl->bestRunTime;
}

void BenchmarkInfo::incrementTotalRunTime(const uint64_t x)
{
	this->pimpl->totalRunTime += x;
}

uint64_t BenchmarkInfo::getTotalRunTime() const
{
	return this->pimpl->totalRunTime;
}

double BenchmarkInfo::getUsPerOp() const
{
	return static_cast<double>(this->getRunTime()) / static_cast<double>(this->getOps());
}

double BenchmarkInfo::getOpsPerSecond() const
{
	return celero::UsPerSec / this->getUsPerOp();
}