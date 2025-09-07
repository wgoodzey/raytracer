cmake_minimum_required(VERSION 3.16)

# Automatically set project name based on current directory
get_filename_component(PROJECT_NAME raytracer NAME)
project(${PROJECT_NAME} LANGUAGES C CXX)

# C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option to build as library
option(BUILD_AS_LIBRARY "Build as static library instead of executable" OFF)

# Include the 'include' directory
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# Glob all source files (.c and .cpp) recursively
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	${PROJECT_SOURCE_DIR}/src/*.c
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Always create a library from source files
add_library(${PROJECT_NAME}_lib STATIC ${SRC_FILES})
target_include_directories(${PROJECT_NAME}_lib PRIVATE ${INCLUDE_DIR})

# Create executable and link it to the core library
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# Link include directory to the target
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

# Enable Wall and Werror
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
elseif(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

# ------------------------
# Google Test Integration
# ------------------------

# Check if 'tests' directory exists
if(EXISTS ${CMAKE_SOURCE_DIR}/tests)

	enable_testing()

	# Glob all test source files (.cpp or .c) in the 'tests' directory
	file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS
		${CMAKE_SOURCE_DIR}/tests/*.cpp
		${CMAKE_SOURCE_DIR}/tests/*.c
	)

	# If test files are found, create a test executable and link to GTest
	if(TEST_FILES)

		# Fetch GoogleTest using FetchContent
		include(FetchContent)
		set(CMAKE_POLICY_VERSION_MINIMUM 3.16)

		FetchContent_Declare(
			GTest
			GIT_REPOSITORY https://github.com/google/googletest.git
			GIT_TAG release-1.11.0
		)
		FetchContent_MakeAvailable(GTest)

		# Create test executable
		add_executable(${PROJECT_NAME}_test ${TEST_FILES})

		# Link test executable against GTest and the main project target
		target_link_libraries(${PROJECT_NAME}_test
			PRIVATE
				GTest::gtest_main
				${PROJECT_NAME}_lib
		)

		# Add include directories (for headers and inline definitions)
		target_include_directories(${PROJECT_NAME}_test
			PRIVATE
				${CMAKE_SOURCE_DIR}/include
				${CMAKE_SOURCE_DIR}/src
		)

		# Register tests with CTest
		include(GoogleTest)
		gtest_discover_tests(${PROJECT_NAME}_test)

		message(STATUS "Test files: ${TEST_FILES}")

	else()
		message(WARNING "No test files found!")
	endif()

endif()

