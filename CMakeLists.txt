cmake_minimum_required(VERSION 3.15)
project(js-libdnf5)

add_compile_definitions(NAPI_VERSION=10)

add_definitions(-DNODE_ADDON_API_CPP_EXCEPTIONS)
add_definitions(-DNODE_ADDON_API_CPP_EXCEPTIONS_ALL)

find_path(DNF_INCLUDE_DIR NAMES libdnf5/base/base.hpp)

find_library(DNF_LIB NAMES libdnf5 libdnf5.so.2)

if(NOT DNF_INCLUDE_DIR)
	message(FATAL_ERROR "Could not find libdnf5 headers. Install libdnf5 headers.")
endif()

if(NOT DNF_LIB)
	message(FATAL_ERROR "Could not find libdnf5 library. Install libdnf5.")
endif()

file(GLOB SRC "src/native/*")

add_library(${PROJECT_NAME} SHARED ${SRC} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	PREFIX ""
	SUFFIX ".node"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
target_include_directories(${PROJECT_NAME} PRIVATE ${DNF_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_JS_LIB} ${DNF_LIB})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
