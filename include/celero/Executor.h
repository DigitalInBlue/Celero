#ifndef H_CELERO_EXECUTOR_H
#define H_CELERO_EXECUTOR_H

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

#include <celero/Benchmark.h>
#include <celero/Export.h>
#include <memory>
#include <string>

namespace celero
{
	namespace executor
	{
		///
		/// Run all baselines and experiments registered within the final application.
		///
		void RunAll();

		///
		/// Run all baselines (but not experiments) registered within the final application.
		///
		void RunAllBaselines();

		///
		/// Run a specific benchmark's baseline.
		///
		void RunBaseline(std::shared_ptr<Benchmark> x);

		///
		/// Run all experiments registered within the final application.
		///
		void RunAllExperiments();

		///
		/// Run all experiments within a specific benchmark.
		///
		void RunExperiments(std::shared_ptr<Benchmark> x);

		///
		/// Run a specific benchmark.
		///
		void Run(std::shared_ptr<Benchmark> x);

		///
		/// Run a specific experiment.
		///
		/// If the baseline is not complete for the given experiment, it will be executed first.
		///
		void Run(std::shared_ptr<Experiment> x);

		///
		/// Run a specific benchmark with the specified name.
		///
		void Run(const std::string& group);

		///
		/// Run a specific benchmark with the specified name and one specific experiment within it.
		///
		/// If the baseline is not complete for the given experiment, it will be executed first.
		///
		void Run(const std::string& group, const std::string& experiment);
	}
}

#endif
