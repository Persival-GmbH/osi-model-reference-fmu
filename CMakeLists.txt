# Copyright 2024 Persival GmbH.

####################################################################################################
# Initial Config
####################################################################################################
set(MODEL_NAME "osi-model-reference")

if (POLICY CMP0003)
    cmake_policy(SET CMP0003 NEW)
endif (POLICY CMP0003)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
	SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
	SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

####################################################################################################
# Versioning and CMake Project setup
####################################################################################################
# Read the version number from the file "VERSION"
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VERSION_CONTENT)
string(REGEX MATCH "VERSION_MAJOR = ([0-9]+)" _ ${VERSION_CONTENT})
set(VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR = ([0-9]+)" _ ${VERSION_CONTENT})
set(VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_PATCH = ([0-9]+)" _ ${VERSION_CONTENT})
set(VERSION_PATCH ${CMAKE_MATCH_1})
set(MODEL_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
add_compile_definitions(MODEL_VERSION="${MODEL_VERSION}")
project(${MODEL_NAME} VERSION ${MODEL_VERSION})

####################################################################################################
# General Requirements
####################################################################################################
add_subdirectory(lib/open-simulation-interface)
set(LINK_WITH_SHARED_OSI OFF CACHE BOOL "Link model and FMU with shared OSI library instead of statically linking")

find_package(Protobuf 2.6.1 REQUIRED)

####################################################################################################
# Model
####################################################################################################
add_subdirectory(src)

####################################################################################################
# OSMP FMU Target
####################################################################################################
add_subdirectory(fmu)
