#ifndef _CELERO_EXPORT_H_
#define _CELERO_EXPORT_H_

// www.helleboreconsulting.com

///
///	\author	John Farrier
///

#ifdef CELERO_STATIC
	#define CELERO_EXPORT
	#define	CELERO_EXPORT_C
#else
	#ifdef WIN32
		#if defined CELERO_EXPORTS
			#define CELERO_EXPORT _declspec(dllexport)
			#define CELERO_EXPORT_C extern "C" _declspec(dllexport)
		#else
			#define CELERO_EXPORT _declspec(dllimport)
			#define CELERO_EXPORT_C extern "C" _declspec(dllimport)
		#endif
	#else
		#define CELERO_EXPORT
		#define CELERO_EXPORT_C extern "C"
	#endif
#endif

#endif
