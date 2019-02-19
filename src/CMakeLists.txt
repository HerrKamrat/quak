cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(quak)

# 3rd party libraries
add_subdirectory(3rd_party/catch)
add_subdirectory(3rd_party/duktape)
add_subdirectory(3rd_party/fmt)
add_subdirectory(3rd_party/physfs)
add_subdirectory(3rd_party/stb)
add_subdirectory(3rd_party/sdl)

add_subdirectory(lib)

set(platform-libraries "")

set(build-executable TRUE)

if(ANDROID)
    set(build-executable FALSE)
    find_library(log-lib log)
    find_library(android-lib android)
    set(platform-libraries "${log-lib} ${android-lib} ${SDL2_LIBRARY}")
elseif(WIN32)
    string(APPEND platform-libraries ${SDL2_LIBRARY})
endif()

if(${build-executable})
    add_executable(${PROJECT_NAME} main.cpp)
else()
    #TODO move android-extras.*pp to platform-src?
    add_library(${PROJECT_NAME} SHARED
            main.cpp
            android-extras.cpp
            android-extras.hpp )
endif()

#TODO move ${log-lib} ${android-lib} ${SDL2_LIBRARY} to platform-libraries
target_link_libraries(${PROJECT_NAME}
        ${log-lib} ${android-lib} ${SDL2_LIBRARY}
        lib::lib
)
