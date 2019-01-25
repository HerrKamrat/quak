# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
project(quak)

add_subdirectory(lib)

set(platform-libraries "")

set(build-executable TRUE)

if(ANDROID)
    set(build-executable FALSE)
    find_library( # Sets the name of the path variable.
            log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )
        string(APPEND platform-libraries ${log-lib})
endif()

if(${build-executable})
    add_executable(${PROJECT_NAME} main.cpp)
else()
    add_library(${PROJECT_NAME} SHARED native-lib.cpp )
endif()




target_link_libraries( # Specifies the target library.
                       ${PROJECT_NAME}

                       # Links the target library to the log library
                       # included in the NDK.
                       ${platform-libraries}
        lib::lib
        )