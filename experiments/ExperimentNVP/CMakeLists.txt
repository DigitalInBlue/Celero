#
# Standard Celero Experiment CMake File
#
# Copyright 2019 John Farrier 
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
# CMake Configuration
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#
# Set the experiment's name here.  Start it with a capital letter use camel case naming.
#
SET(PROJECT_NAME NVP)

# Broiler Plate: Set up a CMake option.
option(CELEROExperiment_${PROJECT_NAME} "Set to ON to build ${PROJECT_NAME}." ON)
    
# Broiler Plate: Test the CMake option.
if(CELEROExperiment_${PROJECT_NAME})
    include(ExternalProject)

	EXTERNALPROJECT_ADD(
		rapidjson
		GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
        GIT_TAG "master"
		SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson"
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
	)
	set(RAPIDJSON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include")

    EXTERNALPROJECT_ADD(
        pugixml
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pugixml"
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_SHARED_AND_STATIC_LIBS=1
    )
    set(PUGIXML_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src")

	find_package(Boost REQUIRED)

    #
    # Add Header Files
    #
    set(TARGET_H
    )

    #
    # Add Sources
    #
    set(TARGET_SRC
        ExperimentNVP.cpp
		${PUGIXML_INCLUDE}/pugixml.cpp
    )

    # Broiler Plate: Assign the src and headers to the executable.
    add_executable(${PROJECT_NAME} 
        ${TARGET_SRC}
        ${TARGET_H}
    )
    
    # Broiler Plate: Celero Project Dependencies
    add_dependencies(${PROJECT_NAME} celero rapidjson)
	target_link_libraries(${PROJECT_NAME} celero)
    
    if(MSVC) 
		# Broiler Plate: VS2012 doesn't support true variadic templates
		target_compile_definitions(${PROJECT_NAME} PRIVATE /D_VARIADIC_MAX=10 )
		target_compile_options(${PROJECT_NAME} PRIVATE /W4)
		target_compile_options(${PROJECT_NAME} PRIVATE /D_CRT_SECURE_NO_WARNINGS)
		target_compile_options(${PROJECT_NAME} PRIVATE /wd4251)
		target_compile_options(${PROJECT_NAME} PRIVATE /nologo)
	endif()

    target_compile_options(${PROJECT_NAME} PRIVATE -DJSON_INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/input.json")
	target_compile_options(${PROJECT_NAME} PRIVATE -DXML_INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/input.xml")
	    
    # Broiler Plate: Add Celer's include directories.
    include_directories(${HEADER_PATH} ${RAPIDJSON_INCLUDE} ${PUGIXML_INCLUDE} ${Boost_INCLUDE_DIRS})
    
    # Broiler Plate: Set up folders for an IDE.
    if(CELERO_ENABLE_FOLDERS)
        set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Celero/Experiments")
    endif()
    
endif(CELEROExperiment_${PROJECT_NAME})
