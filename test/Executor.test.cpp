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

#include <celero/Executor.h>
#include <gtest/gtest.h>

TEST(Executor, RunAll)
{
	EXPECT_NO_THROW(celero::executor::RunAll());
}

TEST(Executor, RunAllBaselines)
{
	EXPECT_NO_THROW(celero::executor::RunAllBaselines());
}

TEST(Executor, RunBaseline)
{
	EXPECT_NO_THROW(celero::executor::RunBaseline(nullptr));
}

TEST(Executor, RunAllExperiments)
{
	EXPECT_NO_THROW(celero::executor::RunAllExperiments());
}

TEST(Executor, RunExperiments)
{
	EXPECT_NO_THROW(celero::executor::RunExperiments(nullptr));
}

TEST(Executor, Run_Benchmark)
{
	std::shared_ptr<celero::Benchmark> x;
	EXPECT_NO_THROW(celero::executor::Run(x));

	x = std::make_shared<celero::Benchmark>();
	EXPECT_NO_THROW(celero::executor::Run(x));
}

TEST(Executor, Run_Experiment)
{
	std::shared_ptr<celero::Experiment> x;
	EXPECT_NO_THROW(celero::executor::Run(x));
}

TEST(Executor, Run_String)
{
	EXPECT_NO_THROW(celero::executor::Run(std::string()));
	EXPECT_NO_THROW(celero::executor::Run(std::string("")));
	EXPECT_NO_THROW(celero::executor::Run(std::string("Foo")));
}

TEST(Executor, Run_String2)
{
	EXPECT_NO_THROW(celero::executor::Run(std::string(), std::string()));
	EXPECT_NO_THROW(celero::executor::Run(std::string(""), std::string()));
	EXPECT_NO_THROW(celero::executor::Run(std::string("Foo"), std::string()));

	EXPECT_NO_THROW(celero::executor::Run(std::string(), std::string("")));
	EXPECT_NO_THROW(celero::executor::Run(std::string(""), std::string("")));
	EXPECT_NO_THROW(celero::executor::Run(std::string("Foo"), std::string("")));

	EXPECT_NO_THROW(celero::executor::Run(std::string(), std::string("Foo")));
	EXPECT_NO_THROW(celero::executor::Run(std::string(""), std::string("Foo")));
	EXPECT_NO_THROW(celero::executor::Run(std::string("Foo"), std::string("Foo")));
}
