#ifndef H_CELERO_TESTVECTOR_H
#define H_CELERO_TESTVECTOR_H

// www.helleboreconsulting.com

///
/// \author	John Farrier
///
/// \copyright Copyright 2014 John Farrier 
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

#include <celero/Export.h>
#include <celero/Pimpl.h>
#include <functional>

namespace celero
{
	class BenchmarkInfo;

	///
	/// \class TestVector
	///
	/// \author	John Farrier
	///
	/// \brief	Don't give me crap about a static global...
	///
	class TestVector
	{
		public:
			static TestVector& Instance();

			void pushBackTest(std::shared_ptr<BenchmarkInfo> x);
			size_t getTestSize() const;
			size_t getTestSize(const std::string& groupName) const;
			void forEachTest(std::function<void(std::shared_ptr<BenchmarkInfo>)> f);

			void pushBackBaseline(std::shared_ptr<BenchmarkInfo> x);
			size_t getBaselineSize() const;
			size_t getBaselineSize(const std::string& groupName) const;
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
