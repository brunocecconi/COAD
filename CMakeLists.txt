
cmake_minimum_required(VERSION 3.23)

project(COAD)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} Wno-deprecated-builtins")
endif()

set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_MODE BATCH)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 8)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
endif()

set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

if(WIN32)
	include(${CMAKE_SOURCE_DIR}/deps/windows/config.cmake)
else()
	message(FATAL_ERROR "Platform not yet supported.")
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE COAD_FILES
	"${CMAKE_SOURCE_DIR}/src/ai/*.h" "${CMAKE_SOURCE_DIR}/src/ai/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/audio/*.h" "${CMAKE_SOURCE_DIR}/src/audio/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/core/*.h" "${CMAKE_SOURCE_DIR}/src/core/*.hpp" "${CMAKE_SOURCE_DIR}/src/core/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/ecs/*.h" "${CMAKE_SOURCE_DIR}/src/ecs/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/engine/*.h" "${CMAKE_SOURCE_DIR}/src/engine/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/game/*.h" "${CMAKE_SOURCE_DIR}/src/game/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/physics/*.h" "${CMAKE_SOURCE_DIR}/src/physics/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/platform/*.h" "${CMAKE_SOURCE_DIR}/src/platform/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/plugin/*.h" "${CMAKE_SOURCE_DIR}/src/plugin/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/render/*.h" "${CMAKE_SOURCE_DIR}/src/render/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/script/*.h" "${CMAKE_SOURCE_DIR}/src/script/*.cpp")

add_library(COAD STATIC ${COAD_FILES})
target_link_libraries(COAD ${MIMALLOC_LIB} ${EASTL_LIB})
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${COAD_FILES})
target_compile_definitions(COAD PUBLIC USE_EXPLICIT USE_SAFE_PTR)

add_definitions(-DEASTL_ASSERT_ENABLED=0
	-D_CRT_SECURE_NO_WARNINGS
	-DEASTL_EASTDC_VSNPRINTF=0)

add_executable(COAD-cmd "${CMAKE_SOURCE_DIR}/src/cmd/main.cpp")
target_link_libraries(COAD-cmd COAD ${MIMALLOC_LIB})

add_executable(COAD-benchmark "${CMAKE_SOURCE_DIR}/src/benchmark/main.cpp")
target_link_libraries(COAD-benchmark COAD ${MIMALLOC_LIB} ${BENCHMARK_LIB} ${BENCHMARK_MAIN_LIB} ${EASTL_LIB})

add_custom_target(natvis SOURCES ${CMAKE_SOURCE_DIR}/deps/windows/share/doc/EASTL.natvis)
