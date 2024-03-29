#pragma once

///
/// \author	John Farrier
///
/// \copyright Copyright 2015-2023 John Farrier
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

#include <celero/Benchmark.h>
#include <celero/Export.h>
#include <celero/Pimpl.h>

#include <functional>

namespace celero
{
	///
	/// \class TestVector
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT TestVector
	{
	public:
		~TestVector();

		TestVector(const TestVector&) = delete;
		TestVector& operator=(TestVector const& other) = delete;

		static TestVector& Instance();

		void push_back(std::shared_ptr<Benchmark> x);

		void clear();
		size_t size() const;

		bool containsGroup(const std::string& x) const;

		std::shared_ptr<Benchmark> operator[](size_t x);
		std::shared_ptr<Benchmark> operator[](const std::string& x);

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
} // namespace celero

