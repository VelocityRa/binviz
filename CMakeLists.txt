### Initial CMake set-up
cmake_minimum_required(VERSION 3.12.4)

# Work around a bug with old MSVC
# if(${CMAKE_VERSION} VERSION_LESS 3.14)
#     cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
# else()
#     cmake_policy(VERSION 3.14)
# endif()

# if(NOT WIN32)
#     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#     link_libraries(stdc++fs)
# endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release)
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

include_directories(src)
include_directories(external)

# vcpkg setup
# set(VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# Project definition
project(binviz VERSION 0.1 DESCRIPTION "BinViz")

# Global CMake options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# if(MSVC)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /openmp")
# endif()

### Main executable
add_executable(${PROJECT_NAME} src/main.cpp)

### vcpkg dependencies

# spdlog
find_package(spdlog CONFIG REQUIRED)
# target_link_libraries(main PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_compile_definitions(spdlog::spdlog INTERFACE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
                                                    SPDLOG_NO_THREAD_ID=1
                                                    SPDLOG_NO_DATETIME=1
                                                    SPDLOG_NO_NAME=1
                                                    SPDLOG_NO_ATOMIC_LEVELS=1
                                                    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
)

# imgui
find_package(imgui CONFIG REQUIRED)

# glm
find_package(glm CONFIG REQUIRED)

# glfw3
find_package(glfw3 CONFIG REQUIRED)

# glad
find_package(glad CONFIG REQUIRED)

# Dependency: nativefiledialog
find_package(unofficial-nativefiledialog CONFIG REQUIRED)

### Add source tree
add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glad::glad glfw imgui::imgui glm spdlog::spdlog imgui_glfw_gl3_backend stb_image util unofficial::nativefiledialog::nfd)

### Add externals directory for additional externals
add_subdirectory(external)

set(SHADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders/screen.vs.glsl
                   ${CMAKE_CURRENT_SOURCE_DIR}/data/shaders/screen.fs.glsl)

set(SHADER_OUTPUTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/shaders/screen.vs.glsl
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data/shaders/screen.fs.glsl)

target_sources(${PROJECT_NAME} PRIVATE ${SHADER_OUTPUTS})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
# set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_custom_command(
    OUTPUT ${SHADER_OUTPUTS}
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data"
    DEPENDS ${SHADER_SOURCES}
)
