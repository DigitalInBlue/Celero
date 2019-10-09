///
/// \author	John Farrier
///
/// \copyright Copyright 2015, 2016, 2017 John Farrier
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

#include <celero/Memory.h>
#include <gtest/gtest.h>
#include <chrono>
#include <iomanip>

constexpr double Gigabyte{1073741824.0};

void PrintTop()
{
	std::cout << celero::GetRAMVirtualUsedByCurrentProcess() << " " << celero::GetRAMPhysicalUsedByCurrentProcess() << " "
			  << celero::GetRAMSystemUsedByCurrentProcess() << "\n";
}

TEST(memory, Report)
{
	EXPECT_NE(int64_t(0), celero::GetRAMSystemTotal());
	EXPECT_NE(int64_t(0), celero::GetRAMSystemAvailable());
	EXPECT_NE(int64_t(0), celero::GetRAMSystemUsed());
	EXPECT_NE(int64_t(0), celero::GetRAMSystemUsedByCurrentProcess());
	EXPECT_NE(int64_t(0), celero::GetRAMPhysicalTotal());
	EXPECT_NE(int64_t(0), celero::GetRAMPhysicalAvailable());
	EXPECT_NE(int64_t(0), celero::GetRAMPhysicalUsed());
	EXPECT_NE(int64_t(0), celero::GetRAMPhysicalUsedByCurrentProcess());
}

TEST(memory, AllocSystem)
{
	const auto startTotal = celero::GetRAMSystemTotal();
	EXPECT_GT(celero::GetRAMSystemTotal(), int64_t(1024));
	EXPECT_GT(celero::GetRAMSystemTotal(), celero::GetRAMSystemAvailable());
	EXPECT_GT(celero::GetRAMSystemTotal(), celero::GetRAMSystemUsed());
	EXPECT_GT(celero::GetRAMSystemAvailable(), celero::GetRAMSystemUsed());

	const auto startUsedByCurrentProcess = celero::GetRAMSystemUsedByCurrentProcess();
	EXPECT_GT(celero::GetRAMSystemTotal(), celero::GetRAMSystemUsedByCurrentProcess());
	EXPECT_GT(celero::GetRAMSystemAvailable(), celero::GetRAMSystemUsedByCurrentProcess());
	EXPECT_GT(celero::GetRAMSystemUsed(), celero::GetRAMSystemUsedByCurrentProcess());

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		const int64_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(startTotal, celero::GetRAMSystemTotal());
		EXPECT_NE(startUsedByCurrentProcess, celero::GetRAMSystemUsedByCurrentProcess())
			<< "We did an allocation, but are using exactly the same amount of RAM.";

		EXPECT_LT(startUsedByCurrentProcess + allocAmmount, celero::GetRAMSystemUsedByCurrentProcess())
			<< "We did an allocation, but are using less RAM. Start: " << startUsedByCurrentProcess << ", Alloc: " << allocAmmount;

		// Assume we are not swapping out to disk
		ASSERT_GT(celero::GetRAMSystemUsedByCurrentProcess(), startUsedByCurrentProcess);

		const auto memoryDelta = celero::GetRAMSystemUsedByCurrentProcess() - startUsedByCurrentProcess;
		EXPECT_NEAR(static_cast<double>(celero::GetRAMSystemUsedByCurrentProcess() - startUsedByCurrentProcess), static_cast<double>(memoryDelta),
					4096.0)
			<< "Memory Delta: " << memoryDelta;

		delete[] megabyte;
	}
}

TEST(memory, AllocPhysical)
{
	const auto startTotal = celero::GetRAMPhysicalTotal();
	EXPECT_GT(celero::GetRAMSystemTotal(), int64_t(1024));
	EXPECT_GT(celero::GetRAMSystemTotal(), celero::GetRAMSystemAvailable());
	EXPECT_GT(celero::GetRAMSystemTotal(), celero::GetRAMSystemUsed());
	EXPECT_GT(celero::GetRAMSystemAvailable(), celero::GetRAMSystemUsed());

	const auto startUsedByCurrentProcess = celero::GetRAMPhysicalUsedByCurrentProcess();
	EXPECT_GT(celero::GetRAMSystemTotal(), celero::GetRAMSystemUsedByCurrentProcess());
	EXPECT_GT(celero::GetRAMSystemAvailable(), celero::GetRAMSystemUsedByCurrentProcess());
	EXPECT_GT(celero::GetRAMSystemUsed(), celero::GetRAMSystemUsedByCurrentProcess());

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		const int64_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		EXPECT_EQ(startTotal, celero::GetRAMPhysicalTotal());
		EXPECT_NE(startUsedByCurrentProcess, celero::GetRAMPhysicalUsedByCurrentProcess());

		EXPECT_LT(startUsedByCurrentProcess + allocAmmount, celero::GetRAMPhysicalUsedByCurrentProcess())
			<< "We did an allocation, but are using less RAM. Start: " << startUsedByCurrentProcess << ", Alloc: " << allocAmmount;

		// Assume we are not swapping out to disk
		ASSERT_GT(celero::GetRAMPhysicalUsedByCurrentProcess(), startUsedByCurrentProcess);

		const auto memoryDelta = celero::GetRAMPhysicalUsedByCurrentProcess() - startUsedByCurrentProcess;
		EXPECT_NEAR(static_cast<double>(celero::GetRAMPhysicalUsedByCurrentProcess() - startUsedByCurrentProcess), static_cast<double>(memoryDelta),
					4096.0)
			<< "Memory Delta: " << memoryDelta;
		delete[] megabyte;
	}
}

TEST(memory, AllocAll)
{
	const auto startCurrentProcess = celero::GetRAMSystemUsedByCurrentProcess() + celero::GetRAMVirtualUsedByCurrentProcess();

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		const int64_t allocAmmount = 1052672;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		// Assume we are not swapping out to disk
		const auto memoryDelta = celero::GetRAMSystemUsedByCurrentProcess() + celero::GetRAMVirtualUsedByCurrentProcess() - startCurrentProcess;
		EXPECT_NEAR(static_cast<double>(allocAmmount), static_cast<double>(memoryDelta), 4096.0);
		delete[] megabyte;
	}
}

TEST(memory, AllocPeak)
{
	const auto startPeak = celero::GetRAMPhysicalUsedByCurrentProcessPeak();
	const auto startCurrentProcess = celero::GetRAMSystemUsedByCurrentProcess() + celero::GetRAMVirtualUsedByCurrentProcess();

	// This will always be true, but the compiler won't know that, preventing the
	// allocation from happening before we want it to.
	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		// Allocate a large amount than the other tests would have up to this point.
		const int64_t allocAmmount = 1052672 * 2;
		volatile uint8_t* megabyte = new uint8_t[allocAmmount];

		// Assume we are not swapping out to disk
		const auto memoryDelta = celero::GetRAMSystemUsedByCurrentProcess() + celero::GetRAMVirtualUsedByCurrentProcess() - startCurrentProcess;
		EXPECT_NEAR(static_cast<double>(allocAmmount), static_cast<double>(memoryDelta), 4096.0);
		EXPECT_GT(celero::GetRAMPhysicalUsedByCurrentProcessPeak(), startPeak + allocAmmount);

		delete[] megabyte;
	}

	EXPECT_LE(startPeak, celero::GetRAMPhysicalUsedByCurrentProcessPeak());
}

TEST(memory, ForceAllocateVirtual)
{
	const auto ramAvailableTotal = celero::GetRAMSystemAvailable();
	EXPECT_GT(ramAvailableTotal, 0);

	const auto ramSystemUsed = celero::GetRAMSystemUsed();
	EXPECT_GT(ramSystemUsed, 0);

	const auto ramUsedByCurrentProcess = celero::GetRAMSystemUsedByCurrentProcess();
	EXPECT_GT(ramUsedByCurrentProcess, 0);

	const auto ramAvailablePhysical = celero::GetRAMPhysicalAvailable();
	EXPECT_GT(ramAvailablePhysical, 0);

	const auto ramAvailableVirtual = celero::GetRAMVirtualAvailable();
	EXPECT_GT(ramAvailableVirtual, 0);

	const auto allocAmmountPhysical = ramAvailablePhysical;
	const auto allocAmmountVirtual = static_cast<decltype(allocAmmountPhysical)>(static_cast<double>(ramAvailableVirtual) * 0.01);
	const auto allocAmmount = allocAmmountPhysical + allocAmmountVirtual;

	EXPECT_GT(ramAvailableTotal, allocAmmount);
	EXPECT_GT(allocAmmount, ramAvailablePhysical);

	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		volatile uint8_t* buffer = new uint8_t[allocAmmount];

		// We should be using more RAM across the board now.
		EXPECT_GT(celero::GetRAMSystemUsed(), ramSystemUsed);
		EXPECT_GT(celero::GetRAMSystemUsedByCurrentProcess(), ramUsedByCurrentProcess);

		EXPECT_LT(celero::GetRAMSystemAvailable(), ramAvailableTotal);
		EXPECT_LT(celero::GetRAMPhysicalAvailable(), ramAvailablePhysical);

		delete[] buffer;
	}
}
