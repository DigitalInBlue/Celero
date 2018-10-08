#ifndef H_CELERO_PIMPLIMPL_H
#define H_CELERO_PIMPLIMPL_H

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

#include <memory>
#include <utility>

namespace celero
{
	template <typename T>
	Pimpl<T>::Pimpl() : _pimpl(new T())
	{
	}

	template <typename T>
	template <typename Arg1>
	Pimpl<T>::Pimpl(Arg1&& arg1) : _pimpl(new T(std::forward<Arg1>(arg1)))
	{
	}

	template <typename T>
	template <typename Arg1, typename Arg2>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2) : _pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)))
	{
	}

	template <typename T>
	template <typename Arg1, typename Arg2, typename Arg3>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		: _pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3)))
	{
	}

	template <typename T>
	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		: _pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4)))
	{
	}

	template <typename T>
	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		: _pimpl(
			  new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5)))
	{
	}

	template <typename T>
	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		: _pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4),
					   std::forward<Arg5>(arg5), std::forward<Arg6>(arg6)))
	{
	}

	template <typename T>
	Pimpl<T>::~Pimpl()
	{
	}

	template <typename T>
	T* Pimpl<T>::operator->()
	{
		return _pimpl.get();
	}

	template <typename T>
	const T* Pimpl<T>::operator->() const
	{
		return _pimpl.get();
	}

	template <typename T>
	T& Pimpl<T>::operator*()
	{
		return *_pimpl.get();
	}
}

#endif
