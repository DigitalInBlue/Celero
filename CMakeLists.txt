#
# Top level makefile for Celero
#
# Copyright 2015, 2016, 2017, 2018 John Farrier
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

# Project Name
PROJECT(CeleroProject)

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

if(POLICY CMP0042)
	cmake_policy(SET CMP0042 OLD) # MACOSX_RPATH migration
endif()

#
# User Options
#

option(CELERO_COMPILE_DYNAMIC_LIBRARIES "Set to ON to build Celero for dynamic linking.  Use OFF for static." ON)
option(CELERO_RUN_EXAMPLE_ON_BUILD "Set to ON to automatically run the example after a successful build." OFF)
option(CELERO_ENABLE_EXPERIMENTS "Set to ON to automatically build all examples." OFF)
option(CELERO_ENABLE_FOLDERS "Enable to put Celero in its own solution folder under Visual Studio" ON)
option(CELERO_ENABLE_TESTS "Enable building and running unit tests." OFF)

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

#message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------

#
# Install Locations
#

#
# include path to be used by all projects
#


set(PROJECT_NAME celero)

if(CELERO_COMPILE_DYNAMIC_LIBRARIES)
	add_definitions(-DCELERO_EXPORTS)
else()
	add_definitions(-DCELERO_STATIC)
endif()

#
# Define header and sources
#

set(TARGET_H
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
	include/celero/Utilities.h
)

set(TARGET_SRC
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
	src/Statistics.cpp
	src/TestVector.cpp
	src/TestFixture.cpp
	src/ThreadTestFixture.cpp
	src/Timer.cpp
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

add_library(${PROJECT_NAME} ${CELERO_USER_DEFINED_SHARED_OR_STATIC} ${TARGET_SRC} ${TARGET_H})

#
# Compiler Settings
#

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4)
	target_compile_options(${PROJECT_NAME} PRIVATE /D_CRT_SECURE_NO_WARNINGS)
	target_compile_options(${PROJECT_NAME} PRIVATE /wd4251)
	target_compile_options(${PROJECT_NAME} PRIVATE /nologo)
endif()

if(BORLAND)
  set(CMAKE_CXX_FLAGS "" CACHE STRING "Compiler Flags for All Builds" FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "Compiler Flags for Debug Builds" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE  "" CACHE STRING "Compiler Flags for Release Builds" FORCE)
endif()

#message (STATUS "SYSTEM: ${CMAKE_SYSTEM_NAME}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	target_compile_options(${PROJECT_NAME} PRIVATE -stdlib=libc++)
	set(CMAKE_COMPILER_IS_GNUCXX 1)
endif()

if(UNIX)
	if(CMAKE_COMPILER_IS_GNUCXX)
	  target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11 -std=gnu++11 -Wall -O3)
	  target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-O0 -pedantic -g>)
	else()
	  target_compile_options(${PROJECT_NAME} PRIVATE -std=c++11 -std=gnu++11 -Wall -O3 -fPIC)
	  target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-O0 -pedantic -g>)
	endif()
endif()

#
# Linker Settings
#

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

# ---------------------------------------------------------------------------
# Google Test Application
# ---------------------------------------------------------------------------

if(CELERO_ENABLE_TESTS)
	# Pull in Google Test
	# https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project

	# Download and unpack googletest at configure time
	configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)

	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . 
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

	if(result)
		message(FATAL_ERROR "CMake step for googletest failed: ${result}")
	endif()

	execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
	
	if(result)
		message(FATAL_ERROR "Build step for googletest failed: ${result}")
	endif()

	# Prevent overriding the parent project's compiler/linker
	# settings on Windows
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

	# Add googletest directly to our build. This defines
	# the gtest and gtest_main targets.
	add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
					 ${CMAKE_BINARY_DIR}/googletest-build
					 EXCLUDE_FROM_ALL)

	include_directories("${gtest_SOURCE_DIR}/include")

	set(PROJECT_NAME CeleroTest)

	add_executable(${PROJECT_NAME}
		test/celero/Benchmark.test.cpp
		test/celero/Memory.test.cpp
		test/celero/Utilities.test.cpp
		)
	
	target_link_libraries(${PROJECT_NAME} gtest gtest_main celero)

	add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

	# VS2012 doesn't support true variadic templates
	if(MSVC)
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()

	if(CELERO_ENABLE_AUTO_RUN_TESTS)
		add_test(${PROJECT_NAME} ${PROJECT_NAME})
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
	endif()

	if(CELERO_ENABLE_FOLDERS)
		set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Celero/Test")
	endif()
endif()

# ---------------------------------------------------------------------------
# Optional
# ---------------------------------------------------------------------------

if(CELERO_ENABLE_FOLDERS)
	set_property(TARGET celero PROPERTY FOLDER "Celero")
endif()

add_subdirectory(experiments)
