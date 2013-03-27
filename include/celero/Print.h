#ifndef _CELERO_PRINT_H_
#define _CELERO_PRINT_H_
 
// www.helleboreconsulting.com

#include <string>
#include <memory>
 
namespace celero
{
	class BenchmarkInfo;

	///
	/// \namespace print
	///
	/// \author	John farrier
	///
	namespace print
	{
		void StageBanner(const std::string& x);
		void GreenBar(const std::string& x);
		void Run(std::shared_ptr<BenchmarkInfo> x);
		void Auto(std::shared_ptr<BenchmarkInfo> x);
		void Done(std::shared_ptr<BenchmarkInfo> x);
		void Baseline(std::shared_ptr<BenchmarkInfo> x);
	}
}

#endif
