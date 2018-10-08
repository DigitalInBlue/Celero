#ifndef H_CELERO_ARCHIVE_H
#define H_CELERO_ARCHIVE_H

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
#include <celero/ExperimentResult.h>
#include <string>

namespace celero
{
	///
	/// \class Archive
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT Archive
	{
	public:
		///
		/// Singleton
		///
		static Archive& Instance();

		///
		/// Specify a file name for a results output file.
		///
		/// \param x	The name of the output file in which to store Celero's results.
		///
		void setFileName(const std::string& x);

		///
		/// Adds or updates a result which will be saved to a results archive file.
		///
		/// This should re-save on every new result so that the output can be monitored externally.
		///
		void add(std::shared_ptr<celero::ExperimentResult> x);

		///
		/// Saves all current results to a results archive file.
		///
		/// Will overwrite all existing data and refresh with new data.
		///
		void save();

	private:
		///
		/// Default Constructor
		///
		Archive();

		///
		/// Non-copyable.
		/// Visual studio 2012 does not support "delete" here.
		///
		Archive(Archive&)
		{
		}

		///
		/// Default Destructor
		///
		~Archive();

		///
		/// Non-assignable.
		/// Visual studio 2012 does not support "delete" here.
		///
		Archive& operator=(const Archive&)
		{
			return *this;
		}

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
