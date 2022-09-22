cmake_minimum_required(VERSION 3.0)

project(duktape)

set(SRC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/duktape/src")
set(EXTRAS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/duktape/extras")

# compile as CXX to enable exceptions

add_library(${PROJECT_NAME}-static
	${SRC_DIR}/duk_config.h
	${SRC_DIR}/duktape.c
	${SRC_DIR}/duktape.h

	${EXTRAS_SRC_DIR}/module-node/duk_module_node.c
	${EXTRAS_SRC_DIR}/module-node/duk_module_node.h
)

set_source_files_properties(
	${SRC_DIR}/duktape.c
	${EXTRAS_SRC_DIR}/module-node/duk_module_node.c
	PROPERTIES LANGUAGE CXX )


target_include_directories(${PROJECT_NAME}-static PUBLIC 
	${SRC_DIR}
	${EXTRAS_SRC_DIR}/module-node
)
