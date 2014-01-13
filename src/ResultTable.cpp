#include <celero/ResultTable.h>
#include <celero/PimplImpl.h>

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
		Impl()
		{
		}

		typedef std::pair<uint32_t, double> ResultPair;
		typedef std::vector<ResultPair> ResultVector;
		typedef std::map<std::string, ResultVector> RunMap;
		typedef std::map<std::string, RunMap> ResultMap;

		std::ofstream os;

		ResultMap results;
};

ResultTable::ResultTable() : pimpl()
{
}

ResultTable::~ResultTable()
{
	this->pimpl->os.close();
}

ResultTable& ResultTable::Instance()
{
	static ResultTable singleton;
	return singleton;
}

void ResultTable::setFileName(const std::string& x)
{
	assert(x.empty() == false);

	this->pimpl->os.open(x);

	if(this->pimpl->os.is_open() == false)
	{
		std::cerr << "Celero: Could not open result output file: \"" << x << "\"\n";
	}
}

void ResultTable::add(const std::string& groupName, const std::string& runName, const uint32_t runSize, const double usPerCall)
{
	auto measurements = std::make_pair(runSize, usPerCall);
	auto groupResults = this->pimpl->results[groupName];
	auto it1 = this->pimpl->results.find(groupName);
	auto runResults = (it1->second)[runName];
	auto it2 = it1->second.find(runName);
	it2->second.push_back(measurements);
}

void ResultTable::save()
{
	if(this->pimpl->os.is_open() == true)
	{
		auto os = &(this->pimpl->os);

		std::for_each(this->pimpl->results.begin(), this->pimpl->results.end(),
			[os](const std::pair<std::string, ResultTable::Impl::RunMap>& group)
			{
				*os << group.first << "\n";
			
				auto run = group.second;

				std::for_each(run.begin(), run.end(),
					[run, os](const std::pair<std::string, ResultTable::Impl::ResultVector>& result)
					{
						auto vec = result.second;

						if(result.first == run.begin()->first)
						{
							*os << ",";
							std::for_each(vec.begin(), vec.end(), 
								[os](const ResultTable::Impl::ResultPair& element)
								{
									*os << element.first << ",";
								});
							*os << "\n";
						}

						*os << result.first << ",";
						std::for_each(vec.begin(), vec.end(), 
							[os](const ResultTable::Impl::ResultPair& element)
							{
								*os << element.second << ",";
							});
						*os << "\n";
					});
				
				*os << "\n";
			});
	}

	this->pimpl->os.close();
}

