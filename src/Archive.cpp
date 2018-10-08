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

#include <assert.h>
#include <celero/Archive.h>
#include <celero/Benchmark.h>
#include <celero/FileReader.h>
#include <celero/PimplImpl.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace celero;

///
/// Structure to assist with archiving data during runtime and to a file.
///
struct ArchiveEntry
{
	ArchiveEntry() :
		GroupName(),
		RunName(),
		ExperimentValue(0),
		ExperimentValueScale(0),
		FirstRanDate(0),
		TotalSamplesCollected(0),
		AverageBaseline(0),
		MinBaseline(0),
		MinBaseline_TimeSinceEpoch(0),
		MinStats(),
		MaxBaseline(0),
		MaxBaseline_TimeSinceEpoch(0),
		MaxStats(),
		CurrentBaseline(0),
		CurrentBaseline_TimeSinceEpoch(0),
		CurrentStats(),
		Failure(false)
	{
	}

	static void WriteHeader(std::ostream& str)
	{
		str << "GroupName,RunName,Failure,ExperimentValue,ExperimentValueScale,FirstRanDate,TotalSamplesCollected,AverageBaseline,";
		str << "MinBaseline,MinBaselineTimeSinceEpoch,";
		str << "MinStatSize,MinStatMean,MinStatVariance,MinStatStandardDeviation,MinStatSkewness,MinStatKurtosis,";
		str << "MinStatMin,MinStatMax,";
		str << "MaxBaseline,MaxBaselineTimeSinceEpoch,";
		str << "MaxStatSize,MaxStatMean,MaxStatVariance,MaxStatStandardDeviation,MaxStatSkewness,MaxStatKurtosis,";
		str << "MaxStatMin,MaxStatMax,";
		str << "CurrentBaseline,CurrentBaselineTimeSinceEpoch,";
		str << "CurrentStatSize,CurrentStatMean,CurrentStatVariance,CurrentStatStandardDeviation,CurrentStatSkewness,CurrentStatKurtosis,";
		str << "CurrentStatMin,CurrentStatMax" << std::endl;
	}

	struct Stat
	{
		Stat() : Size(0), Mean(0), Variance(0), StandardDeviation(0), Skewness(0), Kurtosis(0), Min(0), Max(0)
		{
		}

		Stat& operator=(const celero::Statistics& s)
		{
			this->Size = s.getSize();
			this->Mean = s.getMean();
			this->Variance = s.getVariance();
			this->StandardDeviation = s.getStandardDeviation();
			this->Skewness = s.getSkewness();
			this->Kurtosis = s.getKurtosis();
			this->Min = s.getMin();
			this->Max = s.getMax();

			return *this;
		}

		uint64_t Size;
		double Mean;
		double Variance;
		double StandardDeviation;
		double Skewness;
		double Kurtosis;
		uint64_t Min;
		uint64_t Max;
	};

	std::string GroupName;
	std::string RunName;

	/// The data set size, if one was specified.
	int64_t ExperimentValue;
	double ExperimentValueScale;

	uint64_t FirstRanDate;
	uint32_t TotalSamplesCollected;

	double AverageBaseline;

	double MinBaseline;
	uint64_t MinBaseline_TimeSinceEpoch;
	Stat MinStats;

	double MaxBaseline;
	uint64_t MaxBaseline_TimeSinceEpoch;
	Stat MaxStats;

	double CurrentBaseline;
	uint64_t CurrentBaseline_TimeSinceEpoch;
	Stat CurrentStats;

	bool Failure;
};

///
/// Overload operator<< to allow for easy output of result data to a human-readable text file.
///
std::ostream& operator<<(std::ostream& str, ArchiveEntry::Stat const& data)
{
	str << data.Size << ",";
	str << data.Mean << ",";
	str << data.Variance << ",";
	str << data.StandardDeviation << ",";
	str << data.Skewness << ",";
	str << data.Kurtosis << ",";
	str << data.Min << ",";
	str << data.Max;
	return str;
}

///
/// Overload operator<< to allow for easy output of result data to a human-readable text file.
///
std::ostream& operator<<(std::ostream& str, ArchiveEntry const& data)
{
	str << data.GroupName << ",";
	str << data.RunName << ",";
	str << data.Failure << ",";
	str << data.ExperimentValue << ",";
	str << data.ExperimentValueScale << ",";
	str << data.FirstRanDate << ",";
	str << data.TotalSamplesCollected << ",";
	str << data.AverageBaseline << ",";
	str << data.MinBaseline << ",";
	str << data.MinBaseline_TimeSinceEpoch << ",";
	str << data.MinStats << ",";
	str << data.MaxBaseline << ",";
	str << data.MaxBaseline_TimeSinceEpoch << ",";
	str << data.MaxStats << ",";
	str << data.CurrentBaseline << ",";
	str << data.CurrentBaseline_TimeSinceEpoch << ",";
	str << data.CurrentStats << std::endl;
	return str;
}

///
/// Overload operator>> to allow for easy input of result data from a text file.
///
std::istream& operator>>(std::istream& str, ArchiveEntry::Stat& data)
{
	// Use FieldReader to classify commas as whitespace.
	str.imbue(std::locale(std::locale(), new celero::FieldReader));

	str >> data.Size;
	str >> data.Mean;
	str >> data.Variance;
	str >> data.StandardDeviation;
	str >> data.Skewness;
	str >> data.Kurtosis;
	str >> data.Min;
	str >> data.Max;
	return str;
}

///
/// Overload operator>> to allow for easy input of result data from a text file.
///
std::istream& operator>>(std::istream& str, ArchiveEntry& data)
{
	// Use FieldReader to classify commas as whitespace.
	str.imbue(std::locale(std::locale(), new celero::FieldReader));

	str >> data.GroupName;
	str >> data.RunName;
	str >> data.Failure;
	str >> data.ExperimentValue;
	str >> data.ExperimentValueScale;
	str >> data.FirstRanDate;
	str >> data.TotalSamplesCollected;
	str >> data.AverageBaseline;
	str >> data.MinBaseline;
	str >> data.MinBaseline_TimeSinceEpoch;
	str >> data.MinStats;
	str >> data.MaxBaseline;
	str >> data.MaxBaseline_TimeSinceEpoch;
	str >> data.MaxStats;
	str >> data.CurrentBaseline;
	str >> data.CurrentBaseline_TimeSinceEpoch;
	str >> data.CurrentStats;
	return str;
}

///
/// \class Impl
///
class celero::Archive::Impl
{
public:
	Impl() : results(), fileName()
	{
	}

	/// Return milliseconds since epoch.
	uint64_t now() const
	{
		return static_cast<uint64_t>(
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	}

	void readExistingResults()
	{
		// Read in existing results?
		std::ifstream is;
		is.open(this->fileName, std::fstream::in);

		if((is.is_open() == true) && (is.good() == true) && (is.fail() == false))
		{
			// Throw away the header.
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			// Read in existing results.
			while((is.eof() == false) && (is.tellg() >= 0))
			{
				ArchiveEntry r;
				is >> r;

				if(r.GroupName.empty() == false)
				{
					this->results.push_back(r);
				}
			}

			// Close the file for reading.
			is.close();
		}
	}

	std::vector<ArchiveEntry> results;
	std::string fileName;
};

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
	this->pimpl->readExistingResults();
}

void Archive::add(std::shared_ptr<celero::ExperimentResult> x)
{
	const auto found = std::find_if(std::begin(this->pimpl->results), std::end(this->pimpl->results), [x](const ArchiveEntry& r) -> bool {
		return (r.GroupName == x->getExperiment()->getBenchmark()->getName()) && (r.RunName == x->getExperiment()->getName())
			   && (r.ExperimentValue == x->getProblemSpaceValue());
	});

	if(found != std::end(this->pimpl->results))
	{
		if(x->getFailure() == true)
		{
			return;
		}

		found->CurrentBaseline = x->getBaselineMeasurement();
		found->CurrentBaseline_TimeSinceEpoch = this->pimpl->now();
		found->CurrentStats = *x->getTimeStatistics();

		if(found->Failure || found->CurrentBaseline <= found->MinBaseline)
		{
			found->MinBaseline = found->CurrentBaseline;
			found->MinBaseline_TimeSinceEpoch = found->CurrentBaseline_TimeSinceEpoch;
			found->MinStats = found->CurrentStats;
		}

		if(found->Failure || found->CurrentBaseline >= found->MaxBaseline)
		{
			found->MaxBaseline = found->CurrentBaseline;
			found->MaxBaseline_TimeSinceEpoch = found->CurrentBaseline_TimeSinceEpoch;
			found->MaxStats = found->CurrentStats;
		}

		// This is not good IEEE math.
		if(found->Failure == false)
		{
			found->AverageBaseline =
				((found->AverageBaseline * found->TotalSamplesCollected) + found->CurrentBaseline) / (found->TotalSamplesCollected + 1);
		}
		else
		{
			found->AverageBaseline = found->CurrentBaseline;
		}

		found->TotalSamplesCollected++;
	}
	else
	{
		ArchiveEntry r;

		r.GroupName = x->getExperiment()->getBenchmark()->getName();
		r.RunName = x->getExperiment()->getName();
		r.Failure = x->getFailure();
		r.FirstRanDate = this->pimpl->now();
		r.AverageBaseline = x->getBaselineMeasurement();
		r.ExperimentValue = x->getProblemSpaceValue();
		r.ExperimentValueScale = x->getProblemSpaceValueScale();
		r.TotalSamplesCollected = x->getFailure() ? 0 : 1;

		r.CurrentBaseline = x->getBaselineMeasurement();
		r.CurrentBaseline_TimeSinceEpoch = r.FirstRanDate;
		r.CurrentStats = *x->getTimeStatistics();

		r.MaxBaseline = x->getBaselineMeasurement();
		r.MaxBaseline_TimeSinceEpoch = r.FirstRanDate;
		r.MaxStats = *x->getTimeStatistics();

		r.MinBaseline = x->getBaselineMeasurement();
		r.MinBaseline_TimeSinceEpoch = r.FirstRanDate;
		r.MinStats = *x->getTimeStatistics();

		this->pimpl->results.push_back(r);
	}

	this->save();
}

void Archive::save()
{
	if(this->pimpl->fileName.empty() == false)
	{
		// Get ready to write out new results.
		// We will write all known results every time, replacing file contents.
		std::ofstream os;
		os.open(this->pimpl->fileName.c_str(), std::fstream::out);

		if(os.is_open() == true)
		{
			ArchiveEntry::WriteHeader(os);

			for(auto& i : this->pimpl->results)
			{
				os << i;
			}

			os.flush();
			os.close();
		}
		else
		{
			std::cerr << "Celero: Could not open result output file: \"" << this->pimpl->fileName << "\"" << std::endl;
		}
	}
}
