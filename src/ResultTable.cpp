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

#include <celero/ResultTable.h>
#include <celero/PimplImpl.h>
#include <celero/Benchmark.h>

#include <assert.h>

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace celero;

///
/// \class Impl
///
class celero::ResultTable::Impl
{
	public:
		Impl() :
			fileName(),
			results()
		{
		}

		std::string fileName;
		std::map<std::string, std::map<std::string, std::vector<std::pair<int64_t, double>>>> results;
};

ResultTable::ResultTable() : pimpl()
{
}

ResultTable::~ResultTable()
{
}

ResultTable& ResultTable::Instance()
{
	static ResultTable singleton;
	return singleton;
}

void ResultTable::setFileName(const std::string& x)
{
	assert(x.empty() == false);
	this->pimpl->fileName = x;
}

void ResultTable::add(std::shared_ptr<Result> x)
{
	const auto measurements = std::make_pair(x->getProblemSpaceValue(), x->getUsPerCall());
	this->pimpl->results[x->getExperiment()->getBenchmark()->getName()][x->getExperiment()->getName()].push_back(measurements);
	this->save();
}

void ResultTable::save()
{
	std::ofstream ofs;
	ofs.open(this->pimpl->fileName);

	if(ofs.is_open() == true)
	{
		const auto os = &ofs;

		std::for_each(std::begin(this->pimpl->results), std::end(this->pimpl->results),
			[&os](decltype(*std::begin(this->pimpl->results))& group)
			{
				*os << group.first << "\n";
			
				const auto run = group.second;

				std::for_each(std::begin(run), std::end(run),
					[run, os](decltype(*std::begin(run))& result)
					{
						auto vec = result.second;

						if(result.first == std::begin(run)->first)
						{
							*os << ",";
							std::for_each(std::begin(vec), std::end(vec), 
								[os](decltype(*std::begin(vec))& element)
								{
									*os << element.first << ",";
								});
							*os << "\n";
						}

						*os << result.first << ",";
						std::for_each(std::begin(vec), std::end(vec), 
							[os](decltype(*std::begin(vec))& element)
							{
								*os << element.second << ",";
							});
						*os << "\n";
					});
				
				*os << "\n";
			});

		ofs.close();
	}
}

