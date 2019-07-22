cmake_minimum_required(VERSION 3.0)

project(remotery)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Remotery-master/lib")

# compile as CXX to enable exceptions

add_library(${PROJECT_NAME} 
	${SRC_DIR}/Remotery.c
	${SRC_DIR}/Remotery.h
)

set_source_files_properties(
	${SRC_DIR}/Remotery.c
	PROPERTIES LANGUAGE CXX )


target_include_directories(${PROJECT_NAME} PUBLIC 
	${SRC_DIR}
)
