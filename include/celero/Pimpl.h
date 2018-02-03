#ifndef H_CELERO_PIMPL_H
#define H_CELERO_PIMPL_H

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
#include <memory>

namespace celero
{
	///
	/// \class Pimpl
	///
	///	\author	Herb Sutter
	///	\author	John Farrier
	///
	/// Classes using this must overload the assignment operator.
	/// Original code by Herb Sutter.  Adapted for more primitive compilers by John Farrier.
	///
	template <typename T>
	class Pimpl
	{
	public:
		Pimpl();
		// template<typename ...Args> Pimpl( Args&& ... );
		template <typename Arg1>
		Pimpl(Arg1&&);
		template <typename Arg1, typename Arg2>
		Pimpl(Arg1&&, Arg2&&);
		template <typename Arg1, typename Arg2, typename Arg3>
		Pimpl(Arg1&&, Arg2&&, Arg3&&);
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		Pimpl(Arg1&&, Arg2&&, Arg3&&, Arg4&&);
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		Pimpl(Arg1&&, Arg2&&, Arg3&&, Arg4&&, Arg5&&);
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		Pimpl(Arg1&&, Arg2&&, Arg3&&, Arg4&&, Arg5&&, Arg6&&);
		~Pimpl();

		T* operator->();
		const T* operator->() const;
		T& operator*();

	private:
		std::unique_ptr<T> _pimpl;
	};
} // namespace celero

#endif
