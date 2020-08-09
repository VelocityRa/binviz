add_library(stb_image)

target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

target_sources(stb_image PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/stb_image.c")