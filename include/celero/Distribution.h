#ifndef H_CELERO_DISTRIBUTION_H
#define H_CELERO_DISTRIBUTION_H

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

#include <celero/Export.h>
#include <cstdint>
#include <vector>

namespace celero
{
	///
	/// Collects results from Celero for analysis of a hard-coded internal trivial measurement case.
	///
	CELERO_EXPORT std::vector<uint64_t> BuildDistribution(uint64_t numberOfSamples, uint64_t iterationsPerSample);

	///
	/// Builds a .csv file to help determine Celero's measurement distribution.
	///
	CELERO_EXPORT void RunDistribution(uint64_t iterationsPerSample);
}

#endif
