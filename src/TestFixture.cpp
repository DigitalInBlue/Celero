#include <celero/TestFixture.h>

#include <iostream>
#include <algorithm>

#include <assert.h>

using namespace celero;

TestFixture::TestFixture()
{
}

TestFixture::~TestFixture()
{
}

void TestFixture::SetUp()
{
}

void TestFixture::SetUp(const int32_t)
{
	// There is no SetUp override specified, but problem set sizes were used.
	std::cerr << "There is no SetUp override specified, but a problem set was use.\n";
	assert(false);
}

void TestFixture::TearDown()
{
}
	
std::pair<uint64_t, int32_t> TestFixture::Run(const uint64_t calls, const size_t problemSetValueIndex)
{
	this->setProblemSetSize(this->getProblemSetSize());

	int32_t problemValue = 0;

	// Set up the testing fixture.
	if(this->ProblemSetValues.empty() == false)
	{
		problemValue = this->ProblemSetValues[problemSetValueIndex];
		this->SetUp(problemValue);
	}
	else
	{
		this->SetUp();
	}

	// Run the test body for each operation.
	auto operation = calls;
			
	// Get the starting time.
	auto startTime = celero::timer::GetSystemTime();
			
	while(operation--)
	{
		this->UserBenchmark();
	}
			
	auto endTime = celero::timer::GetSystemTime();

	// Tear down the testing fixture.
	this->TearDown();
			
	// Return the duration in microseconds for the given problem size.
	return std::make_pair((endTime - startTime), problemValue);
}

void TestFixture::UserBenchmark()
{
}

size_t TestFixture::getProblemSetSize() const
{
	return this->ProblemSetValues.size();
}

int32_t TestFixture::getProblemSetValue(const size_t x) const
{
	if(this->ProblemSetValues.empty() == false)
	{
		return this->ProblemSetValues[x];
	}

	return 0;
}
