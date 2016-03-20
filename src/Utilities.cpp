///
/// \author	John Farrier
///
/// \copyright Copyright 2016 John Farrier
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

#include <celero/Utilities.h>
#include <celero/Print.h>

#ifdef WIN32
	#include <Windows.h>
	#include <Powerbase.h>
#endif

template<>
void celero::DoNotOptimizeAway(std::function<void(void)>&& x)
{
	x();

	//
	// We must always do this test, but it will never pass.
	//
	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		// This forces the value to never be optimized away
		// by taking a reference then using it.
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));

		// If we do get here, kick out because something has gone wrong.
		std::abort();
	}
}

void celero::DisableDynamicCPUScaling()
{
	#ifdef WIN32
		// http://stackoverflow.com/questions/3975551/how-to-disable-dynamic-frequency-scaling
		// https://msdn.microsoft.com/en-us/library/aa372675(v=vs.85).aspx
		//
		// NTSTATUS WINAPI CallNtPowerInformation(
		//   _In_   POWER_INFORMATION_LEVEL InformationLevel,
		//   _In_   PVOID lpInputBuffer,
		//   _In_   ULONG nInputBufferSize,
		//   _Out_  PVOID lpOutputBuffer,
		//   _In_   ULONG nOutputBufferSize
		// );

		// https://msdn.microsoft.com/en-us/library/aa373215(v=vs.85).aspx
		SYSTEM_POWER_CAPABILITIES spc;
		
		CallNtPowerInformation(
			SystemPowerCapabilities, 
			NULL,
			NULL,
			&spc,
			sizeof(SYSTEM_POWER_CAPABILITIES));
		
		if(spc.ProcessorThrottle == TRUE)
		{
			celero::print::Console("CPU supports processor throttling.  Attempting to disable.");
			
			if(spc.ProcessorMinThrottle != 100)
			{
				spc.ProcessorMaxThrottle = 100;
				spc.ProcessorMinThrottle = 100;
		
				CallNtPowerInformation(
					SystemPowerCapabilities,
					&spc,
					sizeof(SYSTEM_POWER_CAPABILITIES),
					NULL,
					NULL);
			}
		
			celero::DisableDynamicCPUScaling();
		}
		else
		{
			celero::print::Console("CPU processor throttling disabled.");
		}
	#else
		// The Linux kernel has full SpeedStep support 
		// integrated since version 2.6.
	#endif
}