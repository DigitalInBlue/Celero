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

#include <celero/Distribution.h>
#include <celero/Print.h>
#include <celero/Utilities.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>

using namespace celero;

std::vector<uint64_t> celero::BuildDistribution(uint64_t numberOfSamples, uint64_t iterationsPerSample)
{
	std::vector<uint64_t> measurements;

	while(numberOfSamples--)
	{
		// Dummy variable
		auto dummy = uint64_t(0);
		auto cps = iterationsPerSample;

		// Get the starting time.
		const auto startTime = celero::timer::GetSystemTime();

		while(cps--)
		{
			celero::DoNotOptimizeAway(dummy++);
		}

		const auto endTime = celero::timer::GetSystemTime();

		measurements.push_back(endTime - startTime);
	}

	return measurements;
}

void celero::RunDistribution(uint64_t intArgument)
{
	std::vector<double> series1Normalized(intArgument);
	std::vector<double> series2Normalized(intArgument);
	std::vector<double> series3Normalized(intArgument);
	std::vector<double> series4Normalized(intArgument);

	auto series1 = celero::BuildDistribution(intArgument, uint64_t(64));
	auto series2 = celero::BuildDistribution(intArgument, uint64_t(256));
	auto series3 = celero::BuildDistribution(intArgument, uint64_t(1024));
	auto series4 = celero::BuildDistribution(intArgument, uint64_t(4096));

	std::array<std::map<double, uint64_t>, 4> histograms;

	// Find the global max for all tests:
	auto maxVal = std::max(*(std::max_element(std::begin(series1), std::end(series1))), *(std::max_element(std::begin(series2), std::end(series2))));
	maxVal = std::max(maxVal, *(std::max_element(std::begin(series3), std::end(series3))));
	maxVal = std::max(maxVal, *(std::max_element(std::begin(series4), std::end(series4))));

	// Find the global min for all tests:
	auto minVal = std::min(*(std::min_element(std::begin(series1), std::end(series1))), *(std::min_element(std::begin(series2), std::end(series2))));
	minVal = std::min(minVal, *(std::min_element(std::begin(series3), std::end(series3))));
	minVal = std::min(minVal, *(std::min_element(std::begin(series4), std::end(series4))));

	// Normalize all vectors:
	auto normalize = [minVal, maxVal](uint64_t val) -> double {
		if(val >= minVal)
		{
			if(val <= maxVal)
			{
				const auto delta = maxVal - minVal;
				val -= minVal;
				return static_cast<double>(val) / static_cast<double>(delta);
			}

			return static_cast<double>(maxVal);
		}

		return static_cast<double>(minVal);
	};

	std::transform(std::begin(series1), std::end(series1), std::begin(series1Normalized),
				   [normalize](const uint64_t val) -> double { return normalize(val); });

	std::transform(std::begin(series2), std::end(series2), std::begin(series2Normalized),
				   [normalize](const uint64_t val) -> double { return normalize(val); });

	std::transform(std::begin(series3), std::end(series3), std::begin(series3Normalized),
				   [normalize](const uint64_t val) -> double { return normalize(val); });

	std::transform(std::begin(series4), std::end(series4), std::begin(series4Normalized),
				   [normalize](const uint64_t val) -> double { return normalize(val); });

	// Build histograms of each of the series:
	std::for_each(std::begin(series1Normalized), std::end(series1Normalized),
				  [&histograms](const double val) { histograms[0][static_cast<int>(val * 1024)]++; });

	std::for_each(std::begin(series2Normalized), std::end(series2Normalized),
				  [&histograms](const double val) { histograms[1][static_cast<int>(val * 1024)]++; });

	std::for_each(std::begin(series3Normalized), std::end(series3Normalized),
				  [&histograms](const double val) { histograms[2][static_cast<int>(val * 1024)]++; });

	std::for_each(std::begin(series4Normalized), std::end(series4Normalized),
				  [&histograms](const double val) { histograms[3][static_cast<int>(val * 1024)]++; });

	// Find the maximum length of all histograms:
	auto maxLen = size_t(0);
	maxLen = std::max(maxLen, histograms[0].size());
	maxLen = std::max(maxLen, histograms[1].size());
	maxLen = std::max(maxLen, histograms[2].size());
	maxLen = std::max(maxLen, histograms[3].size());

	// Write out a CSV file that contains all four series:
	std::ofstream os;
	os.open("celeroDistribution.csv");

	os << "64,,256,,1024,,4096,," << std::endl;

	for(size_t i = 0; i < maxLen; ++i)
	{
		for(size_t j = 0; j < histograms.size(); j++)
		{
			if(i < histograms[j].size())
			{
				auto element = std::begin(histograms[j]);
				for(size_t k = 0; k < i; k++)
				{
					++element;
				}

				os << element->first << "," << element->second << ",";
			}
			else
			{
				os << ",,";
			}
		}

		os << std::endl;
	}

	os.close();
}
