cmake_minimum_required(VERSION 3.16)

# Automatically set project name based on current directory
get_filename_component(PROJECT_NAME raytracer NAME)
project(${PROJECT_NAME} LANGUAGES C CXX)

# C and C++ standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option to build as library
option(BUILD_AS_LIBRARY "Build as static library instead of executable" OFF)

set(INCLUDE_DIR  ${PROJECT_SOURCE_DIR}/include)
set(EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
set(SRC_DIR      ${PROJECT_SOURCE_DIR}/src)

# External library stb
add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE ${EXTERNAL_DIR}/stb)

# Glob all source files (.c and .cpp) recursively
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${SRC_DIR}/*.c
  ${SRC_DIR}/*.cpp
)

# Always create a library from source files
add_library(${PROJECT_NAME}_lib STATIC ${SRC_FILES})
target_include_directories(${PROJECT_NAME}_lib 
  PUBLIC 
    ${INCLUDE_DIR}
    ${EXTERNAL_DIR}
  PRIVATE
  ${SRC_DIR}
)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC stb)

# Create executable and link it to the core library
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# Link include directories to the executable
target_include_directories(${PROJECT_NAME} 
  PRIVATE 
    ${INCLUDE_DIR} 
    ${SRC_DIR}
)

# Enable Wall and Werror
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
elseif(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()
