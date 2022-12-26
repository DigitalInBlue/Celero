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

#include <celero/Experiment.h>
#include <memory>
#include <string>

namespace celero
{
	///
	/// \class Printer
	///
	/// \author	John farrier
	///
	class Printer
	{
	public:
		///
		/// Singleton implementation.
		static Printer& get()
		{
			static Printer p;
			return p;
		}

		///
		/// Initialize the Printer object with specific user-defined columns.
		///
		void initialize(std::vector<std::string> userDefinedColumns);

		void Console(const std::string& x);
		void TableBanner();
		void TableRowExperimentHeader(Experiment* x);
		void TableRowFailure(const std::string& msg);
		void TableRowProblemSpaceHeader(std::shared_ptr<celero::ExperimentResult> x);
		void TableRowHeader(std::shared_ptr<celero::ExperimentResult> x);
		void TableResult(std::shared_ptr<celero::ExperimentResult> x);

	private:
		Printer() = default;

		std::vector<std::string> userDefinedColumns;
		std::vector<size_t> columnWidths;
	};
} // namespace celero

