#ifndef H_CELERO_FACTORY_H
#define H_CELERO_FACTORY_H

// www.helleboreconsulting.com

#include <celero/Export.h>
#include <memory>

namespace celero
{
	class TestFixture;

	///
	/// \class Factory
	///
	/// \author	John Farrier
	///
	/// Pure Virtual Base class for benchmarks.
	///
	class CELERO_EXPORT Factory
	{
		public:
			///
			/// \brief	Default Constructor
			///
			Factory()
			{
			}

			///
			/// \brief	Virtual Destructor
			///
			virtual ~Factory()
			{
			}

			///
			/// \brief	Pure virtual function.
			///
			virtual std::shared_ptr<TestFixture> Create() = 0;
	};
}

#endif
