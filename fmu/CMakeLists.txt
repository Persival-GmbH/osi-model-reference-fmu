set(FMU_TARGET ${MODEL_NAME})

add_library(${FMU_TARGET} SHARED
        model_fmu.cpp)
target_include_directories(${FMU_TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/lib/fmu4cpp/export/include ${CMAKE_SOURCE_DIR}/lib/json)
include(${CMAKE_SOURCE_DIR}/lib/fmu4cppOSMP.cmake)
generateFMU("${MODEL_NAME}" "")

# link fmu with model and OSI
target_link_libraries(${FMU_TARGET} ${MODEL_TARGET})
if(LINK_WITH_SHARED_OSI)
    target_link_libraries(${FMU_TARGET} open_simulation_interface)
else()
    target_link_libraries(${FMU_TARGET} open_simulation_interface_pic)
endif()

# Define macros for the project name and target to be used in the generated xml file
target_compile_definitions(${FMU_TARGET} PRIVATE
        CMAKE_PROJECT_NAME="${CMAKE_PROJECT_NAME}"
)

# required for old versions of gcc etc.
find_package(Threads REQUIRED)
target_link_libraries(${FMU_TARGET} Threads::Threads)


if (DEFINED INSTALL_DIR)
    add_custom_command(TARGET ${FMU_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${FMU_TARGET}/${FMU_TARGET}.fmu" "${INSTALL_DIR}"
    )
endif()
