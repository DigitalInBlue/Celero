#
# Top level makefile for Celero
#
# Copyright 2015-2023 John Farrier
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
	set_target_properties(${PROJECT_NAME} PROPERTIES
		POSITION_INDEPENDENT_CODE "${CELERO_COMPILE_PIC}"
	)

	if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
		target_compile_options(${PROJECT_NAME} PRIVATE /D_VARIADIC_MAX=10 )
		target_compile_options(${PROJECT_NAME} PRIVATE /D_CRT_SECURE_NO_WARNINGS)
		target_compile_options(${PROJECT_NAME} PRIVATE /wd4251)
		target_compile_options(${PROJECT_NAME} PRIVATE /D_SCL_SECURE_NO_WARNINGS)
		target_compile_options(${PROJECT_NAME} PRIVATE /permissive-)
		target_compile_options(${PROJECT_NAME} PRIVATE /MP)

		if (NOT CELERO_COMPILE_DYNAMIC_LIBRARIES)
			if(VCPKG_CRT_LINKAGE)
				if(VCPKG_CRT_LINKAGE STREQUAL "static")
					target_compile_options(${PROJECT_NAME} PRIVATE /MT$<$<CONFIG:Debug>:d>)
				else()
					target_compile_options(${PROJECT_NAME} PRIVATE /MD$<$<CONFIG:Debug>:d>)
				endif()
			else()
				target_compile_options(${PROJECT_NAME} PRIVATE /MT$<$<CONFIG:Debug>:d>)
			endif()
		endif()

        if(CELERO_TREAT_WARNINGS_AS_ERRORS)
            target_compile_options(${PROJECT_NAME} PRIVATE /WX)
		endif()

	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
			target_compile_options(${PROJECT_NAME} PRIVATE -Xclang)
			target_compile_options(${PROJECT_NAME} PRIVATE -Wno-c++98-compat)
			target_compile_options(${PROJECT_NAME} PRIVATE -Wno-c++98-compat-pedantic)
			target_compile_options(${PROJECT_NAME} PRIVATE -Wno-reserved-id-macro)
			if(CELERO_TREAT_WARNINGS_AS_ERRORS)
				target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
			endif()
		else()
			target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
			if(CELERO_TREAT_WARNINGS_AS_ERRORS)
				target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
			endif()
		endif()

	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

		if(CELERO_TREAT_WARNINGS_AS_ERRORS)
			target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
		endif()
	endif()
endmacro()

# Project Name
PROJECT(celero)

include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CMakeFindDependencyMacro)

#
# User Options
#

option(CELERO_COMPILE_DYNAMIC_LIBRARIES "Set to ON to build Celero for dynamic linking.  Use OFF for static." ON)
option(CELERO_COMPILE_PIC "Set to ON to build Celero as a position-independent library." ON)
option(CELERO_ENABLE_EXPERIMENTS "Set to ON to automatically build all examples." OFF)
option(CELERO_ENABLE_FOLDERS "Enable to put Celero in its own solution folder under Visual Studio" ON)
option(CELERO_ENABLE_TESTS "Enable building and running unit tests." OFF)
option(CELERO_TREAT_WARNINGS_AS_ERRORS "Treat compile warnings as errors." ON)

if(CELERO_COMPILE_DYNAMIC_LIBRARIES)
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "SHARED")
else()
	SET(CELERO_USER_DEFINED_SHARED_OR_STATIC "STATIC")
endif()

if(CELERO_ENABLE_TESTS)
	find_dependency(GTest REQUIRED)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Build and Install Settings
#

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_DEBUG_POSTFIX          "-d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX        "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "-rd" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX     "" CACHE STRING "add a postfix, usually empty on windows")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------

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

if(WIN32)
	set(SYSLIBS
		powrprof psapi
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
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ---------------------------------------------------------------------------
# Install and exports
# ---------------------------------------------------------------------------

install(
	DIRECTORY   include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
	TARGETS     celero
	DESTINATION ${CMAKE_INSTALL_LIBDIR}
	EXPORT      celero-targets
)
install(
	EXPORT      celero-targets
	FILE        celero-targets.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/celero
)
export(
	EXPORT 		celero-targets
    FILE   		celero-targets.cmake
)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/celero-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/celero
)
install(
	FILES 		${CMAKE_CURRENT_BINARY_DIR}/celero-config.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/celero
)

if(CELERO_ENABLE_FOLDERS)
	set_property(TARGET celero PROPERTY FOLDER "celero")
endif()

add_subdirectory(experiments)

if(CELERO_ENABLE_TESTS)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	add_subdirectory(test)
endif()


# ------------------------------------------------------------------------------
# CppCheck
# ------------------------------------------------------------------------------

# Experimental
if(CELERODEVOPS_ENABLE_CPPCHECK)

    list(APPEND CPPCHECK_CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
    )

    ExternalProject_Add(
        cppcheck
        GIT_REPOSITORY      https://github.com/danmar/cppcheck.git
        GIT_TAG             1.79
        GIT_SHALLOW         1
        CMAKE_ARGS          ${CPPCHECK_CMAKE_ARGS}
        PREFIX              ${CMAKE_BINARY_DIR}/external/cppcheck/prefix
        TMP_DIR             ${CMAKE_BINARY_DIR}/external/cppcheck/tmp
        STAMP_DIR           ${CMAKE_BINARY_DIR}/external/cppcheck/stamp
        DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/cppcheck/download
        SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/cppcheck/src
        BINARY_DIR          ${CMAKE_BINARY_DIR}/external/cppcheck/build
    )

    list(APPEND CPPCHECK_ARGS
        --enable=warning,style,performance,portability,unusedFunction
        --std=c++11
        --verbose
        --error-exitcode=1
        --language=c++
        -DMAIN=main
        -I ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/*.h
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/test/*.cpp
    )

    add_custom_target(
        check
        COMMAND ${CMAKE_BINARY_DIR}/bin/cppcheck ${CPPCHECK_ARGS}
        COMMENT "running cppcheck"
    )

endif()

# ------------------------------------------------------------------------------
# Coverage
# ------------------------------------------------------------------------------

# Experimental
if(CELERODEVOPS_ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# ------------------------------------------------------------------------------
# Google Sanitizers
# ------------------------------------------------------------------------------

# Experimental
if(CELERODEVOPS_ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
endif()

# Experimental
if(CELERODEVOPS_ENABLE_USAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

# Experimental
if(CELERODEVOPS_ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()
