#
# Top level makefile for Celero
#
# Copyright 2015, 2016, 2017, 2018, 2019, 2020, 2021, 2022  John Farrier
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

project(celero-test)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	Archive.test.cpp
	Benchmark.test.cpp
	Callbacks.test.cpp
	Celero.test.cpp
	Distribution.test.cpp
	Executor.test.cpp
	Memory.test.cpp
	Statistics.test.cpp
	TestFixture.test.cpp
	TestVector.test.cpp
	Timer.test.cpp
	UserDefinedMeasurementCollector.test.cpp
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
	GTest::gtest
    GTest::gtest_main
	celero
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
  ${CMAKE_SOURCE_DIR}/googletest/googletest/include
  ${CMAKE_SOURCE_DIR}/include
  ${GTEST_INCLUDE_DIR}
)

if(CELERO_ENABLE_AUTO_RUN_TESTS)
	add_test(${PROJECT_NAME} ${PROJECT_NAME})
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
endif()

if(CELERO_ENABLE_FOLDERS)
	set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "celero")
endif()
