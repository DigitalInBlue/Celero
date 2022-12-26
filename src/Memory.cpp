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

#include <celero/Memory.h>

#include <sstream>

#ifdef _WIN32
#include <windows.h>
//
#include <psapi.h>
#elif defined(__APPLE__)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <cstring>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

///
/// References:
/// http://blogs.microsoft.co.il/sasha/2016/01/05/windows-process-memory-usage-demystified/
/// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366770(v=vs.85).aspx
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684877(v=vs.85).aspx
/// http://nadeausoftware.com/articles/2012/09/c_c_tip_how_get_physical_memory_size_system
/// http://nadeausoftware.com/articles/2012/07/c_c_tip_how_get_process_resident_set_size_physical_memory_use
/// https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-run-time-in-c
/// https://stackoverflow.com/questions/2513505/how-to-get-available-memory-c-g
///

using namespace celero;

#ifdef _WIN32
#else

#ifndef __APPLE__
constexpr int64_t Kilobytes2Bytes{1024};
#endif

namespace celero
{
	namespace impl
	{
		int ParseLine(char* line)
		{
			const auto i = strlen(line);

			while(*line < '0' || *line > '9')
			{
				line++;
			}

			line[i - 3] = '\0';
			return atoi(line);
		}
	} // namespace impl
} // namespace celero
#endif

celero::RAMReport::operator std::string()
{
	std::stringstream ss;

	ss << "System Total:                      " << this->RamSystemTotal << std::endl;
	ss << "System Available:                  " << this->RamSystemAvailable << std::endl;
	ss << "System Used:                       " << this->RamSystemUsed << std::endl;
	ss << "System UsedByCurrentProcess:       " << this->RamSystemUsedByCurrentProcess << std::endl;
	ss << "Physical Total:                    " << this->RamPhysicalTotal << std::endl;
	ss << "Physical Available:                " << this->RamPhysicalAvailable << std::endl;
	ss << "Physical Used:                     " << this->RamPhysicalUsed << std::endl;
	ss << "Physical UsedByCurrentProcess:     " << this->RamPhysicalUsedByCurrentProcess << std::endl;
	ss << "Physical UsedByCurrentProcessPeak: " << this->RamPhysicalUsedByCurrentProcessPeak << std::endl;
	ss << "Virtual Total:                     " << this->RamVirtualTotal << std::endl;
	ss << "Virtual Available:                 " << this->RamVirtualAvailable << std::endl;
	ss << "Virtual Used:                      " << this->RamVirtualUsed << std::endl;
	ss << "Virtual UsedByCurrentProcess:      " << this->RamVirtualUsedByCurrentProcess << std::endl;

	return ss.str();
}

celero::RAMReport celero::RAMReport::operator-(const RAMReport& x)
{
	celero::RAMReport r;
	r.RamSystemTotal = this->RamSystemTotal - x.RamSystemTotal;
	r.RamSystemAvailable = this->RamSystemAvailable - x.RamSystemAvailable;
	r.RamSystemUsed = this->RamSystemUsed - x.RamSystemUsed;
	r.RamSystemUsedByCurrentProcess = this->RamSystemUsedByCurrentProcess - x.RamSystemUsedByCurrentProcess;
	r.RamPhysicalTotal = this->RamPhysicalTotal - x.RamPhysicalTotal;
	r.RamPhysicalAvailable = this->RamPhysicalAvailable - x.RamPhysicalAvailable;
	r.RamPhysicalUsed = this->RamPhysicalUsed - x.RamPhysicalUsed;
	r.RamPhysicalUsedByCurrentProcess = this->RamPhysicalUsedByCurrentProcess - x.RamPhysicalUsedByCurrentProcess;
	r.RamPhysicalUsedByCurrentProcessPeak = this->RamPhysicalUsedByCurrentProcessPeak - x.RamPhysicalUsedByCurrentProcessPeak;
	r.RamVirtualTotal = this->RamVirtualTotal - x.RamVirtualTotal;
	r.RamVirtualAvailable = this->RamVirtualAvailable - x.RamVirtualAvailable;
	r.RamVirtualUsed = this->RamVirtualUsed - x.RamVirtualUsed;
	r.RamVirtualUsedByCurrentProcess = this->RamVirtualUsedByCurrentProcess - x.RamVirtualUsedByCurrentProcess;

	return r;
}

int64_t celero::GetRAMSystemTotal()
{
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return static_cast<int64_t>(memInfo.ullTotalPhys) + static_cast<int64_t>(memInfo.ullTotalVirtual);
#elif defined(__unix__) || defined(__unix) || defined(unix)
	// Prefer sysctl() over sysconf() except sysctl() HW_REALMEM and HW_PHYSMEM
	// return static_cast<int64_t>(sysconf(_SC_PHYS_PAGES)) * static_cast<int64_t>(sysconf(_SC_PAGE_SIZE));
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	int64_t total = memInfo.totalram;
	total += memInfo.totalswap;
	total += memInfo.totalhigh;
	return total * static_cast<int64_t>(memInfo.mem_unit);
#elif defined(__APPLE__)
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE;

	int64_t memInfo{0};
	auto len = sizeof(memInfo);

	if(sysctl(mib, 2, &memInfo, &len, nullptr, 0) == 0)
	{
		return memInfo;
	}

	return -1;
#else
	return -1;
#endif
}

int64_t celero::GetRAMSystemAvailable()
{
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return static_cast<int64_t>(memInfo.ullAvailPhys) + static_cast<int64_t>(memInfo.ullAvailVirtual);
#else
	return celero::GetRAMSystemTotal() - celero::GetRAMSystemUsed();
#endif
}

int64_t celero::GetRAMSystemUsed()
{
#ifdef _WIN32
	return celero::GetRAMSystemTotal() - celero::GetRAMSystemAvailable();
#elif defined(__APPLE__)
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE;

	std::array<int64_t, 2> memInfo{{0, 0}};
	auto len = sizeof(memInfo[0]);

	if(sysctl(mib, 2, &memInfo[0], &len, nullptr, 0) == 0)
	{
		if(sysctl(mib, 2, &memInfo[1], &len, nullptr, 0) == 0)
		{
			return memInfo[0] + memInfo[1];
		}
	}

	return -1;
#else
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	int64_t total = memInfo.totalram - memInfo.freeram;
	total += memInfo.totalswap - memInfo.freeswap;
	total += memInfo.totalhigh - memInfo.freehigh;
	return total * static_cast<int64_t>(memInfo.mem_unit);
#endif
}

int64_t celero::GetRAMSystemUsedByCurrentProcess()
{
#ifdef _WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
	return static_cast<int64_t>(pmc.WorkingSetSize);
#else
	return celero::GetRAMPhysicalUsedByCurrentProcess() + celero::GetRAMVirtualUsedByCurrentProcess();
#endif
}

int64_t celero::GetRAMPhysicalTotal()
{
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return static_cast<int64_t>(memInfo.ullTotalPhys);
#elif defined(__APPLE__)
	return -1;
#else
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	return memInfo.totalram * memInfo.mem_unit;
#endif
}

int64_t celero::GetRAMPhysicalAvailable()
{
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return static_cast<int64_t>(memInfo.ullAvailPhys);
#else
	return celero::GetRAMPhysicalTotal() - celero::GetRAMPhysicalUsed();
#endif
}

int64_t celero::GetRAMPhysicalUsed()
{
#ifdef _WIN32
	return celero::GetRAMPhysicalTotal() - celero::GetRAMPhysicalAvailable();
#elif defined(__APPLE__)
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	return (size_t)rusage.ru_maxrss;
#else
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	return (static_cast<int64_t>(memInfo.totalram) - static_cast<int64_t>(memInfo.freeram)) * static_cast<int64_t>(memInfo.mem_unit);
#endif
}

int64_t celero::GetRAMPhysicalUsedByCurrentProcess()
{
#ifdef _WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
	return static_cast<int64_t>(pmc.WorkingSetSize);
#elif defined(__APPLE__)
	return -1;
#else
	constexpr int BufferSize{128};
	int64_t result = 0;
	auto file = fopen("/proc/self/status", "r");
	char line[BufferSize];

	while(fgets(line, BufferSize, file) != nullptr)
	{
		if(strncmp(line, "VmRSS:", 6) == 0)
		{
			result += celero::impl::ParseLine(line) * Kilobytes2Bytes;
		}
	}

	fclose(file);
	return static_cast<int64_t>(result);
#endif
}

int64_t celero::GetRAMPhysicalUsedByCurrentProcessPeak()
{
#if defined(_WIN32)
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	return static_cast<int64_t>(pmc.PeakWorkingSetSize);
#elif defined(__APPLE__) && defined(__MACH__)
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	return static_cast<int64_t>(rusage.ru_maxrss);
#else
	constexpr int BufferSize{128};
	int64_t result = 0;
	auto file = fopen("/proc/self/status", "r");
	char line[BufferSize];

	while(fgets(line, BufferSize, file) != nullptr)
	{
		if(strncmp(line, "VmHWM:", 6) == 0)
		{
			result += celero::impl::ParseLine(line) * Kilobytes2Bytes;
		}
	}

	fclose(file);
	return static_cast<int64_t>(result);
#endif
}

int64_t celero::GetRAMVirtualTotal()
{
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return memInfo.ullTotalPageFile;
#elif defined(__APPLE__)
	return -1;
#else
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	return static_cast<int64_t>(memInfo.totalswap) * static_cast<int64_t>(memInfo.mem_unit);
#endif
}

int64_t celero::GetRAMVirtualAvailable()
{
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return memInfo.ullTotalPageFile;
#else
	return celero::GetRAMVirtualTotal() - celero::GetRAMVirtualUsed();
#endif
}

int64_t celero::GetRAMVirtualUsed()
{
#ifdef _WIN32
	return celero::GetRAMVirtualTotal() - celero::GetRAMVirtualAvailable();
#elif defined(__APPLE__)
	return -1;
#else
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	const int64_t total = memInfo.totalswap - memInfo.freeswap;
	return total * static_cast<int64_t>(memInfo.mem_unit);
#endif
}

int64_t celero::GetRAMVirtualUsedByCurrentProcess()
{
#ifdef _WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
	return pmc.PrivateUsage;
#elif defined(__APPLE__)
	return -1;
#else
	// Verified Correct.
	constexpr int BufferSize{128};
	int64_t result = 0;
	FILE* file = fopen("/proc/self/status", "r");
	char line[BufferSize];

	while(fgets(line, BufferSize, file) != NULL)
	{
		if(strncmp(line, "VmSize:", 7) == 0)
		{
			result = celero::impl::ParseLine(line) * Kilobytes2Bytes;
			break;
		}
	}

	fclose(file);
	return result;
#endif
}

celero::RAMReport celero::GetRAMReport()
{
	celero::RAMReport r;
	r.RamSystemTotal = GetRAMSystemTotal();
	r.RamSystemAvailable = GetRAMSystemAvailable();
	r.RamSystemUsed = GetRAMSystemUsed();
	r.RamSystemUsedByCurrentProcess = GetRAMSystemUsedByCurrentProcess();
	r.RamPhysicalTotal = GetRAMPhysicalTotal();
	r.RamPhysicalAvailable = GetRAMPhysicalAvailable();
	r.RamPhysicalUsed = GetRAMPhysicalUsed();
	r.RamPhysicalUsedByCurrentProcess = GetRAMPhysicalUsedByCurrentProcess();
	r.RamVirtualTotal = GetRAMVirtualTotal();
	r.RamVirtualAvailable = GetRAMVirtualAvailable();
	r.RamVirtualUsed = GetRAMVirtualUsed();
	r.RamVirtualUsedByCurrentProcess = GetRAMVirtualUsedByCurrentProcess();
	return r;
}
