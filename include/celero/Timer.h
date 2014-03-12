#ifndef H_CELERO_TIMER_H
#define H_CELERO_TIMER_H

// www.helleboreconsulting.com

#include <stdint.h>

namespace celero
{
	///
	/// \namespace timer
	///
	/// \author	John Farrier
	///
	/// \brief	Provide basic cross-platform timing functions to measure code performance speed.
	///
	namespace timer
	{
		///
		///	\brief	Retrieves the current time.
		///
		///	\author	John Farrier
		///
		///	\return	The time, in ticks.
		///
		uint64_t GetSystemTime();

		///
		///	\brief	Converts the gathered system time into seconds.
		///
		/// This assumes "x" is a delta and relatively small (easily fits inside of a double).
		///
		///	\author	John Farrier
		///
		///	\param	x	The time, in ticks.
		///
		///	\return	The time, in seconds.
		///
		double ConvertSystemTime(const uint64_t x);

		///
		/// On Windows, this caches the frequency of the high performance clock.
		///
		void CachePerformanceFrequency();
	}
}

#endif
