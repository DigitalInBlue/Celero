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

#ifdef CELERO_STATIC
#define CELERO_EXPORT
#define CELERO_EXPORT_C
#else
#ifdef _WIN32
#if defined CELERO_EXPORTS
#define CELERO_EXPORT __declspec(dllexport)
#define CELERO_EXPORT_C extern "C" __declspec(dllexport)
#else
#define CELERO_EXPORT __declspec(dllimport)
#define CELERO_EXPORT_C extern "C" __declspec(dllimport)
#endif
#else
#define CELERO_EXPORT
#define CELERO_EXPORT_C extern "C"
#endif
#endif

