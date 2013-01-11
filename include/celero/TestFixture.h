#ifndef _CELERO_TESTFIXTURE_H_
#define _CELERO_TESTFIXTURE_H_

#include <cstddef>
#include <stdint.h>

#include <celero/Timer.h>
#include <celero/Export.h>

namespace celero
{
	///
	/// \class TestFixture
	///
	class CELERO_EXPORT TestFixture
	{
		public:
			///
			/// Default Constructor.
			///
			TestFixture();

			///
			/// Virtual destructor for inheritance.
			///
			virtual ~TestFixture();

			///
			/// Set up the test fixture before benchmark execution.
			///
			virtual void SetUp();
		
			///
			/// Called after test completion to destroy the fixture.
			///
			virtual void TearDown();
		
			///
			/// \param calls The number of times to loop over the UserBenchmark function.
			///
			/// \return Returns the number of microseconds the run took.
			///
			int64_t Run(const uint64_t calls);

		protected:
			/// Executed for each operation the benchmarking test is run.
			virtual void UserBenchmark();

		private:
	};
}

#endif
