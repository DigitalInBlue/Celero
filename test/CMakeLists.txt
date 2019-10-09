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

include(ExternalProject)

# Add gtest
# http://stackoverflow.com/questions/9689183/cmake-googletest
ExternalProject_Add(
    googletest
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/googletest
    GIT_SUBMODULES ${CMAKE_SOURCE_DIR}/googletest
    INSTALL_COMMAND ""
    LOG_CONFIGURE ON
    LOG_BUILD ON
    )

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/include)

project(celero-test)

add_executable(${PROJECT_NAME})	

add_dependencies(${PROJECT_NAME} googletest)

target_sources(${PROJECT_NAME} PRIVATE
	Benchmark.test.cpp
	Memory.test.cpp
	Utilities.test.cpp
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

target_link_libraries(${PROJECT_NAME} 
	${SYSLIBS} 
	optimized gtest debug gtestd
	optimized gtest_main debug gtest_maind
	celero
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
  ${CMAKE_SOURCE_DIR}/include
)
	
if(CELERO_ENABLE_AUTO_RUN_TESTS)
	add_test(${PROJECT_NAME} ${PROJECT_NAME})
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
endif()

if(CELERO_ENABLE_FOLDERS)
	set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "celero")
endif()
