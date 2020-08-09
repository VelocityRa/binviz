add_library(util STATIC log.hpp
                        log.cpp)

target_link_libraries(util PRIVATE spdlog::spdlog)
