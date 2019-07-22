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
add_subdirectory(3rd_party/remotery)

add_subdirectory(lib)

set(platform-src)
set(platform-lib)

set(build-executable TRUE)
set(build-tests FALSE)

if(ANDROID)
    set(platform-src 
        android-extras.cpp
        android-extras.hpp )

    set(build-executable FALSE)
    find_library(log-lib log)
    find_library(android-lib android)

    set(platform-lib 
        ${log-lib} 
        ${android-lib} 
        ${SDL2_LIBRARY} )
    

elseif(WIN32)
    set(platform-lib 
        ${SDL2_LIBRARY} )
endif()

if(${build-executable})
	set(build-tests TRUE)
    add_executable(${PROJECT_NAME} 
        main.cpp
        ${platform-src} )
else()
    add_library(${PROJECT_NAME} SHARED
            main.cpp
            ${platform-src} )
endif()

target_link_libraries(${PROJECT_NAME}
    ${platform-lib}
    lib::lib )

if(${build-tests})
	#project(qtest)
	add_executable(test 
	tests/main.cpp
	tests/buffers.cpp)
	
	target_link_libraries(test
		${platform-lib}
		lib::lib
		catch
		remotery)

endif()