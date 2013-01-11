#include <celero/TestFixture.h>

#include <iostream>
#include <algorithm>

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

void TestFixture::TearDown()
{
}
	
int64_t TestFixture::Run(const uint64_t calls)
{
	// Set up the testing fixture.
	this->SetUp();

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
			
	// Return the duration in microseconds.
	return (endTime - startTime);
}

void TestFixture::UserBenchmark()
{
}
