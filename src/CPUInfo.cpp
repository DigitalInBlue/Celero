///
/// \author	Aryan
///
/// \copyright Copyright 2026 John Farrier
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

#include <celero/CPUInfo.h>

#include <sstream>

#ifdef _WIN32
#include <intrin.h>
#include <windows.h>

#include <vector>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#include <sys/types.h>
#else
#include <fstream>
#include <thread>
#endif

using namespace celero;

celero::CPUInfoReport::operator std::string()
{
	std::stringstream ss;
	ss << this->BrandName << ", " << this->PhysicalCore << " physical / " << this->LogicalCore << " logical cores, L1=" << this->L1CacheSize
	   << "B L2=" << this->L2CacheSize << "B L3=" << this->L3CacheSize << "B";
	return ss.str();
}

#ifdef _WIN32

namespace celero
{
	namespace impl
	{
		std::string GetBrandName()
		{
			int regs[4] = {0};
			char brand[0x40] = {0};

			__cpuid(regs, 0x80000000);
			const auto highestExtId = static_cast<unsigned int>(regs[0]);

			if(highestExtId >= 0x80000004)
			{
				__cpuid(reinterpret_cast<int*>(brand), 0x80000002);
				__cpuid(reinterpret_cast<int*>(brand + 16), 0x80000003);
				__cpuid(reinterpret_cast<int*>(brand + 32), 0x80000004);
			}

			return std::string(brand);
		}
	} // namespace impl
} // namespace celero

celero::CPUInfoReport celero::GetCPUInfoReport()
{
	CPUInfoReport report;

	report.BrandName = celero::impl::GetBrandName();

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	report.LogicalCore = static_cast<int32_t>(sysInfo.dwNumberOfProcessors);

	DWORD bufferSize = 0;
	GetLogicalProcessorInformation(nullptr, &bufferSize);
	std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

	if(GetLogicalProcessorInformation(buffer.data(), &bufferSize) != 0)
	{
		int32_t physicalCoreCount = 0;

		for(const auto& info : buffer)
		{
			if(info.Relationship == RelationProcessorCore)
			{
				++physicalCoreCount;
			}
			else if(info.Relationship == RelationCache)
			{
				const auto& cache = info.Cache;

				if((cache.Level == 1) && (cache.Type == CacheData))
				{
					report.L1CacheSize = static_cast<int64_t>(cache.Size);
				}
				else if(cache.Level == 2)
				{
					report.L2CacheSize = static_cast<int64_t>(cache.Size);
				}
				else if(cache.Level == 3)
				{
					report.L3CacheSize = static_cast<int64_t>(cache.Size);
				}
			}
		}

		report.PhysicalCore = physicalCoreCount;
	}

	return report;
}

#elif defined(__APPLE__)

namespace celero
{
	namespace impl
	{
		std::string GetSysctlString(const char* name)
		{
			size_t size = 0;
			if(sysctlbyname(name, nullptr, &size, nullptr, 0) != 0)
			{
				return std::string();
			}

			std::string value(size, '\0');
			if(sysctlbyname(name, &value[0], &size, nullptr, 0) != 0)
			{
				return std::string();
			}

			if(!value.empty() && (value.back() == '\0'))
			{
				value.pop_back();
			}

			return value;
		}

		int64_t GetSysctlInt64(const char* name)
		{
			int64_t value = 0;
			size_t size = sizeof(value);
			sysctlbyname(name, &value, &size, nullptr, 0);
			return value;
		}

		int64_t GetCacheSize(const char* perflevelKey, const char* flatKey)
		{
			const auto value = GetSysctlInt64(perflevelKey);
			return (value != 0) ? value : GetSysctlInt64(flatKey);
		}
	} // namespace impl
} // namespace celero

celero::CPUInfoReport celero::GetCPUInfoReport()
{
	CPUInfoReport report;

	report.BrandName = celero::impl::GetSysctlString("machdep.cpu.brand_string");
	report.PhysicalCore = static_cast<int32_t>(celero::impl::GetSysctlInt64("hw.physicalcpu"));
	report.LogicalCore = static_cast<int32_t>(celero::impl::GetSysctlInt64("hw.logicalcpu"));
	report.L1CacheSize = celero::impl::GetCacheSize("hw.perflevel0.l1dcachesize", "hw.l1dcachesize");
	report.L2CacheSize = celero::impl::GetCacheSize("hw.perflevel0.l2cachesize", "hw.l2cachesize");
	report.L3CacheSize = celero::impl::GetCacheSize("hw.perflevel0.l3cachesize", "hw.l3cachesize");

	return report;
}

#else

namespace celero
{
	namespace impl
	{
		std::string GetLinuxBrandName()
		{
			std::ifstream cpuinfo("/proc/cpuinfo");
			std::string line;

			while(std::getline(cpuinfo, line))
			{
				if(line.compare(0, 10, "model name") == 0)
				{
					const auto colon = line.find(':');
					if(colon != std::string::npos)
					{
						auto value = line.substr(colon + 1);
						const auto firstNonSpace = value.find_first_not_of(' ');
						if(firstNonSpace != std::string::npos)
						{
							return value.substr(firstNonSpace);
						}
					}
				}
			}

			return std::string();
		}

		int32_t GetLinuxPhysicalCores()
		{
			std::ifstream cpuinfo("/proc/cpuinfo");
			std::string line;

			while(std::getline(cpuinfo, line))
			{
				if(line.compare(0, 9, "cpu cores") == 0)
				{
					const auto colon = line.find(':');
					if(colon != std::string::npos)
					{
						return std::stoi(line.substr(colon + 1));
					}
				}
			}

			return 0;
		}

		int64_t ReadCacheSizeBytes(const std::string& path)
		{
			std::ifstream file(path);
			std::string value;

			if(!std::getline(file, value) || value.empty())
			{
				return 0;
			}

			int64_t multiplier = 1;
			if(value.back() == 'K')
			{
				multiplier = 1024;
				value.pop_back();
			}

			return std::stoll(value) * multiplier;
		}

		void FillLinuxCacheSizes(CPUInfoReport& report)
		{
			for(int index = 0; index < 8; ++index)
			{
				const std::string base = "/sys/devices/system/cpu/cpu0/cache/index" + std::to_string(index) + "/";

				std::ifstream levelFile(base + "level");
				std::ifstream typeFile(base + "type");

				int level = 0;
				std::string type;

				if(!(levelFile >> level) || !(typeFile >> type))
				{
					continue;
				}

				if(type == "Instruction")
				{
					continue;
				}

				const auto size = ReadCacheSizeBytes(base + "size");

				if(level == 1)
				{
					report.L1CacheSize = size;
				}
				else if(level == 2)
				{
					report.L2CacheSize = size;
				}
				else if(level == 3)
				{
					report.L3CacheSize = size;
				}
			}
		}
	} // namespace impl
} // namespace celero

celero::CPUInfoReport celero::GetCPUInfoReport()
{
	CPUInfoReport report;

	report.BrandName = celero::impl::GetLinuxBrandName();
	report.PhysicalCore = celero::impl::GetLinuxPhysicalCores();
	report.LogicalCore = static_cast<int32_t>(std::thread::hardware_concurrency());

	celero::impl::FillLinuxCacheSizes(report);

	return report;
}

#endif
