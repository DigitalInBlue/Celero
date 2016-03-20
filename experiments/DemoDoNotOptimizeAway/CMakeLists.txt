#
# Standard Celero Experiment CMake File
#
# Copyright 2016 John Farrier 
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
SET(PROJECT_EXPERIMENT_NAME celeroDemoDoNotOptimizeAway)

# Broiler Plate: Set up a CMake option.
option(CELEROExperiment_${PROJECT_EXPERIMENT_NAME} "Set to ON to build ${PROJECT_EXPERIMENT_NAME}." ON)
    
# Broiler Plate: Test the CMake option.
if(CELEROExperiment_${PROJECT_EXPERIMENT_NAME})
    #
    # Add Header Files
    #
    set(TARGET_H
    )

    #
    # Add Sources
    #
    set(TARGET_SRC
        DemoDoNotOptimizeAway.cpp
    )

    # Broiler Plate: Assign the src and headers to the executable.
    add_executable(${PROJECT_EXPERIMENT_NAME} 
        ${TARGET_SRC}
        ${TARGET_H}
    )
    
    # Broiler Plate: Celero Project Dependencies
    add_dependencies(${PROJECT_EXPERIMENT_NAME} celero)
    target_link_libraries(${PROJECT_EXPERIMENT_NAME} celero)
    
    # Broiler Plate: VS2012 doesn't support true variadic templates
    if(MSVC) 
      add_definitions( /D _VARIADIC_MAX=10 )
    endif()
    
    # Broiler Plate: Add Celer's include directories.
    include_directories(${HEADER_PATH})
    
    # Broiler Plate: Show here how to automate running after a build (comment in if desired)
    # if(CELERO_RUN_EXAMPLE_ON_BUILD)
    #	add_custom_command(TARGET ${PROJECT_EXPERIMENT_NAME} POST_BUILD COMMAND $<TARGET_FILE:${PROJECT_EXPERIMENT_NAME}>)
    # endif()
    
    # Broiler Plate: Set up folders for an IDE.
    if(CELERO_ENABLE_FOLDERS)
        set_property(TARGET ${PROJECT_EXPERIMENT_NAME} PROPERTY FOLDER "Celero/Experiments")
    endif()
    
endif(CELEROExperiment_${PROJECT_EXPERIMENT_NAME})
