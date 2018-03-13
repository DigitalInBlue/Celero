#ifndef H_CELERO_PRINT_H
#define H_CELERO_PRINT_H

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

#include <celero/Experiment.h>
#include <memory>
#include <string>

namespace celero
{
	///
	/// \namespace print
	///
	/// \author	John farrier
	///
	namespace print
	{
		// void StageBanner(const std::string& x);
		// void GreenBar(const std::string& x);
		// void Run(const std::string& x);
		// void Status(const std::string& x);
		// void Failure(const std::string& x);
		// void Run(std::shared_ptr<celero::ExperimentResult> x);
		// void Done(std::shared_ptr<celero::ExperimentResult> x);
		// void Baseline(std::shared_ptr<celero::ExperimentResult> x);
		// void SummaryTest(const std::string& x);
		// void Summary(std::shared_ptr<celero::ExperimentResult> x);

		void Console(const std::string& x);
		void TableBanner();
		void TableRowExperimentHeader(Experiment* x);
		void TableRowFailure(const std::string& msg);
		void TableRowProblemSpaceHeader(std::shared_ptr<celero::ExperimentResult> x);
		void TableRowHeader(std::shared_ptr<celero::ExperimentResult> x);
		void TableResult(std::shared_ptr<celero::ExperimentResult> x);
	}
}

#endif
