#ifndef H_CELERO_RESULT_H
#define H_CELERO_RESULT_H

// www.helleboreconsulting.com

///
/// \author	John Farrier
///
/// \copyright Copyright 2015 John Farrier
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

#include <string>
#include <celero/Export.h>
#include <celero/Pimpl.h>

namespace celero
{
	class Experiment;
	class Statistics;

	///
	/// \class Experiment
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT Result
	{
		public:
			explicit Result(Experiment* x);

			~Result();

			///
			/// Gets a pointer to the owning experiment.
			///
			Experiment* getExperiment() const;

			///
			///
			///
			void setProblemSpaceValue(int64_t x, double scale = 1.0);

			///
			///
			///
			int64_t getProblemSpaceValue() const;

			///
			///
			///
			double getProblemSpaceValueScale() const;

			///
			///
			///
			Statistics* getStatistics();

			///
			/// Adds a run time sample during experiment execution.
			///
			void addRunTimeSample(const uint64_t x);

			///
			/// Returns the best run time sample observed.
			///
			uint64_t getRunTime() const;

			///
			/// \brief	Get the number of computed microseconds per operation.
			///
			///	An operation is defined as one operation of one run.
			///
			double getUsPerCall() const;

			///
			/// \brief	Get the number of computer operations per second.
			///
			///	An operation is defined as one operation of one run.
			///
			double getOpsPerSecond() const;

			///
			/// \brief	Get the processing speed in units per second.
			///
			///	An operation is defined as one operation of one run.
			///
			double getUnitsPerSecond() const;

			///
			/// Calculate this experiments baseline value.
			///
			/// If this IS a baseline experiment, the function will return 1.0;
			/// Returns -1 on error.
			///
			double getBaselineMeasurement();

			///
			/// Sets a flag indicating if this result is complete.
			///
			void setComplete(bool x);

			///
			/// Gets a flag indicating if this result is complete.
			///
			bool getComplete() const;

		private:
			///
			/// Disable default constructor
			///
			Result();

			///
			/// \brief	Pimpl Idiom
			///
			class Impl;

			///
			/// \brief	Pimpl Idiom
			///
			Pimpl<Impl> pimpl;
	};
}

#endif
