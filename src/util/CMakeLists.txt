add_library(util STATIC log.hpp
                        log.cpp
                        util.cpp
                        fs.hpp
                        types.hpp
                        load_file.hpp)

target_link_libraries(util PRIVATE spdlog::spdlog)
