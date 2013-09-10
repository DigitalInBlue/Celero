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
		///	\return	The time, in microseconds.
		///
		uint64_t GetSystemTime();

		///
		///	\brief	Retrieves the current time.
		///
		/// This assumes "x" is a delta and relatively small (easily fits inside of a double).
		///
		///	\author	John Farrier
		///
		///	\param	x	The time, in microseconds.
		///
		///	\return	The time, in seconds.
		///
		double GetSystemTime(const uint64_t x);
	}
}

#endif
