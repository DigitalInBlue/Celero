#ifndef _CELERO_PIMPL_H_
#define _CELERO_PIMPL_H_
 
// www.helleboreconsulting.com

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
	template<typename T> class Pimpl 
	{
		public:
			Pimpl();
			//template<typename ...Args> Pimpl( Args&& ... );
			template<typename Arg1> Pimpl(Arg1&&);
			template<typename Arg1, typename Arg2> Pimpl(Arg1&&, Arg2&&);
			template<typename Arg1, typename Arg2, typename Arg3> Pimpl(Arg1&&, Arg2&&, Arg3&&);
			template<typename Arg1, typename Arg2, typename Arg3, typename Arg4> Pimpl(Arg1&&, Arg2&&, Arg3&&, Arg4&&);
			template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> Pimpl(Arg1&&, Arg2&&, Arg3&&, Arg4&&, Arg5&&);
			template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6> Pimpl(Arg1&&, Arg2&&, Arg3&&, Arg4&&, Arg5&&, Arg6&&);
			~Pimpl();

			T* operator->();
			const T* const operator->() const;
			T& operator*();

		private:
			std::unique_ptr<T> _pimpl;
	};
}

#endif
