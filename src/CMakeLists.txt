cmake_policy(SET CMP0072 NEW)
set(MODEL_TARGET ${MODEL_NAME}_lib)
set(MODEL_TARGET ${MODEL_TARGET} PARENT_SCOPE) # make it available to fmu and executable

SET(CMAKE_CXX_FLAGS "-fPIC")

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
    SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

####################################################################################################
# model config
####################################################################################################
file(GLOB MODEL_SOURCES_CPP "*.cpp")
file(GLOB MODEL_SOURCES_H "*.h")

add_library(${MODEL_TARGET} STATIC
        Model.cpp
        ${MODEL_SOURCES_CPP}
        ${MODEL_SOURCES_H})

target_include_directories(${MODEL_TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/lib/json)

if(LINK_WITH_SHARED_OSI)
    target_link_libraries(${MODEL_TARGET} open_simulation_interface)
else()
    target_link_libraries(${MODEL_TARGET} open_simulation_interface_pic)
endif()
