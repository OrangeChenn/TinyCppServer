file(GLOB_RECURSE orange_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(orange_shared SHARED ${orange_sources})