#
# Top level makefile for Celero
#
# Copyright 2015 John Farrier
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Project Name
PROJECT(CeleroProject)

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

#
# User Options
#

option(CELERO_COMPILE_DYNAMIC_LIBRARIES "Set to ON to build Celero for dynamic linking.  Use OFF for static." ON)
option(CELERO_RUN_EXAMPLE_ON_BUILD "Set to ON to automatically run the example after a successful build." OFF)
option(CELERO_ENABLE_EXPERIMENTS "Set to ON to automatically build all examples." ON)
option(CELERO_ENABLE_FOLDERS "Enable to put Celero in its own solution folder under Visual Studio" ON)
option(CELERO_ENABLE_TESTS "Enable building and running unit tests." OFF)

if(CELERO_COMPILE_DYNAMIC_LIBRARIES)
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "SHARED")
else()
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "STATIC")
endif()

if(CELERO_ENABLE_TESTS)
	set(CELERO_ENABLE_AUTO_RUN_TESTS ON CACHE BOOL "Enable Celero tests to automatically run after building")
	set(GTEST_DIRECTORY test/gtest-1.7.0)
	set(GTEST_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/include)
	set(GTEST_LIBRARY gtest)
	set(GTEST_MAIN_LIBRARY gtest_main)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/include ${CMAKE_CURRENT_SOURCE_DIR}/${GTEST_DIRECTORY}/src)
	ADD_SUBDIRECTORY(${GTEST_DIRECTORY})
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Compiler Settings
#

if(MSVC)
  # Force to always compile with warning level 4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "Compiler Flags for All Builds" FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Compiler Flags for Debug Builds" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE)

  add_definitions("/D_CRT_SECURE_NO_WARNINGS /wd4251 /nologo")
endif()

if(BORLAND)
  set(CMAKE_CXX_FLAGS "" CACHE STRING "Compiler Flags for All Builds" FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "Compiler Flags for Debug Builds" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE  "" CACHE STRING "Compiler Flags for Release Builds" FORCE)
endif()

#message (STATUS "SYSTEM: ${CMAKE_SYSTEM_NAME}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	set(CMAKE_COMPILER_IS_GNUCXX 1)
endif()

if(UNIX)
	if(CMAKE_COMPILER_IS_GNUCXX)
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11 -Wall -O3" CACHE STRING "Compiler Flags for All Builds" FORCE)
	  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic -O0 -g" CACHE STRING "Compiler Flags for Debug Builds" FORCE)
	  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE)
	else()
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11 -Wall -fPIC -O3" CACHE STRING "Compiler Flags for All Builds" FORCE)
	  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic -O0 -g" CACHE STRING "Compiler Flags for Debug Builds" FORCE)
	  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE)
	endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Build and Install Settings
#

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

SET(HEADER_PATH ${celero_SOURCE_DIR}/include)

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
	include/celero/Executor.h
	include/celero/Export.h
	include/celero/Factory.h
	include/celero/FileReader.h
	include/celero/GenericFactory.h
	include/celero/JUnit.h
	include/celero/Pimpl.h
	include/celero/PimplImpl.h
	include/celero/Print.h
	include/celero/Experiment.h
	include/celero/Result.h
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
	src/Executor.cpp
	src/JUnit.cpp
	src/Print.cpp
	src/Experiment.cpp
	src/Result.cpp
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
target_link_libraries(${PROJECT_NAME} ${SYSLIBS})
include_directories(${HEADER_PATH})

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# ---------------------------------------------------------------------------
# Google Test Application
# ---------------------------------------------------------------------------

if(CELERO_ENABLE_TESTS)
	set(PROJECT_NAME CeleroTest)

	add_executable(${PROJECT_NAME}
		test/celero/Benchmark.test.cpp
		test/celero/Utilities.test.cpp
		)

	# VS2012 doesn't support true variadic templates
	if(MSVC)
		add_definitions( /D _VARIADIC_MAX=10 )
	endif()

	SET(HEADER_PATH ${celero_SOURCE_DIR}/include)
	include_directories(${HEADER_PATH})
	include_directories(${GTEST_INCLUDE_DIR})

	add_dependencies(${PROJECT_NAME} celero)
	target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} celero)

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

ADD_SUBDIRECTORY(experiments)
