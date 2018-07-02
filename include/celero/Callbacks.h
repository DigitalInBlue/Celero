#ifndef H_CELERO_CALLBACKS_H
#define H_CELERO_CALLBACKS_H

///
/// \namespace celero
///
/// \author	John Farrier
///
/// \copyright Copyright 2015, 2016, 2017, 2018 John Farrier
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
/// Ideas from Nick Brunn's Hayai (https://github.com/nickbruun/hayai) were used and I likely owe him a beer.
///
/// Special thanks to the band "3" for providing the development soundtrack.
///
/// "Iterations" refers to how many loops of the test function are measured as a time.
/// For very fast code, many iterations would help amoratize measurement error.
///
/// "Samples" refers to how many sets of "iterations" will be performed.  Each "sample" is
/// a single measurement.  Set to 0 to have Celero decide how many samples are required
/// for a minimally significant answer.
///
/// It is highly encouraged to only run this code compiled in a "Release" mode to use all available optimizations.
///

#include <celero/Experiment.h>
#include <celero/Export.h>
#include <functional>
#include <memory>

namespace celero
{
	///
	/// \brief	Add a function to call when a experiment is completed.
	///
	/// This will be called at the end of a complete experiment (benchmark + experiment results.)
	///
	CELERO_EXPORT void AddExperimentCompleteFunction(std::function<void(std::shared_ptr<celero::Experiment>)> x);

	///
	/// \brief	Add a function to call when a experiment is completed.
	///
	/// This will be called at the end of every benchmark or user experiment upon completion.
	///
	CELERO_EXPORT void AddExperimentResultCompleteFunction(std::function<void(std::shared_ptr<celero::ExperimentResult>)> x);

	namespace impl
	{
		void ExperimentComplete(std::shared_ptr<Experiment> x);
		void ExperimentResultComplete(std::shared_ptr<celero::ExperimentResult> x);
	} // namespace impl
} // namespace celero

#endif
