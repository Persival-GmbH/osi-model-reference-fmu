cmake_minimum_required(VERSION 3.15)
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/fmu4cpp/version.txt" projectVersion)
project(fmu4cpp-template VERSION ${projectVersion})

set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

include("${CMAKE_CURRENT_LIST_DIR}/fmu4cpp/cmake/generate_fmu.cmake")
if (MSVC)
    # link statically against the the Visual C runtime
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif ()

if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(BITNESS 64)
else ()
    set(BITNESS 32)
endif ()

if (WIN32)
    set(TARGET_PLATFORM win${BITNESS})
elseif (APPLE)
    set(TARGET_PLATFORM darwin${BITNESS})
else ()
    set(TARGET_PLATFORM linux${BITNESS})
endif ()


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fmu4cpp/export/src ${CMAKE_BINARY_DIR}/lib/fmu4cpp/export/src)
# internal executable invoked by CMake to generate the modelDescription.xml from a shared library
add_executable(descriptionGenerator "${CMAKE_CURRENT_LIST_DIR}/fmu4cpp/export/descriptionGenerator.cpp")
if (UNIX)
    target_link_libraries(descriptionGenerator PRIVATE dl)
endif()