# Set the project name
project (lib)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/lib.cpp src/lib.hpp)
add_library(lib::lib ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/src
)
