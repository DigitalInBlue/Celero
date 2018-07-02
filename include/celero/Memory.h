#ifndef H_CELERO_MEORY_H
#define H_CELERO_MEORY_H

///
/// \author	John Farrier
///
/// \copyright Copyright 2018 John Farrier
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
#include <cstdint>
#include <string>

namespace celero
{
	///
	/// \struct RAMReport
	///
	/// Contans all Memory measurements (in bytes)
	///
	struct RAMReport
	{
		int64_t RamSystemTotal{0};
		int64_t RamSystemAvailable{0};
		int64_t RamSystemUsed{0};
		int64_t RamSystemUsedByCurrentProcess{0};
		int64_t RamPhysicalTotal{0};
		int64_t RamPhysicalAvailable{0};
		int64_t RamPhysicalUsed{0};
		int64_t RamPhysicalUsedByCurrentProcess{0};
		int64_t RamPhysicalUsedByCurrentProcessPeak{0};
		int64_t RamVirtualTotal{0};
		int64_t RamVirtualAvailable{0};
		int64_t RamVirtualUsed{0};
		int64_t RamVirtualUsedByCurrentProcess{0};

		operator std::string();
		celero::RAMReport operator-(const celero::RAMReport& x);
	};

	// ----------------------------------------------------------------
	// Physical + Virtual Memory

	CELERO_EXPORT int64_t GetRAMSystemTotal();
	CELERO_EXPORT int64_t GetRAMSystemAvailable();
	CELERO_EXPORT int64_t GetRAMSystemUsed();

	///
	/// The sum of the physical RAM used by the current process and the virtual RAM used by the current process.
	///
	CELERO_EXPORT int64_t GetRAMSystemUsedByCurrentProcess();

	// ----------------------------------------------------------------
	// Physical Memory

	///
	/// The total physical RAM, in bytes.
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366770(v=vs.85).aspx
	///
	CELERO_EXPORT int64_t GetRAMPhysicalTotal();

	///
	/// The total physical RAM available to the current process, in bytes.
	///
	/// On Windows, this is defined as "This is the amount of physical memory that can be immediately reused without having to write its contents to
	/// disk first. It is the sum of the size of the standby, free, and zero lists."
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/aa366770(v=vs.85).aspx
	///
	CELERO_EXPORT int64_t GetRAMPhysicalAvailable();

	///
	/// The total amount of physical RAM minus the amount of physical RAM which is available.
	///
	CELERO_EXPORT int64_t GetRAMPhysicalUsed();

	///
	/// On Windows, this is defined by the Working Set Size.  The working set size is defined as "The working set of a process is the set of pages in
	/// the virtual address space of the process that are currently resident in physical memory. The working set contains only pageable memory
	/// allocations; nonpageable memory allocations such as Address Windowing Extensions (AWE) or large page allocations are not included in the
	/// working set."
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684877(v=vs.85).aspx
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/cc441804(v=vs.85).aspx
	///
	CELERO_EXPORT int64_t GetRAMPhysicalUsedByCurrentProcess();

	///
	///
	///
	CELERO_EXPORT int64_t GetRAMPhysicalUsedByCurrentProcessPeak();

	// ----------------------------------------------------------------
	// Virtual Memory

	///
	/// The total amount of Virtual RAM (page file size).
	///
	/// On Windows, this is defined by the amount of page file that the current process has access to.  It is not the total available on the system.
	/// From the Windows documentation: "The current committed memory limit for the system or the current process, whichever is smaller, in bytes. To
	/// get the system-wide committed memory limit, call GetPerformanceInfo."
	///
	CELERO_EXPORT int64_t GetRAMVirtualTotal();

	///
	/// The amount of non-physical memory (page file) available.
	///
	/// On Windows, this is defined by the amount of page file that the current process has access to.  It is not the total available on the system.
	/// From the Windows documentation: "The maximum amount of memory the current process can commit, in bytes. This value is equal to or smaller than
	/// the system-wide available commit value."
	///
	CELERO_EXPORT int64_t GetRAMVirtualAvailable();

	///
	/// The total virtual RAM minus the available virtual RAM.
	///
	CELERO_EXPORT int64_t GetRAMVirtualUsed();

	///
	/// On Windows, this is defined as the commit charge.  "The Commit Charge value in bytes for this process. Commit Charge is the total amount of
	/// memory that the memory manager has committed for a running process."
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684877(v=vs.85).aspx
	///
	CELERO_EXPORT int64_t GetRAMVirtualUsedByCurrentProcess();

	///
	/// Returns a RAMReport class containing all RAM measurements.
	///
	CELERO_EXPORT celero::RAMReport GetRAMReport();
} // namespace celero

#endif
