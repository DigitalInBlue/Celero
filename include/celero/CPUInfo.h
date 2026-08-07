#pragma once

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

#include <celero/Export.h>

#include <cstring>
#include <string>

namespace celero
{
	///
	/// \class CPUInfoReport
	///
	/// \author	Aryan
	///
	/// \brief	Holds a snapshot of the host CPU's brand, core counts, and cache sizes.
	///
	class CELERO_EXPORT CPUInfoReport
	{
	public:
		/// The CPU's marketing/brand name (e.g. "Intel(R) Core(TM) i7-9700K CPU @ 3.60GHz").
		std::string BrandName;

		/// The number of physical cores.
		int32_t PhysicalCore{0};

		/// The number of logical cores (includes hardware threads such as hyperthreads).
		int32_t LogicalCore{0};

		/// The L1 (data) cache size, in bytes.
		int64_t L1CacheSize{0};

		/// The L2 cache size, in bytes.
		int64_t L2CacheSize{0};

		/// The L3 cache size, in bytes.
		int64_t L3CacheSize{0};

		///
		/// \brief	Formats this report as a human-readable summary string.
		///
		operator std::string();
	};

	///
	/// \brief	Gathers CPU brand, core count, and cache size information for the host machine.
	///
	/// \author	Aryan
	///
	/// \return	A populated CPUInfoReport.
	///
	CELERO_EXPORT CPUInfoReport GetCPUInfoReport();

} // namespace celero
