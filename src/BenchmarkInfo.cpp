#include <celero/BenchmarkInfo.h>
#include <celero/TestFixture.h>
#include <celero/Factory.h>
#include <celero/PimplImpl.h>
#include <celero/TestVector.h>
#include <celero/Utilities.h>

#include <algorithm>

using namespace celero;

class BenchmarkInfo::Impl
{
	public:
		Impl() : 
			groupName(),
			benchmarkName(),
			baselineUnit(1),
			samples(0),
			resetSamples(0),
			calls(0),
			resetCalls(0),
			bestRunTime(UINT64_MAX),
			totalRunTime(0),
			problemSetSize(0),
			problemSetSizeIndex(0),
			factory(),
			lockedBaseline(false),
			isBaselineCase(false)
		{
		}

		Impl(const std::string& groupName, const std::string& benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<Factory> testFactory, double pBaselineTarget) :
			groupName(groupName),
			benchmarkName(benchmarkName),
			baselineUnit(1),
			savedBaselineMeasurement(0),
			baselineTarget(pBaselineTarget),
			samples(samples),
			resetSamples(samples),
			calls(calls),
			resetCalls(calls),
			bestRunTime(UINT64_MAX),
			totalRunTime(0),
			problemSetSize(0),
			problemSetSizeIndex(0),
			factory(testFactory),
			lockedBaseline(false),
			isBaselineCase(false)
		{
		}

		Impl(const BenchmarkInfo& other) : 
			groupName(other.pimpl->groupName),
			benchmarkName(other.pimpl->benchmarkName),
			baselineUnit(other.pimpl->baselineUnit),
			savedBaselineMeasurement(other.pimpl->savedBaselineMeasurement),
			baselineTarget(other.pimpl->baselineTarget),
			samples(other.pimpl->samples),
			resetSamples(other.pimpl->resetSamples),
			calls(other.pimpl->calls),
			resetCalls(other.pimpl->resetCalls),
			bestRunTime(other.pimpl->bestRunTime),
			totalRunTime(other.pimpl->totalRunTime),
			problemSetSize(other.pimpl->problemSetSize),
			problemSetSizeIndex(other.pimpl->problemSetSizeIndex),
			factory(other.pimpl->factory),
			lockedBaseline(other.pimpl->lockedBaseline),
			isBaselineCase(other.pimpl->isBaselineCase)
		{
		}

		/// Group name
		std::string groupName;
		
		/// Test name
		std::string benchmarkName;

		/// The number of microseconds per test (which makes up one baseline unit).
		double baselineUnit;
		double savedBaselineMeasurement;

		/// Used to pass/fail benchmarks when outputting JUnit.
		double baselineTarget;

		/// Test samples completed
		uint64_t samples;
		uint64_t resetSamples;
		
		/// Calls per test run
		uint64_t calls;
		uint64_t resetCalls;

		/// The best run time for this test
		uint64_t bestRunTime;

		/// The best run time for this test
		uint64_t totalRunTime;

		size_t problemSetSize;
		size_t problemSetSizeIndex;
		
		/// The factory to associate with this benchmark.
		std::shared_ptr<Factory> factory;

		bool lockedBaseline;
		bool isBaselineCase;
};

BenchmarkInfo::BenchmarkInfo() : 
	pimpl()
{
}

BenchmarkInfo::BenchmarkInfo(const std::string& groupName, const std::string& benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<Factory> testFactory, double baselineTarget) :
	pimpl(groupName, benchmarkName, samples, calls, testFactory, baselineTarget)
{
}

BenchmarkInfo::BenchmarkInfo(const BenchmarkInfo& other) : 
	pimpl(other)
{
}

BenchmarkInfo::~BenchmarkInfo()
{
}

void BenchmarkInfo::reset()
{
	this->pimpl->samples = this->pimpl->resetSamples;
	this->pimpl->calls = this->pimpl->resetCalls;
	this->pimpl->baselineUnit = 1;
	this->pimpl->bestRunTime = UINT64_MAX;
	this->pimpl->totalRunTime = 0;
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
		output += " call per run.";
	}
	else
	{
		output += " calls per run.";
	}

	if(this->getProblemSetSize() > 0)
	{
		// Leave it zero based.  We're obviously C++ programmers.
		output += " Problem Set " + std::to_string(this->getProblemSetSizeIndex());
		output += " of " + std::to_string(this->getProblemSetSize() - 1) + ".";
	}

	return output;
}

std::string BenchmarkInfo::getShort() const
{
	return this->getGroupName() + "." + this->getTestName();
}

void BenchmarkInfo::setBaselineUnit(const double x)
{
	this->pimpl->baselineUnit = x;
}

double BenchmarkInfo::getBaselineUnit() const
{
	return this->pimpl->baselineUnit;
}

void BenchmarkInfo::saveBaselineMeasurement()
{
	this->pimpl->savedBaselineMeasurement = this->getBaselineMeasurement();
	this->pimpl->lockedBaseline = true;
}

double BenchmarkInfo::getBaselineMeasurement() const
{	
	if(this->pimpl->lockedBaseline == false)
	{
		auto baselineGroupName = this->getGroupName();
	
		if(baselineGroupName.empty() == false)
		{
			auto baselineTest = celero::TestVector::Instance().getBaseline(baselineGroupName);

			if(baselineTest != nullptr)
			{
				return (this->getUsPerOp()/baselineTest->getBaselineUnit());
			}
		}

		return -1.0;
	}

	return this->pimpl->savedBaselineMeasurement;
}

double BenchmarkInfo::getBaselineTarget() const
{
	return this->pimpl->baselineTarget;
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
	return celero::timer::ConvertSystemTime(this->getRunTime()) / static_cast<double>(this->getOps());
}

double BenchmarkInfo::getOpsPerSecond() const
{
	return 100000.0 / (this->getUsPerOp() * celero::UsPerSec);
}

void BenchmarkInfo::setProblemSetSize(const size_t x)
{
	this->pimpl->problemSetSize = x;
}

size_t BenchmarkInfo::getProblemSetSize() const
{
	return this->pimpl->problemSetSize;
}

size_t BenchmarkInfo::getProblemSetSizeIndex() const
{
	return this->pimpl->problemSetSizeIndex;
}

void BenchmarkInfo::incrementProblemSetSizeIndex()
{
	this->pimpl->problemSetSizeIndex++;
}

void BenchmarkInfo::setIsBaselineCase(bool x)
{
	this->pimpl->isBaselineCase = x;
}

bool BenchmarkInfo::getIsBaselineCase() const
{
	return this->pimpl->isBaselineCase;
}
