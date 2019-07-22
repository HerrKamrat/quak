cmake_minimum_required(VERSION 3.10)

project(SDL2)

# SDL2
set(SDL2_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL2-2.0.8")


if(ANDROID)
	set(SDL2_DIR "${SDL2_BASE_DIR}/android")

	#add_library(libsdl SHARED IMPORTED)
	#set_target_properties(libsdl PROPERTIES IMPORTED_LOCATION ${distribution_DIR}/libs/${ANDROID_ABI}/libSDL2.so)
	#set(SDL2_INCLUDE_DIR "${SDL2_BASE_DIR}/include")
	#target_include_directories(libsdl INTERFACE "${SDL2_INCLUDE_DIR}")

	set(SDL2_LIBRARY_DIR "${SDL2_DIR}/libs/${ANDROID_ABI}")

	set(SDL2_LIBRARY "${SDL2_LIBRARY_DIR}/libSDL2.so")
	set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")


	add_library(libsdl INTERFACE)
	target_link_libraries(libsdl INTERFACE "${SDL2_LIBRARY}")
	target_include_directories(libsdl INTERFACE "${SDL2_INCLUDE_DIR}")

	add_library(libsdlprebuilt SHARED IMPORTED)
	set_target_properties(libsdlprebuilt PROPERTIES IMPORTED_LOCATION ${SDL2_LIBRARY})

elseif(WIN32)
	set(SDL2_DIR "${SDL2_BASE_DIR}/win")

	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set(SDL2_LIBRARY_DIR "${SDL2_DIR}/lib/x64")
	else()
		set(SDL2_LIBRARY_DIR "${SDL2_DIR}/lib/x86")
	endif()

	set(SDL2_LIBRARY "${SDL2_LIBRARY_DIR}/SDL2main.lib" "${SDL2_LIBRARY_DIR}/SDL2.lib")
	set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
	
	add_library(libsdl INTERFACE)
	target_link_libraries(libsdl INTERFACE "${SDL2_LIBRARY}")
	target_include_directories(libsdl INTERFACE "${SDL2_INCLUDE_DIR}")

endif()