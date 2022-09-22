add_subdirectory(SDL)

set(INSTALL_LIBRARY OFF)
set(BUILD_FRAMEWORK OFF)

set(SDL2_FOUND TRUE)
message ( WARNING "${CMAKE_CURRENT_SOURCE_DIR}/SDL/include/ is NOTFOUND" )

set(SDL2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL/include/")
set(SDL2MAIN_LIBRARY "")
set(SDL2_LIBRARY "SDL2-static")


set(BUILD_DEMOS OFF)
set(BUILD_TESTS OFF)
set(BUILD_VIDEO_TEST OFF)
set(BUILD_TOOLS OFF)
set(INSTALL_LIBRARY OFF)
set(BUILD_SHARED OFF)
add_subdirectory(sdl-gpu)

set(PHYSFS_DISABLE_INSTALL ON)
set(PHYSFS_BUILD_SHARED OFF)
set(PHYSFS_BUILD_TEST OFF)
set(PHYSFS_TARGETNAME_UNINSTALL "PHYSFS_uninstall" CACHE STRING "Name of 'uninstall' build target")

add_subdirectory(physfs)

include(stb.cmake)
include(duktape.cmake)

add_subdirectory(fmt)

find_package(OpenGL REQUIRED)


set(core-libraries 
    SDL2-static
    SDL_gpu
    ${OPENGL_LIBRARIES}
    physfs-static
    stb-header-only
    duktape-static
    fmt-header-only
    PARENT_SCOPE
)
