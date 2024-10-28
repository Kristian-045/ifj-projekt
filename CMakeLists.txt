cmake_minimum_required(VERSION 3.10)

# Set project name and language
project(ifj_projekt C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Add source files for the main executable
set(SOURCES
        main.c
        scanner.c
        parser.c
        visualization.c
)

# Add header files
set(HEADERS
        scanner.h
        parser.h
        visualization.h
)

# Create main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set output directory for main executable
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add test source files (excluding main.c)
set(TEST_SOURCES
        tests/test1.c
)

# Create a test executable without including main.c
add_executable(test_${PROJECT_NAME} ${TEST_SOURCES} scanner.c parser.c visualization.c)

# Ensure that the test executable depends on the main executable
add_dependencies(test_${PROJECT_NAME} ${PROJECT_NAME})

# Link test executable with required libraries if needed
# target_link_libraries(test_${PROJECT_NAME} <libraries>)

# Set output directory for test executable
set_target_properties(test_${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)