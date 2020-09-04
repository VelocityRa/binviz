add_library(util STATIC log.hpp
                        log.cpp
                        util.cpp
                        util.hpp
                        fs.hpp
                        types.hpp
                        load_file.hpp
                        hsv.hpp)

target_link_libraries(util PRIVATE spdlog::spdlog)
