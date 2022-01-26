#
# Standard Celero Experiment CMake File
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
# Set the experiment's name here.  Start it with a capital letter use camel case naming.
#
SET(PROJECT_NAME CeleroExperiment-SortingRandomInts)

#
# Add Header Files
#
set(TARGET_H
)

#
# Add Sources
#
set(TARGET_SRC
    ExperimentSortingRandomInts.cpp
)

# Broiler Plate: Assign the src and headers to the executable.
add_executable(${PROJECT_NAME} 
    ${TARGET_SRC}
    ${TARGET_H}
)
    
# Broiler Plate: Celero Project Dependencies
target_link_libraries(${PROJECT_NAME} celero)
    
CeleroSetDefaultCompilerOptions()
    
# Broiler Plate: Add Celer's include directories.
include_directories(${HEADER_PATH})
    
# Broiler Plate: Set up folders for an IDE.
if(CELERO_ENABLE_FOLDERS)
    set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "celero/experiments")
endif()
    