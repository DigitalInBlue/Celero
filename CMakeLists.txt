#
# Top level makefile for Celero
#
# Copyright 2015, 2016, 2017, 2018, 2019 John Farrier
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#

#
# Cmake Configuration
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)

macro(CeleroSetDefaultCompilerOptions)
	if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
		target_compile_options(${PROJECT_NAME} PRIVATE /D_VARIADIC_MAX=10 )
		target_compile_options(${PROJECT_NAME} PRIVATE /D_CRT_SECURE_NO_WARNINGS)
		target_compile_options(${PROJECT_NAME} PRIVATE /wd4251)
		target_compile_options(${PROJECT_NAME} PRIVATE /MP)
		target_compile_options(${PROJECT_NAME} PRIVATE /D_SCL_SECURE_NO_WARNINGS)
		target_compile_options(${PROJECT_NAME} PRIVATE /permissive-)
		target_compile_options(${PROJECT_NAME} PRIVATE /std:c++14)

        if(CELERO_TREAT_WARNINGS_AS_ERRORS)
            target_compile_options(${PROJECT_NAME} PRIVATE /WX)
		endif()

	elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
		target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
		target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14)

	elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
		if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
			target_compile_options(${PROJECT_NAME} PRIVATE -Xclang -std=c++14)
			target_compile_options(${PROJECT_NAME} PRIVATE -Wno-c++98-compat)
			target_compile_options(${PROJECT_NAME} PRIVATE -Wno-c++98-compat-pedantic)
			target_compile_options(${PROJECT_NAME} PRIVATE -Wno-reserved-id-macro)
			if(CELERO_TREAT_WARNINGS_AS_ERRORS)
				target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
			endif()
		else()
			target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
			target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
			target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14)
			if(CELERO_TREAT_WARNINGS_AS_ERRORS)
				target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
			endif()
		endif()

	elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
		target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
		target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14)

		if(CELERO_TREAT_WARNINGS_AS_ERRORS)
			target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
		endif()
	endif()
endmacro()

# Project Name
PROJECT(CeleroProject)

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

#
# User Options
#

option(CELERO_COMPILE_DYNAMIC_LIBRARIES "Set to ON to build Celero for dynamic linking.  Use OFF for static." ON)
option(CELERO_ENABLE_EXPERIMENTS "Set to ON to automatically build all examples." OFF)
option(CELERO_ENABLE_FOLDERS "Enable to put Celero in its own solution folder under Visual Studio" ON)
option(CELERO_ENABLE_TESTS "Enable building and running unit tests." OFF)
option(CELERO_TREAT_WARNINGS_AS_ERRORS "Treat compile warnings as errors." ON)

if(CELERO_COMPILE_DYNAMIC_LIBRARIES)
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "SHARED")
else()
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "STATIC")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Build and Install Settings
#

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_DEBUG_POSTFIX          "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX        ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX     ""  CACHE STRING "add a postfix, usually empty on windows")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------

set(PROJECT_NAME celero)

if(CELERO_COMPILE_DYNAMIC_LIBRARIES)
	add_definitions(-DCELERO_EXPORTS)
else()
	add_definitions(-DCELERO_STATIC)
endif()

add_library(${PROJECT_NAME} ${CELERO_USER_DEFINED_SHARED_OR_STATIC})

target_sources(${PROJECT_NAME} PRIVATE
	include/celero/Archive.h
	include/celero/Benchmark.h
	include/celero/Callbacks.h
	include/celero/Celero.h
	include/celero/CommandLine.h
	include/celero/Console.h
	include/celero/Distribution.h
	include/celero/Exceptions.h
	include/celero/Executor.h
	include/celero/Export.h
	include/celero/Factory.h
	include/celero/FileReader.h
	include/celero/GenericFactory.h
	include/celero/JUnit.h
	include/celero/Memory.h
	include/celero/Pimpl.h
	include/celero/PimplImpl.h
	include/celero/Print.h
	include/celero/Experiment.h
	include/celero/ExperimentResult.h
	include/celero/ResultTable.h
	include/celero/Statistics.h
	include/celero/TestFixture.h
	include/celero/ThreadLocal.h
	include/celero/ThreadTestFixture.h
	include/celero/TestVector.h
	include/celero/Timer.h
	include/celero/UserDefinedMeasurement.h
	include/celero/UserDefinedMeasurementCollector.h
	include/celero/UserDefinedMeasurementTemplate.h
	include/celero/Utilities.h
	src/Archive.cpp
	src/Benchmark.cpp
	src/Callbacks.cpp
	src/Celero.cpp
	src/Console.cpp
	src/Distribution.cpp
	src/Exceptions.cpp
	src/Executor.cpp
	src/JUnit.cpp
	src/Memory.cpp
	src/Print.cpp
	src/Experiment.cpp
	src/ExperimentResult.cpp
	src/ResultTable.cpp
	src/TestVector.cpp
	src/TestFixture.cpp
	src/ThreadTestFixture.cpp
	src/Timer.cpp
	src/UserDefinedMeasurementCollector.cpp
	src/Utilities.cpp
)

if(MSVC)
	set(SYSLIBS
		PowrProf.lib
		)
else()
	#pthread is required for std::thread to work.
	set(SYSLIBS
		pthread
		)
endif()

CeleroSetDefaultCompilerOptions()

target_link_libraries(${PROJECT_NAME} ${SYSLIBS})

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

# ---------------------------------------------------------------------------
# Install and exports
# ---------------------------------------------------------------------------

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-target
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/static
)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})

# export to be used from install location
install(EXPORT ${PROJECT_NAME}-target
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share
)

# export to be used from build directory
export(EXPORT ${PROJECT_NAME}-target
  FILE ${PROJECT_NAME}-target.cmake
)

if(CELERO_ENABLE_FOLDERS)
	set_property(TARGET celero PROPERTY FOLDER "celero")
endif()

add_subdirectory(experiments)

if(CELERO_ENABLE_TESTS)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	add_subdirectory(googletest)
	add_subdirectory(test)
endif()	
