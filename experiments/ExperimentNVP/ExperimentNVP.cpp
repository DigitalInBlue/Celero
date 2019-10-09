#include <celero/Celero.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/rapidjson.h>
#include <algorithm>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <pugixml.hpp>
#include <string>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

// Random JSON generated with: https://onlinerandomtools.com/generate-random-json
// 64 levels deep, 128 elements per level.
// RapidJSON is used based on the results of this benchmark: https://github.com/miloyip/nativejson-benchmark
// It was converted to XML using https://www.freeformatter.com/json-to-xml-converter.html

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

///
/// \class	NVPFixture
///	\autho	John Farrier
///
class NVPFixture : public celero::TestFixture
{
public:
	class UDM : public celero::UserDefinedMeasurementTemplate<double>
	{
		virtual std::string getName() const override
		{
			return "NVP/sec";
		}

		// Turn off some of the output reporting.
		virtual bool reportSize() const override
		{
			return false;
		}

		virtual bool reportMean() const override
		{
			return false;
		}

		virtual bool reportVariance() const override
		{
			return false;
		}

		virtual bool reportStandardDeviation() const override
		{
			return false;
		}

		virtual bool reportSkewness() const override
		{
			return false;
		}

		virtual bool reportKurtosis() const override
		{
			return false;
		}

		virtual bool reportZScore() const override
		{
			return false;
		}

		virtual bool reportMin() const override
		{
			return false;
		}

		virtual bool reportMax() const override
		{
			return true;
		}
	};

	NVPFixture()
	{
		std::string line;
		std::ifstream fstream{JSON_INPUT_FILE};

		while(std::getline(fstream, line))
		{
			this->lineCount++;
		}

		fstream.close();
	}

	virtual ~NVPFixture()
	{
	}

	// Before each sample, build a vector of random integers.
	virtual void setUp(const celero::TestFixture::ExperimentValue&) override
	{
	}

	// Before each iteration
	virtual void onExperimentStart(const celero::TestFixture::ExperimentValue&) override
	{
		// Reset this for each sample.
		this->tempRecordCount = 0;

		// Property Tree
		boost::property_tree::read_json(JSON_INPUT_FILE, docPTree);

		// JSON
		std::ifstream fstream{JSON_INPUT_FILE};
		rapidjson::IStreamWrapper fstreamWrapper(fstream);
		docJSON.ParseStream(fstreamWrapper);

		// XML
		this->docXML.load_file(XML_INPUT_FILE, pugi::parse_default | pugi::parse_declaration);
	}

	virtual void onExperimentEnd() override
	{
	}

	virtual void tearDown() override
	{
		// Compute a user-defined metric (UDM) that is the total number of records per second that could be processed.
		const auto totalTimePerRecord =
			static_cast<double>(this->tempRecordCount * this->getExperimentIterations()) / static_cast<double>(this->getExperimentTime());
		this->udm->addValue(totalTimePerRecord * celero::UsPerSec);
	}

	virtual std::vector<std::shared_ptr<celero::UserDefinedMeasurement>> getUserDefinedMeasurements() const override
	{
		return {this->udm};
	}

	/// User-defined metric (number of records processed per second)
	std::shared_ptr<UDM> udm{new UDM};

	/// The total number of likes in the file.  Can also represent the order of magnitude of the total number of records.
	int lineCount{0};

	/// Stores the total number of records processed during the experiment over all iterations.
	/// It is reset between samples.
	int tempRecordCount{0};

	/// Helps prevent compiler optimization of results.
	std::string stringValue{""};

	/// Read once to not measure file load time.
	boost::property_tree::ptree docPTree;

	/// Read once to not measure file load time.
	rapidjson::Document docJSON;

	/// Read once to not measure file load time.
	pugi::xml_document docXML;
};

constexpr int SamplesCount{100};
constexpr int IterationsCount{0};
constexpr int RandomAccessIterations{10000};

BASELINE_F(NVPRead, Native, NVPFixture, SamplesCount, IterationsCount)
{
	// simulate "processing" on the same scale as the file using multiple data types.
	std::string m = "seeing";
	for(int i = 0; i < RandomAccessIterations; i++)
	{
		if(i % 4 == 0)
		{
			m = "seeing";
		}
		else if(i % 4 == 1)
		{
			m = "rate";
		}
		else if(i % 4 == 2)
		{
			m = "acres";
		}
		else if(i % 4 == 3)
		{
			m = "doesn't exist";
		}

		if(m != "doesn't exist")
		{
			this->stringValue += m;
		}
	}

	this->tempRecordCount += RandomAccessIterations;
}

BENCHMARK_F(NVPRead, RapidJSON, NVPFixture, SamplesCount, IterationsCount)
{
	std::ifstream fstream{JSON_INPUT_FILE};
	rapidjson::IStreamWrapper fstreamWrapper(fstream);
	rapidjson::Document doc;
	doc.ParseStream(fstreamWrapper);
	this->tempRecordCount += this->lineCount;
}

BENCHMARK_F(NVPRead, RapidJSONIter, NVPFixture, SamplesCount, IterationsCount)
{
	static const std::string TypeNames[] = {"Null", "False", "True", "Object", "Array", "String", "Number"};

	auto m = this->docJSON.FindMember("acres");

	for(int i = 0; i < RandomAccessIterations; i++)
	{
		if(i % 4 == 0)
		{
			m = this->docJSON.FindMember("seeing");
		}
		else if(i % 4 == 1)
		{
			m = this->docJSON.FindMember("rate");
		}
		else if(i % 4 == 2)
		{
			m = this->docJSON.FindMember("acres");
		}
		else if(i % 4 == 3)
		{
			m = this->docJSON.FindMember("doesn't exist");
		}

		if(m != this->docJSON.FindMember("doesn't exist"))
		{
			this->stringValue += m->name.GetString();
		}
	}

	this->tempRecordCount += RandomAccessIterations;
}

BENCHMARK_F(NVPRead, PTree, NVPFixture, SamplesCount, IterationsCount)
{
	boost::property_tree::ptree doc;
	boost::property_tree::read_json(JSON_INPUT_FILE, doc);
	this->tempRecordCount += this->lineCount;
}

void BoostPTreeRead(NVPFixture* nvp, boost::property_tree::ptree& tree)
{
	for(auto pos = tree.begin(); pos != tree.end(); ++pos)
	{
		nvp->stringValue += pos->first;
		BoostPTreeRead(nvp, pos->second);
		nvp->tempRecordCount++;
	}
}

BENCHMARK_F(NVPRead, PTreeIter, NVPFixture, SamplesCount, IterationsCount)
{
	auto m = this->docPTree.find("acres");

	for(int i = 0; i < RandomAccessIterations; i++)
	{
		if(i % 4 == 0)
		{
			m = this->docPTree.find("seeing");
		}
		else if(i % 4 == 1)
		{
			m = this->docPTree.find("rate");
		}
		else if(i % 4 == 2)
		{
			m = this->docPTree.find("acres");
		}
		else if(i % 4 == 3)
		{
			m = this->docPTree.find("doesn't exist");
		}

		if(m != this->docPTree.find("doesn't exist"))
		{
			this->stringValue += m->first.data();
		}
	}

	this->tempRecordCount += RandomAccessIterations;
}

BENCHMARK_F(NVPRead, PugiXML, NVPFixture, SamplesCount, IterationsCount)
{
	pugi::xml_document doc;
	doc.load_file(XML_INPUT_FILE, pugi::parse_default | pugi::parse_declaration);
	this->tempRecordCount += this->lineCount;
}

struct PugiIterator : pugi::xml_tree_walker
{
	PugiIterator(NVPFixture* x) : nvp(x)
	{
	}

	virtual bool for_each(pugi::xml_node& node)
	{
		nvp->stringValue += node.name() + std::string(node.value());
		nvp->tempRecordCount++;
		return true;
	}

	NVPFixture* nvp{nullptr};
};

BENCHMARK_F(NVPRead, PugiXMLIter, NVPFixture, SamplesCount, IterationsCount)
{
	auto m = this->docXML.attribute("acres");

	for(int i = 0; i < RandomAccessIterations; i++)
	{
		if(i % 4 == 0)
		{
			m = this->docXML.attribute("seeing");
		}
		else if(i % 4 == 1)
		{
			m = this->docXML.attribute("rate");
		}
		else if(i % 4 == 2)
		{
			m = this->docXML.attribute("acres");
		}
		else if(i % 4 == 3)
		{
			m = this->docXML.attribute("doesn't exist");
		}

		if(m != this->docXML.attribute("doesn't exist"))
		{
			this->stringValue += m.as_string();
		}
	}

	this->tempRecordCount += RandomAccessIterations;
}
