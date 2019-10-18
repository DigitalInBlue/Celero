///
/// \author	John Farrier
///
/// \copyright Copyright 2019 John Farrier
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
#include <celero/Experiment.h>
#include <celero/ExperimentResult.h>
#include <gtest/gtest.h>

TEST(Callbacks, AddExperimentCompleteFunction)
{
	std::function<void(std::shared_ptr<celero::Experiment>)> x = [](std::shared_ptr<celero::Experiment>) {};
	EXPECT_NO_THROW(celero::AddExperimentCompleteFunction(x));
}

TEST(Callbacks, AddExperimentResultCompleteFunction)
{
	std::function<void(std::shared_ptr<celero::ExperimentResult>)> x = [](std::shared_ptr<celero::ExperimentResult>) {};
	EXPECT_NO_THROW(celero::AddExperimentResultCompleteFunction(x));
}

TEST(Callbacks, ExperimentComplete)
{
	EXPECT_NO_THROW(celero::impl::ExperimentComplete(nullptr));

	std::function<void(std::shared_ptr<celero::Experiment>)> x = [](std::shared_ptr<celero::Experiment>) {};
	EXPECT_NO_THROW(celero::AddExperimentCompleteFunction(x));
	EXPECT_NO_THROW(celero::impl::ExperimentComplete(nullptr));
}

TEST(Callbacks, ExperimentResultComplete)
{
	EXPECT_NO_THROW(celero::impl::ExperimentResultComplete(nullptr));

	std::function<void(std::shared_ptr<celero::ExperimentResult>)> x = [](std::shared_ptr<celero::ExperimentResult>) {};
	EXPECT_NO_THROW(celero::AddExperimentResultCompleteFunction(x));
	EXPECT_NO_THROW(celero::impl::ExperimentResultComplete(nullptr));
}
