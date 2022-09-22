cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(quak)

# 3rd party libraries
add_subdirectory(3rd_party/)

add_subdirectory(src/lib)

add_executable(${PROJECT_NAME}
    main.cpp
)
target_link_libraries(${PROJECT_NAME}
    ${core-libraries}
    lib::lib
)
