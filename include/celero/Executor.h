#ifndef H_CELERO_EXECUTOR_H
#define H_CELERO_EXECUTOR_H
 
// www.helleboreconsulting.com

///
/// \author	John Farrier
///
/// \copyright Copyright 2014 John Farrier 
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

#include <string>
#include <memory>
#include <celero/Export.h>
#include <celero/BenchmarkInfo.h>
 
namespace celero
{
	namespace executor
	{
		void Execute(std::shared_ptr<BenchmarkInfo> x);

		void RunAll();
		bool RunAllBaselines();
		bool RunAllTests();

		void RunGroup(const std::string& x);

		void Run(const std::string& group, const std::string& test);
		void RunBaseline(const std::string& x);
		void RunTest(const std::string& x, const std::string& test);
	}
}

#endif
