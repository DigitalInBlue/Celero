#ifndef H_CELERO_EXPORT_H
#define H_CELERO_EXPORT_H

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

#ifdef CELERO_STATIC
#define CELERO_EXPORT
#define CELERO_EXPORT_C
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
