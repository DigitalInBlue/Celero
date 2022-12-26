#pragma once

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

#include <celero/Export.h>
#include <celero/Pimpl.h>
#include <celero/Statistics.h>

#include <string>

namespace celero
{
	class Experiment;
	class UserDefinedMeasurementCollector;

	///
	/// \class ExperimentResult
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT ExperimentResult
	{
	public:
		explicit ExperimentResult(Experiment* x);

		~ExperimentResult();

		///
		/// Gets a pointer to the owning experiment.
		///
		Experiment* getExperiment() const;

		///
		///
		///
		void setProblemSpaceValue(int64_t x, double scale = 1.0, uint64_t iterations = 0);

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
		uint64_t getProblemSpaceIterations() const;

		///
		///
		///
		const Statistics<int64_t>& getTimeStatistics() const;

		///
		///
		///
		const Statistics<int64_t>& getRAMStatistics() const;

		///
		/// Adds a run time sample during experiment execution.
		///
		void addRunTimeSample(const uint64_t x);

		///
		/// Adds a memory sample during experiment execution.
		///
		void addMemorySample(const int64_t x);

		///
		/// Returns the best run time sample observed.
		///
		uint64_t getRunTime() const;

		///
		/// Returns the best RAM usage sample observed.
		///
		int64_t getRAM() const;

		///
		/// \brief	Get the number of computed microseconds per iteration (i.e. a single call to the code under test.)
		///
		///	A call is defined as one iteration of one execution of the code under test.
		///
		double getUsPerCall() const;

		///
		/// \brief	Get the number of times the code under test could be called per second.
		///
		///	A call is defined as one iteration of one execution of the code under test.
		///
		double getCallsPerSecond() const;

		///
		/// \brief	Get the processing speed in units per second.
		///
		///	A call is defined as one iteration of one execution of the code under test.
		///
		double getUnitsPerSecond() const;

		///
		/// Calculate this experiments baseline value.
		///
		/// If this IS a baseline experiment, the function will return 1.0;
		/// Returns -1 on error.
		///
		double getBaselineMeasurement() const;

		///
		/// Sets a flag indicating if this result is complete.
		///
		void setComplete(bool x);

		///
		/// Gets a flag indicating if this result is complete.
		///
		bool getComplete() const;

		///
		/// Sets a flag indicating if failure happened during evaluation.
		///
		void setFailure(bool x);

		///
		/// Gets a flag indicating if failure happened during evaluation.
		///
		bool getFailure() const;

		///
		///
		///
		void setUserDefinedMeasurements(std::shared_ptr<UserDefinedMeasurementCollector> x);

		///
		///
		///
		std::shared_ptr<UserDefinedMeasurementCollector> getUserDefinedMeasurements() const;

	private:
		///
		/// Disable default constructor
		///
		ExperimentResult();

		///
		/// \brief	Pimpl Idiom
		///
		class Impl;

		///
		/// \brief	Pimpl Idiom
		///
		Pimpl<Impl> pimpl;
	};
} // namespace celero

