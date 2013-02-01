#
# Top level makefile for Celero
#
# John Farrier
#

#
# Cmake Configuration
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# Project Name
PROJECT(celero)

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

#
# User Options
#

option(CELERO_COMPILE_DYNAMIC_LIBRARIES "Set to ON to build Celero for dynamic linking.  Use OFF for static." ON)
option(CELERO_RUN_EXAMPLE_ON_BUILD "Set to ON to automatically run the example after a successful build." ON)
option(CELERO_USE_FOLDERS "Enable to put Celero in its own solution folder under Visual Studio" OFF)

if(CELERO_COMPILE_DYNAMIC_LIBRARIES)
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "SHARED")
else()
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "STATIC")
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

message (STATUS "SYSTEM: ${CMAKE_SYSTEM_NAME}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message (STATUS "I'm home")
    EXEC_PROGRAM(xcrun ARGS -find clang++ OUTPUT_VARIABLE XCODE_CLANG_PATH)
    set (XCODE_CLANG_PATH "${XCODE_CLANG_PATH}" CACHE STRING "Xcode clang path")
    set (CMAKE_CXX_COMPILER "${XCODE_CLANG_PATH}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") 
    set (CMAKE_COMPILER_IS_GNUCXX 1)
endif()
 
if(CMAKE_COMPILER_IS_GNUCXX) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11 -Wall" CACHE STRING "Compiler Flags for All Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pedantic" CACHE STRING "Compiler Flags for Debug Builds" FORCE) 
  set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compiler Flags for Release Builds" FORCE) 
endif() 

#
# Build and Install Settings
#

set(CMAKE_DEBUG_POSTFIX          "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX        ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX ""  CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX     ""  CACHE STRING "add a postfix, usually empty on windows")


# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #



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
	include/celero/BenchmarkInfo.h
	include/celero/Celero.h
	include/celero/Console.h
	include/celero/Executor.h
	include/celero/Export.h
	include/celero/Factory.h
	include/celero/GenericFactory.h
	include/celero/Pimpl.h
	include/celero/PimplImpl.h
	include/celero/Print.h
	include/celero/ResultTable.h
	include/celero/TestFixture.h
	include/celero/TestVector.h
	include/celero/Timer.h
	include/celero/Utilities.h
)

set(TARGET_SRC
	src/BenchmarkInfo.cpp
	src/Celero.cpp
	src/Console.cpp
	src/Executor.cpp
	src/Print.cpp
	src/ResultTable.cpp
	src/TestVector.cpp
	src/TestFixture.cpp
	src/Timer.cpp
)

set(TARGET_LIBRARIES ${SYSLIBS})
add_library(${PROJECT_NAME} ${CELERO_USER_DEFINED_SHARED_OR_STATIC} ${TARGET_SRC} ${TARGET_H})
include_directories(${HEADER_PATH})

# --------------------------------------------------------------------------- #
# --------------------------------------------------------------------------- #

SET(PROJECT_TEST_NAME celeroDemo)

#
# Add Sources
#

add_executable(${PROJECT_TEST_NAME} 
  src/DemoSimple.cpp
  src/DemoSort.cpp
  src/DemoTransform.cpp)

#
# Celero Project Dependencies
#

add_dependencies(${PROJECT_TEST_NAME} celero)
target_link_libraries(${PROJECT_TEST_NAME} celero)

#
# Broiler Plate
#

# VS2012 doesn't support true variadic templates
if(MSVC) 
  add_definitions( /D _VARIADIC_MAX=10 )
endif()

include_directories(${HEADER_PATH})

if(CELERO_RUN_EXAMPLE_ON_BUILD)
	add_custom_command(TARGET ${PROJECT_TEST_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_TEST_NAME}>)
endif()

#
# Optional
#

if(CELERO_USE_FOLDERS)
	set_property(TARGET celero PROPERTY FOLDER "Celero")
	set_property(TARGET celeroTest PROPERTY FOLDER "Celero")
endif()
