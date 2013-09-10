#ifndef H_CELERO_PIMPLIMPL_H
#define H_CELERO_PIMPLIMPL_H
 
// www.helleboreconsulting.com

#include <memory>
#include <utility>

namespace celero
{
	template<typename T> Pimpl<T>::Pimpl() : 
		_pimpl(new T()) 
	{
	}

	template<typename T> 
	template<typename Arg1>
	Pimpl<T>::Pimpl(Arg1&& arg1) : 
		_pimpl(new T(std::forward<Arg1>(arg1))) 
	{
	}

	template<typename T> 
	template<typename Arg1, typename Arg2>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2) : 
		_pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2))) 
	{
	}

	template<typename T> 
	template<typename Arg1, typename Arg2, typename Arg3>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3) : 
		_pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3))) 
	{
	}

	template<typename T> 
	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4) : 
		_pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4))) 
	{
	}

	template<typename T> 
	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5) : 
		_pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5))) 
	{
	}

	template<typename T> 
	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
	Pimpl<T>::Pimpl(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6) : 
		_pimpl(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3), std::forward<Arg4>(arg4), std::forward<Arg5>(arg5), std::forward<Arg6>(arg6))) 
	{
	}

	template<typename T> Pimpl<T>::~Pimpl() 
	{ 
	}
 
	template<typename T> T* Pimpl<T>::operator->() 
	{ 
		return _pimpl.get(); 
	}

	template<typename T> const T* const Pimpl<T>::operator->() const
	{ 
		return _pimpl.get(); 
	}
 
	template<typename T> T& Pimpl<T>::operator*() 
	{ 
		return *_pimpl.get(); 
	}
}

#endif
