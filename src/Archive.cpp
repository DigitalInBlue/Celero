#include <celero/Archive.h>
#include <celero/PimplImpl.h>

#include <assert.h>

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace celero;

///
/// Structure to assist with archiving data during runtime and to a file.
///
struct Result
{
	Result() :
		DataSetSize(0),
		FirstRanDate(0),
		TotalSamplesCollected(0),
		AverageBaseline(0),
		MinBaseline(0),
		MinBaseline_TimeSinceEpoch(0),
		MaxBaseline(0),
		MaxBaseline_TimeSinceEpoch(0),
		CurrentBaseline(0),
		CurrentBaseline_TimeSinceEpoch(0)
	{
	}

	std::string GroupName;
	std::string RunName;

	/// The data set size, if one was specified.
	uint64_t DataSetSize;

	uint64_t FirstRanDate;
	uint32_t TotalSamplesCollected;

	double AverageBaseline;

	double MinBaseline;
	uint64_t MinBaseline_TimeSinceEpoch;
			
	double MaxBaseline;
	uint64_t MaxBaseline_TimeSinceEpoch;
			
	double CurrentBaseline;
	uint64_t CurrentBaseline_TimeSinceEpoch;
};

///
/// \class Impl
///
class celero::Archive::Impl
{
	public:
		Impl()
		{
			
		}

		/// Return milliseconds since epoch.
		uint64_t now() const
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}

		std::vector<Result> results;
		std::string fileName;
};

///
/// Overload operator<< to allow for easy output of result data to a human-readable text file.
///
std::ostream& operator<<(std::ostream& str, Result const& data)
{
	str << data.GroupName << " ";
	str << data.RunName << " ";
	str << data.DataSetSize << " ";
	str << data.FirstRanDate << " ";
	str << data.TotalSamplesCollected << " ";
	str << data.AverageBaseline << " ";
	str << data.MinBaseline << " ";
	str << data.MinBaseline_TimeSinceEpoch << " ";
	str << data.MaxBaseline << " ";
	str << data.MaxBaseline_TimeSinceEpoch << " ";
	str << data.CurrentBaseline << " ";
	str << data.CurrentBaseline_TimeSinceEpoch << "\n";
	return str;
}

///
/// Overload operator>> to allow for easy input of result data from a text file.
///
std::istream& operator>>(std::istream& str, Result& data)
{
	str >> data.GroupName;
	str >> data.RunName;
	str >> data.DataSetSize;
	str >> data.FirstRanDate;
	str >> data.TotalSamplesCollected;
	str >> data.AverageBaseline;
	str >> data.MinBaseline;
	str >> data.MinBaseline_TimeSinceEpoch;
	str >> data.MaxBaseline;
	str >> data.MaxBaseline_TimeSinceEpoch;
	str >> data.CurrentBaseline;
	str >> data.CurrentBaseline_TimeSinceEpoch;
	return str;
}

Archive::Archive() : pimpl()
{
}

Archive::~Archive()
{
}

Archive& Archive::Instance()
{
	static Archive singleton;
	return singleton;
}

void Archive::setFileName(const std::string& x)
{
	assert(x.empty() == false);

	this->pimpl->fileName = x;

	// Read in existing results?
	std::ifstream is;
	is.open(this->pimpl->fileName, std::fstream::in);

	if(is.is_open() == true)
	{
		// Read in existing results.
		while(is.eof() == false && is.tellg() >= 0)
		{
			Result r;
			is >> r;

			if(r.GroupName.empty() == false)
			{
				this->pimpl->results.push_back(r);
			}
		}

		// Close the file for reading.
		is.close();
	}
}

void Archive::add(celero::BenchmarkInfo benchmark)
{
	if(benchmark.getIsBaselineCase() == false)
	{
		auto found = std::find_if(std::begin(this->pimpl->results), std::end(this->pimpl->results),
			[benchmark](const Result& r)->bool
			{
				return (r.GroupName == benchmark.getGroupName()) && (r.RunName == benchmark.getTestName()) && (r.DataSetSize == benchmark.getProblemSetSize());
			});

		if(found != std::end(this->pimpl->results))
		{
			found->CurrentBaseline = benchmark.getBaselineMeasurement();
			found->CurrentBaseline_TimeSinceEpoch = this->pimpl->now();

			if(found->CurrentBaseline <= found->MinBaseline)
			{
				 found->MinBaseline = found->CurrentBaseline;
				 found->MinBaseline_TimeSinceEpoch = found->CurrentBaseline_TimeSinceEpoch;
			}

			if(found->CurrentBaseline >= found->MaxBaseline)
			{
				 found->MaxBaseline = found->CurrentBaseline;
				 found->MaxBaseline_TimeSinceEpoch = found->CurrentBaseline_TimeSinceEpoch;
			}

			// This is not good IEEE math.
			found->AverageBaseline = ((found->AverageBaseline * found->TotalSamplesCollected) + found->CurrentBaseline) / (found->TotalSamplesCollected + 1);
			found->TotalSamplesCollected++;
		}	
		else
		{
			Result r;
			r.GroupName = benchmark.getGroupName();
			r.RunName = benchmark.getTestName();
			r.FirstRanDate = this->pimpl->now();
			r.AverageBaseline = benchmark.getBaselineMeasurement();
			r.CurrentBaseline = benchmark.getBaselineMeasurement();
			r.CurrentBaseline_TimeSinceEpoch = r.FirstRanDate;
			r.DataSetSize = benchmark.getProblemSetSize();
			r.MaxBaseline = benchmark.getBaselineMeasurement();
			r.MaxBaseline_TimeSinceEpoch = r.FirstRanDate;
			r.MinBaseline = benchmark.getBaselineMeasurement();
			r.MinBaseline_TimeSinceEpoch = r.FirstRanDate;
			r.TotalSamplesCollected = 1;
		
			this->pimpl->results.push_back(r);
		}

		this->save();
	}
}

void Archive::save()
{
	if(this->pimpl->fileName.empty() == false)
	{
		// Get ready to write out new results.
		std::ofstream os;
		os.open(this->pimpl->fileName.c_str(), std::fstream::out);

		if(os.is_open() == true)
		{
			for(auto i : this->pimpl->results)
			{
				os << i;
			}

			os.flush();
			os.close();
		}
		else
		{
			std::cerr << "Celero: Could not open result output file: \"" << this->pimpl->fileName << "\"\n";
		}
	}
}
