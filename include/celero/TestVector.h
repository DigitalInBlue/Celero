#ifndef _CELERO_TESTVECTOR_H_
#define _CELERO_TESTVECTOR_H_

#include <celero/Export.h>
#include <celero/Pimpl.h>
#include <functional>

namespace celero
{
	class BenchmarkInfo;

	///
	/// \class TestVector
	///
	/// \brief	Don't give me crap about a static global...
	///
	/// \author	John Farrier
	///
	class TestVector
	{
		public:
			static TestVector& Instance();

			void pushBackTest(std::shared_ptr<BenchmarkInfo> x);
			size_t getTestSize() const;
			void forEachTest(std::function<void(std::shared_ptr<BenchmarkInfo>)> f);

			void pushBackBaseline(std::shared_ptr<BenchmarkInfo> x);
			size_t getBaselineSize() const;
			void forEachBaseline(std::function<void(std::shared_ptr<BenchmarkInfo>)> f);
			std::shared_ptr<BenchmarkInfo> getBaseline(const std::string& groupName);

		private:
			///
			/// Default Constructor
			///
			TestVector();

			///
			/// \brief	Pimpl Idiom
			///
			class Impl;

			///
			/// \brief	Pimpl Idiom
			///
			Pimpl<Impl> pimpl;		
	};
}

#endif
