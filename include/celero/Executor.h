#ifndef _CELERO_EXECUTOR_H_
#define _CELERO_EXECUTOR_H_
 
// www.helleboreconsulting.com

#include <string>
#include <memory>
 
namespace celero
{
	class BenchmarkInfo;

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
