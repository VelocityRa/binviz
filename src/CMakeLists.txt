add_subdirectory(util)

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/renderer.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/renderer.hpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/shader.hpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/gui.cpp
                                       ${CMAKE_CURRENT_SOURCE_DIR}/gui.hpp)
