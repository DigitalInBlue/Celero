#ifndef _CELERO_UTILITIES_H_
#define _CELERO_UTILITIES_H_

#ifndef WIN32
#include <unistd.h>
#endif

namespace celero
{
	///
	/// \func DoNotOptimizeAway
	///
	/// \author Andrei Alexandrescu
	///
	template<class T> void DoNotOptimizeAway(T&& datum) 
	{
		#ifdef WIN32
		if(_getpid() == 1) 
		#else
		if(getpid() == 1) 
		#endif
		{
			const void* p = &datum;
			putchar(*static_cast<const char*>(p));
		}
	}

	///
	/// Quick definition of the number of microseconds per second.
	///
	const auto UsPerSec(1000000.0);

	///
	/// Define the number of samples to default to for a good stastical sample when automatically timing tests.
	///
	const auto StatisticalSample(30);
}

#endif
