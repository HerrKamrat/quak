project (lib)

# Add a library with the above sources
add_library(${PROJECT_NAME} 
src/buffer.cpp
src/buffer.hpp

src/cmd.cpp
src/cmd.hpp

src/color.cpp
src/color.hpp

src/data_view.cpp
src/data_view.hpp

src/event.cpp
src/event.hpp

src/gfx.cpp
src/gfx.hpp

src/file.cpp
src/file.hpp

src/image.hpp
src/image.cpp

src/lib.cpp
src/lib.hpp

src/log.cpp
src/log.hpp

src/platform.cpp
src/platform.hpp

src/vm.cpp
src/vm.hpp

src/uri.cpp
src/uri.hpp

)

add_library(lib::lib ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
	#libsdl
	${core-libraries}

	#PhysicsFS
	#duktape
    #fmt-header-only
	#stb-header-only
)