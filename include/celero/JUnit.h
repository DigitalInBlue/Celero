#ifndef H_CELERO_JUNIT_H
#define H_CELERO_JUNIT_H

// www.helleboreconsulting.com

#include <celero/Pimpl.h>
#include <string>

namespace celero
{
	class BenchmarkInfo;

	///
	/// \class JUnit
	///
	/// \author	John Farrier
	///
	class JUnit
	{
		public:
			///
			/// Singleton 
			///
			static JUnit& Instance();

			///
			/// Specify a file name for a results output file.
			///
			/// \param x	The name of the output file in which to store Celero's results.
			///
			void setFileName(const std::string& x);

			///
			///
			///
			void add(celero::BenchmarkInfo x);

			///
			///
			///
			void save();

		private:
			///
			/// Default Constructor
			///
			JUnit();

			///
			/// Default Destructor
			///
			~JUnit();

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
