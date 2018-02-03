#
# Standard Celero Experiment CMake File
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
# CMake Configuration
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#
# Set the experiment's name here.  Start it with "celero" and use camel case naming.
#
SET(PROJECT_NAME celeroDemoSleep)

# Broiler Plate: Set up a CMake option.
option(CELEROExperiment_${PROJECT_NAME} "Set to ON to build ${PROJECT_NAME}." ON)
    
# Broiler Plate: Test the CMake option.
if(CELEROExperiment_${PROJECT_NAME})
    #
    # Add Header Files
    #
    set(TARGET_H
    )

    #
    # Add Sources
    #
    set(TARGET_SRC
        DemoSleep.cpp
    )

    # Broiler Plate: Assign the src and headers to the executable.
    add_executable(${PROJECT_NAME} 
        ${TARGET_SRC}
        ${TARGET_H}
    )
    
    # Broiler Plate: Celero Project Dependencies
    add_dependencies(${PROJECT_NAME} celero)
    target_link_libraries(${PROJECT_NAME} celero)
    
    if(MSVC) 
		# Broiler Plate: VS2012 doesn't support true variadic templates
		target_compile_definitions(${PROJECT_NAME} PRIVATE /D_VARIADIC_MAX=10 )
		target_compile_options(${PROJECT_NAME} PRIVATE /W4)
		target_compile_options(${PROJECT_NAME} PRIVATE /D_CRT_SECURE_NO_WARNINGS)
		target_compile_options(${PROJECT_NAME} PRIVATE /wd4251)
		target_compile_options(${PROJECT_NAME} PRIVATE /nologo)
	endif()
    
    # Broiler Plate: Add Celer's include directories.
    include_directories(${HEADER_PATH})
    
    # Broiler Plate: Show here how to automate running after a build (comment in if desired)
    # if(CELERO_RUN_EXAMPLE_ON_BUILD)
    #	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_NAME}>)
    # endif()
    
    # Broiler Plate: Set up folders for an IDE.
    if(CELERO_ENABLE_FOLDERS)
        set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Celero/Experiments")
    endif()
    
endif(CELEROExperiment_${PROJECT_NAME})
