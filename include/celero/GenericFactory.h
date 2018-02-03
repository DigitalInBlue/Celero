#ifndef H_CELERO_GENERICFACTORY_H
#define H_CELERO_GENERICFACTORY_H

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

#include <celero/Export.h>
#include <celero/Factory.h>

namespace celero
{
	///
	/// \class GenericFactory
	///
	/// \author	John farrier
	///
	template <class T>
	class GenericFactory : public Factory
	{
	public:
		///
		/// \brief	Default Constructor
		///
		GenericFactory() : Factory()
		{
		}

		///
		/// \brief	Virtual Destructor
		///
		virtual ~GenericFactory()
		{
		}

		///
		/// \brief	Overload the pure virtual base class function.
		///
		virtual std::shared_ptr<TestFixture> Create()
		{
			return std::make_shared<T>();
		}
	};
}

#endif
