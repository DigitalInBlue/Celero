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

#include <celero/Callbacks.h>
#include <algorithm>
#include <vector>

using namespace celero;

std::vector<std::function<void(std::shared_ptr<Experiment>)>> ExperimentFunctions;
std::vector<std::function<void(std::shared_ptr<celero::ExperimentResult>)>> ExperimentResultFunctions;

void celero::impl::ExperimentComplete(std::shared_ptr<Experiment> x)
{
	for(auto& i : ExperimentFunctions)
	{
		i(x);
	}
}

void celero::impl::ExperimentResultComplete(std::shared_ptr<celero::ExperimentResult> x)
{
	for(auto& i : ExperimentResultFunctions)
	{
		i(x);
	}
}

void celero::AddExperimentCompleteFunction(std::function<void(std::shared_ptr<Experiment>)> x)
{
	ExperimentFunctions.push_back(x);
}

void celero::AddExperimentResultCompleteFunction(std::function<void(std::shared_ptr<celero::ExperimentResult>)> x)
{
	ExperimentResultFunctions.push_back(x);
}
