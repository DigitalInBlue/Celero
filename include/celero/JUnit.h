#ifndef H_CELERO_JUNIT_H
#define H_CELERO_JUNIT_H

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
#include <celero/Pimpl.h>
#include <string>

namespace celero
{
	///
	/// \class JUnit
	///
	/// \author	John Farrier
	///
	class JUnit
	{
	public:
		///
		/// Singleton
		///
		static JUnit& Instance();

		///
		/// Specify a file name for a results output file.
		///
		/// \param x	The name of the output file in which to store Celero's results.
		///
		void setFileName(const std::string& x);

		///
		/// Add a new result to the JUnit output XML.
		///
		/// This should re-save on every new result so that the output can be monitored externally.
		///
		void add(std::shared_ptr<celero::ExperimentResult> x);

		///
		/// Save the JUnit (XUnit) formatted file to the given file name.
		///
		void save();

	private:
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

#endif
